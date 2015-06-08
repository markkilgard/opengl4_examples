
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/double.hpp>
#include <Cg/vector.hpp>
#include <Cg/isnan.hpp>
#include <Cg/isinf.hpp>
#include <Cg/isfinite.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/inout.hpp>
#include <Cg/iostream.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 isnan_stuff(float4 a, half4 b, In<float4> c, InOut<half4> d, Out<float2> e, In<float> f)
{
  OUTPUT(isnan(f));
  OUTPUT(isnan(a));
  OUTPUT(isnan(b.xyz));
  OUTPUT(isnan(c));
  OUTPUT(isnan(d));
  e = a.xy;
  e += a.xy;
  c = a.xyxy;
  c.zyx = c.xyz;
  c *= a.xyxy;
  d = b.xyxz;
  d.xy = b.xx;
  d.xy += b.xx;
  OUTPUT(isnan(e));
  OUTPUT(isnan(e.yx));
  return float4(isnan(a));
}

float4 isinf_stuff(float4 a, half4 b, In<float4> c, InOut<half4> d, Out<float2> e, In<float> f)
{
  OUTPUT(isinf(f));
  OUTPUT(isinf(a));
  OUTPUT(isinf(b.xyz));
  OUTPUT(isinf(c));
  OUTPUT(isinf(d));
  e = a.xy;
  e += a.xy;
  c.zyx = c.xyz;
  c = a.xyxy;
  c *= a.xyxy;
  d = b.xyxz;
  d.xy = b.xx;
  d.xy += b.xx;
  OUTPUT(isinf(e));
  OUTPUT(isinf(e.yx));
  return float4(isinf(a));
}


float4 isfinite_stuff(float4 a, half4 b, In<float4> c, InOut<half4> d, Out<float2> e, In<float> f)
{
  OUTPUT(isfinite(f));
  OUTPUT(isfinite(a));
  OUTPUT(isfinite(b.xyz));
  OUTPUT(isfinite(c));
  OUTPUT(isfinite(d));
  e = a.xy;
  OUTPUT(isfinite(e));
  OUTPUT(isfinite(e.yx));
  return float4(isfinite(a));
}

void test_isnan()
{
    float4 f4 = float4(1,2,3,4);
    half4 f4a = half4(0,1,2,3);
    float4 ee = float4(3,4,5,6);
    half4 hd = half4(-1,-2,-3,-4);

    isnan(3.4f);
    OUTPUT(isnan(3.4f));
    OUTPUT(isnan(half(3.2f)));
    OUTPUT(isnan(3.0));

    isinf(3.4f);
    OUTPUT(isinf(3.4f));
    OUTPUT(isinf(half(3.2f)));
    OUTPUT(isinf(3.0));

    isfinite(3.4f);
    OUTPUT(isfinite(3.4f));
    OUTPUT(isfinite(half(3.2f)));
    OUTPUT(isfinite(3.0));

    float4 r = isnan_stuff(f4, f4a, float4(1,2,3,4), hd, ee.xy, -5.0f);
    OUTPUT(r);
    OUTPUT(hd);
    OUTPUT(ee);
}
