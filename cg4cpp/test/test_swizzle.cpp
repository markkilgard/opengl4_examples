
#include <Cg/vector/xyzw.hpp>
#include <Cg/vector/rgba.hpp>
#include <Cg/vector/stpq.hpp>
#include <Cg/half.hpp>
#include <Cg/vector.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4
homodiff(float4 a, float4 b)
{
    return a * b.w - b * a.w;
}

float4
homodiff(float3 a, float3 b)
{
    return float4(a - b, 0);
}

float4
homodiff(float4 a, float3 b)
{
    return a - float4(b * a.w, a.w);
}

float4
homodiff(float3 a, float4 b)
{
    return float4(a * b.w, b.w) - b;
}

void homodiff_test()
{
  float4 a1 = float4(1,2,3,1);
  float4 a2 = float4(2,4,6,2);
  float4 b1 = float4(-5,0,6,1);
  float4 b2 = float4(-10,0,12,2);
  float3 a = float3(1,2,3);
  float3 b = float3(-5,0,6);

  OUTPUT(homodiff(a1,b1));
  OUTPUT(normalize(homodiff(a1,b1)));
  OUTPUT(homodiff(a2,b2));
  OUTPUT(normalize(homodiff(a2,b2)));
  OUTPUT(homodiff(a1,b2));
  OUTPUT(homodiff(a2,b1));
  OUTPUT(homodiff(a,b));
  OUTPUT(homodiff(a2,b));
  OUTPUT(normalize(homodiff(a2,b)));
  OUTPUT(homodiff(a,b2));
  OUTPUT(normalize(homodiff(a,b2)));

  OUTPUT(homodiff(b1,a1));
  OUTPUT(homodiff(b2,a2));
  OUTPUT(homodiff(b,a));
}

void swizzle_stuff(void)
{
#ifdef __Cg_rgba_hpp__ // include <Cg/rgba.hpp> for .rgba swizzling
  int1 colorval1 = int1(2);
  OUTPUT(colorval1.r);
  OUTPUT(colorval1.rr);
  OUTPUT(colorval1.rrr);
  OUTPUT(colorval1.rrrr);
  float4 colorval = float4(1,2,3,4);
  OUTPUT(colorval.r);
  OUTPUT(colorval.g);
  OUTPUT(colorval.b);
  OUTPUT(colorval.a);
  OUTPUT(colorval.rg);
  OUTPUT(colorval.gb);
  OUTPUT(colorval.ba);
  OUTPUT(colorval.ar);
  OUTPUT(colorval.arar);
  OUTPUT(colorval.rgba);
#endif
#ifdef __Cg_stpq_hpp__ // include <Cg/stpq.hpp> for .stpq swizzling
  int1 texcoord1 = int1(2);
  OUTPUT(texcoord1.s);
  OUTPUT(texcoord1.ss);
  OUTPUT(texcoord1.sss);
  OUTPUT(texcoord1.ssss);
  float4 texcoord = float4(1,2,3,4);
  OUTPUT(texcoord.s);
  OUTPUT(texcoord.t);
  OUTPUT(texcoord.p);
  OUTPUT(texcoord.q);
  OUTPUT(texcoord.st);
  OUTPUT(texcoord.tp);
  OUTPUT(texcoord.pq);
  OUTPUT(texcoord.qs);
  OUTPUT(texcoord.qsqs);
  OUTPUT(texcoord.stpq);
#endif
}

void test_swizzle(void)
{
    swizzle_stuff();

    homodiff_test();

    float2 var = float2(1,2);
    float scalar = var.t;
    OUTPUT(scalar);

    half3 foo = half3(1,2,3);
    float3 bar;
    bar = foo;
    OUTPUT(foo);
    OUTPUT(bar);
}
