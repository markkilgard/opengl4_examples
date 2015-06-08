
#include <Cg/vector.hpp>
#include <Cg/inout.hpp>
#include <Cg/dot.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float dotme(In<float3> a, In<float3> b, float c, In<float> ci, Out<float3> ao, Out<float> oc)
{
  OUTPUT(dot(a,b));
  OUTPUT(dot(c,a));
  OUTPUT(dot(b,c));
  OUTPUT(dot(ci,a));
  OUTPUT(dot(b,ci));
  OUTPUT(dot(c,c));
  OUTPUT(dot(ci,ci));
  ao = float3(-1,-2,4);
  oc = dot(a,ao);
  return oc;
}

void test_inout_dot()
{
  float3 a = float3(1,2,3);
  float3 b = float3(1,2,3);
  float c = 3;
  float3 aao;
  float cco;

  OUTPUT(dot(a,b));
  float g = dotme(a, b, c, c, aao, cco);
  OUTPUT(g);
  OUTPUT(aao);
  OUTPUT(cco);
}
