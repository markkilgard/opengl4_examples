#include <Cg/cghalf.h>
#include <Cg/cgdouble.h>
#include <Cg/cgfixed.h>
#include <Cg/cgvector.h>
#include <Cg/cgmatrix.h>
#include <Cg/cgstdlib.h>
#include <Cg/cgiostream.h>
#include <Cg/cginout.h>
#include <Cg/cgsampler.h>
#include <Cg/cgsemantic.h>


#include <GL/glut.h>

/* An OpenGL 1.2 define */
#define GL_CLAMP_TO_EDGE                    0x812F
#define GL_PACK_SKIP_IMAGES                 0x806B
#define GL_PACK_IMAGE_HEIGHT                0x806C

/* A few OpenGL 1.3 defines */
#define GL_TEXTURE_CUBE_MAP                 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP         0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X      0x8515


#include <math.h>
#include <string>

//using namespace std;  // damn, the C++ standard library has fixed in <ios>

/* OpenGL texture object (TO) handles. */
enum {
  TO_NORMALIZE_VECTOR_CUBE_MAP = 1,
  TO_NORMAL_MAP = 2,
};

static const GLubyte
myBrickNormalMapImage[3*(128*128+64*64+32*32+16*16+8*8+4*4+2*2+1*1)] = {
/* RGB8 image data for a mipmapped 128x128 normal map for a brick pattern */
#include "brick_image.h"
};

static const GLubyte
myNormalizeVectorCubeMapImage[6*3*32*32] = {
/* RGB8 image data for a normalization vector cube map with 32x32 faces */
#include "normcm_image.h"
};

float4 a4, b4, c4;
float3 a3, b3, c3;

float mylength( float4 v )
{
    return sqrt(dot(v,v));
}

#ifdef __cginout_h__
void changeYto5(InOutBase<float3> a)
{
    a.y = 2;
    a[1] = a.y * 2;
    a.y++;
    a[1] *= 2;
    a.y -= 5;
}

void mysincos(float radians, Out<float> sinResult, Out<float> cosResult)
{
    cout << "sizeof(radians) = " << sizeof(radians) << endl;
    cout << "sizeof(sinResult) = " << sizeof(sinResult) << endl;
    cout << "sizeof(cosResult) = " << sizeof(cosResult) << endl;
    sinResult = sin(radians);
    sinResult += 1;
    sinResult *= 2;
    cosResult = cos(radians);
    ++cosResult;
    cosResult++;
}

void doubleTwoFloats(InOut<float> v1, InOut<float> v2)
{
     v1 *= 2;
     v2 *= 2;
}

typedef float Float12array[12];

void arraySequence(Out<Float12array> arr)
{
    for (int i=0; i<12; i++) {
        arr[i] = i;
    }
}

typedef float3 Float32array[2];

void arraySequence2(Out<Float32array> arr)
{
    for (int i=0; i<2; i++) {
        arr[i] = float3(11,6,i);
    }
}

void arrayDouble(InOut<Float12array> arr)
{
    for (int i=0; i<12; i++) {
        arr[i] *= 2;
    }
}
#endif // __cginout_h__

#ifdef __cghalf_h__
float4 float4convert(half4 v)
{
    float4 rv;
    rv = v;
    return rv;
}
#endif

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutCreateWindow("hello");

  float4 dd8 = float4(1,2,3,4);
  float4 dd9 = float4(5,6,7,8);

  cout << "dd8 = " << dd8 << endl;
  cout << "dd9 = " << dd9 << endl;

    dd8.xy = dd9.z;

  cout << "dd8 = " << dd8 << endl;

  float2 dd7;
  dd7 = dd9.x;
  dd8 = float4(dd7,dd7);

  cout << "dd8 = " << dd8 << endl;
  cout << "dd7 = " << dd7 << endl;

    //float3 qw = { 1, 2, 3 };  // Works in Cg but in C++, types with user defined constructors are not aggregate
    a3 = float3(1,2,3);
    b3 = float3(-3,4,5);
    c3 = a3 * b3;
    cout << "a3 * b3 = " << c3 << endl;
    c3 = a3.zxy * b3;
