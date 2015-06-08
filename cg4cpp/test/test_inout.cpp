
#include <Cg/vector/xyzw.hpp>
#include <Cg/half.hpp>
#include <Cg/double.hpp>
#include <Cg/fixed.hpp>
#include <Cg/vector.hpp>
#include <Cg/inout.hpp>
#include <Cg/iostream.hpp>
#include <Cg/stdlib.hpp>

using namespace Cg;

using std::cout;
using std::endl;

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

void ChangeYto5(InOut<float3> a)
{
    a.y = 2;
    a[1] = a.y * 2;
    a.y++;
    a[1] *= 2;
    a.y -= 5;
}

void ChangeYto5(InOut<float4> a)
{
    a *= 1.1f;
    a.y = 2;
    a[1] = a.y * 2;
    a.y++;
    a[1] *= 2;
    a.y -= 5;
}

void setOneThenTwo(InOut<float2> a)
{
    a.x = 1;
    a.y = 2;
}

typedef struct {
    float3 xyz;
    float4 rgba;
    float density;
    int hello;
} Info;

void initInfo(Out<Info> a)
{
    a.xyz = float3(1,2,3);
    a.rgba = float4(0.1f, 0.2f, 0.3f, 0.4f);
    a.density = 3.14159f;
    a.hello = 45;
}

void modInfo(InOut<Info> a)
{
    a.xyz *= 2;
    a.rgba = float4(a.xyz,1) + a.rgba;
    a.density = a.hello + a.density;
    a.hello = 5;
}

void testInfoInOut()
{
    Info b, c;
    initInfo(b);
    cout << "xyz=" << b.xyz << endl;
    cout << "rgba=" << b.rgba << endl;
    cout << "density=" << b.density << endl;
    cout << "hello=" << b.hello << endl;
    modInfo(b);
    cout << "xyz=" << b.xyz << endl;
    cout << "rgba=" << b.rgba << endl;
    cout << "density=" << b.density << endl;
    cout << "hello=" << b.hello << endl;
    initInfo(c);
    cout << "xyz=" << c.xyz << endl;
    cout << "rgba=" << c.rgba << endl;
    cout << "density=" << c.density << endl;
    cout << "hello=" << c.hello << endl;
}

void inout_stuff(void)
{
  float4 fooabcd = float4(1,2,3,4);
  cout << "fooabcd = " << fooabcd << endl;
  ChangeYto5(fooabcd);
  cout << "fooabcd = " << fooabcd << endl;
  ChangeYto5(fooabcd.yzx);
  cout << "fooabcd = " << fooabcd << endl;
  ChangeYto5(fooabcd.wxy);
  cout << "fooabcd = " << fooabcd << endl;
  float2 f;
  setOneThenTwo(f);
  OUTPUT(f);
  setOneThenTwo(f.yx);
  OUTPUT(f);
}

float4 insin(In<float4> a, In<float4> b)
{
    return a + b;
}

float4 insin(In<float> a, In<float4> b)
{
#if 1
    float aa = a;
    aa = a*2;
#endif
    OUTPUT(b);
    OUTPUT(a);
    float4 bb = b;
    bb = b*2;
    return float(a) + b;
}

float4 insin2(float4 a, float4 b)
{
    return a + b;
}

float4 inoutsin(InOut<float4> a, InOut<float4> b)
{
    a + b;
    return sin(a) + sin(b);
}

void test_in()
{
    float a;
    float4 b = float4(1,2,3,4);

    a = 5;
    float4 g = insin(a, b);
    OUTPUT(g);
    g = insin2(a, b);
    OUTPUT(g);
    OUTPUT(insin(4,float4(2,3,4,5)));
    OUTPUT(insin2(4,float4(2,3,4,5)));
}

void test_inoutsin()
{
    float a;
    float4 b = float4(1,2,3,4);

    a = 5;
    float4 g = inoutsin(b, b);
    OUTPUT(g);
    OUTPUT(a);
    OUTPUT(b);
}

float gotfloat(float f)
{
    return f;
}

float gotfloatc(const float f)
{
    return f;
}

