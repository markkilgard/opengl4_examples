
// mandelbrot.cpp - OpenGL 4.x mandelbrot explorer example

#if defined(WIN32)
#  include <windows.h>
#  include <GL/glew.h>
#  include <GL/glut.h>
#elif defined(MACOS)
#  include <GL/glew.h>
#  include <GLUT/glut.h>
#elif defined(UNIX)
#  include <GL/glew.h>
#  include <GL/glut.h>
#endif

#include <string>
#include <sstream>
#include <math.h>

#include "read_text_file.h"
#include "nv_dds.h"
#include "shaders.h"

#include "showfps.h"
#include "request_vsync.h"

void reshape(int w, int h);

FPSusage fps_render_mode = FPS_USAGE_TEXTURE;
FPScontext gl_fps_context;

GLuint spProgram;
GLuint dpProgram;
GLuint program;
GLuint fbo;

int width = 768;
int height = 512;
float ar = width / (float)height;
double scale;
double offsetX;
double offsetY;
float max_iterations;
float juliaOffsetX;
float juliaOffsetY;
bool animating = true;

int enable_sync = 0;

int tex_w = 512;
int tex_h = 512;
int output_w = 7200;
int output_h = 7200;

std::string fragment_shader_source;
std::string vertex_shader_source;

bool fp64supported = false;
bool julia = false;

/* Scaling and rotation state. */
int anchor_x = 0,
    anchor_y = 0;  /* Anchor for rotation and zooming. */
int scale_y = 0, 
    rotate_x = 0;  /* Prior (x,y) location for zooming/scaling (vertical) or rotation (horizontal)? */
int zooming = 0;  /* Are we zooming currently? */
int rotating = 0;  /* Are we rotating (zooming) currently? */

/* Sliding (translation) state. */
float slide_x = 0,
      slide_y = 0;  /* Prior (x,y) location for sliding. */
int sliding = 0;  /* Are we sliding currently? */
int julia_sliding = 0;

typedef double Transform3x2[2][3];

Transform3x2 view;

void tr3x2_mult(Transform3x2 dst, const Transform3x2 a, const Transform3x2 b)
{
  Transform3x2 result;

  result[0][0] = a[0][0]*b[0][0] + a[0][1]*b[1][0];
  result[0][1] = a[0][0]*b[0][1] + a[0][1]*b[1][1];
  result[0][2] = a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2];

  result[1][0] = a[1][0]*b[0][0] + a[1][1]*b[1][0];
  result[1][1] = a[1][0]*b[0][1] + a[1][1]*b[1][1];
  result[1][2] = a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2];

  dst[0][0] = result[0][0];
  dst[0][1] = result[0][1];
  dst[0][2] = result[0][2];
  dst[1][0] = result[1][0];
  dst[1][1] = result[1][1];
  dst[1][2] = result[1][2];
}

void tr3x2_translate(Transform3x2 dst, double x, double y)
{
  dst[0][0] = 1;
  dst[0][1] = 0;
  dst[0][2] = x;
  dst[1][0] = 0;
  dst[1][1] = 1;
  dst[1][2] = y;
}

void tr3x2_ortho(Transform3x2 dst, double l, double r, double b, double t)
{
  float tx = - (r+l) / (r-l),
        ty = - (t+b) / (t-b);
  dst[0][0] = 2/(r-l);
  dst[0][1] = 0;
  dst[0][2] = tx;
  dst[1][0] = 0;
  dst[1][1] = 2/(t-b);
  dst[1][2] = ty;
}


void tr3x2_scale(Transform3x2 dst, double x, double y)
{
  dst[0][0] = x;
  dst[0][1] = 0;
  dst[0][2] = 0;

  dst[1][0] = 0;
  dst[1][1] = y;
  dst[1][2] = 0;
}

void tr3x2_rotate(Transform3x2 dst, double angle)
{
  float radians = angle*3.14159/180.0,
        s = sin(radians),
        c = cos(radians);

  dst[0][0] = c;
  dst[0][1] = -s;
  dst[0][2] = 0;
  dst[1][0] = s;
  dst[1][1] = c;
  dst[1][2] = 0;
}

void initModelAndViewMatrices()
{
  tr3x2_translate(view, 0, 0);
}