#ifdef __cghalf_h__
    half3 hb3 = half3(-3,4,5);
    c3 = a3.zxy * hb3;
#endif
    cout << "a3.zxy * b3 = " << c3 << endl;
    c3 = a3 * b3.yyz;
    cout << "a3 * b3.yyz = " << c3 << endl;
    c3 = cross(a3, b3);
    cout << "cross(a3,b3) = " << c3 << endl;

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
    float4 bbb; // = idd;
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
    float2 f2 = f3.xz;
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

#ifdef __cgmatrix_h__
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
#endif

    float4 pos4 = float4(-1, 2, 3, -1.5);
    float len = length(pos4);
    cout << "pos4 = " << pos4 << endl;
    cout << "len = " << len << endl;

#ifdef __cghalf_h__
    half myhalf = -2.4f;
    cout << "myhalf = " << myhalf << endl;
    half newhalf = myhalf;
    cout << "newhalf = " << newhalf << endl;

    half newhalf2(3.2f);
    cout << "newhalf2 = " << newhalf2 << endl;

    half hhdat = half(3.9f);
    cout << "hhdat = " << hhdat << endl;

    half3 hdat = half3(1.1f,2.2f,3.3f);
    cout << "hdat = " << hdat << endl;

    hdat = half3(1.1,2.2,3.3);
    cout << "hdat = " << hdat << endl;
    hdat = half3(half(4),half(5),half(6));
    cout << "hdat = " << hdat << endl;
    hdat = float3(1,2,3);
    cout << "hdat = " << hdat << endl;

    float3 fdat = hdat;

    half3 foorr = half3(1,2,3);
    float3 barrr = float3(1,2,3);
    //dot(foorr,barrr);  // not allowed to mix types for Cg stdlib routines
    dot(float3(foorr),barrr);
