
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/vector.hpp>
#include <Cg/inout.hpp>
#include <Cg/abs.hpp>
//#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

static float absme(In<float3> a, In<float3> b, float c, In<float> ci, Out<float3> ao, Out<float> oc)
{
  float3 la = a;
  OUTPUT(abs(la));
  OUTPUT(abs(a));
  OUTPUT(abs(b));
  OUTPUT(abs(b.xy));
  OUTPUT(abs(c));
  abs(ci);
  OUTPUT(abs(ci));
  ao = a;
  ao = abs(a);
  OUTPUT(abs(ao));
  OUTPUT(abs(ao.xy));
  oc = abs(c);
  oc = abs(a.y);
  oc = abs(ao.z);
  return oc;
}

static half absmeh(In<half3> a, In<half3> b, half c, In<half> ci, Out<half3> ao, Out<half> oc)
{
  half3 la = a;
  OUTPUT(abs(la));
  OUTPUT(abs(a));
  OUTPUT(abs(b));
  OUTPUT(abs(b.xy));
  OUTPUT(abs(c));
  abs(ci);
  OUTPUT(abs(ci));
  ao = a;
  ao = abs(a);
  OUTPUT(abs(ao));
  OUTPUT(abs(ao.xy));
  oc = abs(c);
  //oc = abs(a.y);  // requires abs.hpp to avoid ambiguity
  oc = abs(a[1]);
  oc = abs(half(a.y));
  oc = abs(half1(a.y));
  //oc = abs(ao.z);  // requires abs.hpp to avoid ambiguity
  oc = abs(ao[2]);
  oc = abs(half(ao.z));
  oc = abs(half1(ao.z));
  return oc;
}

void test_inout_abs()
{
  const float3 a = float3(-1,-2,-3);
  float3 b = float3(-1,-2,-3);
  half3 bh = half3(-1,-2,-3);
  float c = -3;
  float3 aao;
  float cco;
  half3 aaoh;
  half ccoh;

  OUTPUT(abs(a));
  OUTPUT(abs(b));
  OUTPUT(abs(b.xxyz));
  OUTPUT(abs(c));
  float g = absme(a, b, c, c, aao, cco);
  OUTPUT(g);
  OUTPUT(aao);
  OUTPUT(cco);
  half gh = absmeh(half3(a), bh, c, half(c), aaoh, ccoh);
  OUTPUT(gh);
  OUTPUT(aaoh);
  OUTPUT(ccoh);
}
