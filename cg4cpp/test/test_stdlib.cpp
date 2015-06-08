
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/vector.hpp>
#include <Cg/length.hpp>
#include <Cg/normalize.hpp>
#include <Cg/faceforward.hpp>
#include <Cg/distance.hpp>
#include <Cg/step.hpp>
#include <Cg/fmod.hpp>
#include <Cg/max.hpp>
#include <Cg/min.hpp>
#include <Cg/cos.hpp>
#include <Cg/sin.hpp>
#include <Cg/tan.hpp>
#include <Cg/acos.hpp>
#include <Cg/asin.hpp>
#include <Cg/atan.hpp>
#include <Cg/atan2.hpp>
#include <Cg/cosh.hpp>
#include <Cg/sinh.hpp>
#include <Cg/tanh.hpp>
#include <Cg/dot.hpp>
#include <Cg/log.hpp>
#include <Cg/log2.hpp>
#include <Cg/log10.hpp>
#include <Cg/exp.hpp>
#include <Cg/exp2.hpp>
#include <Cg/sqrt.hpp>
#include <Cg/rsqrt.hpp>
#include <Cg/ceil.hpp>
#include <Cg/floor.hpp>
#include <Cg/frac.hpp>
#include <Cg/round.hpp>
#include <Cg/trunc.hpp>
#include <Cg/radians.hpp>
#include <Cg/degrees.hpp>
#include <Cg/sign.hpp>
#include <Cg/saturate.hpp>
#include <Cg/pow.hpp>
#include <Cg/iostream.hpp>
#include <Cg/inout.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 stdlib_stuff(float4 a, float4 b, In<float> c, In<half4> d, InOut<float> e)
{
    OUTPUT(length(a));
    OUTPUT(normalize(a));
    OUTPUT(length(normalize(a)));
    OUTPUT(faceforward(a,b,d));
    OUTPUT(faceforward(a.xyz,b.xyw,d.xyz));
    OUTPUT(distance(a,b));
    OUTPUT(distance(b,a));
    OUTPUT(distance(b.xyz,a.xyz));
    OUTPUT(distance(float3(0),a.xyz));
    OUTPUT(length(a.xyz));
    OUTPUT(distance(b.xyz,float3(0)));
    OUTPUT(length(b.xyz));

    OUTPUT(fmod(5.0,3.0));
    OUTPUT(fmod(half(5.0),half(3.0)));
    OUTPUT(fmod(half(5.0),3.0));
    OUTPUT(fmod(5.0,-3.0));
    OUTPUT(fmod(-5.0,3.0));
    OUTPUT(fmod(a,b+1));
    OUTPUT(fmod(a.x,b+1));
    OUTPUT(fmod(a.x,b.xy+1));
    OUTPUT(fmod(a,b.w+1));
    OUTPUT(fmod(a.xy,b.w+1));
    OUTPUT(fmod(a.xyz,b.wxz+1));
    OUTPUT(fmod(2.0f,b.wxz+1));
    OUTPUT(fmod(a.xyz,2.0f));

    OUTPUT(step(3.4, float4(2,3,4,5)));
    OUTPUT(step(3.4, float4(2,3,4,5).wzx));
    OUTPUT(step(float4(3.4f,4.1f,0.1f,5.0f), float4(2,3,4,5).wzxy));

    OUTPUT(step(5.0,3.0));
    OUTPUT(step(5.0,-3.0));
    OUTPUT(step(-5.0,3.0));
    OUTPUT(step(a,b+1));
    OUTPUT(step(a.x,b+1));
    OUTPUT(step(a.x,b.xy+1));
    OUTPUT(step(a,b.w+1));
    OUTPUT(step(a.xy,b.w+1));
    OUTPUT(step(a.xyz,b.wxz+1));
    OUTPUT(step(2.0f,b.wxz+1));
    OUTPUT(step(a.xyz,2.0f));

    OUTPUT(max(a,b));
    OUTPUT(max(a.x,b));
    OUTPUT(max(a.x,b.xy));
    OUTPUT(max(a,b.w));
    OUTPUT(max(a.xy,b.w));
    OUTPUT(max(a.xyz,b.wxz));
    OUTPUT(max(1.0f,b.wxz));
    OUTPUT(max(a.xyz,1.0f));

    OUTPUT(max(a,e));
    OUTPUT(max(e,b));

    OUTPUT(min(a,b));
    OUTPUT(min(a.x,b));
    OUTPUT(min(a,b.w));
    OUTPUT(min(a.xyz,b.wxz));
    OUTPUT(min(1.0f,b.wxz));
    OUTPUT(min(a.xyz,1.0f));

    OUTPUT(min(a,e));
    OUTPUT(min(e,b));

    OUTPUT(dot(a,b));
    OUTPUT(sin(a));
    OUTPUT(cos(a));
    OUTPUT(tan(a));
    OUTPUT(asin(a));
    OUTPUT(acos(a));
    OUTPUT(atan(a));

    OUTPUT(atan2(a,b));
    OUTPUT(atan2(a.x,b));
    OUTPUT(atan2(a.x,b.xy));
    OUTPUT(atan2(a,b.w));
    OUTPUT(atan2(a.xy,b.w));
    OUTPUT(atan2(a.xyz,b.wxz));
    OUTPUT(atan2(1.0f,b.wxz));
    OUTPUT(atan2(a.xyz,1.0f));

    OUTPUT(atan2(a,e));
    OUTPUT(atan2(e,b));

    OUTPUT(pow(a,b));
    OUTPUT(pow(a.x,b));
    OUTPUT(pow(a.x,b.xy));
    OUTPUT(pow(a,b.w));
    OUTPUT(pow(a.xy,b.w));
    OUTPUT(pow(a.xyz,b.wxz));
    OUTPUT(pow(2.0f,b.wxz));
    OUTPUT(pow(a.xyz,2.0f));
    OUTPUT(pow(a.xyz,2));
    OUTPUT(pow(2,a.xyz));
    OUTPUT(pow(a.xyz,2.0));
    OUTPUT(pow(2.0f,2.0f));
    OUTPUT(pow(2.0f,2.0));
    OUTPUT(pow(half(2.0f),half(2.0f)));

    OUTPUT(pow(a,e));
    OUTPUT(pow(e,b));

    float4 gof = float4(1,2,3,4);

    OUTPUT(gof + 3.4);      // float4 + literal-double = float4
    OUTPUT(gof[2] + 3.4f);  // float4[i] + literal-double = double

    OUTPUT(dot(a,b));
    OUTPUT(dot(a.x,b));
    OUTPUT(dot(a.x,b.xy));
    OUTPUT(dot(a,b.w));
    OUTPUT(dot(a.xy,b.w));
    OUTPUT(dot(a.xyz,b.wxz));
    OUTPUT(dot(1.0f,b.wxz));
    OUTPUT(dot(a.xyz,1.0f));

    OUTPUT(sinh(a));
    OUTPUT(cosh(a));
    OUTPUT(tanh(a));
    OUTPUT(rsqrt(a));
    OUTPUT(sqrt(a));
    OUTPUT(log(a));
    OUTPUT(log2(a));
    OUTPUT(log10(a));
    OUTPUT(exp(a));
    OUTPUT(exp2(a));

    OUTPUT(radians(a));
    OUTPUT(degrees(a));
    OUTPUT(a);
    OUTPUT(radians(degrees(a)));

    OUTPUT(floor(a * 6.3f));
    OUTPUT(ceil(a * 6.3f));
    OUTPUT(trunc(a * 6.3f));
    OUTPUT(trunc(a * -6.3f));
    OUTPUT(trunc(float4(0, -1.3f, -1.7f, 1.8f)));
    OUTPUT(round(float4(0, -1.3f, -1.7f, 1.8f)));
    OUTPUT(ceil(float4(0, -1.3f, -1.7f, 1.8f)));
    OUTPUT(floor(float4(0, -1.3f, -1.7f, 1.8f)));
    OUTPUT(frac(float4(0, -1.3f, -1.7f, 1.8f)));

    OUTPUT(sin(d));
    OUTPUT(sin(d.x));
    OUTPUT(cos(d));
    OUTPUT(tan(d));
    OUTPUT(asin(d));
    OUTPUT(acos(d));
    OUTPUT(atan(d));
    OUTPUT(sinh(d));
    OUTPUT(cosh(d));
    OUTPUT(tanh(d));

    OUTPUT(sign(-1.1f));
    OUTPUT(sign(d.x));
    OUTPUT(sign(d));
    OUTPUT(sign(d + half(2.3)));
    OUTPUT(sign(a));

    // float3(1,2,3) + 0.3 is of type double3 technically so convert to float3
    float3 axysd = float3(1,2,3) + 0.3;

    OUTPUT(saturate(-1.1f));
    OUTPUT(saturate(d.x));
    OUTPUT(saturate(d));
    OUTPUT(saturate(d + half(2.3)));
    OUTPUT(saturate(a * 0.34f));
    a + 1;
    a + 1.1f;
    0.344f + a;
    half(0.344f) + a;
    0.344 + a;
    a + 0.34;
    a * 0.34;
    OUTPUT(saturate(a * 0.34));

    return a;
}

void test_stdlib()
{
    float4 f4 = float4(1,2,3,4);
    float4 f4a = float4(0,1,2,3);
    half4 h = half4(-1,-2,-3,-5);

    OUTPUT(cos(3.4f));
    OUTPUT(sin(3.4f));
    OUTPUT(tan(3.4f));
    OUTPUT(acos(0.4f));
    OUTPUT(asin(0.4f));
    OUTPUT(atan(0.4f));
    OUTPUT(cosh(0.4f));
    OUTPUT(sinh(0.4f));
    OUTPUT(tanh(0.4f));
    OUTPUT(sqrt(0.4f));
    OUTPUT(rsqrt(0.4f));
    OUTPUT(sqrt(0.4f)*rsqrt(0.4f));

    float ee = 2.0f;

    float4 r = stdlib_stuff(f4, f4a, 3.4f, h, ee);
}