#endif
    int3 idat = int3(1,2,3);
    //float3 fdat2 = idat;  // generates warning
    float3 fdat2 = float3(idat);

    //unsigned int3 ui3;  // Legal in Cg, but not here.

    float3 iv = float3(0.3f,0.8f,1.f);
    cout << "iv = " << iv << endl;
    cout << "sin(iv) = " << sin(iv) << endl;
    cout << "cos(iv) = " << cos(iv) << endl;
    cout << "tan(iv) = " << tan(iv) << endl;
    cout << "asin(iv) = " << asin(iv) << endl;
    cout << "acos(iv) = " << acos(iv) << endl;
    cout << "atan(iv) = " << atan(iv) << endl;
    cout << "sinh(iv) = " << sinh(iv) << endl;
    cout << "cosh(iv) = " << cosh(iv) << endl;
    cout << "tanh(iv) = " << tanh(iv) << endl;
    cout << "sqrt(iv) = " << sqrt(iv) << endl;
    cout << "rsqrt(iv) = " << rsqrt(iv) << endl;
    float3 nv = normalize(float3(-0.1f,0.1f,1.1f));
    cout << "max(iv,nv) = " << max(iv,nv) << endl;
    cout << "min(iv,nv) = " << min(iv,nv) << endl;
    cout << "iv = " << iv << endl;
    cout << "nv = " << nv << endl;
    cout << "abs(nv) = " << abs(nv) << endl;
    cout << "refract(iv,nv,1.1f) = " << refract(iv,nv,1.1f) << endl;
    float3 rv;
    rv = nv * nv; // * dot(nv,iv);
    rv = 2 * nv; // * dot(nv,iv);
    rv = iv - 2 * nv * dot(nv,iv);
    cout << "rv = " << rv << endl;
    rv = reflect(iv, nv);
    cout << "rv = " << rv << endl;

    cout << "fresnel(iv,nv,1.1f) = " << fresnel(iv,nv,1.1f) << endl;

    iv = normalize(iv);
    cout << "normalize(iv) = " << iv << endl;
    cout << "length(normalize(iv)) = " << length(iv) << endl;

    iv = float3(2,3,4);
    float2 iv_exp = exp(iv.zx);

    float trylen = mylength(iv.xxxy);

    cout << "exp(iv.zx) = " << iv_exp << endl;
    cout << "exp(iv.zx) = " << exp(iv.zx) << endl;
    cout << "exp(iv) = " << exp(iv) << endl;
    cout << "exp(iv) = " << exp(float4(1,2,3,4)) << endl;
    cout << "exp2(iv) = " << exp2(iv) << endl;
    cout << "exp2(1,2,3,4) = " << exp2(float4(1,2,3,4)) << endl;
    cout << "log2(1,2,4,8) = " << log2(float4(1,2,4,8)) << endl;
    cout << "log10(1,10,100,1000) = " << log10(float4(1,10,100,1000)) << endl;
    //cout << "exp2(1,2,3,4) = " << exp2(float4(1,2,3,4).wzyx) << endl;  // works in Cg
    cout << "frac(0.5, 3.3, 4.7, -9.1) = " << frac(float4(0.5f, 3.3f, 4.7f, -9.1f)) << endl;
    cout << "ceil(0.5, 3.3, 4.7, -9.1) = " << ceil(float4(0.5f, 3.3f, 4.7f, -9.1f)) << endl;
    cout << "floor(0.5, 3.3, 4.7, -9.1) = " << floor(float4(0.5f, 3.3f, 4.7f, -9.1f)) << endl;

    float2 swap2 = float2(3,8);

    cout << "swap2 = " << swap2 << endl;
    swap2 = swap2.yx;
    cout << "swap2 = " << swap2 << endl;
    swap2.yx = swap2;
    cout << "swap2 = " << swap2 << endl;

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
  float2 varz = float2(1,2); float scalar = float1(varz.y); 
  float2 varz2 = float2(1,2); float scalar2 = float1(varz2[1]); 
  //ere[0] = ere.y;  // works in Cg; not in C++
  ere[0] = float1(ere.y);  // works in Cg; not in C++

  bool4 tf = bool4(0,0,1,0);
  cout << "tf = " << tf << endl;
  cout << "any(tf) = " << any(tf) << endl;
  cout << "all(tf) = " << all(tf) << endl;

  float4 powr = float4(0.5f, 3, 1, 4.1f);
  float4 ps = float4(2,3,4,5);
  cout << "powr = " << powr << endl;
  float4 powr_4 = pow(powr, 4);
  cout << "powr_4 = " << powr_4 << endl;
  powr_4 = pow(powr.wxyz, 4);
  cout << "pow(powr.wxyz) = " << powr_4 << endl;
  float4 powr_ps = pow(powr, ps);
  cout << "powr_ps = " << powr_ps << endl;
  
    cout << "pow(2,powr) = " << pow(2,powr) << endl;

    float4 sgn4 = float4(-3, 4.5f, 1, 0);

    cout << "sgn4 = " << sgn4 << endl;
    cout << "sign(sgn4) = " << sign(sgn4) << endl;
    cout << "abs(sgn4.zzxw).wyzx = " << abs(sgn4.zzxw).wyzx << endl;
    cout << "abs(int4(sgn4).zzxw).wyzx = " << abs(int4(sgn4).zzxw).wyzx << endl;

