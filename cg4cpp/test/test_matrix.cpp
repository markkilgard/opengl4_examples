
#include <Cg/double.hpp>
#include <Cg/vector/xyzw.hpp>
#include <Cg/matrix/0based.hpp>
#include <Cg/matrix/rows.hpp>
#include <Cg/matrix/columns.hpp>
#include <Cg/matrix.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

using std::cout;
using std::endl;

#define OUTPUT(_x) cout << #_x << "=" << (_x) << endl

float4x3 genCatmullRomForwardDifferenceMatrix(int steps, float3 p0, float3 p1, float3 p2, float3 p3)
{
  const float k = 1.0f/steps;

  const float3 p    = p1;
  const float3 dp   = (-p0+p2)*k/2+(2*p0-5*p1+4*p2-p3)*k*k/2+(-p0+3*p1-3*p2+p3)*k*k*k/2;
  const float3 ddp  = (2*p0-5*p1+4*p2-p3)*k*k+3*(-p0+3*p1-3*p2+p3)*k*k*k;
  const float3 dddp = 3*(-p0+3*p1-3*p2+p3)*k*k*k;

  return float4x3(p, dp, ddp, dddp);
}

float4x4 genCatmullRomForwardDifferenceStepMatrix(int steps)
{
  const float k = 1.0f/steps;

  const float4 r0 = float4(0, 1, 0, 0);
  const float4 r1 = float4(-k/2+k*k-k*k*k/2,
                           -5.0f/2*k*k+3.0f/2*k*k*k,
                           k/2+2*k*k-3/2*k*k*k,
                           -k*k/2+k*k*k/2);
  const float4 r2 = float4(2*k*k-3*k*k*k,
                           -5*k*k+9*k*k*k,
                           4*k*k-9*k*k*k,
                           -k*k+3*k*k*k);
  const float4 r3 = float4(-3*k*k*k,
                           9*k*k*k,
                           -9*k*k*k,
                           3*k*k*k);

  return float4x4(r0, r1, r2, r3);
}

