
// diffuse.glslf

// DESCRIPTION: GLSL fragment shader to generate diffuse.

// To off-line compile this code to assembly with Cg 3.0, use:
// cgc -profile gp5fp -oglsl diffuse.glslf

void main(void)
{
  if (gl_FrontFacing) {
    gl_FragColor = gl_TexCoord[0].zzzz;
  } else {
    gl_FragColor = -gl_TexCoord[0].zzzz;
  }
}
