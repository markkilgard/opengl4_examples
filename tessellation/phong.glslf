
// phong.glslf

// DESCRIPTION: GLSL fragment shader for Phong-shaded lighting model.

// To off-line compile this code to assembly with Cg 3.0, use:
// cgc -profile gp5fp -oglsl phong.glslf

uniform vec3 light_pos;

void main(void)
{
  vec3 vertex_pos = gl_TexCoord[1].xyz;
  vec3 V = normalize(-vertex_pos);
  vec3 L = normalize(light_pos - vertex_pos);
  vec3 H = normalize(V+L);
  vec3 N = normalize(gl_TexCoord[0].xyz);  // renormalize after interpolation
  if (!gl_FrontFacing) {
    N *= -1;
  }
  float diffuse = max(0.0,dot(N,L));
  float specular = diffuse > 0.0 ? max(0.0,pow(dot(H,N),30)) : 0.0;
  
  const vec3 surface_color = vec3(0.9,0.1,0.1);  // reddish
  const vec3 ambient_color = 0.2 * surface_color;
  const vec3 diffuse_color = 0.8 * surface_color;
  const vec3 specular_color = 0.5*vec3(1);

  vec3 illuminance = specular*specular_color + diffuse*diffuse_color + ambient_color;
  gl_FragColor = vec4(illuminance, 1);
}
