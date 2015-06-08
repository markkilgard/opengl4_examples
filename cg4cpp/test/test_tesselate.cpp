
#include <assert.h>

#include <Cg/vector/xyzw.hpp>
#include <Cg/vector.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>
#include <Cg/inout.hpp>

using namespace Cg;

using std::cout;
using std::endl;

float2 bilinearPatch(float2 uv, float2 c00, float2 c01, float2 c10, float2 c11)
{
  float u         = uv.x, v         = uv.y;
  float oneMinusU = 1-u,  oneMinusV = 1-v;

  float2 c1 = u*v        *c00 + oneMinusU*v        *c01,
         c2 = u*oneMinusV*c10 + oneMinusU*oneMinusV*c11;

  return (c1 + c2);
}

float3 bilinearPatch(float2 uv, float3 c00, float3 c01, float3 c10, float3 c11)
{
  float u         = uv.x, v         = uv.y;
  float oneMinusU = 1-u,  oneMinusV = 1-v;

  float3 c1 = u*v        *c00 + oneMinusU*v        *c01,
         c2 = u*oneMinusV*c10 + oneMinusU*oneMinusV*c11;

  return (c1 + c2);
}

float3 bilinearPatch(float2 uv, float3 c[4])
{
  return bilinearPatch(uv, c[0], c[1], c[2], c[3]);
}

float3 linearCurve(float1 u, float3 c0, float3 c1)
{
  float oneMinusU = 1-u;

  c0 *= oneMinusU;
  c1 *= u;

  return (c0 + c1);
}

float3 bicubicBezierPatch(float2 uv, float3 c00, float3 c01, float3 c02, float3 c03,
                                     float3 c10, float3 c11, float3 c12, float3 c13,
                                     float3 c20, float3 c21, float3 c22, float3 c23,
                                     float3 c30, float3 c31, float3 c32, float3 c33)
{
  // Compute Bernstein weights for U (1 ADD, 8 MULs)
  float u = uv.x, oneMinusU = 1-u, uu = u*u, oneMinusUU = oneMinusU*oneMinusU;
  float Bu[4] = { oneMinusU * oneMinusUU,  // (1-u)^3
                  3*u*oneMinusUU,          // 3u(1-u)^2
                  3*uu*oneMinusU,          // 3u^2(1-u)
                  u*uu                     // u^3
                };

  // Multiply corner and edge control points by Bu weights (3*12 MULs)
  float3 w00 = Bu[0]*c00,
         w01 = Bu[1]*c01,
         w02 = Bu[2]*c02,
         w03 = Bu[3]*c03,

         w10 = Bu[0]*c10,
         // exclude w11 & w12 for interior
         w13 = Bu[3]*c13,

         w20 = Bu[0]*c20,
         // exclude w21 & w22 for interior
         w23 = Bu[3]*c23,

         w30 = Bu[0]*c30,
         w31 = Bu[1]*c31,
         w32 = Bu[2]*c32,
         w33 = Bu[3]*c33;

  // Row-wise sums (3*(3+1+1+3) ADDs)
  float3 w0a = w00 + w03,
         w0b = w01 + w02,
         w0  = w0a + w0b,
         
         w1a = w10 + w13,  // w11 & w12 part of interior
         
         w2a = w20 + w23,  // w21 & w22 part of interior
         
         w3a = w30 + w33,
         w3b = w31 + w32,
         w3  = w3a + w3b;

  // Compute Bernstein weights for V (1 ADD, 8 MULs)
  float v = uv.y, oneMinusV = 1-v, vv = v*v, oneMinusVV = oneMinusV*oneMinusV;
  float Bv[4] = { oneMinusV * oneMinusVV,  // (1-v)^3
                  3*v*oneMinusVV,          // 3v(1-v)^2
                  3*vv*oneMinusV,          // 3v^2(1-v)
                  v*vv                     // v^3
                };

  // Multiply row-sum of Bu-weighted control points by Bv weights (3*4 MULs)
  w0  *= Bv[0];
  w1a *= Bv[1];
  w2a *= Bv[2];
  w3  *= Bv[3];

  // Sum column-wise (3*3 ADDs)
  float3 wA = w0  + w1a,
         wB = w2a + w3,
         exterior = wA + wB;

  // Multiply interior control points by Bu weights (3*2+1 MULs, 3*2+1 MADs)
  // NOTE: interior is *guaranteed* to be zero on patch edges and corners!
  float3 interior = Bv[1] * (Bu[1]*c11 + Bu[2]*c12) +
                    Bv[2] * (Bu[1]*c21 + Bu[2]*c22);

  // Sum exterior and interior (3 ADDs)
  return exterior + interior;
}