float mix_func(float myf, int myi, half myh,
               float3 myf3, int3 myi3, half3 myh3,
               
               In<float> myif, In<int> myii, In<half> myih,
               In<float3> myif3, In<int3> myii3, In<half3> myih3,

               InOut<float> myiof, InOut<int> myioi, InOut<half> myioh,
               InOut<float3> myiof3, InOut<int3> myioi3, InOut<half3> myioh3,

               Out<float3> myof3, Out<int3> myoi3, Out<half3> myoh3,
               Out<float> myof, Out<int> myoi, Out<half> myoh)
{
    float rv = myf;

    OUTPUT(myf);
    OUTPUT(myi);
    OUTPUT(myh);
    OUTPUT(myf3);
    OUTPUT(myi3);
    OUTPUT(myh3);

    OUTPUT(myif);
    OUTPUT(myii);
    OUTPUT(myih);
    OUTPUT(myif3);
    OUTPUT(myii3);
    OUTPUT(myih3);

    OUTPUT(myiof);
    OUTPUT(myioi);
    OUTPUT(myioh);
    OUTPUT(myiof3);
    OUTPUT(myioi3);
    OUTPUT(myioh3);

    myof = 123.456f;
    myoi = 999;
    myoh = 3.14159f;
    myof3 = float3(-1,-2,-3);
    myoi3 = int3(-4,-5,-6);
    myoh3 = half3(0.5, 0.25, 0.125);

    OUTPUT(myof);
    OUTPUT(myoi);
    OUTPUT(myoh);
    OUTPUT(myof3);
    OUTPUT(myoi3);
    OUTPUT(myoh3);

    myoi3 *= 2;
    myof3 = myoi3 + myof3;
    myoh3 = myoh3 + myoh3;

    rv += myif;
    rv = rv + myii;
    rv = rv + myih;
    myiof = myif + myioi;
    myioi = myii + myioi;
    myioh = myih + myioi;

    myii3 + myioi3;
    myiof3 = float3(myii3 + myioi3);
    myiof3 = myii3 + myiof3;

    myii3 + myioi3;
    myioh3 = half3(myii3 + myioi3);
    myioh3 = myii3 + myioh3;

    myioi3;
    myioi3.xz;
    int3 local_int3 = myioi3;
    myioh3;
    myioh3.xz;
    half3 local_half3 = myioh3;
    myif;
    float dd = myif;
    gotfloat(myif);
    gotfloatc(myif);
    int3 dddint3 = int3(myioi3);
    myif + float3(myioi3);
    float dddf = myif + myif;
    float(myif) + myioi3;
    OUTPUT(myif + myioi3);
    OUTPUT(myif - myioi3);
    OUTPUT(myif * myioi3);
    OUTPUT(myif / myioi3);
    myioi3 + myif;
    myiof3 = myif + myioi3;
    OUTPUT(678 + myioi3);
    OUTPUT(678 + myii3);
    OUTPUT(678 + myoi3);
    OUTPUT(myii + myioi3);
    OUTPUT(myii - myioi3);
    OUTPUT(myii * myioi3);
    OUTPUT(myii / myioi3);
    OUTPUT(myii & myioi3);
    OUTPUT(myii | myioi3);
    OUTPUT(myii ^ myioi3);
    OUTPUT(myii << myioi3);
    OUTPUT(myii >> myioi3);
    OUTPUT(myii > myioi3);
    OUTPUT(myii < myioi3);
    OUTPUT(myii >= myioi3);
    OUTPUT(myii <= myioi3);
    OUTPUT(myii == myioi3);
    OUTPUT(myii != myioi3);
    OUTPUT(myii && myioi3);
    OUTPUT(myii || myioi3);

    OUTPUT(myi + myoi3);
    OUTPUT(myi - myoi3);
    OUTPUT(myi * myoi3);
    OUTPUT(myi / myoi3);
    OUTPUT(myi & myoi3);
    OUTPUT(myi | myoi3);
    OUTPUT(myi ^ myoi3);
    OUTPUT(myi << myoi3);
    OUTPUT(myi >> myoi3);
    OUTPUT(myi > myoi3);
    OUTPUT(myi < myoi3);
    OUTPUT(myi >= myoi3);
    OUTPUT(myi <= myoi3);
    OUTPUT(myi == myoi3);
    OUTPUT(myi != myoi3);
    OUTPUT(myi && myoi3);
    OUTPUT(myi || myoi3);

    OUTPUT(myh + myoh3);
    OUTPUT(myh - myoh3);
    OUTPUT(myh * myoh3);
    OUTPUT(myh / myoh3);
    OUTPUT(myh > myoh3);
    OUTPUT(myh < myoh3);
    OUTPUT(myh >= myoh3);
    OUTPUT(myh <= myoh3);
    OUTPUT(myh == myoh3);
    OUTPUT(myh != myoh3);
    OUTPUT(myh && myoh3);
    OUTPUT(myh || myoh3);

    myi++;
    OUTPUT(myi);
    myi--;
    OUTPUT(myi);
    ++myi;
    OUTPUT(myi);
    --myi;
    OUTPUT(myi);

    myh++;
    OUTPUT(myh);
    myh--;
    OUTPUT(myh);
    ++myh;
    OUTPUT(myh);
    --myh;
    OUTPUT(myh);

    myih++;
    OUTPUT(myih);
    myih--;
    OUTPUT(myih);
    ++myih;
    OUTPUT(myih);
    --myih;
    OUTPUT(myih);

    myioh++;
    OUTPUT(myioh);
    myioh--;
    OUTPUT(myioh);
    ++myioh;
    OUTPUT(myioh);
    --myioh;
    OUTPUT(myioh);

    myi += myi;
    OUTPUT(myi);
    myi += 1;
    OUTPUT(myi);

    myioi3 + myii;
    myii * myioi3;
    myioi3 = myii + myioi3;

    return rv;
}