void matrix_stuff(void)
{
    float4x4 mat4x4; // = cgMatrix<float,float,4,4>(float4(1,2,3,4),float4(5,6,7,8),float4(9,10,11,12),float4(13,14,15,16));

    mat4x4._m00_m01_m02_m03 = float4(1,2,3,4);
    mat4x4._m10_m11_m12_m13 = float4(5,6,7,8);
    mat4x4._m20_m21_m22_m23 = float4(9,10,11,12);
    mat4x4._m30_m31_m32_m33 = float4(13,14,15,16);

    cout << "mat4x4 = " << mat4x4 << endl;
    cout << "sizeof(mat4x4) = " << sizeof(mat4x4) << endl;

    mat4x4._m00_m01_m02_m03 = float4(1,2,3,4).xxyz;
    mat4x4._m10_m11_m12_m13 = float4(5,6,7,8).yyyy;
    mat4x4._m20_m21_m22_m23 = float4(9,10,11,12).wzyx;
    mat4x4._m30_m31_m32_m33 = float4(13,14,15,16).xyzz;

    cout << "mat4x4 = " << mat4x4 << endl;

    // Effectively transposes
    mat4x4._m00_m10_m20_m30 = float4(1,2,3,4);
    mat4x4._m01_m11_m21_m31 = float4(5,6,7,8);
    mat4x4._m02_m12_m22_m32 = float4(9,10,11,12);
    mat4x4._m03_m13_m23_m33 = float4(13,14,15,16);

    cout << "mat4x4 = " << mat4x4 << endl;
    OUTPUT(transpose(mat4x4));

    cout << "mat4x4[0] = " << mat4x4[0] << endl;
    cout << "mat4x4[1] = " << mat4x4[1] << endl;
    cout << "mat4x4[2] = " << mat4x4[2] << endl;
    cout << "mat4x4[3] = " << mat4x4[3] << endl;

    cout << "mat4x4[1][2] = " << mat4x4[1][2] << endl;
    cout << "mat4x4[0][3] = " << mat4x4[0][3] << endl;
    cout << "mat4x4[3][3] = " << mat4x4[3][3] << endl;

    mat4x4 = -2.9f;
    cout << "mat4x4 = " << mat4x4 << endl;
    mat4x4 = 0;
    cout << "mat4x4 = " << mat4x4 << endl;

    float4 vec4a = float4(1,2,3,4);
    float4 vec4b = vec4a * 2;

    float4x4 max4x4 = float4x4(1,2,3,4,
                      5,6,7,8,
                      9,10,11,12,
                      13,14,15,16);
    cout << "max4x4 = " << max4x4 << endl;

    float4x4 mat4x4gg = float4x4(16,15,14,13,
                      12,11,10,9,
                      8,7,6,5,
                      4,3,2,1);
    cout << "mat4x4gg = " << mat4x4gg << endl;

    cout << "max4x4 > mat4x4gg = " << (max4x4 > mat4x4gg) << endl;

#ifdef __Cg_double_hpp__
    float4x4 dmat4x4a = max4x4 * 2.0;  // generates a warning if simply 2
    cout << "dmat4x4a = " << dmat4x4a << endl;
    float4x4 dmat4x4b = 3.0 * max4x4;  // generates a warning if simply 3
    cout << "dmat4x4b = " << dmat4x4b << endl;
#endif

    float4x4 mat4x4a = max4x4 * 2.0f;  // generates a warning if simply 2
    cout << "mat4x4a = " << mat4x4a << endl;
    float4x4 mat4x4b = 3.0f * max4x4;  // generates a warning if simply 3
    cout << "mat4x4b = " << mat4x4b << endl;

    float4x3 mat4x3;
    mat4x3 = 1.8f;
    cout << "mat4x3 = " << mat4x3 << endl;

    float4x2 mat4x2;
    mat4x2 = 1.8f;
    cout << "mat4x2 = " << mat4x2 << endl;

    float4x1 mat4x1;
    mat4x1 = 1.8f;
    cout << "mat4x1 = " << mat4x1 << endl;

    float3x4 mat3x4;
    mat3x4 = 1.8f;
    cout << "mat3x4 = " << mat3x4 << endl;

    float3x3 mat3x3;
    mat3x3 = 1.8f;
    cout << "mat3x3 = " << mat3x3 << endl;

    float3x2 mat3x2;
    mat3x2 = 1.8f;
    cout << "mat3x2 = " << mat3x2 << endl;

    float3x1 mat3x1;
    mat3x1 = 1.8f;
    cout << "mat3x1 = " << mat3x1 << endl;

    float2x4 mat2x4;
    mat2x4 = 1.7f;
    cout << "mat2x4 = " << mat2x4 << endl;

    float2x3 mat2x3;
    mat2x3 = 1.75f;
    cout << "mat2x3 = " << mat2x3 << endl;

    float2x2 mat2x2;
    mat2x2 = 1.76f;
    cout << "mat2x2 = " << mat2x2 << endl;

    float2x1 mat2x1;
    mat2x1 = 1.77f;
    cout << "mat2x1 = " << mat2x1 << endl;

    float1x4 mat1x4;
    mat1x4 = 1.6f;
    cout << "mat1x4 = " << mat1x4 << endl;

    float1x3 mat1x3;
    mat1x3 = 1.5f;
    cout << "mat1x3 = " << mat1x3 << endl;

    float1x2 mat1x2;
    mat1x2 = 1.4f;
    cout << "mat1x2 = " << mat1x2 << endl;

    float1x1 mat1x1;
    mat1x1 = 1.3f;
    cout << "mat1x1 = " << mat1x1 << endl;
    OUTPUT(transpose(mat1x1));

    float2x4 am = float2x4(2,0,-1,1, 1,2,0,1);
    float4x3 bm = float4x3(1,5,-7, 1,1,0, 0,-1,1, 2,0,0);
    float2x3 cm = mul(am,bm);
    cout << "am = " << am << endl;
    cout << "bm = " << bm << endl;
    cout << "cm = " << cm << endl;

    cout << "transpose(am) = " << transpose(am) << endl;
    cout << "transpose(bm) = " << transpose(bm) << endl;
    cout << "transpose(cm) = " << transpose(cm) << endl;

    float4x4 ident4x4 = 0;
    ident4x4._m00 = 1;
    ident4x4._m11 = 1;
    ident4x4._m22 = 1;
    ident4x4._m33 = 1;
    cout << "ident4x4 = " << ident4x4 << endl;
    OUTPUT(transpose(ident4x4));
    OUTPUT((ident4x4 == transpose(ident4x4)));

    float4 f4(100,2,3,6);
    float4 goober = mul(ident4x4,f4);
    cout << "goober = " << goober << endl;
    float4 howdy = mul(f4,ident4x4);
    cout << "howdy = " << howdy << endl;

    float1x1 det1a = float1x1(8);
    float det1 = determinant(det1a);
    cout << "det1a = " << det1a << endl;
    cout << "det1 = " << det1 << endl;

#ifdef __Cg_half_hpp__
    half2x2(8,2, 5,7);
    half2x2 det2ah = half2x2(8,2, 5,7);
    float det2h = determinant(float2x2(det2ah));
    cout << "det2ah = " << det2ah << endl;
    cout << "det2h = " << det2h << endl;
#endif

    float3x3 det3a = float3x3(8,2,4, 5,7,7, 5,2,-6);
    float det3 = determinant(det3a);
    cout << "det3a = " << det3a << endl;
    cout << "det3 = " << det3 << endl;
    OUTPUT(transpose(det3a));
    OUTPUT(determinant(transpose(det3a)));

    float4x4 det4a = float4x4(3,2,-1,4, 2,1,5,7, 0,5,2,-6, -1,2,1,0);
    float det4 = determinant(det4a);
    cout << "det4a = " << det4a << endl;
    cout << "det4 = " << det4 << endl;

    det4a = float4x4(3,2,0,1, 4,0,1,2, 3,0,2,1, 9,2,3,1);
    det4 = determinant(det4a);
    cout << "det4a = " << det4a << endl;
    cout << "det4 = " << det4 << endl;

    float2x2 det2a = float2x2(3,1,5,2);
    float det2 = determinant(det2a);
    cout << "det2a = " << det2a << endl;
    cout << "det2 = " << det2 << endl;
    OUTPUT(transpose(det2a));
    OUTPUT(determinant(transpose(det2a)));

#ifdef __Cg_double_hpp__
    double2x2 ddet2a = double2x2(3,1,5,2);
    double ddet2 = determinant(ddet2a);
    cout << "ddet2a = " << ddet2a << endl;
    cout << "det2 = " << ddet2 << endl;
#endif

    int2x3 i2x3 = int2x3(3,4,0,6,7,8);
    cout << "i2x3 = " << i2x3 << endl;
    cout << "!i2x3 = " << !i2x3 << endl;
    cout << "-i2x3 = " << -i2x3 << endl;
    cout << "~i2x3 = " << ~i2x3 << endl;

    i2x3++;
    cout << "i2x3 = " << i2x3 << endl;
    i2x3 += 3;
    cout << "i2x3 = " << i2x3 << endl;
    i2x3 *= i2x3;
    cout << "i2x3 = " << i2x3 << endl;
    --i2x3;
    cout << "i2x3 = " << i2x3-- << endl;
    cout << "i2x3 = " << i2x3 << endl;
}