#ifdef __cghalf_h__
    half fooh = 3.14159f;
    cout << "fooh = " << fooh << endl;
    float pi_f = fooh;
    cout << "pi_f = " << pi_f << endl;
    half zeroh = 0.0f;
    cout << "zeroh = " << zeroh << endl;
    half nzeroh = -0.0f;
    cout << "nzeroh = " << nzeroh << endl;
    half oneh = 1.0f;
    cout << "oneh = " << oneh << endl;
    half noneh = -1.0f;
    cout << "noneh = " << noneh << endl;
    fooh = 3.14159f;
    fooh = fooh * 2;
    cout << "fooh = " << fooh << endl;
    half barh = 2.71;
    half resulth = fooh + barh;
    resulth *= fooh;

    float4 hvec4a2f = float4(1.1f,2.1f,3.1f,4.1f);
    cout << "hvec4a2f = " << hvec4a2f << endl;
    float4 hvec4a2fa = half4(1.1f,2.1f,3.1f,4.1f);
    cout << "hvec4a2fa = " << hvec4a2fa << endl;
    half4 hvec4a2 = half4(1.1,2.1,3.1,4.1);
    cout << "hvec4a2 = " << hvec4a2 << endl;
    half4 hvec4a = half4(1,2,3,4);
    hvec4a.xy;
    cout << "hvec4a = " << hvec4a << endl;
    cout << "hvec4a.wx = " << hvec4a.wx << endl;
    cout << "length(hvec4a.wx) = " << length(hvec4a.wx) << endl;
    cout << "dot(hvec4a,hvec4a) = " << dot(hvec4a,hvec4a) << endl;
    float hvec4a_dot = dot(hvec4a,hvec4a);
    cout << "dot(hvec4a,hvec4a) = " << hvec4a_dot << endl;
    cout << "dot(float4(hvec4a),float4(hvec4a)) = " << dot(float4(hvec4a),float4(hvec4a)) << endl;

    hvec4a = half4(1,2,3,4);
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a += 2.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a -= 1.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a *= 1.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a *= 0.3;
    cout << "hvec4a = " << hvec4a << endl;

    hvec4a = half4(1,2,3,4);
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a.zw += 2.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a.xz -= 1.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a.zw *= 1.7;
    cout << "hvec4a = " << hvec4a << endl;
    hvec4a.xyz *= 0.3;
    cout << "hvec4a = " << hvec4a << endl;

    hvec4a = half4(1,2,3,4);
    cout << "hvec4a = " << hvec4a << endl;
    cout << "hvec4a = " << hvec4a-- << endl;
    cout << "hvec4a = " << hvec4a << endl;
    cout << "hvec4a = " << hvec4a++ << endl;
    cout << "hvec4a = " << hvec4a << endl;
    cout << "hvec4a = " << --hvec4a << endl;
    cout << "hvec4a = " << hvec4a << endl;
    cout << "hvec4a = " << ++hvec4a << endl;
    cout << "hvec4a = " << hvec4a << endl;

    hvec4a = half4(1,2,3,4);
    float4 gosh = float3(0.9f, 0.7f, 0.8f).xyzx;
    float4 gosh2 = gosh * float4(hvec4a);
    cout << "gosh = " << gosh << endl;
    cout << "gosh2 = " << gosh2 << endl;
    float4 gosh3 = gosh.wwxy * float4(hvec4a);
    cout << "gosh3 = " << gosh3 << endl;
    float4(half4(hvec4a.zzxy));
    float4convert(hvec4a.zzxy);  // fix me, probably requires more operators
    float4 gosh4 = gosh * hvec4a.zzxy;
    //float4(hvec4a.zzxy);  // fix me, probably requires more operators
    //float4 gosh4 = gosh * float4(hvec4a.zzxy);  // fix me, probably requires more operators
    float4 gosh4h = half4(gosh) * hvec4a.zzxy;
    //cout << "gosh4 = " << gosh4 << endl;  // fix me too
    cout << "gosh4h = " << gosh4h << endl;
    //float4 gosh5 = gosh.xxxy * float4(hvec4a.yyyz);  // fix me too
    //cout << "gosh5 = " << gosh5 << endl;  // fix me too
#endif

#ifdef __cginout_h__
  float3 fooabc = float3(1,2,3);
  cout << "fooabc = " << fooabc << endl;
  changeYto5(fooabc);
  cout << "fooabc = " << fooabc << endl;

#if 0
  float4 fooabcd = float4(1,2,3,4);
  cout << "fooabcd = " << fooabcd << endl;
  changeYto5(fooabcd.xyz);  // fix me (hard)
  cout << "fooabcd = " << fooabcd << endl;
#endif

#if 0
  float3 fooxzw = float3(1,2,3);
  cout << "fooxzw = " << fooxzw << endl;
  {
    InOutBase<float3> barxzw(fooxzw);

    fooxzw += 1;
    //barxzw *= fooxzw;
    barxzw = float3(barxzw) * fooxzw;
    barxzw = barxzw * fooxzw;
    //barxzw = float3(5,6,7);
    cout << "barxzw = " << barxzw << endl;
  }
  float resultxzw = dot(fooxzw,float3(1,1,1));
  cout << "fooxzw = " << fooxzw << endl;
  cout << "resultxzw = " << resultxzw << endl;
