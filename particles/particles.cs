#version 430 compatibility
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

struct pos
{
	vec4 pxyzw;	// positions
};

struct vel
{
	vec4 vxyzw;	// velocities
};

struct color
{
	vec4 crgba;	// colors
};

layout( std140, binding=4 ) buffer Pos {
	struct pos  Positions[  ];		// array of structures
};

layout( std140, binding=5 ) buffer Vel {
	struct vel  Velocities[  ];		// array of structures
};

layout( std140, binding=6 ) buffer Col {
	struct color  Colors[  ];		// array of structures
};

layout( local_size_x = 128,  local_size_y = 1, local_size_z = 1 )   in;


vec3
Bounce( vec3 vin, vec3 n )
{
	vec3 vout = reflect( vin, n );
	return vout;
}

vec3
BounceSphere( vec3 p, vec3 v, vec4 s )
{
	vec3 n = normalize( p - s.xyz );
	return Bounce( v, n );
}

bool
IsInsideSphere( vec3 p, vec4 s )
{
	float r = length( p - s.xyz );
	return  ( r < s.w );
}


void
main( )
{
	const vec3    G      = vec3( 0., -9.8, 0. );
	const float  DT      = 0.1;
	const vec4   SPHERE = vec4( -100., -800., 0.,  600. );

	uint  gid = gl_GlobalInvocationID.x;	// the .y and .z are both 1 in this case

    vec3 p  = Positions[  gid  ].pxyzw.xyz;
    vec3 v  = Velocities[ gid  ].vxyzw.xyz;

	vec3 pp = p + v*DT + .5*DT*DT*G;
	vec3 vp = v + G*DT;

	if( IsInsideSphere( pp, SPHERE ) )
	{
		vp = BounceSphere( p, v, SPHERE );
		pp = p + vp*DT + .5*DT*DT*G;
	}

    Positions[  gid  ].pxyzw     = vec4( pp, 1. );
    Velocities[ gid  ].vxyzw.xyz = vp;
}
