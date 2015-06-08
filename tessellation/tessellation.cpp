
// tessellation.cpp - OpenGL 4 example of programmable tessellation grids

#ifdef _MSC_VER
#pragma optimize( "p", on)
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include <Cg/vector/xyzw.hpp>
#include <Cg/vector.hpp>
#include <Cg/matrix.hpp>
#include <Cg/mul.hpp>
#include <Cg/dot.hpp>
#include <Cg/min.hpp>
#include <Cg/max.hpp>
#include <Cg/length.hpp>
//#include <Cg/lerp.hpp>  // This has problems with Visual Studio 2008
#define lerp(a,b,t) ((a) + (t)*((b)-(a)))
#include <Cg/inverse.hpp>

#include "trackball.h"
#include "showfps.h"
#include "request_vsync.h"
#include "teapot.hpp" // classic Utah teapot
#include "gumbo.hpp"  // the elephant
#include "monkey.hpp" // monkey head

using namespace boost;
using namespace Cg;
using std::map;
using std::string;

float curquat[4];
/* Initial slight rotation */
float lastquat[4] = { 1.78721e-006, -0.00139029, 3.47222e-005, 0.999999 };
GLfloat m[4][4];
int spinning = 1, moving = 0;
int beginx, beginy;
int newModel = 1;
int line_width = 3.0;
int draw_control_points = 0;
float pixelsPerSubEdge = 15;
int window_width, window_height;
float2 wh;
int show_patch = 0;
bool show_single_patch = false;
bool tri_sub = true;
int enable_sync = 1;
bool explicit_tess = false;  // be adaptive by default
bool verbose = false;
int face_mode = 0;

GLuint sphereDlist = 0;
float light_radius = 3;
float light_height = 0.75;
float light_angle = 60;
float3 light_pos = float3(light_radius, 0, 0);
bool avoid_cracks = true;

FPSusage fps_render_mode = FPS_USAGE_TEXTURE;
FPScontext gl_fps_context;

void updateLightPos()
{
    float a = light_angle * 3.14159 / 180.0;
    light_pos = float3(light_radius * cos(a), light_height, light_radius * sin(a));
}

bool read_depth = false;
int read_depth_x, read_depth_y;

float ztrans = -4;

char *programName = "tessellation";

static void fatalError(const char *message)
{
    fprintf(stderr, "%s: %s\n", programName, message);
    exit(1);
}

static char *readTextFile(const char *fileName, GLint *bytes)
{
    FILE *file = fopen(fileName, "r");
    char *data = NULL;

    if (file) {
        long size;

        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        data = (char*) malloc(size);
        if (data == NULL) {
            fprintf(stderr, "%s: malloc failed\n", programName);
            exit(1);
        }
        *bytes = (int) fread(data, 1, size, file);
        fclose(file);
    } else {
        fprintf(stderr, "%s: could not open file %s\n",
            programName, fileName);
        exit(1);
    }
    return data;
}

