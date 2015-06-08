
// cp_transform2.glslv

// DESCRIPTION:  GLSL vertex shader to transform patch control points
// by the current concatenation of the OpenGL modelview and projection matrices
// and pass through the first two texture coordinate sets.

// To off-line compile this code to assembly with Cg 3.0, use:
// cgc -profile gp5vp -oglsl cp_transform2.glslv

#version 400 compatibility

// Multiply XYZ vertex by 4x3 modelview matrix (assumed non-projective)
precise vec3 transform(precise vec3 v)
{
  precise vec3 r;
  // Remember: GLSL is oddly column-major so [col][row]
  for (int i=0; i<3; i++) {
    r[i] = v[0] * gl_ModelViewMatrix[0][i] +
           v[1] * gl_ModelViewMatrix[1][i] +
           v[2] * gl_ModelViewMatrix[2][i] +
                  gl_ModelViewMatrix[3][i];
  }
  return r;
}

uniform vec2 wh;  // scaled width and height of screen

out precise vec3 eye_space_pos;
out precise vec2 scaled_window_space_pos;
out precise vec3 eye_normal;

void main(void)
{
  eye_space_pos = transform(gl_Vertex.xyz);
  
  precise vec4 ndc_pos = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1);
  scaled_window_space_pos = wh*(ndc_pos.xy / ndc_pos.w);
 
  // Pass along two sets of texture coordinates...
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  
  precise mat3 nm = gl_NormalMatrix;
  precise vec3 n = gl_Normal;
  eye_normal = normalize(nm * n);
}