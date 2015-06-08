
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/vector.hpp>
#include <Cg/floatToIntBits.hpp>
#include <Cg/floatToRawIntBits.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/inout.hpp>
#include <Cg/iostream.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << std::hex << std::showbase << (_x) << std::endl
#define OUTPUTF(_x) std::cout << #_x << "=" << (_x) << std::endl

void test_cast()
{
    float a = 0;
    float b = 0;
#ifdef _MSC_VER
# pragma warning (once : 4723) // warning C4723: potential divide by 0
#endif
    float nan = a / b;

    union { float f; int i; } otherNan;

    otherNan.f = nan;
    otherNan.i |= 1;

    float4 f4 = float4(1,2,3,4);
    float3 f3 = float3(2,3,4);
    float2 f2 = float2(2,4);
    float4 ee = float4(3,4,5,6);

    int3 i3 = floatToIntBits(int3(0,1,2));
    OUTPUT(i3);
    int3 ii3 = floatToIntBits(float3(0,1,2));
    OUTPUT(ii3);
    OUTPUT(floatToIntBits(half2(1,2)));
    OUTPUT(floatToIntBits(half2(1,2).yyx));

    OUTPUT(floatToIntBits(-1.0f));
    OUTPUT(floatToIntBits(1.0f));
    OUTPUT(floatToIntBits(-1.0));
    OUTPUT(floatToIntBits(1.0));
    OUTPUT(floatToIntBits(-1));
    OUTPUT(floatToIntBits(1));
    OUTPUT(floatToIntBits(half(1)));
    OUTPUT(floatToIntBits(fixed(1)));
    OUTPUT(floatToIntBits(f4));
    OUTPUT(floatToIntBits(f3));
    OUTPUT(floatToIntBits(f2));
    OUTPUT(floatToIntBits(ee.xxyw));
    OUTPUT(floatToIntBits(nan));
    OUTPUT(floatToIntBits(float2(nan)));
    OUTPUT(floatToIntBits(otherNan.f));
    OUTPUT(floatToRawIntBits(otherNan.f));

    OUTPUT(floatToRawIntBits(-1.0f));
    OUTPUT(floatToRawIntBits(1.0f));
    OUTPUT(floatToRawIntBits(-1.0));
    OUTPUT(floatToRawIntBits(1.0));
    OUTPUT(floatToRawIntBits(-1));
    OUTPUT(floatToRawIntBits(1));
    OUTPUT(floatToRawIntBits(half(1)));
    OUTPUT(floatToRawIntBits(fixed(1)));
    OUTPUT(floatToRawIntBits(f4));
    OUTPUT(floatToRawIntBits(f3));
    OUTPUT(floatToRawIntBits(f2));
    OUTPUT(floatToRawIntBits(ee.xxyw));
    OUTPUT(floatToRawIntBits(nan));
    OUTPUT(floatToRawIntBits(float2(nan)));
    OUTPUT(floatToRawIntBits(otherNan.f));
    
    OUTPUTF(intBitsToFloat(0x7f800000));
    OUTPUTF(intBitsToFloat(0x3f800000));
    OUTPUTF(intBitsToFloat(0x40000000));
    OUTPUTF(intBitsToFloat(~0));
#ifdef _MSC_VER
// DISABLE warning C4244: 'argument' : conversion from 'double' to 'int', possible loss of data
#pragma warning( disable : 4244 )
#endif
    OUTPUTF(intBitsToFloat(0.0));  
    OUTPUTF(intBitsToFloat(half(0.0)));
#ifdef _MSC_VER
#pragma warning( default : 4244 )
#endif
    OUTPUTF(intBitsToFloat(int4(0x3f800000, 0x1, 0xbf800000, 0)));
    OUTPUTF(intBitsToFloat(int4(0x3f800000, 0x1, 0xbf800000, 0).wxyz));
}