enum PrecisionMode {
    SinglePrecisionMode,
    DoublePrecisionMode,
    VerticalSplitMode,
    HorizontalSplitMode,
    CheckerboardMode,
    UndefinedMode,
};

PrecisionMode precisionMode = SinglePrecisionMode;

#define OUTPUT_IMAGE 0

void resetSettings()
{
    initModelAndViewMatrices();
    juliaOffsetX = 0.0;
    juliaOffsetY = 0.0;

    if (julia) {
        scale = 2.0;
        offsetX = 0.0;
    } else {
        scale = 1.2;
        offsetX = -0.7;
    }

    offsetY = 0.0;
    max_iterations = 100.0;
}

void saveSettings()
{
    FILE *fp = fopen("settings.b", "wb");
    if (fp) {
        fwrite(&scale, sizeof(scale), 1, fp);
        fwrite(&offsetX, sizeof(offsetX), 1, fp);
        fwrite(&offsetY, sizeof(offsetY), 1, fp);
        fwrite(&max_iterations, sizeof(max_iterations), 1, fp);
        fwrite(&julia, sizeof(julia), 1, fp);
        fwrite(&juliaOffsetX, sizeof(juliaOffsetX), 1, fp);
        fwrite(&juliaOffsetY, sizeof(juliaOffsetY), 1, fp);
        fclose(fp);
    }
}

void loadSettings()
{
    FILE *fp = fopen("settings.b", "rb");
    if (fp) {
        fread(&scale, sizeof(scale), 1, fp);
        fread(&offsetX, sizeof(offsetX), 1, fp);
        fread(&offsetY, sizeof(offsetY), 1, fp);
        fread(&max_iterations, sizeof(max_iterations), 1, fp);
        fread(&julia, sizeof(julia), 1, fp);
        fread(&juliaOffsetX, sizeof(juliaOffsetX), 1, fp);
        fread(&juliaOffsetY, sizeof(juliaOffsetY), 1, fp);
        fclose(fp);
    } else {
        resetSettings();
    }
}

void updateProgram()
{
    glUseProgram(program);
    glUniform2f(glGetUniformLocation(program, "aspectRatio"), ar, 1.0);
    glUniform1f(glGetUniformLocation(program, "max_iterations"), max_iterations);
    glUniform1i(glGetUniformLocation(program, "julia"), (int)julia);
    glUniform2f(glGetUniformLocation(program, "juliaOffsets"), juliaOffsetX, juliaOffsetY);
    
    if (program == dpProgram) {
        GLdouble mat2x3[6];
        glUniform1d(glGetUniformLocation(program, "scale"), scale);
        glUniform2d(glGetUniformLocation(program, "offset"), offsetX, offsetY);
        glUniformMatrix2x3dv(glGetUniformLocation(program, "matrix"), 1, GL_FALSE, mat2x3);
    } else {
        GLfloat mat2x3[6];
        glUniform1f(glGetUniformLocation(program, "scale"), (float)scale);
        glUniform2f(glGetUniformLocation(program, "offset"), (float)offsetX, (float)offsetY);
        glUniformMatrix2x3fv(glGetUniformLocation(program, "matrix"), 1, GL_FALSE, mat2x3);
    }
}

void updateMatrix(float ds_dx, float dt_dy)
{
    Transform3x2 s, t, c, all;

    tr3x2_scale(s, ds_dx, dt_dy);
    tr3x2_translate(t, -1, -1);
    tr3x2_mult(c, t, s);
    tr3x2_mult(all, c, view);

    if (program == dpProgram) {
        GLdouble mat2x3[6];
        mat2x3[0] = all[0][0];
        mat2x3[1] = all[0][1];
        mat2x3[2] = all[0][2];
        mat2x3[3] = all[1][0];
        mat2x3[4] = all[1][1];
        mat2x3[5] = all[1][2];
        glUniformMatrix2x3dv(glGetUniformLocation(program, "matrix"), 1, GL_FALSE, mat2x3);
    } else {
        GLfloat mat2x3[6];
        mat2x3[0] = all[0][0];
        mat2x3[1] = all[0][1];
        mat2x3[2] = all[0][2];
        mat2x3[3] = all[1][0];
        mat2x3[4] = all[1][1];
        mat2x3[5] = all[1][2];
        glUniformMatrix2x3fv(glGetUniformLocation(program, "matrix"), 1, GL_FALSE, mat2x3);
    }
}

