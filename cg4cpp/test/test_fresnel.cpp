
#include <Cg/vector/rgba.hpp>
#include <Cg/vector/stpq.hpp>
#include <Cg/vector.hpp>
#include <Cg/inout.hpp> // needed for inout-using fresnel routine
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

static void test()
{
    float3 a = normalize(float3(1,2,3));
    float3 b = normalize(float3(-2,-3,7));
    float c = 0.4f;
    float d;
    float3 e;
    float3 f;
    float3 r;
    //r = fresnel(a, b, c, d, e, f);
    r = float3(fresnel(a, b, c, d, e, f));
    OUTPUT(r);
    OUTPUT(d);
    OUTPUT(e);
    OUTPUT(f);
    //r = fresnel(a, b, c);
    r = float3(fresnel(a, b, c));
    OUTPUT(r);
    //r = fresnel(a, b, c, d);
    r = float3(fresnel(a, b, c, d));
    OUTPUT(r);
    OUTPUT(d);
    //r = fresnel(a, b, c, e, f);
    r = float3(fresnel(a, b, c, e, f));
    OUTPUT(r);
    OUTPUT(e);
    OUTPUT(f);

    float3 aa = float3(0.0f, 3.14159f, 0.2f);
    float3 cc, ss;
    sincos(aa, cc, ss);
    OUTPUT(aa);
    OUTPUT(cc);
    OUTPUT(ss);

    float4 xxx = float4(1024, 1000, 34, 0.4f);
    float4 frac, exp;
    frac = frexp(xxx, exp);
    OUTPUT(xxx);
    OUTPUT(frac);
    OUTPUT(exp);
    OUTPUT(exp2(exp));
    OUTPUT(frac * exp2(exp));

    float4 yyy = float4(3.14159f, 0.0, 1000, 56.23f);
    float4 frac2, integer2;
    frac2 = modf(yyy, integer2);
    OUTPUT(yyy);
    OUTPUT(frac2);
    OUTPUT(integer2);
    OUTPUT(frac2 + integer2);
}

void test_fresnel(void)
{
    test();
}
