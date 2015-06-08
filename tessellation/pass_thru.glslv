// pass_thru.glslv - GLSL vertex shader to pass thru interpolated color

void main(void)
{
  gl_Position = gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  gl_TexCoord[2] = gl_MultiTexCoord2;
  gl_TexCoord[3] = gl_MultiTexCoord3;
  gl_TexCoord[4] = gl_MultiTexCoord4;
}
