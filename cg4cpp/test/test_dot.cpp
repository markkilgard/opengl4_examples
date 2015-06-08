
#include <Cg/half.hpp>
#include <Cg/vector/xyzw.hpp>
#include <Cg/dot.hpp>
#include <Cg/vector.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 dot_stuff(float4 a, float4 b, int3 c, int3 d)
{
  float rd = dot(a,b);
  int ri = dot(c,d);
  return float4(rd,float(ri),rd,float(ri));
}

struct myfloat {
  myfloat(float _v) : v(_v) { }
  float v;
};

void test_dot()
{
    float4 f4 = float4(1,2,3,4);
    float4 f4a = float4(0,1,2,3);
    int3 i3 = int3(1,2,3);
    int3 i3a = int3(-1,0,1);
    int1 i1 = int(9);
    int2 i2 = int2(7,8);

    half hh0 = 1.0;
    half hh = 3.0;
    half hh1 = 2.0;
    half hh2 = 6.0;
    half hh6 = -6.0;
    half hh7 = -12.0;
    half hh3 = 12.0;
    half4 vhh3 = half4(0.0, 1.1, 12.23, 12.0);
    half inf = 400000000000.0;

    f4 += f4.yyxz;

    half x = 1.0, y = 1.0;
    for (int i=0; i<27; i++) {
      x /= 2;
      y *= 2;
      OUTPUT(x);
      OUTPUT(y);
    }

    half a = x * y;
    float af = x * y;
    float inff = 1.0f / x;
    OUTPUT(a);
    OUTPUT(af);
    OUTPUT(inff);

    myfloat ggg(4.5);

    dot_stuff(f4, f4a, i3, i3a);
}