static void matrix_func()
{
      int2x2 a = int2x2(4,126,3,15);
      int2x2 b = int2x2(39,63,1,2);

      OUTPUT(a+b);
      OUTPUT(a-b);
      OUTPUT(a*b);
      OUTPUT(a/b);
      OUTPUT(a^b);
      OUTPUT(a%b);
      OUTPUT(a|b);
      OUTPUT(a&b);
      OUTPUT(a||b);
      OUTPUT(a&&b);
      OUTPUT(a==b);
      OUTPUT(a!=b);
      OUTPUT(a<=b);
      OUTPUT(a>=b);
      OUTPUT(a<<b);
      OUTPUT(a>>b);

      OUTPUT(2+b);
      OUTPUT(2-b);
      OUTPUT(2*b);
      OUTPUT(2/b);
      OUTPUT(2^b);
      OUTPUT(2%b);
      OUTPUT(2|b);
      OUTPUT(2&b);
      OUTPUT(1||b);
      OUTPUT(1&&b);
      OUTPUT(2==b);
      OUTPUT(2!=b);
      OUTPUT(2<=b);
      OUTPUT(2>=b);
      OUTPUT(2<<b);
      OUTPUT(2>>b);

      OUTPUT(a+2);
      OUTPUT(a-2);
      OUTPUT(a*2);
      OUTPUT(a/2);
      OUTPUT(a^2);
      OUTPUT(a%2);
      OUTPUT(a|2);
      OUTPUT(a&2);
      OUTPUT(a||1);
      OUTPUT(a&&1);
      OUTPUT(a==2);
      OUTPUT(a!=2);
      OUTPUT(a<=2);
      OUTPUT(a>=2);
      OUTPUT(a<<2);
      OUTPUT(a>>2);

      OUTPUT(2.0f+b);
      OUTPUT(2.0f-b);
      OUTPUT(2.0f*b);
      OUTPUT(2.0f/b);
      OUTPUT(1.0f||b);
      OUTPUT(1.0f&&b);
      OUTPUT(2.0f==b);
      OUTPUT(2.0f!=b);
      OUTPUT(2.0f<=b);
      OUTPUT(2.0f>=b);

      OUTPUT(a+2.0f);
      OUTPUT(a-2.0f);
      OUTPUT(a*2.0f);
      OUTPUT(a/2.0f);
      OUTPUT(a||1.0f);
      OUTPUT(a&&1.0f);
      OUTPUT(a==2.0f);
      OUTPUT(a!=2.0f);
      OUTPUT(a<=2.0f);
      OUTPUT(a>=2.0f);
}

