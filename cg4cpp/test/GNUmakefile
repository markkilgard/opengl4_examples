
# GNUmakefile for g++ 4.0

UNAME := $(shell uname)

OBJ_SUFFIX := .o
DEP_SUFFIX := .P
DEP_FILES :=
DEP_DIR := .deps

SRCS = template_test.cpp test_condition.cpp test_dot.cpp test_inout_dot.cpp \
       test_fresnel.cpp test_inout.cpp test_matrix.cpp test_sampler.cpp \
       test_smear.cpp test_sse.cpp test_swizzle.cpp  test_inout_abs.cpp \
       test_isnan.cpp test_cos.cpp test_stdlib.cpp test_clamp.cpp \
       test_lerp.cpp test_smoothstep.cpp test_cast.cpp test_tesselate.cpp test_inverse.cpp \
       glprocs.c test_types.cpp \
       $(NULL)

DEP_FILES += $(addsuffix $(DEP_SUFFIX),\
                         $(addprefix $(DEP_DIR)/,$(notdir $(SRCS))))

OBJS = $(SRCS:.cpp=.o)
CG4CPP_LIB = ../src/libcg4cpp.a
LIBS = $(CG4CPP_LIB)

CXXFLAGS = -I../include

TARGETS = cg4cpp_test

all: $(TARGETS)

ifeq ($(UNAME), Darwin)
LDFLAGS = -framework GLUT -framework OpenGL -lstdc++
else
LDFLAGS = -lglut -lGL -lstdc++
endif

cg4cpp_test : $(LIBS) $(OBJS)
	@ echo "Linking $@..."
	@ $(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

$(CG4CPP_LIB) : force
	$(MAKE) -C ../src

.PHONY: force

CC_GEN_DEPS = -Wp,-MD,$(DEP_DIR)/$(<F).pp
CC_FIX_DEPS = -sed -e 's;$(*F)\.o:;$@:;' $(DEP_DIR)/$(<F).pp \
                    > $(DEP_DIR)/$(<F).P; \
               tr ' ' '\012' < $(DEP_DIR)/$(<F).pp \
                 | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' \
                       -e 's/$$/ :/' \
                 >> $(DEP_DIR)/$(<F).P; \
               rm $(DEP_DIR)/$(<F).pp

CPP_GEN_DEPS = -Wp,-MD,$(DEP_DIR)/$(<F).pp
CPP_FIX_DEPS = -sed -e 's;$(<F)\.o:;$@:;' $(DEP_DIR)/$(<F).pp \
                    > $(DEP_DIR)/$(<F).P; \
                tr ' ' '\012' < $(DEP_DIR)/$(<F).pp \
                  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' \
                        -e 's/$$/ :/' \
                  >> $(DEP_DIR)/$(<F).P; \
                rm $(DEP_DIR)/$(<F).pp

# make sure DEP_DIR is always created
DEP_DIR_MAGIC := $(shell mkdir -p $(DEP_DIR))

RM = rm -rf

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGETS)

%$(OBJ_SUFFIX): %.cpp
	echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(CC_GEN_DEPS)
	$(CC_FIX_DEPS)

ifndef VERBOSE
.SILENT:
endif

-include $(sort $(DEP_FILES))
