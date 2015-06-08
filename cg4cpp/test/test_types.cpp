
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/double.hpp>
#include <Cg/vector.hpp>
#include <Cg/cos.hpp>
#include <Cg/sqrt.hpp>
#include <Cg/isnan.hpp>
#include <Cg/isinf.hpp>
#include <Cg/sqrt.hpp>
#include <Cg/iostream.hpp>
#include <Cg/inout.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

int exact = 0;
int misses = 0;
int infcount = 0;
int nancount = 0;

void half_rcp(float v, int output)
{
    if (output) printf("v = %f\n", v);
    float rcp_v = 1.0f/v;
    if (output) printf("rcp_v = %f\n", rcp_v);
    half hv = half(v);
    if (output) std::cout << "hv = " << hv << std::endl;
    half rcp_hv = half(1.0)/hv;
    if (output) std::cout << "rcp_hv = " << rcp_hv << std::endl;

    if (isnan(v)) {
        nancount++;
        return;
    }
    if (isnan(rcp_hv)) {
        nancount++;
        return;
    }
    if (isinf(rcp_hv)) {
        infcount++;
        return;
    }
#if 1
    if (isinf(v)) {
        infcount++;
        return;
    }
#endif

    unsigned short hbits = *reinterpret_cast<unsigned short*>(&rcp_hv);
    hbits &= 0xff00;
    if (output) printf("hbits = 0x%x\n", hbits);
    half x0 = *reinterpret_cast<half*>(&hbits);

#if 1
    x0 = 2.0;  // seed with the constant 1/0.5, that is 2.0
#endif

#if 0
    if (output) std::cout << "x0 = " << x0 << " err=" << rcp_hv-x0 << std::endl;
    half x1 = x0*(2-hv*x0);
    if (output) std::cout << "x1 = " << x1 << " err=" << rcp_hv-x1 << std::endl;
    half x2 = x1*(2-hv*x1);
    if (output) std::cout << "x2 = " << x2 << " err=" << rcp_hv-x2 << std::endl;
    half x3 = x2*(2-hv*x2);
    if (output) std::cout << "x3 = " << x3 << " err=" << rcp_hv-x3 << std::endl;
#else
    half xarray[5+1];
    xarray[0] = x0;
    for (int i=0; i<5; i++) {
        if (output) std::cout << "x0 = " << x0 << " err=" << rcp_hv-x0 << std::endl;
        x0 = x0*(2-hv*x0);
        xarray[i+1] = x0;
    }
    half x3 = x0;
#endif
    if (rcp_hv == x3) {
        exact++;
        if (output) printf("%s\n", rcp_hv == x3 ? "EXACT" : "miss");
    } else {
        unsigned short ahbits = *reinterpret_cast<unsigned short*>(&x3);
        float rel_err = (float(x3)-float(rcp_hv))/float(rcp_hv);
        if (output) printf("miss = %f, rel_err=%f\n", rcp_v, float(rel_err));
        if (output) printf("exact=0x%x, approx=0x%x, bitdiff=%d\n", hbits, ahbits, hbits-ahbits);
        misses++;
    }
    printf("\n");
}

void test_half_rcp()
{
#if 1
  half_rcp(0, 1);
  half_rcp(0.7, 0);
  half_rcp(56.7, 0);
  half_rcp(1000.7, 0);
  half_rcp(0.103, 0);
  half_rcp(50000, 0);
  half_rcp(0.000323, 0);
#endif

  for (unsigned int i=0; i<(1<<16); i++) {
    half x = *reinterpret_cast<half*>(&i);
    half_rcp(x, 0);
  }

  printf("exact = %d\n", exact);
  printf("misses = %d\n", misses);
  printf("inf = %d\n", infcount);
  printf("nan = %d\n", nancount);
  printf("total = %d\n", exact+misses);
  printf("percent = %f\n", float(exact)/(exact+misses));
}