void default_matrix()
{
    float1x1 f11;
    float2x2 f22;
    float3x3 f33;
    float4x4 f44;
    int1x1 i11;
    int1x2 i12;
    int1x3 i13;
    int1x4 i14;
    int2x2 i22;
    int2x1 i21;
    int3x3 i33;
    int3x2 i32;
    int3x1 i31;
    int4x4 i44;
    int4x3 i43;
    int4x2 i42;
    int4x1 i41;
}

void double_crud()
{
    float4x4 a = float4x4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);

    double4x4 b = a;
}

void test_matrix(void)
{
    float2x2 mat2;
    mat2[0] = float2(1,2);
    mat2[1] = float2(3,4);
    cout << "mat2[0].yx = " << mat2[0].yx << endl;
    cout << "mat2[1].xxyx = " << mat2[1].xxyx << endl;

    bool4x4 bmat4x4;
    bool2x2 bmat2x2 = bool2x2(0,1,0,1);

    float4 mma = float4(1,2,3,4), mmb = float4(5,6,7,8), mmc = float4(9,10,11,12);
    float3x4 mmm_swiz = float3x4(mma.xyzw, mmb.xyzw, mmc.xyzw);
    cout << "mmm_swiz = " << mmm_swiz << endl;
    float3x4 mmm = float3x4(mma, mmb, mmc);
    cout << "mmm = " << mmm << endl;
    float3x4 mmm1 = float3x4(mma.xxww, mmb, mmc);
    cout << "mmm1 = " << mmm1 << endl;
    float2x4 mmm2 = float2x4(mma.xxww, mmb);
    cout << "mmm2 = " << mmm2 << endl;
    float1x4 mmm3 = float1x4(mma.xxww);
    cout << "mmm3 = " << mmm3 << endl;
    float4x4 mmm4 = float4x4(mma.xxww, mmb, mmc, mma.yyyy);
    cout << "mmm4 = " << mmm4 << endl;

    float4x1 ffc = float4x1(1,2,3,4);
    cout << "ffc = " << ffc << endl;
    float1x4 ffr = float1x4(1,2,3,4);
    cout << "ffr = " << ffr << endl;
    float4x4 ff44 = mul(ffc, ffr);
    cout << "ff44 = " << ff44 << endl;
    float1x1 ff11 = mul(ffr, ffc);
    cout << "ff11 = " << ff11 << endl;
    float ff11s = mul(ffr, ffc);
    cout << "ff11s = " << ff11s << endl;

    matrix_stuff();

    OUTPUT(genCatmullRomForwardDifferenceStepMatrix(4));

    matrix_func();

    double_crud();
}