void mix_inout()
{
    float f = 45.3f;
    int i = 666;
    half h = 2.8;
    float3 f3 = float3(1,2,3);
    half3 h3 = half3(1.15,2.15,3.15);
    int3 i3 = int3(4,5,6);
    float iof = 13;
    int ioi = 14;
    half ioh = 15;
    float3 iof3 = float3(7,8,9);
    int3 ioi3 = int3(10, 11, 12);
    half3 ioh3 = half3(10.1, 11.1, 12.1);
    float3 of3;
    int3 oi3;
    half3 oh3;
    float of;
    int xif;
    half oh;

    float c = mix_func(5.0f, 3, half(1.125),
                       float3(88,77,66), int3(55,44,33), half3(1.7, 2.7, 3.7),
                       f, i, h,
                       f3, i3, h3,
                       iof, ioi, ioh,
                       iof3, ioi3, ioh3,
                       of3, oi3, oh3,
                       of, xif, oh);
    OUTPUT(c);
    OUTPUT(iof3);
    OUTPUT(ioi3);
    OUTPUT(of3);
    OUTPUT(oi3);
    OUTPUT(of);
    OUTPUT(xif);
}

void weird_inout()
{
  float3 fooxzw = float3(1,2,3);
  float3 baby;
  cout << "fooxzw = " << fooxzw << endl;
  {
    InOut<float3> barxzw(fooxzw);
    Out<float3> gg(baby);

    fooxzw += 1;
    cout << "fooxzw = " << fooxzw << endl;
    barxzw *= fooxzw;
    OUTPUT(barxzw);
    barxzw = float3(barxzw) * fooxzw;
    barxzw = barxzw * fooxzw;
    cout << "barxzw = " << barxzw << endl;
    gg = float3(1,2,3);
    gg.z = 9;
  }
  cout << "fooxzw = " << fooxzw << endl;
  OUTPUT(baby);

  float resultxzw = dot(fooxzw,float3(1,1,1));
  cout << "fooxzw = " << fooxzw << endl;
  cout << "resultxzw = " << resultxzw << endl;
}

void mysincos(float1 radians, Out<float> sinResult, Out<float> cosResult)
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

void setToTwo(Out<int> i2, Out<float> f2, Out<double> d2, 
              Out<char> c2, Out<short> s2, Out<half> h2, Out<fixed> x2,
              Out<float4> vf2, Out<int3> vi2, Out<double2> vd2, Out<half2> vh2, Out<fixed4> vx4, Out<float1> fr)
{
  i2 = 2;
  f2 = 2;
  d2 = 2;
  c2 = 2;
  s2 = 2;
  h2 = 2;
  x2 = 2;
  vf2 = 2;
  vi2 = 2;
  vd2 = 2;
  vh2 = half(2);
  vx4 = fixed(2);
  fr = 2;

  h2 += 1;
  h2 -= 1;
}

void out_test()
{
  int i;
  float f;
  double d;
  char c;
  short s;
  half h;
  fixed x;
  float4 f4;
  int3 i3;
  double2 d2;
  half2 h2;
  fixed4 x4;
  float4 fff = float4(9,8,7,6);
  setToTwo(i, f, d, c, s, h, x, f4, i3, d2, h2, x4, fff.z);
  OUTPUT(i);
  OUTPUT(f);
  OUTPUT(d);
  OUTPUT(int(c));
  OUTPUT(s);
  OUTPUT(h);
  OUTPUT(x);
  OUTPUT(f4);
  OUTPUT(i3);
  OUTPUT(d2);
  OUTPUT(h2);
  OUTPUT(x4);
  OUTPUT(fff);
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
        arr[i] = float(i);
    }
}

typedef float3 Float32array[2];

void arraySequence2(Out<Float32array> arr)
{
    for (int i=0; i<2; i++) {
        arr[i] = float3(11,6,float(i));
    }
}

void arrayDouble(InOut<Float12array> arr)
{
    for (int i=0; i<12; i++) {
        arr[i] *= 2;
        arr[i];
        arr[i] = (arr[i]) * 2.0f;
    }
}

void big_test()
{
    float radangle = 0.67f;
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
}

void test_inout(void)
{
    inout_stuff();
    testInfoInOut();
    weird_inout();
    big_test();
    out_test();
    test_in();
    test_inoutsin();
    mix_inout();
}