void test_types()
{
    test_half_rcp();

    float4 f4 = float4(1,2,3,4);
    float4 f4a = float4(0,1,2,3);
    half4 h4 = half4(-1,-2,-3,-5);
    double4 d4 = double4(-6,-7,8,9);

    float3 f3 = float3(55,66,77);

    OUTPUT(f3);

    OUTPUT(f4);
    OUTPUT(f4a);
    OUTPUT(h4);
    OUTPUT(d4);

    OUTPUT(double4(f3, 5));
    OUTPUT(double4(f3.xxz, 5));
    OUTPUT(double3(f4.xy, 7));
    OUTPUT(double3(7, f4.xy));
    OUTPUT(double4(double(d4.x), f3));
    OUTPUT(double4(double(d4.x), f3.xyz));
    OUTPUT(double4(double(d4.x), f4.xyz));
    OUTPUT(double4(float(f4.x), f4.xyz));
    OUTPUT(double4(double(f4.x), f4.xyz));
    OUTPUT(double4(f4.x, f4.xyz));

    OUTPUT(double4(d4.x, f4.xyz));
    
    OUTPUT(double4(f4.xx, f4.yy));
    OUTPUT(double4(d4.xx, h4.zz));
    OUTPUT(double4(d4.x, h4.xyz));
    OUTPUT(double4(9, h4.xy, 9));
    OUTPUT(double4(9, h4.xyz));
    OUTPUT(double4(h4.xyz, 9));
    OUTPUT(double4(h4.xx, f4.ww));

    OUTPUT(double3(f4.xy, 7));
    OUTPUT(double3(7, f4.xy));

    OUTPUT(double4(-3,-2,f4.zz));
    OUTPUT(double4(f4.yy,-1,-9));
    OUTPUT(double4(5,f4.yy,7));

    OUTPUT(half4(f3, 5));
    OUTPUT(half4(f3.xxz, 5));
    OUTPUT(half3(f4.xy, 7));
    OUTPUT(half3(7, f4.xy));
    OUTPUT(half4(double(d4.x), f3));
    OUTPUT(half4(double(d4.x), f3.xyz));
    OUTPUT(half4(double(d4.x), f4.xyz));
    OUTPUT(half4(float(f4.x), f4.xyz));
    OUTPUT(half4(double(f4.x), f4.xyz));
    OUTPUT(half4(f4.x, f4.xyz));

    OUTPUT(half4(d4.x, f4.xyz));
    
    OUTPUT(half4(f4.xx, f4.yy));
    OUTPUT(half4(d4.xx, h4.zz));
    OUTPUT(half4(d4.x, h4.xyz));
    OUTPUT(half4(9, h4.xy, 9));
    OUTPUT(half4(9, h4.xyz));
    OUTPUT(half4(h4.xyz, 9));
    OUTPUT(half4(h4.xx, f4.ww));

    OUTPUT(half3(f4.xy, 7));
    OUTPUT(half3(7, f4.xy));

    OUTPUT(half4(-3,-2,f4.zz));
    OUTPUT(half4(f4.yy,-1,-9));
    OUTPUT(half4(5,f4.yy,7));

    OUTPUT(half2(f4.x, d4.z));
    OUTPUT(int2(f4.x, d4.z));

    OUTPUT(double1(int(3)));
    OUTPUT(double2(int(3), 4.0f));
    OUTPUT(double3(int(3), short(2), 4.0f));
    OUTPUT(double4(int(3), short(2), char(7), 4.0f));

    OUTPUT(half1(int(3)));
    OUTPUT(half2(int(3), 4.0f));
    OUTPUT(half3(int(3), short(2), 4.0f));
    OUTPUT(half4(int(3), short(2), char(7), 4.0f));

# pragma warning (disable : 4305) // warning C4305: 'argument' : truncation from 'int' to 'const bool'
    OUTPUT(bool1(int(3)));
    OUTPUT(bool2(int(3), 4.0f));
    OUTPUT(bool3(int(3), short(2), 4.0f));
    OUTPUT(bool4(int(3), short(2), char(7), 4.0f));
}
