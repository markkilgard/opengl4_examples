# mandelbrot.mk

include ../../../../cg/main/src/build_tools/cg/getprofile.mk

EXECUTABLE        := mandelbrot
PROJ_DIR          := .

PROJ_DEPENDENCIES :=
PROJ_LIBRARIES    :=

#
# Compile
#

FILES += mandelbrot/mandelbrot.cpp

FILES += common/request_vsync.c
FILES += common/showfps.c
FILES += common/trackball.c
FILES += common/nv_dds.cpp
FILES += common/read_text_file.cpp

FILES += common/boost.cpp # Boost needs fake exception handler

FILES += cg4cpp/src/inverse.cpp

INCLUDES += $(NV_TOOLS)/cg/Common/include
INCLUDES += cg4cpp/include
INCLUDES += common
INCLUDES += glew/include
INCLUDES += glut/include

# Use STLport for all platforms except Solaris

ifneq ($(OS), SunOS)
INCLUDES      += $(STLPORT_INCLUDES)
CXXFLAGS      += $(STLPORT_CXXFLAGS)
DEBUG_DEFINES += _STLP_DEBUG_ALLOC=1
DEBUG_DEFINES += _STLP_DEBUG_UNINITIALIZED=1
endif

# Use Boost for all platforms

INCLUDES  += $(BOOST_INCLUDES)
CXXFLAGS  += $(BOOST_CXXFLAGS)

# Use statically-linked GLEW

DEFINES += GLEW_STATIC GLEW_BUILD

# Subset GLEW to only what we need

INCLUDES += glew/include
FILES    += glew/src/glew.c

DEFINES  += GLEW_SUBSET

DEFINES  += GLEW_SUBSET_GL_VERSION_1_2
DEFINES  += GLEW_SUBSET_GL_VERSION_1_3
DEFINES  += GLEW_SUBSET_GL_VERSION_1_4
DEFINES  += GLEW_SUBSET_GL_VERSION_1_5
DEFINES  += GLEW_SUBSET_GL_VERSION_2_0
DEFINES  += GLEW_SUBSET_GL_VERSION_3_0
DEFINES  += GLEW_SUBSET_GL_VERSION_3_1
DEFINES  += GLEW_SUBSET_GL_VERSION_3_2
DEFINES  += GLEW_SUBSET_GL_VERSION_3_3
DEFINES  += GLEW_SUBSET_GL_VERSION_4_0
DEFINES  += GLEW_SUBSET_GL_VERSION_4_1
DEFINES  += GLEW_SUBSET_WGL_EXT_swap_control
DEFINES  += GLEW_SUBSET_GLX_SGI_swap_control
DEFINES  += GLEW_SUBSET_GL_ARB_tessellation_shader
DEFINES  += GLEW_SUBSET_GL_EXT_direct_state_access
DEFINES  += GLEW_SUBSET_GL_NV_primitive_restart

#
# Link
#

ifdef RELEASE
LIBDIRS += $(NV_TOOLS)/cg/Common/bin/$(PROFILE)_release
else
LIBDIRS += $(NV_TOOLS)/cg/Common/bin/$(PROFILE)_debug
endif

ifeq ($(OS),Windows)
#LIBRARIES += glewlib
endif

ifeq ($(OS),Windows)
INCLUDES += $(GLUT_INCLUDES) $(GLU_INCLUDES) $(OPENGL_INCLUDES)
CFLAGS   += $(GLUT_CFLAGS)   $(GLU_CFLAGS)   $(OPENGL_CFLAGS)
LDFLAGS  += $(GLUT_LDFLAGS)  $(GLU_LDFLAGS)  $(OPENGL_LDFLAGS)
LIBDIRS  += $(GLUT_LIBDIRS)  $(GLU_LIBDIRS)  $(OPENGL_LIBDIRS)
endif

ifeq ($(OS),Linux)
LIBRARIES += glutlib glulib 
#LIBRARIES += glut GLU
#LIBRARIES += glewlib
LIBRARIES += GL Xmu X11 Xt Xext SM ICE
LIBRARIES += dl
LIBRARIES += cgvirtual cgnew cgexception # Fake stdc++
#LIBRARIES += stdc++ gcc_s
LIBRARIES += m pthread
endif

ifeq ($(OS),Darwin)
#LIBRARIES += glewlib
LIBRARIES += gcc
LDFLAGS += -framework GLUT
LDFLAGS += -framework OpenGL
endif

ifeq ($(OS),SunOS)
#LIBRARIES += glewlib
LIBRARIES += glutlib glulib GL Xmu X11 Xt Xext SM ICE Cg CgGL
endif

include ../../../../cg/main/src/build_tools/cg/common.mk

