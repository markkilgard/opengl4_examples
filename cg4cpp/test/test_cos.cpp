
#include <Cg/half.hpp>
#include <Cg/vector.hpp>
#include <Cg/cos.hpp>
#include <Cg/sqrt.hpp>
#include <Cg/iostream.hpp>
#include <Cg/inout.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 cos_stuff(float4 a, float4 b, In<float> c, In<half4> d)
{
  OUTPUT(cos(c));
  OUTPUT(cos(d));
  return sqrt(a) + sqrt(b);
}

void test_cos()
{
    float4 f4 = float4(1,2,3,4);
    float4 f4a = float4(0,1,2,3);
    half4 h = half4(-1,-2,-3,-5);

    OUTPUT(cos(3.4f));

    float4 r = cos_stuff(f4, f4a, 3.4f, h);
}
