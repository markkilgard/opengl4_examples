
/* glprocs.c - define and initialize OpenGL entry points */

#include <stdlib.h>  /* for exit */
#include <stdio.h>

#include "glprocs.h"

#ifndef __APPLE__
/* OpenGL 1.3 minimal multitexture API */
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;

/* OpenGL 1.5 minimal buffer object API */
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;

/* OpenGL 2.0 two-sided stencil */
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

/* EXT_texture_buffer_object */
PFNGLTEXBUFFEREXTPROC glTexBufferEXT;
#endif

/* Cross-platform OpenGL extension proc handling. */
#ifdef _WIN32
#  include <windows.h>
#  define GET_PROC_ADDRESS(p)   wglGetProcAddress(p) 
#elif defined(__APPLE__)
extern void *OSXGetProcAddress(const char *name);
#  define GET_PROC_ADDRESS(p)   OSXGetProcAddress("_" p)
#elif defined(__ADM__)
#  include <GL/adm.h>
#  define GET_PROC_ADDRESS(p)   admGetProcAddress( (const GLubyte *) p)
#else
#  include <string.h>
#  include <GL/glx.h>
#  define GET_PROC_ADDRESS(p)   glXGetProcAddressARB( (const GLubyte *) p) 
#endif

extern const char *programName;

static int
supportsTwoDotWhatever(int whatever)
{
  const char *version;
  int major, minor;

  version = (char *) glGetString(GL_VERSION);
  if (sscanf(version, "%d.%d", &major, &minor) == 2) {
    return major > 2 || (major == 2 && minor >= whatever);
  }
  return 0;            /* OpenGL version string malformed! */
}

void
init_glprocs(void)
{
  int problems = 0;

  if (!supportsTwoDotWhatever(0)) {
    fprintf(stderr, "%s: at least OpenGL 2.0 is required; you have %s\n", programName, glGetString(GL_VERSION));
    exit(1);
  }

#define LOAD(t,n) \
  n = (t) GET_PROC_ADDRESS(#n); \
  if (0 == n) { \
    fprintf(stderr, "%s: could not GetProcAddress for %s from OpenGL\n", programName, #n); \
    problems++; \
  }

#ifndef __APPLE__
  /* Minimal OpenGL 1.3 multitexture API */
  LOAD(PFNGLACTIVETEXTUREPROC,glActiveTexture)
  LOAD(PFNGLCLIENTACTIVETEXTUREPROC,glClientActiveTexture)
  LOAD(PFNGLLOADTRANSPOSEMATRIXFPROC,glLoadTransposeMatrixf);

  /* Minimal OpenGL 1.5 buffer object API */
  LOAD(PFNGLBINDBUFFERPROC,glBindBuffer)
  LOAD(PFNGLBUFFERDATAPROC,glBufferData)
  LOAD(PFNGLDELETEBUFFERSPROC,glDeleteBuffers)
  LOAD(PFNGLGENBUFFERSPROC,glGenBuffers)

  /* OpenGL 2.0 two-sided stencil API */
  LOAD(PFNGLSTENCILFUNCSEPARATEPROC, glStencilFuncSeparate);
  LOAD(PFNGLSTENCILOPSEPARATEPROC, glStencilOpSeparate);
  LOAD(PFNGLSTENCILMASKSEPARATEPROC, glStencilMaskSeparate);

  /* EXT_texture_buffer_object */
  LOAD(PFNGLTEXBUFFEREXTPROC, glTexBufferEXT);
#endif

  if (problems) {
#if 0
    exit(1);
#endif
  }
}