void init()
{
    glewInit();
    loadSettings();

    if (!GLEW_ARB_shader_objects) {
        printf("Required extensions are not supported by current GPU.\n");
        exit(0);
    }

    // GLEW_ARB_shader_fp64 and glewIsSupported("GL_ARB_gpu_shader_fp64") both fail for
    // some reason even when the extension is supported, fall back to searching the 
    // extension string
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "GL_ARB_gpu_shader_fp64") != 0)
        fp64supported = true;

    int numSamples;
    glGetIntegerv(GL_SAMPLES_ARB, &numSamples);
    if (numSamples > 1 && GLEW_ARB_sample_shading) 
        glMinSampleShadingARB(1.0);

#if OUTPUT_IMAGE
    if (!GLEW_ARB_framebuffer_object) {
        printf("Required extensions are not supported by current GPU.\n");
        exit(0);
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("fbo error\n");
        exit(0);
    }

    resetSettings();
    reshape(tex_w, tex_h);
#endif

    vertex_shader_source = read_text_file("shaders/vertex.glsl");
    fragment_shader_source = read_text_file("shaders/mandelbrot.glsl");

    spProgram = glCreateProgram();
    addShader(spProgram, GL_VERTEX_SHADER, vertex_shader_source);
    addShader(spProgram, GL_FRAGMENT_SHADER, fragment_shader_source);
    linkProgram(spProgram);

    if (fp64supported) {
        dpProgram = glCreateProgram();
        addShader(dpProgram, GL_VERTEX_SHADER, vertex_shader_source);
        addShader(dpProgram, GL_FRAGMENT_SHADER, "#define USE_DOUBLE_PRECISION\n\n" + fragment_shader_source);
        linkProgram(dpProgram);
    } else {
        dpProgram = 0;
    }

    program = spProgram;

    nv_dds::CDDSImage image;
    image.load("gradient4.dds");
    if (!image.is_valid()) {
        printf("could not load gradient file gradient4.dds\n");
        exit(1);
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    image.upload_texture1D();

    glEnableClientState(GL_VERTEX_ARRAY);
}

// draw large quad made up of many tiny quads to work around TDR issue
void draw_quad(double width, double height, int rows, int columns)
{
    float dx = width / columns;
    float dy = height / rows;

    float ds = 2.0 / columns;
    float dt = 2.0 / rows;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            bool evenRow = (row % 2 == 0);
            bool evenCol = (col % 2 == 0);

            switch (precisionMode) {
            case SinglePrecisionMode:
                program = spProgram;
                break;
            case DoublePrecisionMode:
                program = dpProgram;
                break;
            case VerticalSplitMode:
                program = dpProgram;
                if (col >= (columns / 2))
                    program = spProgram;
                break;
            case HorizontalSplitMode:
                program = dpProgram;
                if (row >= (rows / 2))
                    program = spProgram;
                break;
            case CheckerboardMode:
                program = dpProgram;
                if (evenRow && !evenCol || !evenRow && evenCol)
                    program = spProgram;
                break;
            }
            updateProgram();
            updateMatrix(2.0/width, 2.0/height);

            float x = col * dx,
                  x2 = (col+1) * dx;
            float y = row * dy,
                  y2 = (row+1) * dy;
            glBegin(GL_TRIANGLE_STRIP); {
                glVertex2f(x, y);
                glVertex2f(x2, y);
                glVertex2f(x, y2);
                glVertex2f(x2, y2);
            } glEnd();
        }
    }

    switch (precisionMode) {
        case SinglePrecisionMode:
        case DoublePrecisionMode:
            break;
        case VerticalSplitMode:
            glUseProgram(0);
            glColor3f(0,0,1);
            glBegin(GL_LINES); {
                glVertex2f(width/2, height);
                glVertex2f(width/2, 0);
            } glEnd();
            break;
        case HorizontalSplitMode:
            glUseProgram(0);
            glColor3f(0,0,1);
            glBegin(GL_LINES); {
                glVertex2f(0, height/2);
                glVertex2f(width, height/2);
            } glEnd();
            break;
        case CheckerboardMode:
            glUseProgram(0);
            glColor3f(0,0,1);
            glBegin(GL_LINES); {
                for (int i=1; i<columns; i++) {
                    glVertex2f(i*dx, height);
                    glVertex2f(i*dx, 0);
                }
                for (int i=1; i<rows; i++) {
                    glVertex2f(0, i*dy);
                    glVertex2f(width, i*dy);
                }
            } glEnd();
            break;
    }
}

