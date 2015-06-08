
#include <Cg/double.hpp>
#include <Cg/vector/xyzw.hpp>
#include <Cg/matrix/columns.hpp>
#include <Cg/matrix/1based.hpp>
#include <Cg/matrix.hpp>
#include <Cg/iostream.hpp>
#include <Cg/inverse.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4x4 ident4()
{
    float4x4 r = 0;

    for (int i=0; i<4; i++) {
        r[i][i] = 1.0f;
    }
    return r;
}

float4x4 translate(float3 xyz)
{
    float4x4 r = ident4();

    r._14_24_34 = xyz;
    return r;
}

void
test_inverse()
{
    float4x4 i4 = ident4();
    float4x4 z = float4x4(-3,0.2f,4,1,-0.4f,5,9,-2,1,-1,0,8.2f,0,0,-1,1);
    float4x4 inv_z = inverse(z);
    float4x4 ii4 = mul(z,inv_z);
    OUTPUT(ii4);

    ii4 = mul(inv_z,z);
    OUTPUT(ii4);
    ii4 = transpose(inverse(z));
    OUTPUT(ii4);

    OUTPUT(z);
    OUTPUT(inverse(z));
    OUTPUT(mul(z,inverse(z)));
    OUTPUT(mul(inverse(z),z));

    OUTPUT(i4);
    float4x4 inv_i4 = inverse(i4);
    OUTPUT(inv_i4);

    float4x4 a = translate(float3(-2,4,0.5));
    OUTPUT(a);
    OUTPUT(inverse(a));
    OUTPUT(a*inverse(a));
    OUTPUT(inverse(a)*a);
}