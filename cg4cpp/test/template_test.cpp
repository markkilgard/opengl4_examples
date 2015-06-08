
#define NOMINMAX // define to Win32's windef.h doesn't define min and max macros

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Cg/vector/xyzw.hpp>
#include <Cg/double.hpp>
#include <Cg/fixed.hpp>
#include <Cg/half.hpp>
#include <Cg/inout.hpp>
#include <Cg/vector.hpp>
//#include <Cg/matrix_swizzle.hpp>
//#include <Cg/matrix.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

#include "glprocs.h"

using namespace Cg;

using std::cout;
using std::endl;

//#include <cmath>

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

float4 a4, b4, c4;
float3 a3, b3, c3;

extern "C" {
const char *programName = "cg4cpp test";
}

void sign_test()
{
  float3 a = float3(1,-2,3);
  float4 c = float4(1,-2,3,-4);
  float1 b = 5;

  sign(a);
  OUTPUT(a);
  OUTPUT(sign(a));
  OUTPUT(sign(a).yyx);
  dot(a,a);
  OUTPUT(dot(a,a));
  OUTPUT(dot(a.zzy,a.xxx));
  OUTPUT(dot(a.zzy,b.xxx));
  OUTPUT(dot(a.zzy,b));
  OUTPUT(dot(a.zzy,a.y));
  OUTPUT(dot(a.xzzy,c));
}