float3 bicubicBezierPatch(float2 uv, float3 c[16])
{
  return bicubicBezierPatch(uv, c[0],  c[1],  c[2],  c[3],
                                c[4],  c[5],  c[6],  c[7],
                                c[8],  c[9],  c[10], c[11],
                                c[12], c[13], c[14], c[15]);
}

float3 bicubicBezierTriangle(float2 st, float3 cAAA, float3 cACA, float3 cCAC, float3 cCCC,
                                        float3 cABA, float3 cABC, float3 cCBC,
                                        float3 cBAB, float3 cBCB,
                                        float3 cBBB)
{
  // Compute Bernstein weights for U
  float s = st.x;
  float t = st.y;
  float u = (1-s-t);

  float3 sss  = s*s*s     * cAAA,
         sus3 = s*u*s * 3 * cACA,
         usu3 = u*s*u * 3 * cCAC,
         uuu  = u*u*u     * cCCC,

         sts3 = s*t*s * 3 * cABA,
         stu6 = s*t*u * 6 * cABC,
         utu3 = u*t*u * 3 * cCBC,

         tst3 = t*s*t * 3 * cBAB,
         tut3 = t*u*t * 3 * cBCB,
         
         ttt  = t*t*t     * cBBB;

  float3 stE = sts3 + tst3,
         tuE = utu3 + tut3,
         usE = sus3 + usu3,
         E = stE + tuE + usE;

  float3 stC = sss + ttt,
         tuC = ttt + uuu,
         usC = uuu + sss,
         C = (stC + tuC + usC) * 0.5;

  return (C + E);
}

float3 bicubicBezierTriangle(float2 st, float3 c[16])
{
  return bicubicBezierTriangle(st, c[0],  c[1],  c[2],  c[3],
                                   c[4],  c[5],  c[6],
                                   c[7],  c[8],
                                   c[10]);
}

float2 texcoord_ownership(float2 uv,
                          float2 interior_texcoord[4],
                          float2 u_edge_texcoord[4],
                          float2 v_edge_texcoord[4],
                          float2 corner_texcoord[4],
                          Out<float2> texcoord)
{
  float u = uv.x,
        v = uv.y;

  texcoord = bilinearPatch(uv, interior_texcoord[0], interior_texcoord[1],
                               interior_texcoord[2], interior_texcoord[3]);

  float2 owned_texcoord = texcoord;

  bool uConstantEdge = floor(u)==u,  // true when u is either 0.0 or 1.0
       vConstantEdge = floor(v)==v;  // true when v is either 0.0 or 1.0

  bool notInterior = uConstantEdge || vConstantEdge ;

  if (notInterior) {
    bool isCorner = uConstantEdge && vConstantEdge;
    if (isCorner) {
      owned_texcoord = corner_texcoord[int(2*v+u)];
    } else if (uConstantEdge) {
      owned_texcoord = lerp(u_edge_texcoord[int(2*u)], u_edge_texcoord[int(2*u+1)], v);
    } else {
      assert(vConstantEdge);
      owned_texcoord = lerp(v_edge_texcoord[int(v)], v_edge_texcoord[int(v+2)], u);
    }
  }

  return owned_texcoord;
}