#endif
#endif

#ifdef __cgfixed_h__
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

    fixed2 qwerty = float2(0.3f, 0.5f);
    cout << "qwerty = " << qwerty << endl;
    qwerty = 1 - qwerty;
    cout << "1-qwerty = " << qwerty << endl;
    qwerty *= float2(0.8f, 0.7f);
    cout << "qwerty = " << qwerty << endl;

    cout << "value:   sizeof(fixed1) == " << sizeof(fixed1) << endl;
    cout << "value:   sizeof(fixed2) == " << sizeof(fixed2) << endl;
    cout << "value:   sizeof(fixed3) == " << sizeof(fixed3) << endl;
    cout << "value:   sizeof(fixed4) == " << sizeof(fixed4) << endl;
#endif

#ifdef __cghalf_h__
    cout << "value:   sizeof(half1) == " << sizeof(half1) << endl;
    cout << "value:   sizeof(half2) == " << sizeof(half2) << endl;
    cout << "value:   sizeof(half3) == " << sizeof(half3) << endl;
    cout << "value:   sizeof(half4) == " << sizeof(half4) << endl;
#endif

    cout << "value:   sizeof(int1) == " << sizeof(int1) << endl;
    cout << "value:   sizeof(int2) == " << sizeof(int2) << endl;
    cout << "value:   sizeof(int3) == " << sizeof(int3) << endl;
    cout << "value:   sizeof(int4) == " << sizeof(int4) << endl;

    float4 sncs = float4(23.2f, -12.2f, 0.6f, 1.4f);
    float4 sn, cs;
    sincos(sncs, sn, cs);
    cout << "sncs = " << sncs << endl;
    cout << "sn = " << sn << endl;
    cout << "cs = " << cs << endl;
    cout << "sin(sncs) = " << sin(sncs) << endl;
    cout << "cos(sncs) = " << cos(sncs) << endl;

    float4 bigvals = float4(2.0f, 3.0f, 0.8f, 2.0f);
    float4 smallvals = float4(-2.0f, -3.0f, -0.8f, 0.0f);
    float4 clamvals = clamp(sncs, smallvals, bigvals);
#ifdef __cghalf_h__
    half4 hbigvals = half4(2.0f, 3.0f, 0.8f, 2.0f);
    half4 hclamvals = clamp(sncs, smallvals, float4(hbigvals));
    cout << "hclamvals = " << hclamvals << endl;
#endif
    cout << "bigvals = " << bigvals << endl;
    cout << "smallvals = " << smallvals << endl;
    cout << "clamvals = " << clamvals << endl;

    cout << "clamp(sncs, -0.2f, 1.2f) = " << clamp(sncs, -0.2f, 1.2f) << endl;

    cout << "saturate(sn) = " << sn << endl;
    cout << "saturate(cs.wyzx) = " << cs.wyzx << endl;

    cout << "atan2(cs, sn) = " << atan2(cs, sn) << endl;

    // lerp
    float4 factors = float4(0.3f, 0.8f, 1.0f, 0.0f);
    float4 g81 = lerp(bigvals, smallvals, float1(0.5));  // fix me
    float4 g81a = lerp(bigvals, smallvals, float1(0.75));
    float4 g81z = lerp(bigvals, smallvals, float4(0));
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
    g81 = smoothstep(bigvals, smallvals, 0.5);
    g81a = smoothstep(bigvals, smallvals, 0.75);
    g81z = smoothstep(bigvals, smallvals, 0);
    g81o = smoothstep(bigvals, smallvals, 1);
    g82 = smoothstep(bigvals, smallvals, factors);
    g83 = smoothstep(bigvals.xxy, smallvals.yzw, factors.wzy);
    cout << "factors = " << factors << endl;
    cout << "g81 = " << g81 << endl;
    cout << "g81a = " << g81a << endl;
    cout << "g81z = " << g81z << endl;
    cout << "g81o = " << g81o << endl;
    cout << "g82 = " << g82 << endl;
    cout << "g83 = " << g83 << endl;

    //for (float x = -0.3; x < 1.2; x += 0.1) {
    for (float x = 0; x <= 1; x += 0.1f) {
        cout << "x(" << x << ") = " << smoothstep(smallvals, bigvals, x) << endl;
    }

    for (float x = 2; x <= 4.5; x += 0.1f) {
        cout << "x(" << x << ") = " << smoothstep(float1(2.f), float1(4.5f), x) << endl;
    }

    float zero = 0;
    float inf = 2 / zero;
    float nan = zero * inf;

    cout << "inf = " << inf << endl;
    cout << "nan = " << nan << endl;

    float4 special = float4(inf, -inf, nan, -nan);
    cout << "special = " << special << endl;
    cout << "isnan(bigvals) = " << isnan(bigvals) << endl;
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