int
main(int argc, char **argv)
{
  float1 y67;

  // Create a GLUT window so we'll have an OpenGL context to create textures to initialize samplers from
  glutInit(&argc, argv);
  glutCreateWindow("cg4cpp test");

  init_glprocs();

  float3 a;
  float3 c(2,3,4);
  float3 b = float3(0,2,3);

  float4 d23 = c.xxyy + b.xyzz;

  cout << "b = " << b.xyzz << endl;
  cout << "!b = " << !b << endl;
  cout << "!b = " << !b.xy << endl;
  cout << "!b = " << !b.xxy << endl;
  cout << "!b = " << !b.xxxy << endl;
  cout << "d23 = " << d23 << endl;

  c += c;
  cout << "c = " << c << endl;
  c += c.yyx;
  cout << "c = " << c << endl;
  c.zx++;
  cout << "c = " << c << endl;

  float4 w1 = float4(1,2,3,4);
  float4 w2 = float4(-2,4,1,-2);

  float4 ww = w1 * w2;
  float4 wws = w1.xyzz * w2;
  float4 wwt = w1 * w2.xxyy;
  float4 wa = w1 * 2;
  float4 waa = 2 * w1;
  float3 wb = w1.xyz*w2.w;
  float3 wbb = w2.w*w1.xyz;

  wbb += wbb;
  wbb *= ww.xyz;
  float3 wbbc;
  wbbc = wbb;
  cout << "wbbc = " << wbbc << endl;


#if defined(__Cg_rgba_hpp__) && defined(__Cg_stpq_hpp__)
  cout << "texcoord.stpq * colorval.rgba * colorval.xyzw = " << texcoord.stpq * colorval.rgba * colorval.xyzw << endl;
#endif

  float4 r4 = float4(1,-2,3,5);
  r4.xz *= 2;
  cout << "r4 = " << r4 << endl;
  wbbc = wbb.xxy;
  cout << "wbbc = " << wbbc << endl;

  float2 swap2 = float2(4,5);
  cout << "swap2 = " << swap2 << endl;
  swap2.yx = swap2.xy;
  cout << "swap2 = " << swap2 << endl;
  cout << "swap2[0] = " << swap2[0] << endl;
  cout << "swap2[1] = " << swap2[1] << endl;
  cout << "swap2.yx[0] = " << swap2.yx[0] << endl;
  cout << "swap2.xyx[2] = " << swap2.xyx[2] << endl;
  cout << "swap2.xyx = " << swap2.xyx << endl;

  cout << "swap2 * 2 = " << swap2*2 << endl;

  wbbc = 7.0f;

  cout << "wbbc = " << wbbc << endl;
  cout << "wbb = " << wbb << endl;
  wbb = wbb.zzx;
  wbb.xz = wbb.zx;
  cout << "wbb = " << wbb << endl;

  float1 y1;

  y1 = -2.3f;
  float2 y4 = float2(-2.f,-3.f);
  cout << "y4=" << y4 << endl;
  cout << "y4.xyyx=" << y4.xyyx << endl;
  // y4.xyyx.yz is illegal but using float4().yz can make this work
  cout << "float4(y4.xyyx).yz=" << float4(y4.xyyx).yz << endl;
  float1 y22222222 = float1(-2.f);

  float4 dif = float4(w1.xyz*w2.w - w2.xyz*w1.w, 0);
  cout << "dif = " << dif << endl;

  float2 ff = float2(2,3);
  ff *= 2;
  cout << "ff = " << ff << endl;

      //float4 g = { 2, 4, 5, 9 };  // legal in Cg, not in C++ with cgvector.h

    float3 d = float3(9,8,7);
    int3 id = int3(1,2,1);
    cout << "d = " << d << endl;
    d *= float3(2,3,7);
    // cgVectorSwizzle<float,3,0x0002)
    d.zx *= float2(2,3);
    cout << "d = " << d << endl;

    cout << "d = " << d++ << endl;
    d.zx++;
    cout << "d = " << d << endl;
    d.z--;
    cout << "d = " << d << endl;
    ++d.y;
    cout << "d = " << d << endl;
    --d.yx;
    cout << "d = " << d << endl;
    d.xz = 23;
    cout << "d = " << d << endl;
    d = !d;
    cout << "d = " << d << endl;
    d = !d;
    cout << "d = " << d << endl;
    id = id << id;
    cout << "id = " << id << endl;

    int4 idd = int4(1,2,1,2);
    float4 bbb = float4(idd);
    cout << "idd = " << idd << endl;
    cout << "bbb = " << bbb << endl;

    float3 f3(0,1,2);
    cout << "code:    float3 f3(0,1,2);" << endl;
    cout << "value:   f3 = " <<  f3 << endl;
    cout << "value:   f3.zz = " <<  f3.zz << endl;
    cout << "value:   f3.xz = " <<  f3.xz << endl;
    float3 f3a = float3(6,7,8);
    float3 g;
    cout << "f3a = " << f3a << endl;
    g.xy = f3a.yz;
    g.z = 99;
    cout << "g = " << g << endl;
    f3a = f3;
    cout << "code:    float3 f3a;" << endl;
    cout << "code:    f3a = f3;" << endl;
    cout << "value:   f3a = " <<  f3a << endl;
    float2 f2;
    f2 = f3.xz;
    cout << "code:    float2 f2 = f3.xz;" << endl;
    cout << "value:   f2 = " <<  f2 << endl;
    cout << "value:   f2.yx = " <<  f2.yx << endl;
    cout << "code:    f2.x = 10; " << endl;
    f2.x = 10.f;
    cout << "value:   f2 = " <<  f2 << endl;
    f2 = f2.yx;
    cout << "code:    f2 = f2.yx;" << endl;
    cout << "value:   f2 = " <<  f2 << endl;
    f3.xz = f2;
    cout << "code:    f3.xz = f2;" << endl;
    cout << "value:   f3 = " <<  f3 << endl;
    cout << "value:   sizeof(f3) == " << sizeof(f3) << endl;
    cout << "value:   sizeof(float1) == " << sizeof(float1) << endl;
    cout << "value:   sizeof(float2) == " << sizeof(float2) << endl;
    cout << "value:   sizeof(float3) == " << sizeof(float3) << endl;
    cout << "value:   sizeof(float4) == " << sizeof(float4) << endl;

    float4 f4(100,2,3,6);
    cout << "value:   f4 = " <<  f4 << endl;
    f4.xzw *= f4.zxx;
    cout << "value:   f4 = " <<  f4 << endl;

    f4 = float4(1,2,3,4);
    cout << "value:   f4 = " <<  f4 << endl;
    f4.xyzw = f4.wzyx;
    cout << "value:   f4 = " <<  f4 << endl;

    f4 = float4(1,2,3,4);
    cout << "value:   f4 = " <<  f4 << endl;
    f4 = -f4.wzyx;
    f4 = f4 * 4.0f;
    cout << "value:   f4 = " <<  f4 << endl;
    f4 = 4.0f * f4.xyyx;
    cout << "value:   f4 = " <<  f4 << endl;
    f4 *= 0.25f;
    cout << "value:   f4 = " <<  f4 << endl;
    f4 *= 2;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.xw *= 2;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.z = f4.x * 4.0f;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.yw = f4.xy * 4;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.yw = f4.xy + 4;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.yw = 4 + f4.xy;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.yw = f4.xy - 4;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.yw = 4 - f4.xy;
    cout << "value:   f4 = " <<  f4 << endl;
    f4.x = 2;
    float1 tot = 4;
    float1 tot9 = 8;
    tot += tot9;
    tot -= 1;
    tot += 2;
    float1 tot2 = 4.0;
    tot2 = 2.0;
    tot2.x = 3.0;
    float ftot = tot;
    int itot = (int) tot;

    float4 list[7];



    int3 idat = int3(1,2,3);
    //float3 fdat2 = idat;  // generates warning
    float3 fdat2 = float3(idat);

    float2 swap22 = float2(3,8);

    cout << "swap22 = " << swap22 << endl;
    swap22 = swap22.yx;
    cout << "swap22 = " << swap22 << endl;
    swap22.yx = swap22;
    cout << "swap22 = " << swap22 << endl;

  float4 ere = float4(1,2,3,4);

    cout << "ere = " << ere << endl;
  //ere.xy = ere.zx.yx;   // works in Cg; can we make this work in C++?
  ere.xy = float2(ere.zx).yx;   // works in Cg; can we make this work in C++?
    cout << "ere = " << ere << endl;

  ere = float4(1,2,3,4);
  // ere.xzw.y = ere.y;  // works in Cg; not in C++
  ere[0] = 3.4f;
  cout << "ere = " << ere << endl;
  ere[0] = ere[1];
  cout << "ere = " << ere << endl;
  ere.x = ere[1];
  cout << "ere = " << ere << endl;
  float2 varza = float2(1,2);
  float2 scalara = float2(varza); 
  cout << "scalara = " << scalara << endl;
  float2 scalara2 = float2(varza.xx); 
  cout << "scalara2 = " << scalara2 << endl;
  float2 varz = float2(19,72);
  float scalar = float1(varz.y); 
  cout << "scalar = " << scalar << endl;
  float2 varz2 = float2(1,2);
  float scalar2 = float1(varz2[1]); 
  cout << "scalar2 = " << scalar2 << endl;
  //ere[0] = ere.y;  // works in Cg; not in C++
  ere[0] = float1(ere.y);  // works in BOTH

  float4 z14 = float4(1,2,3,4);
  cout << "z14=" << z14 << endl;
  float1 z15 = 5;
  cout << "z15=" << z15 << endl;
  float4 z16 = z14 * z15;
  cout << "z16=" << z16 << endl;
  z16 = z14.xyzz * z15.x;
  cout << "z16=" << z16 << endl;
  cout << "z15.x * z14.xyzz=" << z15.x * z14.xyzz << endl;
  cout << "z15.x * z14.wwz=" << z15.x * z14.wwz << endl;
  cout << "z15.xxx * z14.wwz=" << z15.xxx * z14.wwz << endl;
  cout << "2 * z14.wwz=" << 2 * z14.wwz << endl;

  {
  float4 z14 = float4(1,2,3,4);
  cout << "z14=" << z14 << endl;
  float1 z15 = 5;
  cout << "z15=" << z15 << endl;
  float4 z16 = z14 + z15;
  cout << "z16=" << z16 << endl;
  z16 = z14.xyzz + z15.x;
  cout << "z16=" << z16 << endl;
  cout << "z15 + z14.xyzz=" << z15 + z14.xyzz << endl;
  cout << "z15.x + z14.xyzz=" << z15.x + z14.xyzz << endl;
  cout << "z15.x + z14.wwz=" << z15.x + z14.wwz << endl;
  cout << "z15.xxx + z14.wwz=" << z15.xxx + z14.wwz << endl;
  cout << "2 + z14.wwz=" << 2 + z14.wwz << endl;
  }

  float2 z17 = float2(7,8);
  cout << "z17=" << z17 << endl;
  //float4 z18 = z14 * z17;  // ILLEGAL in Cg

  float2 foo = float2(3.1f,2.7f);
  foo.y[0];  // NOT allowed by Cg but allowed by cg4cpp

  {
      int2 a = int2(4,126);
      int2 b = int2(39,63);
      int1 c = int1(255);

      cout << "a+b=" << a+b << endl;
      cout << "a-b=" << a-b << endl;
      cout << "a*b=" << a*b << endl;
      cout << "a/b=" << a/b << endl;
      cout << "a^b=" << (a^b) << endl;
      cout << "a%b=" << (a%b) << endl;
      cout << "a|b=" << (a|b) << endl;
      cout << "a&b=" << (a&b) << endl;
      cout << "a||b=" << (a||b) << endl;
      cout << "a&&b=" << (a&&b) << endl;
      cout << "a==b=" << (a==b) << endl;
      cout << "a!=b=" << (a!=b) << endl;
      cout << "a<=b=" << (a<=b) << endl;
      cout << "a>=b=" << (a>=b) << endl;
      cout << "a<<b=" << (a<<b) << endl;
      cout << "a>>b=" << (a>>b) << endl;

      cout << "a+c=" << a+c << endl;
      cout << "a-c=" << a-c << endl;
      cout << "a*c=" << a*c << endl;
      cout << "a/c=" << a/c << endl;
      cout << "a^c=" << (a^c) << endl;
      cout << "a%c=" << (a%c) << endl;
      cout << "a|c=" << (a|c) << endl;
      cout << "a&c=" << (a&c) << endl;
      cout << "a||c=" << (a||c) << endl;
      cout << "a&&c=" << (a&&c) << endl;
      cout << "a==c=" << (a==c) << endl;
      cout << "a!=c=" << (a!=c) << endl;
      cout << "a<=c=" << (a<=c) << endl;
      cout << "a>=c=" << (a>=c) << endl;
      cout << "a<<c=" << (a<<c) << endl;
      cout << "a>>c=" << (a>>c) << endl;

      cout << "c+b=" << c+b << endl;
      cout << "c-b=" << c-b << endl;
      cout << "c*b=" << c*b << endl;
      cout << "c/b=" << c/b << endl;
      cout << "c^b=" << (c^b) << endl;
      cout << "c%b=" << (c%b) << endl;
      cout << "c|b=" << (c|b) << endl;
      cout << "c&b=" << (c&b) << endl;
      cout << "c||b=" << (c||b) << endl;
      cout << "c&&b=" << (c&&b) << endl;
      cout << "c==b=" << (c==b) << endl;
      cout << "c!=b=" << (c!=b) << endl;
      cout << "c<=b=" << (c<=b) << endl;
      cout << "c>=b=" << (c>=b) << endl;
      cout << "c<<b=" << (c<<b) << endl;
      cout << "c>>b=" << (c>>b) << endl;

      cout << "2+b=" << 2+b << endl;
      cout << "2-b=" << 2-b << endl;
      cout << "2*b=" << 2*b << endl;
      cout << "2/b=" << 2/b << endl;
      cout << "2^b=" << (2^b) << endl;
      cout << "2%b=" << (2%b) << endl;
      cout << "2|b=" << (2|b) << endl;
      cout << "2&b=" << (2&b) << endl;
      cout << "1||b=" << (2||b) << endl;
      cout << "1&&b=" << (2&&b) << endl;
      cout << "2==b=" << (2==b) << endl;
      cout << "2!=b=" << (2!=b) << endl;
      cout << "2<=b=" << (2<=b) << endl;
      cout << "2>=b=" << (2>=b) << endl;
      cout << "2<<b=" << (2<<b) << endl;
      cout << "2>>b=" << (2>>b) << endl;

      cout << "a+2=" << a+2 << endl;
      cout << "a-2=" << a-2 << endl;
      cout << "a*2=" << a*2 << endl;
      cout << "a/2=" << a/2 << endl;
      cout << "a^2=" << (a^2) << endl;
      cout << "a%2=" << (a%2) << endl;
      cout << "a|2=" << (a|2) << endl;
      cout << "a&2=" << (a&2) << endl;
      cout << "a||1=" << (a||2) << endl;
      cout << "a&&1=" << (a&&2) << endl;
      cout << "a==2=" << (a==2) << endl;
      cout << "a!=2=" << (a!=2) << endl;
      cout << "a<=2=" << (a<=2) << endl;
      cout << "a>=2=" << (a>=2) << endl;
      cout << "a<<2=" << (a<<2) << endl;
      cout << "a>>2=" << (a>>2) << endl;

      cout << "2.0f+b=" << 2.0f+b << endl;
      cout << "2.0f-b=" << 2.0f-b << endl;
      cout << "2.0f*b=" << 2.0f*b << endl;
      cout << "2.0f/b=" << 2.0f/b << endl;
      cout << "1.0f||b=" << (2.0f||b) << endl;
      cout << "1.0f&&b=" << (2.0f&&b) << endl;
      cout << "2.0f==b=" << (2.0f==b) << endl;
      cout << "2.0f!=b=" << (2.0f!=b) << endl;
      cout << "2.0f<=b=" << (2.0f<=b) << endl;
      cout << "2.0f>=b=" << (2.0f>=b) << endl;

      cout << "a+2.0f=" << a+2.0f << endl;
      cout << "a-2.0f=" << a-2.0f << endl;
      cout << "a*2.0f=" << a*2.0f << endl;
      cout << "a/2.0f=" << a/2.0f << endl;
      cout << "a||1.0f=" << (a||2.0f) << endl;
      cout << "a&&1.0f=" << (a&&2.0f) << endl;
      cout << "a==2.0f=" << (a==2.0f) << endl;
      cout << "a!=2.0f=" << (a!=2.0f) << endl;
      cout << "a<=2.0f=" << (a<=2.0f) << endl;
      cout << "a>=2.0f=" << (a>=2.0f) << endl;
  }

  {
      int1 a = int1(4);
      int1 b = int1(39);

      cout << "a+b=" << a+b << endl;
      cout << "a-b=" << a-b << endl;
      cout << "a*b=" << a*b << endl;
      cout << "a/b=" << a/b << endl;
      cout << "a^b=" << (a^b) << endl;
      cout << "a%b=" << (a%b) << endl;
      cout << "a|b=" << (a|b) << endl;
      cout << "a&b=" << (a&b) << endl;
      cout << "a||b=" << (a||b) << endl;
      cout << "a&&b=" << (a&&b) << endl;
      cout << "a==b=" << (a==b) << endl;
      cout << "a!=b=" << (a!=b) << endl;
      cout << "a<=b=" << (a<=b) << endl;
      cout << "a>=b=" << (a>=b) << endl;
      cout << "a<<b=" << (a<<b) << endl;
      cout << "a>>b=" << (a>>b) << endl;
  }

    //float3 qw = { 1, 2, 3 };  // Works in Cg but in C++, types with user defined constructors are not aggregate
    a3 = float3(1,2,3);
    b3 = float3(-3,4,5);
    c3 = a3 * b3;
    cout << "a3 * b3 = " << c3 << endl;
    c3 = a3.zxy * b3;
#ifdef __Cg_half_hpp__
    half3 hb3 = half3(-3,4,5);
    cout << "hb3 = " << hb3 << endl;
    c3 = a3.zxy * hb3;
#endif

#ifdef __Cg_fixed_hpp__
    fixed fx(0.7);
    cout << "fx = " << fx << endl;
    fixed fx2 = 1.7;
    cout << "fx2 = " << fx2 << endl;
    fixed fx3 = -0.3;
    cout << "fx3 = " << fx3 << endl;
    fixed fx4 = -2.0;
    cout << "fx4 = " << fx4 << endl;
    fixed fx5 = -3.1;
    cout << "fx5 = " << fx5 << endl;
    fixed fx6 = 5.2;
    cout << "fx6 = " << fx6 << endl;

    fixed4 fxv4 = fixed4(-2.3f, 3.5f, 1.0f, 0.4f);
    cout << "fxv4 = " << fxv4 << endl;
    cout << "fxv4.wwzx = " << fxv4.wwzx << endl;
    cout << "-fxv4 = " << -fxv4 << endl;
    cout << "-fxv4.wwzx = " << -fxv4.wwzx << endl;
    fixed3 fxv3 = fixed3(-0.3f, 0.1f, 0.4f);
    fixed3 nfxv3 = normalize(fxv3.zyx);
    cout << "normalize(fxv3) = " << nfxv3 << endl;
    cout << "2 * normalize(fxv3) = " << 2 * nfxv3 << endl;
    cout << "nfxv3 * nfxv3 = " << nfxv3 * nfxv3 << endl;
    cout << "length(fxv3) = " << length(fxv3) << endl;
    cout << "length(nfxv3) = " << length(nfxv3) << endl;
    cout << "length(nfxv3).x = " << length(nfxv3).x << endl;
    cout << "length(nfxv3).xx = " << length(nfxv3).xx << endl;
    cout << "length(nfxv3).xxx = " << length(nfxv3).xxx << endl;
    cout << "length(nfxv3).xxxx = " << length(nfxv3).xxxx << endl;

    fixed2 qwerty_balh = fixed2(float2(0.3f, 0.5f));
    fixed2 qwerty = fixed2(0.3f, 0.5f);
    cout << "qwerty = " << qwerty << endl;
    qwerty = 1 - qwerty;
    cout << "1-qwerty = " << qwerty << endl;
    qwerty *= float2(0.8f, 0.7f);
    cout << "qwerty = " << qwerty << endl;

    cout << "value:   sizeof(fixed1) == " << sizeof(fixed1) << endl;
    cout << "value:   sizeof(fixed2) == " << sizeof(fixed2) << endl;
    cout << "value:   sizeof(fixed3) == " << sizeof(fixed3) << endl;
    cout << "value:   sizeof(fixed4) == " << sizeof(fixed4) << endl;

    __CGcustom_fixedU0_8 fixed8 = __CGcustom_fixedU0_8(0.5);
    __CGcustom_fixedU0_8 fixed8big = __CGcustom_fixedU0_8(1.5);
    __CGcustom_fixedU0_8 fixed8neg = __CGcustom_fixedU0_8(-0.5);
#endif

#ifdef __Cg_half_hpp__
    cout << "value:   sizeof(half1) == " << sizeof(half1) << endl;
    cout << "value:   sizeof(half2) == " << sizeof(half2) << endl;
    cout << "value:   sizeof(half3) == " << sizeof(half3) << endl;
    cout << "value:   sizeof(half4) == " << sizeof(half4) << endl;
#endif

    cout << "value:   sizeof(int1) == " << sizeof(int1) << endl;
    cout << "value:   sizeof(int2) == " << sizeof(int2) << endl;
    cout << "value:   sizeof(int3) == " << sizeof(int3) << endl;
    cout << "value:   sizeof(int4) == " << sizeof(int4) << endl;

    {
        float3 a = float3(1,2,3);
        float3 b = float3(4,5,6);
        cout << "dot(a,b)=" << dot(a,b) << endl;
        cout << "dot(a,b)=" << dot(a,b) << endl;
        cout << "dot(1,b)=" << dot(1,b) << endl;
        cout << "dot(a,2)=" << dot(a,2) << endl;
        cout << "dot(a.xxy,b)=" << dot(a.xxy,b) << endl;
        cout << "dot(a.xxy,b)=" << dot(a.xxy,b) << endl;
        cout << "cross(a,b)=" << cross(a,b) << endl;
        cout << "cross(b,a)=" << cross(b,a) << endl;
        cout << "cross(b.xzy,a)=" << cross(b.xzy,a) << endl;
    }
    {
        float3 az = float3(1,2,3);
        double3 aa = double3(az);
        double3(float3(1.0,2.0,3.0));
        double3(1.0,2.0,3.0);
        double3 a = double3(1.0,2.0,3.0);
        double3 b = double3(4.0,5.0,6.0);
        OUTPUT(dot(a,b));
        OUTPUT(dot(a.xxy,b));
        OUTPUT(cross(a,b));
        OUTPUT(cross(b,a));
        OUTPUT(cross(b.xzy,a));
    }
    {
        half3 a = half3(1,2,3);
        half3 b = half3(4,5,6);
        OUTPUT(dot(a,b));
        OUTPUT(cross(a,b));
        OUTPUT(cross(b,a));
        OUTPUT(cross(b.xzy,a));
    }
    {
        fixed3 a = fixed3(1,2,3);
        fixed3 b = fixed3(4,5,6);
        OUTPUT(cross(a,b));
        OUTPUT(cross(b,a));
        OUTPUT(cross(b.xzy,a));
    }
    {
        float4 c = float4(-1, 0.2f, -100.4f, 0);
        OUTPUT(saturate(c));
        OUTPUT(saturate(c.xy));
    }

#if 1
    {
        int3 a = int3(float3(1,2,3));
        int3 b = int3(4,5,6);
        OUTPUT(cross(a,b));
        OUTPUT(cross(b,a));
        OUTPUT(cross(b.xzy,a));
    }
#endif

    {
    float4 sncs = float4(23.2f, -12.2f, 0.6f, 1.4f);
    float4 sn, cs;
    sincos(sncs, sn, cs);
    OUTPUT(sncs);
    OUTPUT(sn);
    OUTPUT(cs);
    OUTPUT(sin(sncs));
    OUTPUT(cos(sncs));

    float4 bigvals = float4(2.0f, 3.0f, 0.8f, 2.0f);
    half4 bigvalsh = half4(2.0f, 3.0f, 0.8f, 2.0f);
    float4 smallvals = float4(-2.0f, -3.0f, -0.8f, 0.0f);
    float4 clamvals = clamp(sncs, smallvals, bigvals);
#ifdef __Cg_half_hpp__
    half4 hbigvals = half4(2.0f, 3.0f, 0.8f, 2.0f);
    //half4 hclamvals = clamp(sncs, smallvals, float4(hbigvals));  // clamp returns float4 which can't convert to half4 implicitly
    half4 hclamvals = half4(clamp(sncs, smallvals, float4(hbigvals)));
    OUTPUT(hclamvals);
#endif
    OUTPUT(bigvals);
    OUTPUT(smallvals);
    OUTPUT(clamvals);

    OUTPUT(clamp(sncs, -0.2f, 1.2f));

    OUTPUT(saturate(sn));
    OUTPUT(saturate(cs.wyzx));

    OUTPUT(atan2(cs, sn));

    // lerp
    float4 factors = float4(0.3f, 0.8f, 1.0f, 0.0f);
    float4 g81 = lerp(bigvals, smallvals, float1(0.5));  // fix me
    float4 g81a = lerp(bigvals, smallvals, float1(0.75));
    //float4 g81z = lerp(bigvals, smallvals, float4(0)); // fix me
    float4(0.0f);
    float4(0.0);
    float4(0);
    float4 g81z = lerp(bigvals, smallvals, float4(0.0f));
    float4 g81o = lerp(bigvals, smallvals, float4(1));
    float4 g82 = lerp(bigvals, smallvals, factors);
    float3 g83 = lerp(bigvals.xxy, smallvals.yzw, factors.wzy);
    cout << "factors = " << factors << endl;
    cout << "g81 = " << g81 << endl;
    cout << "g81a = " << g81a << endl;
    cout << "g81z = " << g81z << endl;
    cout << "g81o = " << g81o << endl;
    cout << "g82 = " << g82 << endl;
    cout << "g83 = " << g83 << endl;

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
#if 0
    g83 = smoothstep(float1(bigvals.x), float1(smallvals.y), float1(factors.w));
    //g83 = smoothstep(bigvals.x, smallvals.y, factors.w);  // hard
#endif
    float1 tifa = bigvals.x;
#if 0
    g83 = smoothstep(tifa, tifa, tifa);
    // hard case when all parameters are 1-component swizzles
    // In order to be unambiguous, at least one parameter must be converted to float1
    g83 = smoothstep(tifa, smallvals.y, factors.w);
    g83 = smoothstep(bigvals.x, float1(smallvals.y), factors.w);
    g83 = smoothstep(float1(bigvals.x), smallvals.y, factors.w);
    g83 = smoothstep(float(bigvals.x), smallvals.y, float1(factors.w));
#endif
    float2 g83_2 = smoothstep(bigvals.x, smallvals.y, factors.zw);

    float1 hjk = 3.2f;
    half1 hjkh = half1(hjk);
    half1 hjkh2 = half(hjk);

    //for (float x = -0.3; x < 1.2; x += 0.1) {
    for (float x = 0; x <= 1; x += 0.1f) {
        cout << "x(" << x << ") = " << smoothstep(smallvals, bigvals, x) << endl;
    }

    for (float x = 2; x <= 4.5; x += 0.1f) {
        cout << "x(" << x << ") = " << smoothstep(float1(2.f), float1(4.5f), x) << endl;
    }

    float zero = 0;
#ifdef _MSC_VER
# pragma warning (once : 4723) // warning C4723: potential divide by 0
#endif
    float inf = 2 / zero;  // potential divide by zero warning ok
    float nan = zero * inf;

    cout << "inf = " << inf << endl;
    cout << "nan = " << nan << endl;

    float4 special = float4(inf, -inf, nan, -nan);
    cout << "special = " << special << endl;
    OUTPUT(isnan(bigvals));
    OUTPUT(isnan(bigvals).zzy);
    OUTPUT(Cg::isnan(4.0));
    OUTPUT(isnan(4.0f));
    cout << "isinf(bigvals) = " << isinf(bigvals) << endl;
    cout << "isfinite(bigvals) = " << isfinite(bigvals) << endl;
    cout << "isnan(special) = " << isnan(special) << endl;
    cout << "isinf(special) = " << isinf(special) << endl;
    cout << "isfinite(special) = " << isfinite(special) << endl;

    float3 ldexpv = ldexp(float3(0.8f, 0.9f, 0.95f), float3(2,3,4));
    cout << "ldexpv = " << ldexpv << endl;

    float4 a99 = float4(7, 8, 9, 10);
    float4 b99 = float4(3, 4, 5, 6);
    float4 c99 = fmod(a99, b99);
    float4 d99 = fmod(-a99, b99);
    cout << "a99 = " << a99 << endl;
    cout << "b99 = " << b99 << endl;
    cout << "c99 = " << c99 << endl;
    cout << "d99 = " << d99 << endl;

    OUTPUT(max(b99,a99));
    OUTPUT(max(b99*2,a99*0.9f));
    OUTPUT(min(b99.xzxw*2,a99*0.9f));

    float4 a98 = float4(8, 9.8f, 15.9f, 33.4f);
    float4 b98;
    float4 c98 = frexp(a98, b98);
    float4 d98 = ldexp(c98, b98);
    float4 f98;
    float4 e98 = modf(float4(-1,1,1,-1) * a98, f98);
    cout << "a98 = " << a98 << endl;
    cout << "b98 = " << b98 << endl;
    cout << "c98 = " << c98 << endl;
    cout << "d98 = " << d98 << endl;
    cout << "e98 = " << e98 << endl;
    cout << "f98 = " << f98 << endl;

    cout << "lit(1,0.8,3) = " << lit(1.0f, 0.8f, 3.1f) << endl;
    cout << "lit(1,-0.8,3) = " << lit(1.0f, -0.8f, 3.1f) << endl;
    cout << "lit(0,0.8,3) = " << lit(1.0f, -0.8f, 3.1f) << endl;
    cout << "lit(0,-0.8,3) = " << lit(1.0f, -0.8f, 3.1f) << endl;
    cout << "lit(-0.4,0.8,3) = " << lit(-0.4f, 0.8f, 3.1f) << endl;
    cout << "lit(-0.4,-0.8,3) = " << lit(-0.4f, -0.8f, 3.1f) << endl;
    }


    {
      float3 a = float3(1,2,3);
      float3 b = float3(2,4,5);
      float1 a1 = 8;
      float1 b1 = 9;

      cout << "a=" << a << endl;
      cout << "b=" << b << endl;
      cout << "a*b=" << a*b << endl;
      cout << "a1*b1=" << a1*b1 << endl;
      cout << "a.xzz*b.z=" << a.xzz*b.z << endl;
      cout << "a.x*b.y=" << a.x*b.y << endl;
      cout << "a.x*b1=" << a.x*b1 << endl;
      cout << "a1*b.z=" << a1*b.z << endl;

    }

    sign_test();

    {
        float1 f1 = 4.2f;
        int1 i1 = 4;
        bool b1 = !!f1;
        bool b2 = bool1(i1 < f1);
        OUTPUT(f1);
        OUTPUT(b1);
        OUTPUT(b2);
    }

    {
        //half1 h1 = 3.4;  // g++ 4.0 issue
        //half1 h1 = 3.4f;   // g++ 4.0 issue
        half1 h1 = half(3.4f);
        OUTPUT(h1);
        half4 h4 = h1;
        OUTPUT(h4);
        float1 f1 = float1(h1);
        OUTPUT(f1);
        float4 f4 = float4(h1);
        OUTPUT(f4);
    }

    extern void test_matrix();
    test_matrix();

    extern void test_swizzle();
    test_swizzle();

    extern void test_inout();
    test_inout();

    extern void test_smear();
    test_smear();

    extern void test_cos();
    test_cos();

    extern void test_condition();
    test_condition();

    extern void test_sse();
    test_sse();

    extern void test_fresnel();
    test_fresnel();

    extern void test_dot();
    test_dot();

    extern void test_inout_dot();
    test_inout_dot();

    extern void test_inout_abs();
    test_inout_abs();

    extern void test_isnan();
    test_isnan();

    extern void test_stdlib();
    test_stdlib();

    extern void test_clamp();
    test_clamp();

    extern void test_lerp();
    test_lerp();

    extern void test_smoothstep();
    test_smoothstep();

    extern void test_cast();
    test_cast();

    extern void test_sampler();
    test_sampler();

    extern void test_tessellate();
    test_tessellate();

    extern void test_inverse();
    test_inverse();

    extern void test_types();
    test_types();

  return 0;
}