void generateSection(int sx, int sy, int resx, int resy, const char *filename)
{
    assert(sx < (resx / (float)tex_w));
    assert(sy < (resy / (float)tex_h));

    int x = sx * tex_w;
    int y = sy * tex_h;

#if 0
    float tx = x / (float)resx * 2.0f - 1.0f;
    float ty = y / (float)resy * 2.0f - 1.0f;
    float tw = tex_w / (float)resx * 2.0f;
    float th = tex_h / (float)resy * 2.0f;
#endif

    int w = min(tex_w, resx-x);
    int h = min(tex_h, resy-y);

    reshape(w, h);

    glBegin(GL_TRIANGLE_STRIP); {
        glVertex2f(0.0, 0.0);
        glVertex2f(tex_w, 0.0);
        glVertex2f(0.0, tex_h);
        glVertex2f(tex_w, tex_h);
    } glEnd();

    int imageSize = w * h * 3;
    GLubyte *pixels = new GLubyte[imageSize];
    glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    nv_dds::CDDSImage image;
    image.create_textureFlat(GL_RGB, 3, nv_dds::CTexture(w, h, 1, imageSize, pixels));
    image.save(filename);

    delete [] pixels;
}

void display()
{
    glClearColor(0,1,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
#if OUTPUT_IMAGE
    int rows = (int)floorf(output_w / (float)tex_w + 0.5f);
    int cols = (int)floorf(output_h / (float)tex_h + 0.5f);

    for (int x = 0; x < cols; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            char filename[256];
            sprintf(filename, "results\\mandelbrot_y%d_x%d.dds", y, x);
            generateSection(x, y, output_w, output_h, filename);
        }
    }

    exit(0);
#else
    if (precisionMode == CheckerboardMode) {
        draw_quad(width, height, 5, 5);
    } else {
        draw_quad(width, height, 6, 6);
    }
#endif

    glUseProgram(0);
    (void) handleFPS(&gl_fps_context);

    glutSwapBuffers();
}

void reportPrecisionMode()
{
    switch (precisionMode) {
    case SinglePrecisionMode:
        printf("Rendering with single precision.\n");
        break;
    case DoublePrecisionMode:
        printf("Rendering with double precision.\n");
        break;
    case VerticalSplitMode:
        printf("Rendering with vertical split between single and double precision.\n");
        break;
    case HorizontalSplitMode:
        printf("Rendering with horizontal split between single and double precision.\n");
        break;
    case CheckerboardMode:
        printf("Rendering with checkerboard split between single and double precision.\n");
        break;
    }
}

void idle()
{
    glutPostRedisplay();
}

void visible(int vis)
{
    if (vis == GLUT_VISIBLE) {
        if (animating) {
            glutIdleFunc(idle);
        } else {
            glutIdleFunc(NULL);
        }
    } else {
        glutIdleFunc(NULL);
    }
}

