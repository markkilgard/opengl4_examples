
#ifdef __APPLE__
# include <GLUT/glut.h>
# include <OpenGL/glext.h>
#else
# include <GL/glut.h>
# include <GL/glext.h>
#endif

#include <Cg/vector/xyzw.hpp>
#include <Cg/sampler1D.hpp>
#include <Cg/sampler.hpp>
#include <Cg/GL/sampler.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

#include "glprocs.h"

using namespace Cg;

using std::cout;
using std::endl;

#ifdef _WIN32
#pragma comment (lib, "opengl32.lib") /* link with Microsoft OpenGL lib */
#endif

#define CHECK cout << "GL error check " __FILE__ << " @ line " << int(__LINE__) << endl; glutReportErrors();

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

void createTextures()
{
    cout << "Entering createTextures..." << endl;
    glutReportErrors();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); /* Tightly packed texture data. */

    glBindTexture(GL_TEXTURE_2D, TO_NORMAL_MAP);
    CHECK
    /* Load each mipmap level of range-compressed 128x128 brick normal
       map texture. */
    const GLubyte *image = myBrickNormalMapImage;
    int level = 0;
    for (int size = 128;
        size > 0;
        size /= 2, image += 3*size*size, level++) {
        glTexImage2D(GL_TEXTURE_2D, level,
            GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        CHECK
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    CHECK
    glBindTexture(GL_TEXTURE_CUBE_MAP, TO_NORMALIZE_VECTOR_CUBE_MAP);
    /* Load each 32x32 face (without mipmaps) of range-compressed "normalize
       vector" cube map. */
    image = myNormalizeVectorCubeMapImage;
    for (int face = 0;
        face < 6;
        face++, image += 3*32*32) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
            GL_RGB8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        CHECK
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECK

    cout << "Leaving createTextures..." << endl;
    glutReportErrors();
}

void useTextures()
{
    cout << "Entering useTextures..." << endl;
    glutReportErrors();

    {
      sampler2D foo = Sampler2DFromGLTextureUnit(GL_TEXTURE0);
      float4 tresult = tex2D(foo, float2(0.4, 0.7));
      cout << "Texture result  = " << tresult << endl;
    }

    sampler2D unused;
    sampler2D footex = sampler2D();
    sampler2D foo = Sampler2DFromGLTextureUnit(GL_TEXTURE0);
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
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, 3, 5, 0, GL_RGB, GL_FLOAT, rectData);

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

    cout << "Leaving useTextures..." << endl;
    glutReportErrors();
}

enum {
  BO_BOGUS = 0,
  BO_ARRAY,
};

void textureBufferTest()
{
  static GLfloat array[5][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}, {17,18,19,20} };

    cout << "Entering textureBufferTest..." << endl;
    glutReportErrors();

  if (!glutExtensionSupported("GL_EXT_texture_buffer_object")) {
    cout << "EXT_texture_buffer_object not supported -- not testing samplerBUF" << endl;
    return;
  }

#ifdef GL_EXT_texture_buffer_object
  const GLuint texobj = BO_ARRAY;
  CHECK
  glDeleteTextures(1, &texobj);
  CHECK
  glBindBuffer(GL_TEXTURE_BUFFER_EXT, texobj);
  CHECK
  glBufferData(GL_TEXTURE_BUFFER_EXT, sizeof(array), array, GL_STATIC_DRAW);
  CHECK
  glActiveTexture(GL_TEXTURE5);
  CHECK
  glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGBA32F_ARB, texobj);
  CHECK
  glActiveTexture(GL_TEXTURE0);
  CHECK

  glBindBuffer(GL_TEXTURE_BUFFER_EXT, BO_BOGUS);
  CHECK

  samplerBUF tbuf(5);

  float4 result;
  for (int i=0; i<5; i++) {
    result = texBUF(tbuf, i);
    cout << "Texture result(" << i << ")  = " << result << endl;
    result = texBUF(tbuf, float(i));
    cout << "Texture result(" << float(i) << ")  = " << result << endl;
    result = texBUF(tbuf, i + 0.2);
    cout << "Texture result(" << i + 0.2 << ")  = " << result << endl;
    result = texBUF(tbuf, i + 0.8);
    cout << "Texture result(" << i + 0.8 << ")  = " << result << endl;
  }
#endif

    cout << "Leaving textureBufferTest..." << endl;
    glutReportErrors();
}

void test_sampler()
{
    cout << "Entering test_sampler..." << endl;
    glutReportErrors();

    createTextures();

    useTextures();

#if 0
    textureBufferTest();
#else
    cout << "XXXX skipping not working textureBufferTest" << endl;
#endif

    CHECK
    sampler1D tex1d;
    CHECK

    if (glutExtensionSupported("GL_EXT_texture_array")) {
      sampler1DARRAY tex1d_array;
      CHECK
    }
    sampler2D tex2d;
    CHECK
    if (glutExtensionSupported("GL_EXT_texture_array")) {
      sampler2DARRAY tex2d_array;
      CHECK
    }
    sampler3D tex3d;
    CHECK
    samplerRECT texRect;
    CHECK
    samplerCUBE texCube;
    CHECK
    if (glutExtensionSupported("GL_EXT_texture_buffer_object")) {
      samplerBUF texBuf;
      CHECK
    }

    cout << "Leaving test_sampler..." << endl;
    glutReportErrors();
}
