
/* glprocs.h - header for OpenGL entry point function pointers */

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef __APPLE__

/* OpenGL 1.3 minimal multitexture API */
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;

/* OpenGL 1.5 minimal buffer object API */
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;

/* OpenGL 2.0 two-sided stencil */
extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

/* EXT_texture_buffer_object */
extern PFNGLTEXBUFFEREXTPROC glTexBufferEXT;

#endif /* __APPLE__ */

extern void init_glprocs(void);

#ifdef  __cplusplus
}
#endif
