#version 400

#if defined(USE_DOUBLE_PRECISION)
    #define VECTOR_TYPE dvec2
    #define VECTOR3_TYPE dvec3
    #define MATRIX_TYPE dmat2x3
    #define SCALAR_TYPE double
#else
    #define VECTOR_TYPE vec2
    #define VECTOR3_TYPE vec3
    #define MATRIX_TYPE mat2x3
    #define SCALAR_TYPE float
#endif

in vec2 position;
out vec4 color;

uniform vec2 aspectRatio;
uniform SCALAR_TYPE scale;
uniform VECTOR_TYPE offset;
uniform float max_iterations;
uniform sampler1D lut;
uniform bool julia;
uniform vec2 juliaOffsets;

uniform MATRIX_TYPE matrix;

struct result {
    float iterations;
    VECTOR_TYPE z;
};

result mandel_julia(VECTOR_TYPE c, VECTOR_TYPE z)
{
    result res;
    res.iterations = 0.0;
    res.z = z;

    for (; res.iterations < max_iterations; res.iterations++) {
        SCALAR_TYPE zx2 = res.z.x * res.z.x;
        SCALAR_TYPE zy2 = res.z.y * res.z.y;

        if (zx2 + zy2 > 4.0)
            break;

        res.z = VECTOR_TYPE(zx2 - zy2, 2.0 * res.z.x * res.z.y) + c;
    }
    return res;
}

void main()
{
    VECTOR_TYPE pos;
    pos.x = dot(matrix[0],VECTOR3_TYPE(position,1));
    pos.y = dot(matrix[1],VECTOR3_TYPE(position,1));
    pos = pos * aspectRatio * scale + offset;

    result res;
    if (julia)
        res = mandel_julia(juliaOffsets, pos);
    else
        res = mandel_julia(pos, VECTOR_TYPE(0.0));

    float s = res.iterations / max_iterations;
    color = texture(lut, s);

    //float f = (res.iterations - log2(log2(length(vec2(res.z))))) / max_iterations;
    //color = vec4(0.0, f, f, 1.0);
}
