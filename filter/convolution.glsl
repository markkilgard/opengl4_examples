//#version 430

// Convolution kernels implemented as OpenGL 4.3 compute shaders

// Implement three different convolutions:
// * separable - fast convolution for separable kernels
// * naive_separable - less fast convolution for separable kernels
// * general - handle generate convolution kernels

// This NVIDIA-specific pragma makes no difference in this example.
#if 0
#pragma optionNV(unroll all)
#endif

// Various kernel-wide constants
const int tileWidth = TILE_WIDTH,
          tileHeight = TILE_HEIGHT;
const int filterWidth = FILTER_WIDTH,
          filterHeight = FILTER_HEIGHT;
const ivec2 tileSize = ivec2(tileWidth,tileHeight);
const ivec2 filterOffset = ivec2(filterWidth/2,filterHeight/2);

// Declare the input and output images.
layout(binding=0,rgba8) uniform image2D input_image;
layout(binding=1,rgba8) uniform image2D output_image;

// GLSL lacks typedef...
#ifdef SCALAR_WEIGHTS
#define WeightType float
#else
#define WeightType vec4
#endif

// Separable weights
uniform WeightType rowWeight[filterWidth];
uniform WeightType columnWeight[filterHeight];

// General weights
uniform WeightType weight[filterHeight][filterWidth];

uniform ivec4 imageBounds;  // Bounds of the input image for pixel coordinate clamping.

#ifndef retirePhase
void retirePhase() { memoryBarrierShared(); barrier(); }
#endif
void nop() { }

ivec2 clampLocation(ivec2 xy)
{
#if 1
    // Clamp the image pixel location to the image boundary.
    return clamp(xy, imageBounds.xy, imageBounds.zw);
#else
    return xy;  // Skip the clamping.
#endif 
}

#ifdef COMPILE_naive_separable

layout(local_size_x=NEIGHBORHOOD_WIDTH,local_size_y=NEIGHBORHOOD_HEIGHT) in;

shared vec4 pixel[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];  // pixel values read from input image
shared vec4 row[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];    // weighted row sums

void naive_separable()
{
  const ivec2 tile_xy = ivec2(gl_WorkGroupID);
  const ivec2 thread_xy = ivec2(gl_LocalInvocationID);
  const ivec2 pixel_xy = tile_xy*tileSize + (thread_xy-filterOffset);
  const uint x = thread_xy.x;
  const uint y = thread_xy.y;

  // Phase 1: Read the image's neighborhood into shared pixel arrays.
  pixel[y][x] = imageLoad(input_image, clampLocation(pixel_xy));
  retirePhase();
  // Phase 2: Weighted sum the rows horizontally.
  row[y][x] = vec4(0);
  for (int i=0; i<filterWidth; i++) {
    row[y][x] += pixel[y][x+i-filterOffset.x] * rowWeight[i];
  }
  retirePhase();
  // Phase 3: Weighted sum the row sums vertically.
  vec4 result = vec4(0);
  for (int i=0; i<filterHeight; i++) {
    result += row[y+i-filterOffset.y][x] * columnWeight[i];
  }
  // Phase 4: Store result to output image.
  // Is the thread's (x,y) location within the tile?
  if ((x - filterOffset.x) < tileWidth &&
      (y - filterOffset.y) < tileHeight) {
      imageStore(output_image, pixel_xy, result);
  }
}

#endif // COMPILE_naive_separable

#ifdef COMPILE_separable

layout(local_size_x=TILE_WIDTH,local_size_y=NEIGHBORHOOD_HEIGHT) in;

shared vec4 pixel[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];  // pixel values read from input image
shared vec4 row[NEIGHBORHOOD_HEIGHT][TILE_WIDTH];    // weighted row sums

void separable()
{
  const ivec2 tile_xy = ivec2(gl_WorkGroupID);
  const ivec2 thread_xy = ivec2(gl_LocalInvocationID);
  const ivec2 pixel_xy = tile_xy*tileSize + (thread_xy-ivec2(0,filterOffset.y));
  const uint x = thread_xy.x;
  const uint y = thread_xy.y;

  // Phase 1: Read the image's neighborhood into shared pixel arrays.
  for (int i=0; i<NEIGHBORHOOD_WIDTH; i += TILE_WIDTH) {
    if (x+i < NEIGHBORHOOD_WIDTH) {
      const ivec2 read_at = clampLocation(pixel_xy+ivec2(i-filterOffset.x,0));
      pixel[y][x+i] = imageLoad(input_image, read_at);
    }
  }
  retirePhase();
  // Phase 2: Weighted sum the rows horizontally.
  row[y][x] = vec4(0);
  for (int i=0; i<filterWidth; i++) {
    row[y][x] += pixel[y][x+i] * rowWeight[i];
  }
  retirePhase();
  // Phase 3: Weighted sum the row sums vertically and write result to output image.
  // Does this thread correspond to a tile pixel?
  // Recall: There are more threads in the Y direction than tileHeight.
  if (y < tileHeight) {
    vec4 result = vec4(0);
    for (int i=0; i<filterHeight; i++) {
      result += row[y+i][x] * columnWeight[i];
    }
    // Phase 4: Store result to output image.
    const ivec2 pixel_xy = tile_xy*tileSize + thread_xy;
    imageStore(output_image, pixel_xy, result);
  }
}

#endif // COMPILE_separable

#ifdef COMPILE_general

layout(local_size_x=TILE_WIDTH,local_size_y=TILE_HEIGHT) in;

shared vec4 pixel[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];  // pixel values read from input image

void general()
{
  const ivec2 tile_xy = ivec2(gl_WorkGroupID);
  const ivec2 thread_xy = ivec2(gl_LocalInvocationID);
  const ivec2 pixel_xy = tile_xy*tileSize + thread_xy;
  const uint x = thread_xy.x;
  const uint y = thread_xy.y;

  // Phase 1: Read the image's neighborhood into shared pixel arrays.
  for (int j=0; j<NEIGHBORHOOD_HEIGHT; j += TILE_HEIGHT) {
    for (int i=0; i<NEIGHBORHOOD_WIDTH; i += TILE_WIDTH) {
      if (x+i < NEIGHBORHOOD_WIDTH && y+j < NEIGHBORHOOD_HEIGHT) {
        const ivec2 read_at = clampLocation(pixel_xy+ivec2(i,j)-filterOffset);
        pixel[y+j][x+i] = imageLoad(input_image, read_at);
      }
    }
  }
  retirePhase();
  // Phase 2: Compute general convolution.
  vec4 result = vec4(0);
  for (int j=0; j<filterHeight; j++) {
    for (int i=0; i<filterWidth; i++) {
      result += pixel[y+j][x+i] * weight[j][i];
    }
  }
  // Phase 3: Store result to output image.
  imageStore(output_image, pixel_xy, result);
}

#endif // COMPILE_general

#ifdef COMPILE_copy

layout(local_size_x=TILE_WIDTH,local_size_y=TILE_HEIGHT) in;

void copy()
{
  const ivec2 tile_xy = ivec2(gl_WorkGroupID);
  const ivec2 thread_xy = ivec2(gl_LocalInvocationID);
  const ivec2 pixel_xy = tile_xy*tileSize + thread_xy;

  vec4 pixel = imageLoad(input_image, pixel_xy);
  imageStore(output_image, pixel_xy, pixel);
}

#endif