void key(unsigned char k, int x, int y)
{
    bool dirty = false;

    switch (k)
    {
        case 27:
        case 'q':
            exit(0);
            break;
        case 'r':
        case 'R':
            resetSettings();
            dirty = true;
            break;
        case 'v':
            enable_sync = !enable_sync;
            requestSynchornizedSwapBuffers(enable_sync);
            break;  
        case '+':
        case '=':
            scale -= 0.1 * scale;
            dirty = true;
            break;
        case '-':
        case '_':
            scale += 0.1 * scale;
            dirty = true;
            break;
        case 'a':
        case 'A':
            max_iterations += 10.0;
            dirty = true;
            break;
        case 'z':
        case 'Z':
            max_iterations -= 10.0;
            dirty = true;
            break;
        case 'm':
        case 'M':
            if (GLEW_ARB_sample_shading) {
                if (glIsEnabled(GL_SAMPLE_SHADING_ARB)) {
                    glDisable(GL_SAMPLE_SHADING_ARB);
                    printf("multisampling\n");
                } else {
                    glEnable(GL_SAMPLE_SHADING_ARB);
                    printf("per-sample shading\n");
                }
            }
            else {
                printf("ARB_sample_shading not supported.\n");
            }
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            if (!fp64supported) {
                printf("Rendering with single precision (double precision not supported).\n");
                break;
            }
            precisionMode = PrecisionMode(k-'1');
            reportPrecisionMode();
            break;
        case 'p':
            if (!fp64supported) {
                printf("Rendering with single precision (double precision not supported).\n");
                break;
            }

            precisionMode = (PrecisionMode)(precisionMode + 1);
            if (precisionMode == UndefinedMode)
                precisionMode = SinglePrecisionMode;

            reportPrecisionMode();
            break;
        case 'P':
            if (!fp64supported) {
                printf("Rendering with single precision (double precision not supported).\n");
                break;
            }

            precisionMode = (PrecisionMode)(precisionMode - 1);
            if (precisionMode < 0)
                precisionMode = PrecisionMode(int(UndefinedMode)-1);

            reportPrecisionMode();
            break;
        case 'j':
        case 'J':
            julia = !julia;
            resetSettings();
            dirty = true;
            break;
        case 's':
            toggleFPS();
            break;
        case ' ':
            animating = !animating;
            if (animating) {
                glutIdleFunc(idle);
                enableFPS();
            } else {
                glutIdleFunc(NULL);
                disableFPS();
            }
            break;
    }

    if (dirty) {
        updateProgram();
        saveSettings();
    }

    glutPostRedisplay();
}

void specialWithModifier(int key, int x, int y, int modifiers)
{
    bool dirty = false;

    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT;
    switch (key) {
        case GLUT_KEY_LEFT:
            if (shiftPressed)
                juliaOffsetX -= 0.05;
            else
                offsetX -= 0.1 * scale;
            dirty = true;
            break;
        case GLUT_KEY_RIGHT:
            if (shiftPressed)
                juliaOffsetX += 0.05;
            else
                offsetX += 0.1 * scale;
            dirty = true;
            break;
        case GLUT_KEY_DOWN:
            if (shiftPressed)
                juliaOffsetY += 0.05;
            else
                offsetY += 0.1 * scale;
            dirty = true;
            break;
        case GLUT_KEY_UP:
            if (shiftPressed)
                juliaOffsetY -= 0.05;
            else
                offsetY -= 0.1 * scale;
            dirty = true;
            break;
    }

    if (dirty) {
        updateProgram();
        saveSettings();
    }

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    specialWithModifier(key, x, y, glutGetModifiers());
}

static void menu(int item)
{
    if (item < 0) {
        specialWithModifier(-item, 0, 0, 0);
    } else {
        key(item, 0, 0);
    }
}

static void juliaMenu(int item)
{
    specialWithModifier(item, 0, 0, GLUT_ACTIVE_SHIFT);
}