float2 texcoord_ortho_ownership(float2 uv,
                                float2 interior_texcoord[2],
                                float2 u_edge_texcoord[4],
                                float2 v_edge_texcoord[4],
                                float2 corner_texcoord[4],
                                Out<float2> texcoord)
{
  float1 u = uv.x,
         v = uv.y;

  texcoord = float2(lerp(interior_texcoord[0].s, interior_texcoord[1].s, u),
                    lerp(interior_texcoord[0].t, interior_texcoord[1].t, v));
  
  float2 owned_texcoord = texcoord;

  bool uConstantEdge = floor(u)==u,  // true when u is either 0.0 or 1.0
       vConstantEdge = floor(v)==v;  // true when v is either 0.0 or 1.0

  bool notInterior = uConstantEdge || vConstantEdge ;

  if (notInterior) {
    bool isCorner = uConstantEdge && vConstantEdge;
    if (isCorner ) {
      owned_texcoord = corner_texcoord[int(2*v+u)];
    } else if (uConstantEdge) {
      assert(u_edge_texcoord[int(2*u)].s == v_edge_texcoord[int(2*u+1)].s);
      owned_texcoord = float2(u_edge_texcoord[int(2*u)].s,
                              lerp(u_edge_texcoord[int(2*u)].t, u_edge_texcoord[int(2*u+1)].t, v));
    } else {
      assert(vConstantEdge);
      assert(u_edge_texcoord[int(v)].t == u_edge_texcoord[int(v+2)].t);
      owned_texcoord = float2(lerp(v_edge_texcoord[int(v)].s, v_edge_texcoord[int(v+2)].s, u),
                              v_edge_texcoord[int(v)].t);
    }
  }

  return owned_texcoord;
}

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

void
test_tessellate()
{
    float3 c[4][4];

    c[3][0] = float3(0,1,0);       c[3][1] = float3(0.333f,01.333f,0);   c[3][2] = float3(0.666f,1,0);         c[3][3] = float3(1,1,0);
    c[2][0] = float3(0,0.666f,0);  c[2][1] = float3(0.333f,0.666f,0.25); c[2][2] = float3(0.666f,0.666f,0.25); c[2][3] = float3(1,0.666f,0);
    c[1][0] = float3(0,0.333f,0);  c[1][1] = float3(0.333f,0.333f,0.25); c[1][2] = float3(0.666f,0.333f,0.25); c[1][3] = float3(1,0.333f,0);
    c[0][0] = float3(0,0,0);       c[0][1] = float3(0.333f,0,0);         c[0][2] = float3(0.666f,0,0);         c[0][3] = float3(1,0,0);

    float3 r;
    float2 uv;
    
    uv = float2(0,0);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(c[0][0]);

    uv = float2(1,0);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(r);
    OUTPUT(c[0][3]);

    uv = float2(1,1);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(r);
    OUTPUT(c[3][3]);

    uv = float2(0.5,0.5);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(r);

    uv = float2(0.25,0.75);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(r);

    uv = float2(0.25,0.25);
    r = bicubicBezierPatch(uv,
        c[0][0], c[0][1], c[0][2], c[0][3],
        c[1][0], c[1][1], c[1][2], c[1][3],
        c[2][0], c[2][1], c[2][2], c[2][3],
        c[3][0], c[3][1], c[3][2], c[3][3]);
    OUTPUT(uv);
    OUTPUT(r);

    uv = float2(0.75,0.25);
    r = bicubicBezierPatch(uv,
        c[0][3], c[0][2], c[0][1], c[0][0],
        c[1][3], c[1][2], c[1][1], c[1][0],
        c[2][3], c[2][2], c[2][1], c[2][0],
        c[3][3], c[3][2], c[3][1], c[3][0]);
    OUTPUT(uv);
    OUTPUT(r);
}