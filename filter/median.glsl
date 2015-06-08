//#version 430

// Medial filter implemented as OpenGL 4.3 compute shaders

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

layout(local_size_x=TILE_WIDTH,local_size_y=TILE_HEIGHT) in;

shared vec4 pixel[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];  // pixel values read from input image
shared float metric[NEIGHBORHOOD_HEIGHT][NEIGHBORHOOD_WIDTH];

struct PixelInfo {
    float metric;
    int ndx;
};

void main() 
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
        vec4 rgba = imageLoad(input_image, read_at);
        pixel[y+j][x+i] = rgba;
        const vec3 rgb2metric = vec3(0.299, 0.587, 0.114); // luminance
        // Alterantive metrics
        //const vec3 rgb2metric = vec3(0.615, -0.51499, -0.10001); // U
        //const vec3 rgb2metric = vec3(1,0,0);                     // red
        //const vec3 rgb2metric = vec3(-0.14713,-0.28886,0.436);   // V
        metric[y+j][x+i] = dot(rgba.rgb, rgb2metric);
      }
    }
  }
  retirePhase();

  // Phase 2: Initialize an array of <metric values,index> pairs to sort.
  PixelInfo info[filterWidth*filterHeight];
  for (int j=0; j<filterHeight; j++) {
    for (int i=0; i<filterWidth; i++) {
      int ndx = filterWidth*j+i;
      info[ndx].metric = metric[y+j][x+i];
      info[ndx].ndx = i*0x100 + j;
    }
  }
  // Phase 3: Perform sort.
  const int n = filterWidth*filterHeight;
#ifdef COMPILE_estimated_median
  // Rather than computing the true median, use 
  // Phase 3.1: Find the top and bottom (min/max) metric in the neighborhood.
  float top = info[0].metric;
  float bottom = info[0].metric;
  for (int i=1; i<n; i++) {
      top = max(top, info[i].metric);
      bottom = min(bottom, info[i].metric);
  }
  // Phase 3.2: Estimate the median as the center of the range.
  float estimated_median = 0.5*(top+bottom);
  float half_interval = 0.5*(top-bottom);
  // Refine the estimated median.
  for (int i=0; i<6; i++) {
    int below_count = 0;
    for (int j=0; j<n; j++) {
      if (info[j].metric < estimated_median) {
        below_count++;
      }
    }
    half_interval *= 0.5;
    const int middle = n/2;
    if (below_count > middle) {
      estimated_median -= half_interval;
    } else {
      estimated_median += half_interval;
    }
  }
  // Phase 3.3: Find actual sample nearest the estimated median.
  float closeness = abs(info[0].metric - estimated_median);
  int middle = 0;
  for (int i=1; i<n; i++) {
      // Is this sample closer to the estimated median?
      const float new_closeness = abs(info[i].metric - estimated_median);
      if (new_closeness < closeness) {
          // Yes, make sample new median.
          closeness = new_closeness;
          middle = i;
      }
  }
#else
  // True median filter using a sort:
  const int middle = n/2;
#if 1  // Emperically, the insertion sort is slightly faster.
  // Insertion sort:
  for (int i=1; i<n; i++) {
    PixelInfo temp = info[i];
    int j = i-1;
    while ((j >= 0) && (temp.metric < info[j].metric)) {
      info[j+1] = info[j];
      j--;
    }
    info[j+1] = temp;
  }
#else
  // Selection sort:
  // Instead of doing "n-1" iterations, stop at "middle+1" when info[middle] is sorted.
  for (int j=0; j<middle+1; j++) {
      int iMin = j;
      for (int i=j+1; i<n; i++) {
          if (info[i].metric < info[iMin].metric) {
              iMin = i;
          }
      }
      // iMin is the index of the minimum element.
      if (iMin != j) {
          // Swap it with the current position.
          PixelInfo temp = info[j];
          info[j] = info[iMin];
          info[iMin] = temp;
      }
  }
#endif
#endif

  // Phase 4: Select the median color value.
  const int dx = (info[middle].ndx >> 8);
  const int dy = (info[middle].ndx & 0xFF);
  const vec4 result = pixel[y+dy][x+dx];

  // Phase 5: Store result to output image.
  imageStore(output_image, pixel_xy, result);
}