void buildMenu()
{
    int precision_menu = glutCreateMenu(menu);
    glutAddMenuEntry("[1] Single precision", '1');
    glutAddMenuEntry("[2] Double precision", '2');
    glutAddMenuEntry("[3] Single/Double mix with vertical split", '3');
    glutAddMenuEntry("[4] Single/Double mix with horizontal split", '4');
    glutAddMenuEntry("[5] Single/Double checkboard mix", '5');

    int julia_menu = glutCreateMenu(juliaMenu);
    glutAddMenuEntry("[Shift Left] Decrease Julia X shift", GLUT_KEY_LEFT);
    glutAddMenuEntry("[Shift Right] Increase Julia X shift", GLUT_KEY_RIGHT);
    glutAddMenuEntry("[Shift Up] Decrease Julia Y shift", GLUT_KEY_UP);
    glutAddMenuEntry("[Shift Down] Increase Julia Y shift", GLUT_KEY_DOWN);

    glutCreateMenu(menu);
    glutAddMenuEntry("[r] Reset settings", 'r');
    glutAddMenuEntry("[v] Toggle vsync", 'v');
    glutAddMenuEntry("[+] Increase scale", '+');
    glutAddMenuEntry("[-] Decrease scale", '-');
    glutAddMenuEntry("[a] Increase iterations", 'a');
    glutAddMenuEntry("[z] Decrease iterations", 'z');
    if (GLEW_ARB_sample_shading) {
        glutAddMenuEntry("[m] Toggle per-sample shading", 'm');
    }
    if (fp64supported) {
        glutAddSubMenu("Precision...", precision_menu);
        glutAddMenuEntry("[p] Cycle precision modes", 'p');
    }
    glutAddMenuEntry("[j] Toggle Julia fractal", 'j');
    glutAddMenuEntry("[s] Toggle showing frames/sec", 's');
    glutAddMenuEntry("[Left] Decrease X translate", -GLUT_KEY_LEFT);
    glutAddMenuEntry("[Right] Increase X translate", -GLUT_KEY_RIGHT);
    glutAddMenuEntry("[Up] Decrease Y translate", -GLUT_KEY_UP);
    glutAddMenuEntry("[Down] Increase Y translate", -GLUT_KEY_DOWN);
    glutAddSubMenu("Julia parameters...", julia_menu);
    glutAddMenuEntry("[Esc] Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

float window_width, window_height;

Transform3x2 p;

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    if (w == 0) w = 1;

    reshapeFPScontext(&gl_fps_context, w, h);
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    tr3x2_ortho(p, 0, w, h, 0);

    window_width = w;
    window_height = h;

    width = w;
    height = h;
    ar = window_width / window_height;
    updateProgram();
}

void
mouse(int button, int state, int mouse_space_x, int mouse_space_y)
{
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      anchor_x = mouse_space_x;
      anchor_y = mouse_space_y;
      rotate_x = mouse_space_x;
      scale_y = mouse_space_y;
      if (!(glutGetModifiers() & GLUT_ACTIVE_CTRL)) {
        rotating = 1;
      } else {
        rotating = 0;
      }
      if (!(glutGetModifiers() & GLUT_ACTIVE_SHIFT)) {
        zooming = 1;
      } else {
        zooming = 0;
      }
    } else {
      zooming = 0;
      rotating = 0;
    }
  }
  if (button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {
      slide_y = mouse_space_y;
      slide_x = mouse_space_x;
      sliding = 1;
      if (!(glutGetModifiers() & GLUT_ACTIVE_SHIFT)) {
        julia_sliding = 0;
      } else {
        julia_sliding = 1;
      }
    } else {
      sliding = 0;
      julia_sliding = 0;
    }
  }
}

void
motion(int mouse_space_x, int mouse_space_y)
{
  if (zooming || rotating) {
    Transform3x2 t, r, s, m;
    float angle = 0;
    float zoom = 1;
    if (rotating) {
      angle = 180 * -(mouse_space_x - rotate_x)  / window_width;
    }
    if (zooming) {
      zoom = pow(1.003, -(mouse_space_y - scale_y) );
    }

    tr3x2_translate(t, anchor_x, anchor_y);
    tr3x2_rotate(r, angle);
    tr3x2_scale(s, zoom, zoom);
    tr3x2_mult(r, r, s);
    tr3x2_mult(m, t, r);
    tr3x2_translate(t, -anchor_x, -anchor_y);
    tr3x2_mult(m, m, t);
    tr3x2_mult(view, view, m);
    rotate_x = mouse_space_x;
    scale_y = mouse_space_y;
    glutPostRedisplay();
  }
  if (sliding) {
    float x_offset = -(mouse_space_x - slide_x);
    float y_offset = -(mouse_space_y - slide_y);

    if (julia_sliding) {
        juliaOffsetX += x_offset / (2*window_width);
        juliaOffsetY += y_offset / (2*window_height);
    } else {
        Transform3x2 m;

        tr3x2_translate(m, x_offset, y_offset);
        tr3x2_mult(view, view, m);
    }
    slide_y = mouse_space_y;
    slide_x = mouse_space_x;
    glutPostRedisplay();
  }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayString("double rgb samples=8");
    glutCreateWindow("OpenGL 4 Mandelbrot explorer (double-precision)");

    glutDisplayFunc(display);
    glutVisibilityFunc(visible);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    initFPScontext(&gl_fps_context, fps_render_mode);
    colorFPS(0,1,0);
    scaleFPS(2);
    init();
    buildMenu();
    initModelAndViewMatrices();
    colorFPS(0,1,0);
    requestSynchornizedSwapBuffers(enable_sync);

    glutMainLoop();

    return 0;
}
