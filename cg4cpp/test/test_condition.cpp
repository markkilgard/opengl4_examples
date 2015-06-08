
#include <Cg/vector/xyzw.hpp>
#include <Cg/double.hpp>
#include <Cg/fixed.hpp>
#include <Cg/half.hpp>
//#include <Cg/inout.hpp>
#include <Cg/vector.hpp>
//#include <Cg/matrix.hpp>
#include <Cg/iostream.hpp>

#include <Cg/dot.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl
#define MESSAGE(_x) std::cout << _x << std::endl

static float4 mysign(float4 v)
{
    return float4(v > 0) - float4(v < 0);
}

void
test_condition()
{
  float4 f4 = float4(3,2,1,0);

  if (f4.w) {
    MESSAGE("f4.w is true");
  } else {
    MESSAGE("f4.w is false");
  }
  if (!f4.w) {
    MESSAGE("!f4.w is true");
  } else {
    MESSAGE("!f4.w is false");
  }
  if (f4.w != 0) {
    MESSAGE("f4.w != 0 is true");
  } else {
    MESSAGE("f4.w != 0 is false");
  }
  if (any(f4 > 0.0f)) {
    MESSAGE("f4 > 0.0f is true");
  } else {
    MESSAGE("f4 > 0.0f is false");
  }
  if (any(f4 > 0)) {
    MESSAGE("f4 > 0 is true");
  } else {
    MESSAGE("f4 > 0 is false");
  }
  if (any(0 > f4)) {
    MESSAGE("0 > f4 is true");
  } else {
    MESSAGE("0 > f4 is false");
  }
  if (any(0.0f > f4)) {
    MESSAGE("0.0f > f4 is true");
  } else {
    MESSAGE("0.0f > f4 is false");
  }
  {
    float a = f4.w * f4.y;
    bool b1 = f4.w && f4.y;
    bool1 b2 = f4.w && f4.y;
    bool2 b2a = f4.wx && f4.y;
    bool3 b3a = f4.w && f4.yxz;
  }
  if (f4.w && f4.y) {
    MESSAGE("f4.w && f4.y is true");
  } else {
    MESSAGE("f4.w && f4.y is false");
  }
  if (f4.w || f4.y) {
    MESSAGE("f4.w || f4.y is true");
  } else {
    MESSAGE("f4.w || f4.y is false");
  }
  {
    float a = f4.w + f4.y;
    bool b1 = f4.w || f4.y;
    bool1 b2 = f4.w || f4.y;
    bool2 b2a = f4.wx || f4.y;
    bool3 b3a = f4.w || f4.yxz;
  }
}
