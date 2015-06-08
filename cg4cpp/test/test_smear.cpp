
#include <Cg/vector/xyzw.hpp>
#include <Cg/double.hpp>
#include <Cg/fixed.hpp>
#include <Cg/half.hpp>
//#include <Cg/inout.hpp>
#include <Cg/vector.hpp>
//#include <Cg/matrix_swizzle.hpp>
//#include <Cg/matrix.hpp>
#include <Cg/iostream.hpp>

#include <Cg/dot.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

static void sign_test()
{
  float3 a = float3(1,-2,3);
  float4 c = float4(1,-2,3,-4);
  float1 b = 5;
  float d = 6;
  float e = 7;

  sign(a);
  OUTPUT(a);
  OUTPUT(sign(a));
  OUTPUT(sign(a).yyx);
  dot(a,a);
  OUTPUT(dot(a,a));
  OUTPUT(dot(a.zzy,a.xxx));
  OUTPUT(dot(a.zzy,b.xxx));
  OUTPUT(dot(a.zzy,b));
  OUTPUT(dot(a.z,b.x));
  OUTPUT(dot(b,b));
  OUTPUT(dot(a,b));
  OUTPUT(dot(b,a));
  OUTPUT(dot(b,5));
  OUTPUT(dot(a.x,a.y));
  OUTPUT(dot(a.zzy,a.y));
  OUTPUT(dot(a.xzzy,c));
  OUTPUT(dot(float1(d),float1(e)));
  OUTPUT(dot((d),float1(e)));
  OUTPUT(dot((d),(e)));

  fixed fixa;
  fixa = 1;
  int inta;
  inta = int(fixa);
}

void
test_smear()
{
    sign_test();

    float4 bigvals = float4(2.0f, 3.0f, 0.8f, 2.0f);
    half4 bigvalsh = half4(2.0f, 3.0f, 0.8f, 2.0f);
    float4 smallvals = float4(-2.0f, -3.0f, -0.8f, 0.0f);

    float4 factors = float4(0.3f, 0.8f, 1.0f, 0.0f);

#if 1
    float4 g81 = lerp(bigvals, smallvals, float1(0.5));  // fix me
    float4 g81a = lerp(bigvals, smallvals, float1(0.75));

    float4 g81z = lerp(bigvals, smallvals, float4(0.0f));
    float4 g81o = lerp(bigvals, smallvals, float4(1));
    float4 g82 = lerp(bigvals, smallvals, factors);
#endif
    float3 g83 = lerp(bigvals.xxy, smallvals.yzw, factors.wzy);
    // smoothstep
    factors = float4(0.3f, 0.8f, 1.0f, 0.0f);
    g81 = smoothstep(bigvals, smallvals, 2);
    //g81 = smoothstep(bigvalsh, smallvals, 2);  // no function could convert all the argument types (smallvalues is float4)
    g81 = smoothstep(float4(bigvalsh), smallvals, 2);
    g81 = smoothstep(bigvalsh, half4(smallvals), half4(2));
    g81 = smoothstep(bigvals.xxyz, smallvals.wwzy, 2);
    g81 = smoothstep(bigvals, smallvals, factors.x);
    g81 = smoothstep(bigvals, smallvals, 0.5f);
    g81 = smoothstep(bigvals, smallvals, 0.5);  // PROBLEM!
    g81a = smoothstep(bigvals, smallvals, 0.75f);  // 0.75 causes problem
    g81z = smoothstep(bigvals, smallvals, 0.0f);  // 0 causes problem
    g81o = smoothstep(bigvals, smallvals, 1.0f);  // 1 causes problem
    g82 = smoothstep(bigvals, smallvals, factors);
    g83 = smoothstep(bigvals.xxy, smallvals.yzw, factors.wzy);
    OUTPUT(factors);
    OUTPUT(g81);
    OUTPUT(g81a);
    OUTPUT(g81z);
    OUTPUT(g81o);
    OUTPUT(g82);
    OUTPUT(g83);
    g83 = smoothstep(bigvals.xxy, smallvals.yzw, factors.w);
    //g83 = smoothstep(float1(bigvals.x), float1(smallvals.y), float1(factors.w)); // g++ 4.0 hard
    g83 = float3(smoothstep(float1(bigvals.x), float1(smallvals.y), float1(factors.w)));
    float me = smoothstep(float1(bigvals.x), float1(smallvals.y), float1(factors.w));
    float me4 = smoothstep(float1(bigvals.x), (smallvals.y), (factors.w));
    float me2 = smoothstep(0.5f, float1(smallvals.y), float1(factors.w));
    float me3 = smallvals.y;
    float1 tifa = bigvals.x;
    //g83 = smoothstep(tifa, tifa, tifa);  // g++ 4.0 hard
    g83 = float3(smoothstep(tifa, tifa, tifa));
    // hard case when all parameters are 1-component swizzles
    //g83 = smoothstep(tifa, smallvals.y, factors.w);  // g++ 4.0 smear hard
    g83 = float3(smoothstep(tifa, smallvals.y, factors.w));
    //g83 = smoothstep(bigvals.x, smallvals.y, factors.w);  // hard
    //g83 = smoothstep(float1(bigvals.x), smallvals.y, factors.w);  // g++ 4.0 smear hard
    g83 = float3(smoothstep(float1(bigvals.x), smallvals.y, factors.w));
    //g83 = smoothstep(float(bigvals.x), smallvals.y, factors.w); // hard
    float2 g83_2 = smoothstep(bigvals.x, smallvals.y, factors.zw);
}
