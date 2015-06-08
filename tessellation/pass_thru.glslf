
// pass_thru.glslf

// DESCRIPTION: GLSL fragment shader to pass thru interpolated color.

// To off-line compile this code to assembly with Cg 3.0, use:
// cgc -profile gp5fp -oglsl pass_thru.glslf

void main(void)
{
  gl_FragColor = gl_Color;
}
