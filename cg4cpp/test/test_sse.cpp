
#include <Cg/vector/rgba.hpp>
#include <Cg/vector/stpq.hpp>
#include <Cg/vector.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 sse_stuff(float4 a, float4 b)
{
  float4 d = a*b;
  float4 c = float4(1.1f,2.2f,3.3f,4.4f);
  return d + c;
}

void test_sse(void)
{
  float4 m = float4(2,3,5,6);
  float4 n = float4(-2,8,-2,7);
  float4 r = sse_stuff(m, n);
  OUTPUT(m);
  OUTPUT(n);
  OUTPUT(r);

  float2 c = float2(4,5);
  float2 foo2 = float2(-1,1);
  float2 var = lerp(c, -c, float2(foo2 < 0));
  OUTPUT(c);
  OUTPUT(foo2);
  OUTPUT(var);
}