#ifdef __cgmatrix_h__
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

#ifdef __cgdouble_h__
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

    float4 goober = mul(ident4x4,f4);
    cout << "goober = " << goober << endl;
    float4 howdy = mul(f4,ident4x4);
    cout << "howdy = " << howdy << endl;

    float1x1 det1a = float1x1(8);
    float det1 = determinant(det1a);
    cout << "det1a = " << det1a << endl;
    cout << "det1 = " << det1 << endl;

#ifdef __cghalf_h__
    half2x2(8,2, 5,7);
    half2x2 det2ah = half2x2(8,2, 5,7);
    float det2h = determinant(det2ah);
    cout << "det2ah = " << det2ah << endl;
    cout << "det2h = " << det2h << endl;
#endif

    float3x3 det3a = float3x3(8,2,4, 5,7,7, 5,2,-6);
    float det3 = determinant(det3a);
    cout << "det3a = " << det3a << endl;
    cout << "det3 = " << det3 << endl;

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
#endif // __cgmatrix_h__

#ifdef __cginout_h__
    float radangle = 0.67;
    float rsin = 0, rcos = 0;

    mysincos(radangle, rsin, rcos);
    cout << "rsin = " << rsin << endl;
    cout << "rcos = " << rcos << endl;

    mysincos(0.0f, rsin, rcos);
    cout << "rsin = " << rsin << endl;
    cout << "rcos = " << rcos << endl;

    doubleTwoFloats(rsin, rcos);
    cout << "rsin = " << rsin << endl;
    cout << "rcos = " << rcos << endl;

    Float12array h;
    arraySequence(h);
    for (int i=0; i<12; i++) {
        cout << "h[" << i << "] = " << h[i] << endl;
    }
    arrayDouble(h);
    for (int i=0; i<12; i++) {
        cout << "h[" << i << "] = " << h[i] << endl;
    }

    float hhh[12];
    arraySequence(hhh);
    for (int i=0; i<12; i++) {
        cout << "hhh[" << i << "] = " << hhh[i] << endl;
    }
    arrayDouble(hhh);
    for (int i=0; i<12; i++) {
        cout << "hhh[" << i << "] = " << hhh[i] << endl;
    }

    float3 vvv[2];
    arraySequence2(vvv);
    for (int i=0; i<2; i++) {
        cout << "vvv[" << i << "] = " << vvv[i] << endl;
    }
#endif // __cginout_h__