void showShaderInfoLog(GLuint shader, const char *msg)
{
    GLint maxLength, length;
    GLchar *infoLog;

    glGetShaderiv(shader,
        GL_INFO_LOG_LENGTH, &maxLength);
    if (maxLength > 1) {
        infoLog = (GLchar *) malloc(maxLength);
        if (infoLog != NULL) {
            glGetShaderInfoLog(shader, maxLength, &length, infoLog);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", infoLog);
            printf("=== end ===\n");
            free(infoLog);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

void showProgramInfoLog(GLuint program, const char *msg)
{
    GLint maxLength, length;
    GLchar *infoLog;

    glGetProgramiv(program,
        GL_INFO_LOG_LENGTH, &maxLength);
    if (maxLength > 1) {
        infoLog = (GLchar *) malloc(maxLength);
        if (infoLog != NULL) {
            glGetProgramInfoLog(program, maxLength, &length, infoLog);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", infoLog);
            printf("=== end ===\n");
            free(infoLog);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

GLuint initTessellationShaderProgram(const char *vertexFileName,
                                     const char *tessControlFileName,
                                     const char *tessEvaluationFileName,
                                     const char *geometryFileName,
                                     const char *fragmentFileName)
{
    struct {
        GLuint shader;
        const char *fileName;
        GLchar *source;
        GLint length;
        GLint compiled;
    } control, evaluation, vertex, geometry, fragment;

    // Require these.
    assert(vertexFileName);
    assert(fragmentFileName);

    vertex.fileName     = vertexFileName;
    control.fileName    = tessControlFileName;
    evaluation.fileName = tessEvaluationFileName;
    geometry.fileName   = geometryFileName;
    fragment.fileName   = fragmentFileName;

    /* Compile vertex shader. */
    vertex.shader   = glCreateShader(GL_VERTEX_SHADER);
    vertex.source   = readTextFile(vertex.fileName, &vertex.length);
    glShaderSource(vertex.shader,
        1, (const GLchar**)&vertex.source, &vertex.length);
    free(vertex.source);
    vertex.source = NULL;

    glCompileShader(vertex.shader);
    glGetShaderiv(vertex.shader, GL_COMPILE_STATUS, &vertex.compiled);
    if (verbose || !vertex.compiled) {
        showShaderInfoLog(vertex.shader, vertex.fileName);
    }

    if (control.fileName && evaluation.fileName) {
        /* Compile tessellation control shader. */
        control.shader   = glCreateShader(GL_TESS_CONTROL_SHADER);
        control.source   = readTextFile(control.fileName, &control.length);
        glShaderSource(control.shader,
            1, (const GLchar**)&control.source, &control.length);
        free(control.source);
        control.source = NULL;

        glCompileShader(control.shader);
        glGetShaderiv(control.shader, GL_COMPILE_STATUS, &control.compiled);
        if (verbose || !control.compiled) {
            showShaderInfoLog(control.shader, control.fileName);
        }

        /* Compile tessellation evaluation shader. */
        evaluation.shader   = glCreateShader(GL_TESS_EVALUATION_SHADER);
        evaluation.source   = readTextFile(evaluation.fileName, &evaluation.length);
        glShaderSource(evaluation.shader,
            1, (const GLchar**)&evaluation.source, &evaluation.length);
        free(evaluation.source);
        evaluation.source = NULL;

        glCompileShader(evaluation.shader);
        glGetShaderiv(evaluation.shader, GL_COMPILE_STATUS, &evaluation.compiled);
        if (verbose || !evaluation.compiled) {
            showShaderInfoLog(evaluation.shader, evaluation.fileName);
        }
    } else {
        // Non-existant tessellation shaders are treated as success.
        control.compiled = GL_TRUE;
        evaluation.compiled = GL_TRUE;
    }

    if (geometry.fileName) {
        // Compile geometry shader.
        geometry.shader   = glCreateShader(GL_GEOMETRY_SHADER);
        geometry.source   = readTextFile(geometry.fileName, &geometry.length);
        glShaderSource(geometry.shader,
            1, (const GLchar**)&geometry.source, &geometry.length);
        free(geometry.source);
        geometry.source = NULL;

        glCompileShader(geometry.shader);
        glGetShaderiv(geometry.shader, GL_COMPILE_STATUS, &geometry.compiled);
        if (verbose || !geometry.compiled) {
            showShaderInfoLog(geometry.shader, geometry.fileName);
        }
    } else {
        // Non-existant geometry shader is treated as success.
        geometry.compiled = GL_TRUE;
    }

    // Compile fragment shader.
    fragment.shader   = glCreateShader(GL_FRAGMENT_SHADER);
    fragment.source   = readTextFile(fragment.fileName, &fragment.length);
    glShaderSource(fragment.shader,
        1, (const GLchar**)&fragment.source, &fragment.length);
    free(fragment.source);
    fragment.source = NULL;

    glCompileShader(fragment.shader);
    glGetShaderiv(fragment.shader, GL_COMPILE_STATUS, &fragment.compiled);
    if (verbose || !fragment.compiled) {
        showShaderInfoLog(fragment.shader, fragment.fileName);
    }

    // Did all the shaders compile successfully?
    if (control.compiled    != GL_TRUE ||
        evaluation.compiled != GL_TRUE ||
        vertex.compiled     != GL_TRUE ||
        geometry.compiled   != GL_TRUE ||
        fragment.compiled   != GL_TRUE) {
            printf("%s: failed to compile shaders, see info logs for details why\n",
                programName);
            exit(1);
    }

    // Create program and attach shaders.
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex.shader);
    // After each shader is attached, they are no longer needed.
    glDeleteShader(vertex.shader);
    if (control.fileName && evaluation.fileName) {
        glAttachShader(program, control.shader);
        glDeleteShader(control.shader);
        glAttachShader(program, evaluation.shader);
        glDeleteShader(evaluation.shader);
    }
    if (geometry.fileName) {
        glAttachShader(program, geometry.shader);
        glDeleteShader(geometry.shader);
    }
    glAttachShader(program, fragment.shader);
    glDeleteShader(fragment.shader);

    // Link GLSL program.
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (verbose || !linked) {
        showProgramInfoLog(program, "linked program");
    }
    if (linked != GL_TRUE) {
        printf("%s: failed to link shaders, see info logs for details why\n",
            programName);
        exit(1);
    }
    return program;
}

float4x4 projection_matrix;
float4x4 view_to_clip;
float4x4 world_to_view;

float4x4 makeFrustum(float l, float r, float b, float t, float n, float f)
{
    return float4x4((2*n)/(r-l),0,(r+l)/(r-l),0,
                    0,(2*n)/(t-b),(t+b)/(t-b),0,
                    0,0,-(f+n)/(f-n),-(2*f*n)/(f-n),
                    0,0,-1,0);
}

float4x4 makePerpective(float fovy, float aspect,
                        float zNear, float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * 3.14159265 / 360.0);
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    return makeFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

// My GLEW doesn't have 4.1 support yet...
static int
supportsOpenGLVersionMajorDotMinor(int requiredMajor, int requiredMinor)
{
  const char *version;
  int major, minor;

  version = (char *) glGetString(GL_VERSION);
  if (sscanf(version, "%d.%d", &major, &minor) == 2) {
    if (major > requiredMajor) {
      /* Major version more than required major version. */
      return 1;
    } else if (major == requiredMajor) {
      /* Major version same as required major version. */
      if (minor >= requiredMinor) {
        /* Minor version at least required minor version. */
        return 1;
      } else {
        /* Major version sufficient but minor version not sufficient. */
      }
    } else {
      /* Major version not sufficient. */
      return 0;
    }
  }
  return 0;            /* OpenGL version string malformed! */
}


void initOpenGL()
{
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        fatalError("OpenGL Extension Wrangler (GLEW) failed to initialize");
    }
    GLboolean hasOpenGL4 = glewIsSupported("GL_VERSION_4_0");
    if (!hasOpenGL4) {
        fatalError("OpenGL implementation doesn't support OpenGL 4.0");
    }
    GLboolean hasOpenGL4_1 = supportsOpenGLVersionMajorDotMinor(4,1);
    if (!hasOpenGL4_1) {
        fatalError("OpenGL implementation doesn't support OpenGL 4.1");
    }
    GLboolean hasCompatibility = glewIsSupported("GL_ARB_compatibility");
    if (!hasCompatibility) {
        fatalError("OpenGL implementation doesn't support GL_ARB_compatibility (you should be using NVIDIA GPUs...)");
    }
    GLboolean hasDSA = glewIsSupported("GL_EXT_direct_state_access");
    if (!hasDSA) {
        fatalError("OpenGL implementation doesn't support GL_EXT_direct_state_access (you should be using NVIDIA GPUs...)");
    }
    GLboolean hasPrimitiveRestart = glewIsSupported("GL_NV_primitive_restart");
    if (!hasDSA) {
        fatalError("OpenGL implementation doesn't support GL_NV_primitive_restart (you should be using NVIDIA GPUs...)");
    }

    float fov = 60;
    float aspect = 1;
    float znear = 0.1;
    float zfar = 150.0;
    projection_matrix = makePerpective(fov, aspect, znear, zfar);
    glMatrixLoadTransposefEXT(GL_PROJECTION, reinterpret_cast<const GLfloat*>(&projection_matrix));
    ztrans = -6;

    view_to_clip = float4x4(1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1);  // identity matrix
}

struct TessellationShaderConfig {
    enum TessMode {
        ADAPTIVE = 0,
        EXPLICIT,
        TESS_MODE_COUNT
    };

    enum ShadeMode {
        UV = 0,
        DIFFUSE,
        PHONG,
        SOLID,
        SHADE_MODE_COUNT
    };

    GLuint program[TESS_MODE_COUNT][SHADE_MODE_COUNT];

    TessellationShaderConfig(const char *control_point_shader,
                             const char *adaptive_tcs,
                             const char *explicit_tcs,
                             const char *tes);
    ~TessellationShaderConfig();

    void bind(ShadeMode shade_mode);
    void updateDetailOfPrograms(const float2 &wh);
    static const char *shadeModeToString(ShadeMode sm) {
        switch (sm) {
        case UV: return "UV visualization";
        case PHONG: return "Phong shading";
        case SOLID: return "Simple diffuse";
        default: return "Unknown";
        }
    }
};

TessellationShaderConfig::ShadeMode shade_mode = TessellationShaderConfig::DIFFUSE;

TessellationShaderConfig::TessellationShaderConfig(const char *control_point_shader,
                                                   const char *adaptive_tcs,
                                                   const char *explicit_tcs,
                                                   const char *tes)
{
    for (int i=0; i<TESS_MODE_COUNT; i++) {
        const char *tcs = i == ADAPTIVE ? adaptive_tcs : explicit_tcs;
        program[i][UV] = initTessellationShaderProgram(
            control_point_shader,
            tcs, tes,
            NULL,
            "pass_thru.glslf");
        program[i][DIFFUSE] = initTessellationShaderProgram(
            control_point_shader,
            tcs, tes,
            NULL,
            "diffuse.glslf");
        program[i][PHONG] = initTessellationShaderProgram(
            control_point_shader,
            tcs, tes,
            NULL,
            "phong.glslf");
        program[i][SOLID] = initTessellationShaderProgram(
            control_point_shader,
            tcs, tes,
            NULL,
            "green.glslf");
    }
}

TessellationShaderConfig::~TessellationShaderConfig()
{
    for (int i=0; i<TESS_MODE_COUNT; i++) {
        for (int j=0; j<SHADE_MODE_COUNT; j++) {
            glDeleteProgram(program[i][j]);
        }
    }
}

void TessellationShaderConfig::bind(ShadeMode shade_mode)
{
    assert(shade_mode < SHADE_MODE_COUNT);
    GLuint p;
    if (explicit_tess) {
        p = program[EXPLICIT][shade_mode];
    } else {
        p = program[ADAPTIVE][shade_mode];
    }
    glUseProgram(p);
    GLint loc = glGetUniformLocation(p, "light_pos");
    if (loc >= 0) {
        float4 lp = mul(world_to_view, float4(light_pos, 1));
        lp /= lp.w;
        glUniform3f(loc, lp.x, lp.y, lp.z);
    }
}

void updateProgram(GLuint p, const float2 &wh)
{
    const char *var_name = "wh";

    glUseProgram(p);
    GLint loc = glGetUniformLocation(p, var_name);
    if (loc >= 0) {
        glUniform2f(loc, wh.x, wh.y);
    } else {
        if (verbose) {
            printf("location of %s not found for program %d\n", var_name, p);
        }
    }
}

void TessellationShaderConfig::updateDetailOfPrograms(const float2 &wh)
{
    for (int i=0; i<TESS_MODE_COUNT; i++) {
        for (int j=0; j<SHADE_MODE_COUNT; j++) {
            updateProgram(program[i][j], wh);
        }
    }
}

typedef shared_ptr<TessellationShaderConfig> TessellationShaderConfigPtr;
typedef map<string,TessellationShaderConfigPtr> ProgramMap;
ProgramMap program_map;


void loadShaders()
{
    program_map.clear();
    program_map["tri"] = TessellationShaderConfigPtr(
        new TessellationShaderConfig("cp_transform.glslv",
                                     "tri_adaptive_lod.glsltc",
                                     "tri_texcoord_lod.glsltc",
                                     "tri_lerp.glslte"));

    program_map["pntri"] = TessellationShaderConfigPtr(
        new TessellationShaderConfig("cp_transform.glslv",
                                     "pntri_adaptive_lod.glsltc",
                                     "pntri_texcoord_lod.glsltc",
                                     "pntri_lerp.glslte"));

    program_map["quad"] = TessellationShaderConfigPtr(
        new TessellationShaderConfig("cp_transform.glslv",
                                     "quad_adaptive_lod.glsltc",
                                     "quad_texcoord_lod.glsltc",
                                     "quad_lerp.glslte"));

    program_map["bicubic"] = TessellationShaderConfigPtr(
        new TessellationShaderConfig("cp_transform.glslv",
                                     "bicubic_adaptive_lod.glsltc",
                                     "bicubic_texcoord_lod.glsltc",
                                     "bicubic_lerp.glslte"));
    program_map["tricubic"] = TessellationShaderConfigPtr(
        new TessellationShaderConfig("cp_transform.glslv",
                                     "tricubic_adaptive_lod.glsltc",
                                     "tricubic_texcoord_lod.glsltc",
                                     "tricubic_lerp.glslte"));
}

int wireframe = 2;

int background_color = 0;

static void background()
{
    switch (background_color & 0x3) {
    case 0:
        glClearColor(0.1, 0.3, 0.6, 0.0);
        colorFPS(1,1,0);
        break;
    case 1:
        glClearColor(0,0,0,0);
        colorFPS(1,1,0);
        break;
    case 2:
        glClearColor(1,1,1,1);
        colorFPS(0,0,1);
        break;
    case 3:
        glClearColor(0.5,0.5,0.5,0.5);
        colorFPS(0,0,1);
        break;
    }
}

static void updateFaceMode()
{
    switch (face_mode) {
    case 0:
        glDisable(GL_CULL_FACE);
        break;
    case 1:
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        break;
    case 2:
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        break;
    }
}

void
makeSphereDlist(void)
{
  if (verbose) printf("creating light list\n");
  sphereDlist = glGenLists(1);
  glNewList(sphereDlist, GL_COMPILE); {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutSolidSphere(0.1, 10, 10);
  } glEndList();
}

void
initGraphics()
{
    trackball(curquat, 0.0, 0.0, 0.0, 0.0);

    glPointSize(5.0);
    glLineWidth(line_width);
    glEnable(GL_DEPTH_TEST);
    background();
    updateFaceMode();

    glEnable(GL_POLYGON_OFFSET_POINT);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glLineStipple(2, 0x5555);   // on-on-off-off pattern

    loadShaders();

    makeSphereDlist();
    updateLightPos();
}

float3 triangle_control_points[3] = {
    float3(-0.8,-0.8,0),
    float3(0.8,-0.8,0),
    float3(0,0.8,0),
};

void glVertex(const float2 &v)
{
    glVertex2fv(reinterpret_cast<const GLfloat*>(&v));
}

void glNormal(const float3 &v)
{
    glNormal3fv(reinterpret_cast<const GLfloat*>(&v));
}


void glVertex(const float3 &v)
{
    glVertex3fv(reinterpret_cast<const GLfloat*>(&v));
}

GLfloat tri_outer_lod[3] = { 5, 6, 7 };
GLfloat tri_inner_lod[1] = { 10 };

GLfloat quad_outer_lod[4] = { 5, 6, 7, 8 };
GLfloat quad_inner_lod[2] = { 10, 12 };

static void configPolygonMode()
{
    switch (wireframe) {
    case 0:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 1:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    default:
        assert(!"bogus wireframe state");
    case 3:
        // Draw nothing.
        break;
    }
}

static void drawTrianglePatch()
{
    glBegin(GL_PATCHES); {
        glVertex(triangle_control_points[0]);
        glVertex(triangle_control_points[1]);
        glVertex(triangle_control_points[2]);
    } glEnd();
}

void
doTrianglePatch()
{
    const int verticesPerPatch = 3;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    if (explicit_tess) {
        glMultiTexCoord3fv(GL_TEXTURE0, tri_outer_lod);
        glMultiTexCoord1fv(GL_TEXTURE1, tri_inner_lod);
    }

    program_map["tri"]->bind(shade_mode);
    switch (wireframe) {
    case 0:
    case 1:
        drawTrianglePatch();
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawTrianglePatch();
        program_map["tri"]->bind(TessellationShaderConfig::SOLID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawTrianglePatch();
        break;
    }

    if (draw_control_points) {
        glUseProgram(0);
        glPolygonOffset(-1,-2);
        if (draw_control_points > 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor3f(0.5,0.5,0);
            glBegin(GL_TRIANGLES); {
                glVertex(triangle_control_points[0]);
                glVertex(triangle_control_points[1]);
                glVertex(triangle_control_points[2]);
            } glEnd();
        }
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glColor3f(1,1,0);
        glBegin(GL_TRIANGLES); {
            glVertex(triangle_control_points[0]);
            glVertex(triangle_control_points[1]);
            glVertex(triangle_control_points[2]);
        } glEnd();
        glPolygonOffset(0,0);
    }
}

// Vertices in quadrilateral_control_points are in "loop" order, so:
//
//   v0 --> v1
//   ^      |
//   |      v
//   v3 <-- v2

float3 quadrilateral_control_points[4] = {
    float3(-0.8,-0.8,0),
    float3(0.8,-0.8,0),
    float3(0.8, 0.8,0),
    float3(-0.8, 0.8,0),
};

static void drawQuadrilateralPatch()
{
    glBegin(GL_PATCHES); {
        glVertex(quadrilateral_control_points[0]);
        glVertex(quadrilateral_control_points[1]);
        glVertex(quadrilateral_control_points[2]);
        glVertex(quadrilateral_control_points[3]);
    } glEnd();
}

void
doQuadrilateralPatch()
{
    const int verticesPerPatch = 4;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    if (explicit_tess) {
        glMultiTexCoord4fv(GL_TEXTURE0, quad_outer_lod);
        glMultiTexCoord2fv(GL_TEXTURE1, quad_inner_lod);
    }

    program_map["quad"]->bind(shade_mode);
    switch (wireframe) {
    case 0:
    case 1:
        drawQuadrilateralPatch();
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawQuadrilateralPatch();
        program_map["quad"]->bind(TessellationShaderConfig::SOLID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawQuadrilateralPatch();
        break;
    }

    if (draw_control_points) {
        glUseProgram(0);
        glPolygonOffset(-1,-2);
        if (draw_control_points > 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor3f(0.5,0.5,0);
            glBegin(GL_QUADS); {
            glVertex(quadrilateral_control_points[0]);
            glVertex(quadrilateral_control_points[1]);
            glVertex(quadrilateral_control_points[2]);
            glVertex(quadrilateral_control_points[3]);
            } glEnd();
        }
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glColor3f(1,1,0);
        glBegin(GL_QUADS); {
            glVertex(quadrilateral_control_points[0]);
            glVertex(quadrilateral_control_points[1]);
            glVertex(quadrilateral_control_points[2]);
            glVertex(quadrilateral_control_points[3]);
        } glEnd();
        glPolygonOffset(0,0);
    }
}

float3 bicubic_tri_control_points[10] = {
    float3(-0.8,-0.8,0),
    float3(-0.4,-0.8,1),
    float3( 0.4,-0.8,1),
    float3( 0.8,-0.8,0),

    float3(-0.5,-0.4,-1),
    float3( 0.0,-0.4,1),
    float3( 0.5,-0.4,-1),

    float3( -0.2, 0.4,-1),
    float3( 0.2, 0.4,-1),

    float3( 0, 0.8,0),
};

float3 bicubic_control_points[16] = {
    float3(-0.8,-0.8,0),
    float3(-0.4,-0.8,0),
    float3( 0.4,-0.8,0),
    float3( 0.8,-0.8,0),

    float3(-0.8,-0.4,0),
    float3(-0.4,-0.4,1),
    float3( 0.4,-0.4,1),
    float3( 0.8,-0.4,0),

    float3(-0.8, 0.4,0),
    float3(-0.4, 0.4,1),
    float3( 0.4, 0.4,1),
    float3( 0.8, 0.4,0),

    float3(-0.8, 0.8,0),
    float3(-0.4, 0.8,0),
    float3( 0.4, 0.8,0),
    float3( 0.8, 0.8,0),
};

void updateDetail()
{
    float2 wh = float2(window_width,window_height)/2.0 / pixelsPerSubEdge;
    if (verbose) {
        printf("wh = %f,%f\n", float(wh.x), float(wh.y));
    }

    ProgramMap::const_iterator itr;
    for(itr = program_map.begin(); itr != program_map.end(); ++itr){
        itr->second->updateDetailOfPrograms(wh);
    }
}

static void drawBezierTrianglesControlPoints(const float3 cp[])
{
    if (draw_control_points) {
        glUseProgram(0);
        glColor3f(1,1,0);
        glPolygonOffset(-1,-2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glBegin(GL_TRIANGLE_STRIP); {
            glVertex(cp[0]);
            glVertex(cp[4]);
            glVertex(cp[1]);
            glVertex(cp[5]);
            glVertex(cp[2]);
            glVertex(cp[6]);
            glVertex(cp[3]);
            glPrimitiveRestartNV();
            glVertex(cp[7]);
            glVertex(cp[8]);
            glVertex(cp[9]);
        } glEnd();

        if (draw_control_points > 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if (draw_control_points > 2) {
                glColor3f(0.25,0.25,0);
                glDepthFunc(GL_GEQUAL);
                glDepthMask(0);
                glLineWidth(1);
                glEnable(GL_LINE_STIPPLE);
                glBegin(GL_TRIANGLE_STRIP); {
                    glVertex(cp[0]);
                    glVertex(cp[4]);
                    glVertex(cp[1]);
                    glVertex(cp[5]);
                    glVertex(cp[2]);
                    glVertex(cp[6]);
                    glVertex(cp[3]);
                    glPrimitiveRestartNV();
                    glVertex(cp[4]);
                    glVertex(cp[7]);
                    glVertex(cp[5]);
                    glVertex(cp[8]);
                    glVertex(cp[6]);
                    glPrimitiveRestartNV();
                    glVertex(cp[7]);
                    glVertex(cp[8]);
                    glVertex(cp[9]);
                } glEnd();
                glDepthFunc(GL_LESS);
                glDepthMask(1);
                glLineWidth(line_width);
                glDisable(GL_LINE_STIPPLE);
            }
            glColor3f(0.5,0.5,0);
            glBegin(GL_TRIANGLE_STRIP); {
                glVertex(cp[0]);
                glVertex(cp[4]);
                glVertex(cp[1]);
                glVertex(cp[5]);
                glVertex(cp[2]);
                glVertex(cp[6]);
                glVertex(cp[3]);
                glPrimitiveRestartNV();
                glVertex(cp[4]);
                glVertex(cp[7]);
                glVertex(cp[5]);
                glVertex(cp[8]);
                glVertex(cp[6]);
                glPrimitiveRestartNV();
                glVertex(cp[7]);
                glVertex(cp[8]);
                glVertex(cp[9]);
            } glEnd();
        }
    }
}

static void doBicubicTrianglePatch()
{
    const int verticesPerPatch = 10;

    glBegin(GL_PATCHES); {
        for (int i=0; i<verticesPerPatch; i++) {
            glVertex(bicubic_tri_control_points[i]);
        }
    } glEnd();
}

void doBicubicTriangle()
{
    const int verticesPerPatch = 10;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    if (explicit_tess) {
        glMultiTexCoord3fv(GL_TEXTURE0, tri_outer_lod);
        glMultiTexCoord1fv(GL_TEXTURE1, tri_inner_lod);
    }

    program_map["tricubic"]->bind(shade_mode);
    switch (wireframe) {
    case 0:
    case 1:
        doBicubicTrianglePatch();
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        doBicubicTrianglePatch();
        program_map["tricubic"]->bind(TessellationShaderConfig::SOLID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        doBicubicTrianglePatch();
        break;
    }

    drawBezierTrianglesControlPoints(bicubic_tri_control_points);
}

// This is meant to be called with a glBegin(GL_LINE_STRIP)
static void outlinePatch(const int patch_ndxs[16], const float3 cp[])
{
    for (int i=0; i<3; i++) {
        for (int j=0; j<4; j++) {
            glVertex(cp[patch_ndxs[i*4+j]]);
            glVertex(cp[patch_ndxs[i*4+j+4]]);
        }
        glPrimitiveRestartNV();
    }
}

static void drawControlPoints(int patchCount, const int patch_ndxs[][16],
                              int cpCount, const float3 cp[])
{
    if (draw_control_points) {
        glUseProgram(0);
        glColor3f(1,1,0);
        glPolygonOffset(-1,-2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glBegin(GL_QUAD_STRIP); {
            int p = (show_patch % (patchCount+1));
            if (p!=0) {
                p--;
                outlinePatch(patch_ndxs[p], cp);
            } else {
                for (int p=0; p<patchCount; p++) {
                    outlinePatch(patch_ndxs[p], cp);
                }
            }
        } glEnd();

        if (draw_control_points > 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if (draw_control_points > 2) {
                glColor3f(0.25,0.25,0);
                glDepthFunc(GL_GEQUAL);
                glDepthMask(0);
                glLineWidth(1);
                glEnable(GL_LINE_STIPPLE);
                glBegin(GL_QUAD_STRIP); {
                    int p = (show_patch % (patchCount+1));
                    if (p!=0) {
                        p--;
                        outlinePatch(patch_ndxs[p], cp);
                    } else {
                        for (int p=0; p<patchCount; p++) {
                            outlinePatch(patch_ndxs[p], cp);
                        }
                    }
                } glEnd();
                glDepthFunc(GL_LESS);
                glDepthMask(1);
                glLineWidth(line_width);
                glDisable(GL_LINE_STIPPLE);
            }
            glColor3f(0.5,0.5,0);
            glBegin(GL_QUAD_STRIP); {
                int p = (show_patch % (patchCount+1));
                if (p!=0) {
                    p--;
                    outlinePatch(patch_ndxs[p], cp);
                } else {
                    for (int p=0; p<patchCount; p++) {
                        outlinePatch(patch_ndxs[p], cp);
                    }
                }
            } glEnd();
        }
    }
}

static void drawBicubicPatch()
{
    const int verticesPerPatch = 16;

    glBegin(GL_PATCHES); {
        for (int i=0; i<verticesPerPatch; i++) {
            glVertex(bicubic_control_points[i]);
        }
    } glEnd();
}

void doBicubicPatch()
{
    const int verticesPerPatch = 16;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    if (explicit_tess) {
        glMultiTexCoord4fv(GL_TEXTURE0, quad_outer_lod);
        glMultiTexCoord2fv(GL_TEXTURE1, quad_inner_lod);
    }
    program_map["bicubic"]->bind(shade_mode);
    switch (wireframe) {
    case 0:
    case 1:
        drawBicubicPatch();
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawBicubicPatch();
        program_map["bicubic"]->bind(TessellationShaderConfig::SOLID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawBicubicPatch();
        break;
    }

    static int ndxs[1][16] = { { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 } };
    drawControlPoints(1, ndxs, 16, bicubic_control_points);
}

float3 bicubic_cube_control_points[16+16+8+8+4+4] = {
    // top
    float3(-1,-1,-1),
    float3(-1,-0.5,-1),
    float3(-1, 0.5,-1),
    float3(-1,1,-1),

    float3(-0.5,-1,-1),
    float3(-0.5,-0.5,-1),
    float3(-0.5, 0.5,-1),
    float3(-0.5,1,-1),

    float3(0.5,-1,-1),
    float3(0.5,-0.5,-1),
    float3(0.5, 0.5,-1),
    float3(0.5,1,-1),

    float3(1,-1,-1),
    float3(1,-0.5,-1),
    float3(1, 0.5,-1),
    float3(1,1,-1),

    // bottom
    float3(-1,-1,1),  // ndx=16
    float3(-1,-0.5,1),
    float3(-1, 0.5,1),
    float3(-1,1,1),

    float3(-0.5,-1,1),
    float3(-0.5,-0.5,1),
    float3(-0.5, 0.5,1),
    float3(-0.5,1,1),

    float3(0.5,-1,1),
    float3(0.5,-0.5,1),
    float3(0.5, 0.5,1),
    float3(0.5,1,1),

    float3(1,-1,1),
    float3(1,-0.5,1),
    float3(1, 0.5,1),
    float3(1,1,1),

    // x=-1 side
    float3(-1,-1,-0.5),  //ndx=32
    float3(-1,-0.5,-0.5),
    float3(-1, 0.5,-0.5),
    float3(-1,1,-0.5),

    float3(-1,-1,0.5),
    float3(-1,-0.5,0.5),
    float3(-1, 0.5,0.5),
    float3(-1,1,0.5),

    // x=+1 side
    float3(1,-1,-0.5),
    float3(1,-0.5,-0.5),
    float3(1, 0.5,-0.5),
    float3(1,1,-0.5),

    float3(1,-1,0.5),
    float3(1,-0.5,0.5),
    float3(1, 0.5,0.5),
    float3(1,1,0.5),

    // y=-1 side
    float3(-0.5,-1,-0.5),  //ndx=48
    float3(-0.5,-1, 0.5),
    float3( 0.5,-1,-0.5),
    float3( 0.5,-1, 0.5),

    // y=+1 side
    float3(-0.5,1,-0.5),  //ndx=52
    float3(-0.5,1, 0.5),
    float3( 0.5,1,-0.5),
    float3( 0.5,1, 0.5),
};
const int bicubic_cube_control_points_count = 16+16+8+8+4+4;

const int bicubic_cube_ndxs[6][16] = {
    { 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15 },
    { 16+0,16+1,16+2,16+3, 16+4,16+5,16+6,16+7, 16+8,16+9,16+10,16+11, 16+12,16+13,16+14,16+15 },
    { 0,1,2,3, 32+0,32+1,32+2,32+3, 32+4,32+5,32+6,32+7, 16+0,16+1,16+2,16+3 },
    { 12,13,14,15, 40+0,40+1,40+2,40+3, 40+4,40+5,40+6,40+7, 16+12,16+13,16+14,16+15 },
    { 0,4,8,12, 32,48,50,40, 36,49,51,44, 0+16,4+16,8+16,12+16 },
    { 3,7,11,15, 35,52,54,43, 39,53,55,47, 19,23,27,31 },
};
const int bicubic_cube_patch_count = 6;

float3 *teapotVertexCopy;
float4x4 teapot_translate = float4x4(
                                     1,0,0,0,
                                     0,0,1,-1.5,
                                     0,1,0,0,
                                     0,0,0,1);

float3 *gumboVertexCopy;
float4x4 gumbo_transform = float4x4(-1,0,0,0,
                                    0,0,1,0,
                                    0,1,0,0,
                                    0,0,0,1);

float3 *monkeyVertexCopy;
float3 *monkeyNormalCopy;
float4x4 monkey_transform = float4x4(-1,0,0,0,
                                     0,0,1,0,
                                     0,1,0,0,
                                     0,0,0,1);

void glMultMatrix(const float4x4 m)
{
    glMultTransposeMatrixf(reinterpret_cast<const GLfloat*>(&m[0][0]));
}

static bool adjust_colocated_control_points = false;

static int reallyTrianglePatch(const int patch_ndxs[16])
{
    if (patch_ndxs[0] == patch_ndxs[1] &&
        patch_ndxs[0] == patch_ndxs[2] &&
        patch_ndxs[0] == patch_ndxs[3]) {
            return 1;
    }
    if (patch_ndxs[0] == patch_ndxs[4] &&
        patch_ndxs[0] == patch_ndxs[8] &&
        patch_ndxs[0] == patch_ndxs[12]) {
            return 2;
    }
    if (patch_ndxs[15] == patch_ndxs[12] &&
        patch_ndxs[15] == patch_ndxs[13] &&
        patch_ndxs[15] == patch_ndxs[14]) {
            return 3;
    }
    if (patch_ndxs[15] == patch_ndxs[11] &&
        patch_ndxs[15] == patch_ndxs[7] &&
        patch_ndxs[15] == patch_ndxs[3]) {
            return 4;
    }
    return 0;
}

static void emitTrianglePatch(int edge, const int patch_ndxs[16], const float3 cp[])
{
    int ndx;
    float3 v;

    switch (edge) {
    case 1:
        ndx = patch_ndxs[12];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[13];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[14];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[15];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[8];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[5];
        v = cp[ndx];
        ndx = patch_ndxs[6];
        v += cp[ndx];
        ndx = patch_ndxs[9];
        v += cp[ndx];
        ndx = patch_ndxs[10];
        v += cp[ndx];
        v /= 4;
        glVertex(v);
        ndx = patch_ndxs[11];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[4];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[7];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[0];
        glVertex(cp[ndx]);
        break;

    case 2:
        ndx = patch_ndxs[3];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[7];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[11];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[15];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[2];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[5];
        v = cp[ndx];
        ndx = patch_ndxs[6];
        v += cp[ndx];
        ndx = patch_ndxs[9];
        v += cp[ndx];
        ndx = patch_ndxs[10];
        v += cp[ndx];
        v /= 4;
        glVertex(v);
        ndx = patch_ndxs[14];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[1];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[13];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[0];
        glVertex(cp[ndx]);
        break;

    default:
        printf("handle edge = %d\n", edge);
        break;
    }
}

// This is meant to be called with a glBegin(GL_PATCHES)
static void emitPatch(const int patch_ndxs[16], const float3 cp[])
{
    if (!adjust_colocated_control_points) {
        for (int i=0; i<16; i++) {
            int ndx = patch_ndxs[i];
            glVertex(cp[ndx]);
        }
    } else {
        int ndx;
        static float scale = 0.999999;

        ndx = patch_ndxs[0];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[1];
        if (ndx == patch_ndxs[0]) {
            float3 b = lerp(cp[patch_ndxs[0]], cp[patch_ndxs[2]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[2];
        if (ndx == patch_ndxs[3]) {
            float3 b = lerp(cp[patch_ndxs[3]], cp[patch_ndxs[1]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[3];
        glVertex(cp[ndx]);

        ndx = patch_ndxs[4];
        if (ndx == patch_ndxs[0]) {
            float3 b = lerp(cp[patch_ndxs[0]], cp[patch_ndxs[8]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[5];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[6];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[7];
        if (ndx == patch_ndxs[3]) {
            float3 b = lerp(cp[patch_ndxs[3]], cp[patch_ndxs[11]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }

        ndx = patch_ndxs[8];
        if (ndx == patch_ndxs[12]) {
            float3 b = lerp(cp[patch_ndxs[12]], cp[patch_ndxs[4]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[9];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[10];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[11];
        if (ndx == patch_ndxs[15]) {
            float3 b = lerp(cp[patch_ndxs[15]], cp[patch_ndxs[7]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }

        ndx = patch_ndxs[12];
        glVertex(cp[ndx]);
        ndx = patch_ndxs[13];
        if (ndx == patch_ndxs[12]) {
            float3 b = lerp(cp[patch_ndxs[12]], cp[patch_ndxs[14]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[14];
        if (ndx == patch_ndxs[15]) {
            float3 b = lerp(cp[patch_ndxs[15]], cp[patch_ndxs[13]], scale);
            glVertex(b);
        } else {
            glVertex(cp[ndx]);
        }
        ndx = patch_ndxs[15];
        glVertex(cp[ndx]);
    }
}

static void drawBicubicPatchSet(TessellationShaderConfig::ShadeMode shade_mode,
                                int patchCount, const int patch_ndxs[][16],
                                const float3 cp[])
{
    bool needs_triangle_patches = false; // until proven otherwise

    const int verticesPerPatch = 16;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    program_map["bicubic"]->bind(shade_mode);
    if (explicit_tess) {
        glMultiTexCoord4fv(GL_TEXTURE0, quad_outer_lod);
        glMultiTexCoord2fv(GL_TEXTURE1, quad_inner_lod);
    }

    glBegin(GL_PATCHES); {
        int p = (show_patch % (patchCount+1));
        if (show_single_patch && p!=0) {
            p--;
            if (tri_sub && reallyTrianglePatch(patch_ndxs[p])) {
                needs_triangle_patches = true;
            } else {
                emitPatch(patch_ndxs[p], cp);
            }
        } else {
            for (int p=0; p<patchCount; p++) {
                if (tri_sub && reallyTrianglePatch(patch_ndxs[p])) {
                    needs_triangle_patches = true;
                } else {
                    emitPatch(patch_ndxs[p], cp);
                }
            }
        }
    } glEnd();

    if (needs_triangle_patches && tri_sub) {
        glPatchParameteri(GL_PATCH_VERTICES, 10);
        program_map["tricubic"]->bind(shade_mode);
        if (explicit_tess) {
            glMultiTexCoord4fv(GL_TEXTURE0, quad_outer_lod);
            glMultiTexCoord2fv(GL_TEXTURE1, quad_inner_lod);
        }
        glBegin(GL_PATCHES); {
            int p = (show_patch % (patchCount+1));
            if (show_single_patch && p!=0) {
                int edge = reallyTrianglePatch(patch_ndxs[p]);
                if (edge) {
                    emitTrianglePatch(edge, patch_ndxs[p], cp);
                }
            } else {
                for (int p=0; p<patchCount; p++) {
                    int edge = reallyTrianglePatch(patch_ndxs[p]);
                    if (edge) {
                        emitTrianglePatch(edge, patch_ndxs[p], cp);
                    }
                }
            }
        } glEnd();
    }
}

void drawTriangleSet(int triangleCount, const int3 tri_ndxs[],
                     int vertexCount, const float3 v[], const float3 n[])
{
    glBegin(GL_TRIANGLES); {
        for (int i=0; i<triangleCount; i++) {
            glNormal(n[tri_ndxs[i][0]]);
            glVertex(v[tri_ndxs[i][0]]);
            glNormal(n[tri_ndxs[i][0]]);
            glVertex(v[tri_ndxs[i][1]]);
            glNormal(n[tri_ndxs[i][0]]);
            glVertex(v[tri_ndxs[i][2]]);
        }
    } glEnd();
}

void drawTrianglePatchSet(const char *geometry_type,
                          TessellationShaderConfig::ShadeMode shade_mode,
                          int triangleCount, const int3 tri_ndxs[],
                          int vertexCount, const float3 v[], const float3 n[])
{
    const int verticesPerPatch = 3;
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);

    program_map[geometry_type]->bind(shade_mode);
    if (explicit_tess) {
        glMultiTexCoord3fv(GL_TEXTURE0, tri_outer_lod);
        glMultiTexCoord1fv(GL_TEXTURE1, tri_inner_lod);
    }

    glBegin(GL_PATCHES); {
        for (int i=0; i<triangleCount; i++) {
            float3 P1 = v[tri_ndxs[i][0]],
                   P2 = v[tri_ndxs[i][1]],
                   P3 = v[tri_ndxs[i][2]],
                   N1 = n[tri_ndxs[i][0]],
                   N2 = n[tri_ndxs[i][1]],
                   N3 = n[tri_ndxs[i][2]];

             if (!explicit_tess) {
                 float w12 = dot(P2-P1,N1),
                       w21 = dot(P1-P2,N2),
                       w23 = dot(P3-P2,N2),
                       w32 = dot(P2-P3,N3),
                       w31 = dot(P1-P3,N3),
                       w13 = dot(P3-P1,N1);

                 if (avoid_cracks) {
                     glMultiTexCoord3f(GL_TEXTURE0, 0,0,0);
                     glMultiTexCoord3f(GL_TEXTURE1, 0,0,0);
                 } else {
                     glMultiTexCoord3f(GL_TEXTURE0, w12, w21, w23);
                     glMultiTexCoord3f(GL_TEXTURE1, w32, w31, w12);
                 }

                 if (verbose) {
                     if (i == 384 || i == 385) {
                         printf("%d: w12=%f, w21=%f, w23=%f, w32=%f, w31=%f, w13=%f\n", i, w12, w21, w23, w32, w31, w13);
                         printf("tri: %d %d %d\n", tri_ndxs[i][0], tri_ndxs[i][1], tri_ndxs[i][2]);
                         for (int c=0; c<3; c++) {
                             printf("v[%d] = %f,%f,%f\n", c,
                                 float(v[tri_ndxs[i][c]].x),
                                 float(v[tri_ndxs[i][c]].y),
                                 float(v[tri_ndxs[i][c]].z));
                             printf("n[%d] = %f,%f,%f\n", c,
                                 float(n[tri_ndxs[i][c]].x),
                                 float(n[tri_ndxs[i][c]].y),
                                 float(n[tri_ndxs[i][c]].z));
                         }
                         float3 P1mP3 = P1-P3;
                         float3 P3mP1 = P3-P1;
                         printf("P1-P3 = %f,%f,%f\n", float(P1mP3.x), float(P1mP3.y), float(P1mP3.z));
                         printf("P3-P1 = %f,%f,%f\n", float(P3mP1.x), float(P3mP1.y), float(P3mP1.z));
                         printf("dot(P1mP3,N3) = %f\n", float(dot(P1mP3,N3)));
                         printf("dot(P3mP1,N3) = %f\n", float(dot(P3mP1,N3)));
                         float3 P2mP3 = P2-P3;
                         float3 P3mP2 = P3-P2;
                         printf("P2-P3 = %f,%f,%f\n", float(P2mP3.x), float(P2mP3.y), float(P2mP3.z));
                         printf("P3-P2 = %f,%f,%f\n", float(P3mP2.x), float(P3mP2.y), float(P3mP2.z));
                         printf("dot(P2mP3,N3) = %f\n", float(dot(P2mP3,N3)));
                         printf("dot(P3mP2,N3) = %f\n", float(dot(P3mP2,N3)));
                     }
                 }
             }

            glNormal(n[tri_ndxs[i][0]]);
            glVertex(v[tri_ndxs[i][0]]);
            glNormal(n[tri_ndxs[i][1]]);
            glVertex(v[tri_ndxs[i][1]]);
            glNormal(n[tri_ndxs[i][2]]);
            glVertex(v[tri_ndxs[i][2]]);
        }
    } glEnd();
}

void doTriangleSet(const char *geometry_type,
                   const float4x4 &xform,
                   int triangleCount, const int3 tri_ndxs[],
                   int vertexCount, const float3 v[], const float3 n[])
{
    glPushMatrix(); {
        glMultMatrix(xform);

        switch (wireframe) {
        case 0:
        case 1:
            drawTrianglePatchSet(geometry_type, shade_mode, triangleCount, tri_ndxs, vertexCount, v, n);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            drawTrianglePatchSet(geometry_type, shade_mode, triangleCount, tri_ndxs, vertexCount, v, n);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawTrianglePatchSet(geometry_type, TessellationShaderConfig::SOLID, triangleCount, tri_ndxs, vertexCount, v, n);
            break;
        }

        if (draw_control_points) {
            glUseProgram(0);

            glPolygonOffset(0,-2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if (draw_control_points > 1) {
                if (draw_control_points > 2) {
                    glColor3f(0.25,0.25,0);
                    glDepthFunc(GL_GEQUAL);
                    glDepthMask(0);
                    glLineWidth(1);
                    glEnable(GL_LINE_STIPPLE);
                    drawTriangleSet(triangleCount, tri_ndxs, vertexCount, v, n);
                    glDepthFunc(GL_LESS);
                    glDepthMask(1);
                    glLineWidth(line_width);
                    glDisable(GL_LINE_STIPPLE);
                }
                glColor3f(0.5,0.5,0);
                drawTriangleSet(triangleCount, tri_ndxs, vertexCount, v, n);
            }
            glEnable(GL_POLYGON_OFFSET_POINT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glColor3f(1,1,0);
            drawTriangleSet(triangleCount, tri_ndxs, vertexCount, v, n);
            glPolygonOffset(0,0);
        }
    } glPopMatrix();
}

void doMonkey(const char *geometry_type)
{
    doTriangleSet(geometry_type,
        monkey_transform,
        MonkeyHead_num_of_triangles, MonkeyHead_triangles, 
        MonkeyHead_num_of_vertices, monkeyVertexCopy, monkeyNormalCopy);
}

void doBicubicPatchSet(const float4x4 &xform,
                       int patchCount, const int patch_ndxs[][16],
                       int cpCount, const float3 cp[])
{
    glPushMatrix(); {
        glMultMatrix(xform);

        switch (wireframe) {
        case 0:
        case 1:
            drawBicubicPatchSet(shade_mode,
                patchCount, patch_ndxs, cp);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            drawBicubicPatchSet(shade_mode,
                patchCount, patch_ndxs, cp);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawBicubicPatchSet(TessellationShaderConfig::SOLID,
                patchCount, patch_ndxs, cp);
            break;
        }

        drawControlPoints(patchCount, patch_ndxs, cpCount, cp);
    } glPopMatrix();
}

void doBicubicCube()
{
    const float4x4 identity = float4x4(1,0,0,0,
                                       0,1,0,0,
                                       0,0,1,0,
                                       0,0,0,1);
    doBicubicPatchSet(identity,
                      bicubic_cube_patch_count, bicubic_cube_ndxs,
                      bicubic_cube_control_points_count, bicubic_cube_control_points);
}

void doBicubicTeapot()
{
    doBicubicPatchSet(teapot_translate,
                      teapotPatchCount, teapotPatch,
                      teapotVertexCount, teapotVertexCopy);
}

void doBicubicGumbo()
{
    doBicubicPatchSet(gumbo_transform,
                      gumboPatchCount, gumboPatch,
                      gumboVertexCount, gumboVertexCopy);
}

enum SceneType {
    ST_BICUBIC_QUAD_GUMBO,  // the elephant
    ST_BICUBIC_QUAD_TEAPOT,
    ST_PN_TRISET_MONKEY,
    ST_TRISET_MONKEY,
    ST_BICUBIC_QUAD_CUBE,
    ST_BICUBIC_TRI_PATCH,
    ST_BICUBIC_QUAD_PATCH,
    ST_LINEAR_TRIANGLE_PATCH,
    ST_LINEAR_QUAD_PATCH,
    ST_COUNT
} scene_type = ST_BICUBIC_QUAD_GUMBO;

const char *sceneNames[] = {
    "Elephant (bicubic patches)",
    "Teapot (bicubic patches)",
    "Smooth Monkey head (PN triangles)",
    "Flat Monkey head (linear triangles)",
    "Cube (bicubic patches)",
    "Single bezier triangle",
    "Single bicubic patch",
    "Linear triangle",
    "Linear quadrilateral",
};

void resetModels()
{
    teapotVertexCopy = new float3[teapotVertexCount];
    for (int i=0; i<teapotVertexCount; i++) {
        teapotVertexCopy[i] = teapotVertex[i];
    }

    gumboVertexCopy = new float3[gumboVertexCount];
    for (int i=0; i<gumboVertexCount; i++) {
        gumboVertexCopy[i] = gumboVertex[i];
    }

    monkeyVertexCopy = new float3[MonkeyHead_num_of_vertices];
    monkeyNormalCopy = new float3[MonkeyHead_num_of_vertices];
    for (int i=0; i<MonkeyHead_num_of_vertices; i++) {
        monkeyVertexCopy[i] = MonkeyHead_vertices[i];
        monkeyNormalCopy[i] = MonkeyHead_normals[i];
    }
}

void doGraphics()
{
    configPolygonMode();
    switch (scene_type) {
    case ST_LINEAR_TRIANGLE_PATCH:
        doTrianglePatch();
        break;
    case ST_LINEAR_QUAD_PATCH:
        doQuadrilateralPatch();
        break;
    case ST_BICUBIC_TRI_PATCH:
        doBicubicTriangle();
        break;
    case ST_BICUBIC_QUAD_PATCH:
        doBicubicPatch();
        break;
    case ST_BICUBIC_QUAD_TEAPOT:
        doBicubicTeapot();
        break;
    case ST_BICUBIC_QUAD_GUMBO:
        doBicubicGumbo();
        break;
    case ST_BICUBIC_QUAD_CUBE:
        doBicubicCube();
        break;
    case ST_TRISET_MONKEY:
        doMonkey("tri");
        break;
    case ST_PN_TRISET_MONKEY:
        doMonkey("pntri");
        break;
    default:
        assert(!"bogus scene_type");
        break;
    }
}

void
recalcModelView()
{
  build_rotmatrix(m, curquat);
  float4x4 rotate = float4x4(m[0][0], m[1][0], m[2][0], m[3][0],
                             m[0][1], m[1][1], m[2][1], m[3][1],
                             m[0][2], m[1][2], m[2][2], m[3][2],
                             m[0][3], m[1][3], m[2][3], m[3][3]);
  float4x4 trans = float4x4(1,0,0,0,
                            0,1,0,0,
                            0,0,1,ztrans,
                            0,0,0,1);
  world_to_view = trans;
  view_to_clip = mul(projection_matrix, mul(trans, rotate));
  newModel = 0;
}

static void drawLight()
{
    if (shade_mode == TessellationShaderConfig::PHONG) {
        glPushMatrix(); {
            glTranslatef(light_pos.x, light_pos.y, light_pos.z);
            glColor3f(1,1,0);
            glCallList(sphereDlist);
        } glPopMatrix();
    }
}

void
display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (newModel) {
        recalcModelView();
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); {
        glMultMatrix(world_to_view);
        glPushMatrix(); {
            glMultMatrixf(&m[0][0]);
            doGraphics();
        } glPopMatrix();
        drawLight();
    } glPopMatrix();

    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    (void) handleFPS(&gl_fps_context);
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();

    if (read_depth) {
        GLfloat z;
        glReadPixels(read_depth_x, read_depth_y, 1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        printf("depth @ %d,%d = %f\n", read_depth_x, read_depth_y, z);
        read_depth = false;
    }
}

void special(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:
        light_height += 0.1;
        break;
    case GLUT_KEY_DOWN:
        light_height -= 0.1;
        break;
    case GLUT_KEY_RIGHT:
        light_angle -= 3;
        break;
    case GLUT_KEY_LEFT:
        light_angle += 3;
        break;
    default:
        return;
    }
    updateLightPos();
    glutPostRedisplay();
}

void
keyboard(unsigned char c, int x, int y)
{
    switch (c) {
    case ',':
        if (show_patch > 0) {
            show_patch--;
        }
        printf("show_patch = %d\n", show_patch);
        break;
    case '.':
        show_patch++;
        printf("show_patch = %d\n", show_patch);
        break;
    case 'b':
        background_color++;
        background();
        break;
    case 'z':
        adjust_colocated_control_points = !adjust_colocated_control_points;
        printf("adjust_colocated_control_points = %d\n", adjust_colocated_control_points);
        break;
    case 'a':
        tri_sub = !tri_sub;
        printf("tri_sub = %d\n", tri_sub);
        break;
    case 'm':
        switch (shade_mode) {
        case TessellationShaderConfig::UV:
            shade_mode = TessellationShaderConfig::DIFFUSE;
            break;
        case TessellationShaderConfig::DIFFUSE:
            shade_mode = TessellationShaderConfig::PHONG;
            break;
        case TessellationShaderConfig::PHONG:
            shade_mode = TessellationShaderConfig::UV;
            break;
        }
        printf("shade_mode = %s (%d)\n",
            TessellationShaderConfig::shadeModeToString(shade_mode), shade_mode);
        break;
    case 's':
        show_single_patch = !show_single_patch;
        break;
    case 'v':
        enable_sync = !enable_sync;
        requestSynchornizedSwapBuffers(enable_sync);
        break;
#define GL_FORCE_SOFTWARE_NV              0x6007
    case 'r':
        if (glIsEnabled(GL_FORCE_SOFTWARE_NV)) {
            printf("Hardware...\n");
            glDisable(GL_FORCE_SOFTWARE_NV);
        } else {
            printf("Software rasterization...\n");
            glEnable(GL_FORCE_SOFTWARE_NV);
        }
        break;
    case 'f':
        toggleFPS();
        break;
    case 'F':
        face_mode = (face_mode + 1) % 3;
        printf("face_mode = %d\n", face_mode);
        updateFaceMode();
        break;
    case 'R':
        resetModels();
        break;
    case '1':
    case '2':
    case '3':
    case '4':
        quad_outer_lod[c - '1'] += 0.5;
        break;
    case '!':
    case '@':
    case '#':
    case '$':
        quad_outer_lod[c - '!'] -= 0.5;
        break;
    case '5':
    case '6':
        quad_inner_lod[c - '5']++;
        break;
    case '%':
        quad_inner_lod[0]--;
        break;
    case '^':
        quad_inner_lod[1]--;
        break;
    case '7':
    case '8':
    case '9':
        tri_outer_lod[c - '7'] += 0.5;
        break;
    case '&':
        tri_outer_lod[0] -= 0.5;
        if (tri_outer_lod[0] < 1) {
            tri_outer_lod[0] = 1;
        }
        break;
    case '*':
        tri_outer_lod[1] -= 0.5;
        if (tri_outer_lod[1] < 1) {
            tri_outer_lod[1] = 1;
        }
        break;
    case '(':
        tri_outer_lod[2] -= 0.5;
        if (tri_outer_lod[2] < 1) {
            tri_outer_lod[2] = 1;
        }
        break;
    case '0':
        tri_inner_lod[0]++;
        break;
    case ')':
        tri_inner_lod[0]--;
        break;
    case '`':
        printf("reloading shaders...");
        fflush(stdout);
        {
            int start = glutGet(GLUT_ELAPSED_TIME);
            loadShaders();
            int done = glutGet(GLUT_ELAPSED_TIME);
            printf(" for %.2f seconds\n", (done-start)/1000.0);
        }
        updateDetail();
        break;
    case 27:  /* Esc quits */
        exit(0);
        return;
    case 13:  /* Enter redisplays */
        break;
    case 'c':
        draw_control_points = (draw_control_points + 1) % 4; // cycle
        break;
    case 'C':
        draw_control_points--;
        if (draw_control_points < 0) {
            draw_control_points = 3;
        }
        break;
    case 'l':
        line_width = ((line_width+1) % 6) + 1;
        printf("line_width = %d\n", line_width);
        glLineWidth(line_width);
        break;
    case 'L':
        line_width = ((line_width+3) % 6) + 1;
        printf("line_width = %d\n", line_width);
        glLineWidth(line_width);
        break;
    case 'p':
        scene_type = SceneType(int(scene_type)+1);
        if (scene_type >= ST_COUNT) {
            scene_type = SceneType(0);
        }
        break;
    case 'P':
        scene_type = SceneType(int(scene_type)-1);
        if (scene_type < SceneType(0)) {
            scene_type = SceneType(ST_COUNT-1);
        }
        break;
    case 'd':
        if (pixelsPerSubEdge >= 1) {
            pixelsPerSubEdge--;
            updateDetail();
        }
        printf("pixelsPerSubEdge = %f\n", pixelsPerSubEdge);
        break;
    case 'D':
        pixelsPerSubEdge++;
        printf("pixelsPerSubEdge = %f\n", pixelsPerSubEdge);
        updateDetail();
        break;
    case 'x':
        explicit_tess = !explicit_tess;
        printf("explicit tessellation = %d\n", explicit_tess);
        break;
    case 'n':
        wireframe = 0;
        draw_control_points = 0;
        break;
    case 'u':
        avoid_cracks = !avoid_cracks;
        printf("avoid_cracks = %d\n", avoid_cracks);
        break;
    case 'w':
        wireframe = (wireframe + 1) % 4;
        break;
    case 'W':
        wireframe--;
        if (wireframe < 0) {
            wireframe = 3;
        }
        configPolygonMode();
        break;
    case '+':
        ztrans += 0.1;
        break;
    case '-':
        ztrans -= 0.1;
        break;
    default:
        return;
    }
    glutPostRedisplay();
}

void menu(int item)
{
    keyboard((unsigned char)item, 0, 0);
}

void change_scene(int item)
{
    scene_type = SceneType(item);
    glutPostRedisplay();
}

void makeMenu()
{
    int scene_menu = glutCreateMenu(change_scene);
    for (int i = 0; i<int(ST_COUNT); i++) {
        glutAddMenuEntry(sceneNames[i], SceneType(i));
    }
    int quad_lod_control = glutCreateMenu(menu);
    glutAddMenuEntry("[1] Quad X0++", '1');
    glutAddMenuEntry("[!] Quad X0--", '!');
    glutAddMenuEntry("[2] Quad Y0++", '2');
    glutAddMenuEntry("[@] Quad Y0--", '@');
    glutAddMenuEntry("[3] Quad X1++", '3');
    glutAddMenuEntry("[#] Quad X1--", '#');
    glutAddMenuEntry("[4] Quad Y1++", '4');
    glutAddMenuEntry("[$] Quad Y1--", '$');
    int tri_lod_control = glutCreateMenu(menu);
    glutAddMenuEntry("[7] Tri S0++", '7');
    glutAddMenuEntry("[&] Tri S0--", '&');
    glutAddMenuEntry("[8] Tri S1++", '8');
    glutAddMenuEntry("[*] Tri S1--", '*');
    glutAddMenuEntry("[9] Tri S2++", '9');
    glutAddMenuEntry("[(] Tri S2++", '(');
    glutCreateMenu(menu);
    glutAddSubMenu("Scene...", scene_menu);
    glutAddSubMenu("Explicit quadrilateral level-of-detail...", quad_lod_control);
    glutAddSubMenu("Explicit triangle level-of-detail...", tri_lod_control);
    glutAddMenuEntry("[p] Cycle patch type", 'p');
    glutAddMenuEntry("[w] Toggle wireframe", 'w');
    glutAddMenuEntry("[c] Toggle control points", 'c');
    glutAddMenuEntry("[a] Substitute triangle bicubic patches for Bezier triangles", 'a');
    glutAddMenuEntry("[m] Toggle shading mode", 'm');
    glutAddMenuEntry("[n] Normal rendering (disable overlaid info)", 'n');
    glutAddMenuEntry("[x] Toggle explicit vs. adaptive tessellation", 'x');
    glutAddMenuEntry("[l] Cycle line widths", 'l');
    glutAddMenuEntry("[+] Translate forward", '+');
    glutAddMenuEntry("[-] Translate backward", '-');
    glutAddMenuEntry("[Esc] Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int w, int h)
{
    reshapeFPScontext(&gl_fps_context, w, h);

    window_width = w;
    window_height = h;
    wh = float2(w,h);
    glViewport(0, 0, w, h);
    if (verbose) {
        printf("window = %dx%d\n", window_width, window_height);
    }
    updateDetail();
}

struct ActiveControlPoint {
    float2 mouse_xy;
    float closeness;
    float depth;

    float4x4 current_transform;
    bool needs_inverse;
    float4x4 inv_transform;

    int ndx;
    int point_count;
    float3 *control_points;

    ActiveControlPoint();
    ActiveControlPoint(float2 xy, float4x4 trans, float closeness = 10);
    ActiveControlPoint & operator = (const ActiveControlPoint & src);
    void set(float2 window_space_xy);
};

ActiveControlPoint::ActiveControlPoint()
{
    ndx = ~0;
}

void ActiveControlPoint::set(float2 window_space_xy)
{
    float4 p = float4(window_space_xy, depth, 1);

    if (needs_inverse) {
        inv_transform = inverse(current_transform);
        needs_inverse = false;
    }

    p = mul(inv_transform, p);
    control_points[ndx] = p.xyz / p.w;
}

ActiveControlPoint::ActiveControlPoint(float2 xy, float4x4 trans, float closeness_)
    : mouse_xy(xy)
    , current_transform(trans)
    , closeness(closeness_)
    , needs_inverse(true)
{
    switch (scene_type) {
    case ST_LINEAR_TRIANGLE_PATCH:
        control_points = triangle_control_points;
        point_count = 3;
        break;
    case ST_LINEAR_QUAD_PATCH:
        control_points = quadrilateral_control_points;
        point_count = 4;
        break;
    case ST_BICUBIC_TRI_PATCH:
        control_points = bicubic_tri_control_points;
        point_count = 10;
        break;
    case ST_BICUBIC_QUAD_PATCH:
        control_points = bicubic_control_points;
        point_count = 16;
        break;
    case ST_BICUBIC_QUAD_TEAPOT:
        control_points = teapotVertexCopy;
        point_count = teapotVertexCount;
        current_transform = mul(current_transform, teapot_translate);
        break;
    case ST_BICUBIC_QUAD_GUMBO:
        control_points = gumboVertexCopy;
        point_count = gumboVertexCount;
        current_transform = mul(current_transform, gumbo_transform);
        break;
    case ST_BICUBIC_QUAD_CUBE:
        control_points = bicubic_cube_control_points;
        point_count = bicubic_cube_control_points_count;
        break;
    case ST_TRISET_MONKEY:
    case ST_PN_TRISET_MONKEY:
        control_points = monkeyVertexCopy;
        point_count = MonkeyHead_num_of_vertices;
        current_transform = mul(current_transform, monkey_transform);
        break;
    default:
        assert(!"bogus scene_type");
        break;
    }
    float closest_so_far = closeness;
    ndx = ~0;
    for (int i=0; i<point_count; i++) {
        float4 p = mul(current_transform, float4(control_points[i], 1));
        p /= p.w;
        float len = length(p.xy - mouse_xy);
        if (len <= closest_so_far) {
            ndx = i;
            closest_so_far = len;
            depth = p.z;
        }
    }
}

ActiveControlPoint & ActiveControlPoint::operator = (const ActiveControlPoint & src)
{
    if (this != &src) {
        mouse_xy = src.mouse_xy;
        closeness = src.closeness;
        current_transform = src.current_transform;
        control_points = src.control_points;
        ndx = src.ndx;
        needs_inverse = src.needs_inverse;
        inv_transform = src.inv_transform;
        depth = src.depth;
    }
    return *this;
}

void
animate(void)
{
  add_quats(lastquat, curquat, curquat);
  newModel = 1;
  glutPostRedisplay();
}

void
visibility(int state)
{
  if (state == GLUT_VISIBLE) {
    if (spinning) {
      glutIdleFunc(animate);
    }
  } else {
    glutIdleFunc(NULL);
  }
}

void
stopSpinning(void)
{
  spinning = 0;
  glutIdleFunc(NULL);
}

ActiveControlPoint active_control_point;

bool zooming = false;
int zoom_trans = 0;

void
mouse(int button, int state, int mouse_space_x, int mouse_space_y)
{
    if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN) {
            float w = wh.x,
                  h = wh.y;
            float2 mousespace_xy = float2(mouse_space_x, mouse_space_y);
            float4x4 clip_to_mouse = float4x4(w/2,0,0,w/2,
                                              0,-h/2,0,h/2, // compute "h-y" to get to GL's lower-left origin
                                              0,0,1,0,
                                              0,0,0,1);

            active_control_point = ActiveControlPoint(mousespace_xy, mul(clip_to_mouse, view_to_clip));
            if (active_control_point.ndx != ~0) {
                printf("mouse point=%d (%d,%d) xy = %f,%f\n",
                    int(active_control_point.ndx), mouse_space_x, mouse_space_y,
                    float(active_control_point.control_points[active_control_point.ndx].x),
                    float(active_control_point.control_points[active_control_point.ndx].y));
                glutPostRedisplay();
            } else {
                printf("no hit\n");
            }
        } else {
            active_control_point.ndx = ~0;
        }
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (glutGetModifiers() & GLUT_ACTIVE_ALT) {
                read_depth = true;;
                read_depth_x = mouse_space_x;
                read_depth_y = window_height - mouse_space_y;
                glutPostRedisplay();
            }
            if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
                zooming = true;
                zoom_trans = mouse_space_y;
            } else {
                stopSpinning();
                moving = 1;
                beginx = mouse_space_x;
                beginy = mouse_space_y;
            }
        } else {
            zooming = false;
            moving = 0;
        }
    }
}

void
motion(int mouse_space_x, int mouse_space_y)
{
    if (active_control_point.ndx != ~0) {
        float2 mousespace_xy = float2(mouse_space_x, mouse_space_y);
        active_control_point.set(mousespace_xy);
        glutPostRedisplay();
    }
    if (moving) {
        float x = mouse_space_x,
              y = mouse_space_y;
        trackball(lastquat,
            (2.0 * beginx - wh.x) / wh.x,
            (wh.y - 2.0 * beginy) / wh.y,
            (2.0 * x - wh.x) / wh.x,
            (window_height - 2.0 * y) / wh.y
            );
        beginx = x;
        beginy = y;
        spinning = 1;
        newModel = 1;
        glutIdleFunc(animate);
    }
    if (zooming) {
        ztrans += 6.0 * (mouse_space_y - zoom_trans) / wh.y;
        if (verbose) {
            printf("ztrans = %f\n", ztrans);
        }
        zoom_trans = mouse_space_y;
        newModel = 1;
        glutPostRedisplay();
    }
}

float4x4 makeObjectTransform(int vertexCount, const float3 vertex[])
{
    // Compute a good transform matrix for gumbo
#undef min
#undef max
    float3 vmin = vertex[0],
           vmax = vertex[0];
    for (int i=1; i<vertexCount; i++) {
        vmin = min(vmin, vertex[i]);
        vmax = max(vmax, vertex[i]);
    }
    float3 range = vmax - vmin;
    float scale = 4.0/max(range[0], max(range[1], range[2]));
    float3 mid = range/2 + vmin;
    float4x4 t = float4x4(1,0,0,-mid[0],
                          0,1,0,-mid[1],
                          0,0,1,-mid[2],
                          0,0,0,1);
    float4x4 s = float4x4(scale,0,0,0,
                          0,scale,0,0,
                          0,0,scale,0,
                          0,0,0,1);
    float4x4 result = mul(s,t);
    return result;
}

void makeObjectTransforms()
{
    gumbo_transform = mul(gumbo_transform,makeObjectTransform(gumboVertexCount, gumboVertex));
    monkey_transform = makeObjectTransform(MonkeyHead_num_of_vertices, monkeyVertexCopy);
}

#if !defined(_WIN32)
#include <strings.h>
#define stricmp strcasecmp
#endif

int
main(int argc, char **argv)
{
    glutInitDisplayString("rgb double depth samples~4");
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInit(&argc, argv);

    for (int i=1; i<argc; i++) {
        if (!stricmp("-nosync", argv[i])) {
            printf("requesting no vertical retrace sync\n");
            enable_sync = 0;
        } else
        if (!strcmp("-vsync", argv[i])) {
            printf("requesting vertical retrace sync\n");
            enable_sync = 1;
        } else
        if (!strcmp("-v", argv[i])) {
            printf("verbose on\n");
            verbose = true;
        } else {
            printf("unrecognized (ignored) option: %s\n", argv[i]);
        }
    }

    glutCreateWindow("OpenGL 4 programmable tessellation");

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Versions: %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutVisibilityFunc(visibility);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    initFPScontext(&gl_fps_context, fps_render_mode);
    colorFPS(0,1,0);
    scaleFPS(2);

    resetModels();
    makeObjectTransforms();
#if 0
    makeEdgeMap(MonkeyHead_num_of_triangles, MonkeyHead_triangles, 
                MonkeyHead_num_of_vertices, monkeyVertexCopy);
#endif

    makeMenu();

    initOpenGL();
    initGraphics();
    requestSynchornizedSwapBuffers(enable_sync);

    glutMainLoop();
    return 0;
}