#ifdef __cgsampler_h__
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); /* Tightly packed texture data. */

    glBindTexture(GL_TEXTURE_2D, TO_NORMAL_MAP);
    /* Load each mipmap level of range-compressed 128x128 brick normal
       map texture. */
    const GLubyte *image = myBrickNormalMapImage;
    int level = 0;
    for (int size = 128;
        size > 0;
        size /= 2, image += 3*size*size, level++) {
        glTexImage2D(GL_TEXTURE_2D, level,
            GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TO_NORMALIZE_VECTOR_CUBE_MAP);
    /* Load each 32x32 face (without mipmaps) of range-compressed "normalize
       vector" cube map. */
    image = myNormalizeVectorCubeMapImage;
    for (int face = 0;
        face < 6;
        face++, image += 3*32*32) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
            GL_RGB8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glutReportErrors();

    sampler2D footex = sampler2D();
    sampler2D foo = sampler2D(0);
    float4 tresult = tex2D(foo, float2(0.4, 0.7));
    cout << "Texture result  = " << tresult << endl;
    tresult = tex2D(foo, float2(1.4, 3.7));
    cout << "Texture result  = " << tresult << endl;
    tresult = tex2D(foo, float2(1/256.0, 1/256.0));
    cout << "Texture result  = " << tresult << endl;
    tresult = tex2D(foo, float2(-1.6, -3.3));
    cout << "Texture result  = " << tresult << endl;
    tresult = tex2Dproj(foo, float3(-1.6, -3.3, 2));
    cout << "Texture result  = " << tresult << endl;

    {
        samplerCUBE bartex = samplerCUBE();
        float3 vec2norm = float3(4.5f, -10.0f, -17.4f);
        //vec2norm = float3(4.5f, 0, 0);
        tresult = texCUBE(bartex, vec2norm);
        cout << "Texture result  = " << tresult << endl;
        cout << "normalize vec2norm = " << normalize(vec2norm) << endl;
        cout << "normalize tresult = " << tresult.xyz * 2 - 1 << endl;
        cout << "length tresult = " << length(tresult.xyz * 2 - 1) << endl;
        cout << "normalize tresult = " << normalize(tresult.xyz * 2 - 1) << endl;
        tresult = texCUBEproj(bartex, float4(vec2norm, 2.0));
        cout << "Texture result  = " << tresult << endl;
        cout << "normalize vec2norm = " << normalize(vec2norm) << endl;
        cout << "normalize tresult = " << tresult.xyz * 2 - 1 << endl;
        cout << "length tresult = " << length(tresult.xyz * 2 - 1) << endl;
        cout << "normalize tresult = " << normalize(tresult.xyz * 2 - 1) << endl;
        vec2norm = float3(0, 0, -3.2f);
        tresult = texCUBEproj(bartex, float4(vec2norm, 2.0));
        cout << "Texture result  = " << tresult << endl;
        cout << "normalize vec2norm = " << normalize(vec2norm) << endl;
        cout << "normalize tresult = " << tresult.xyz * 2 - 1 << endl;
        cout << "length tresult = " << length(tresult.xyz * 2 - 1) << endl;
        cout << "normalize tresult = " << normalize(tresult.xyz * 2 - 1) << endl;
        vec2norm = float3(0, 0.1f, 0);
        tresult = texCUBEproj(bartex, float4(vec2norm, 2.0));
        cout << "Texture result  = " << tresult << endl;
        cout << "normalize vec2norm = " << normalize(vec2norm) << endl;
        cout << "normalize tresult = " << tresult.xyz * 2 - 1 << endl;
        cout << "length tresult = " << length(tresult.xyz * 2 - 1) << endl;
        cout << "normalize tresult = " << normalize(tresult.xyz * 2 - 1) << endl;
    }


    GLfloat rectData[3][5][3];
    for (int i=0; i<5; i++) {
        for (int j=0; j<3; j++) {
            rectData[j][i][0] = i;
            rectData[j][i][1] = j;
            rectData[j][i][2] = 100+i+j;
        }
    }
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB_FLOAT16_ARB, 3, 5, 0, GL_RGB, GL_FLOAT, rectData);

    samplerRECT rect = samplerRECT();
    tresult = texRECT(rect, float2(0.5, 0.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(1, 0.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(1.5, 0.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(0.5, 1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(0.5, 1.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(0.5, 2.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(1.5, 2.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(2.5, 2.5));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(0.4, 0.7));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(-24, 7));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(-1, -1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(1, -1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(-1, 1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(100, 100));
    cout << "Texture result  = " << tresult << endl;

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    rect = samplerRECT();
    tresult = texRECT(rect, float2(-1, -1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(1, -1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(-1, 1));
    cout << "Texture result  = " << tresult << endl;
    tresult = texRECT(rect, float2(100, 100));
    cout << "Texture result  = " << tresult << endl;

    sampler3D tex3d = sampler3D();
    sampler1D tex1d = sampler1D();
#endif // __cgsampler_h__

    glutMainLoop();

    return 0;
}

