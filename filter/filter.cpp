
// filter.cpp - OpenGL 4.3 compute shader example to filter images

#define _USE_MATH_DEFINES
#include <math.h>

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

#include "countof.h"

#include <string>
#include <sstream>
#include <math.h>

#include "read_text_file.h"
#include "nv_dds.h"

#include "showfps.h"
#include "request_vsync.h"

void reshape(int w, int h);

FPSusage fps_render_mode = FPS_USAGE_TEXTURE;
FPScontext gl_fps_context;

bool animating = false;
bool enable_sync = false;
bool verbose = false;

int computeCount = 0;
bool redoConvolve = true;

bool dumpShader = false;

GLuint spProgram;
GLuint dpProgram;
GLuint program;
GLuint fbo;

GLenum texture_filter = GL_NEAREST;

int width = 768;
int height = 512;
float ar = width / (float)height;

int tex_w = 512;
int tex_h = 512;
int output_w = 7200;
int output_h = 7200;

enum FilterType {
    SOBEL_HORIZONTAL,
    SOBEL_VERTICAL,
    IDENTITY,
    GAUSSIAN,
    UNIFORM,
};

FilterType filter_type = GAUSSIAN;
float sigma = 1.5;

std::string fragment_shader_source;
std::string vertex_shader_source;

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
}

char *programName = "compute";

static void fatalError(const char *message)
{
    fprintf(stderr, "%s: %s\n", programName, message);
    exit(1);
}

void initOpenGL()
{
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        fatalError("OpenGL Extension Wrangler (GLEW) failed to initialize");
    }
    GLboolean hasOpenGL43 = glewIsSupported("GL_VERSION_4_3");
    if (!hasOpenGL43) {
        fatalError("OpenGL implementation doesn't support OpenGL 4.3");
    }
}

GLuint texobj = 43;

#include "stb/stb_image.h"

void setTextureFilter(GLuint texobj, GLenum filter_mode)
{
    glTextureParameteriEXT(texobj, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode);
    glTextureParameteriEXT(texobj, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode);
}

void loadTexture(GLuint texobj,
                 int width, int height,
                 const void *image)
{
    glDeleteTextures(1, &texobj);
    glTextureStorage2DEXT(texobj, GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    setTextureFilter(texobj, texture_filter);

    glTextureParameteriEXT(texobj, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteriEXT(texobj, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (image) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2DEXT(texobj, GL_TEXTURE_2D, 0, 0,0,
            width, height,
            GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    glutReportErrors();
}

GLuint naive_separable_program = 0;
GLuint separable_program = 0;
GLuint general_program = 0;
GLuint median_program = 0;
GLuint estimated_median_program = 0;
GLuint copy_program = 0;
GLuint active_convolve_program = 0;
GLuint active_compute_program = 0;

int tileSize = 16;
int filterSize = 5;
bool nopPhase = false;
bool scalarWeights = false;

GLuint compileComputeProgram(const char *filename,
                             const char *entry,
                             int tile_size,
                             int filter_size,
                             bool scalarWeights)
{
    assert(filter_size & 1);
    int filter_offset = filter_size/2;
    int neighborhood_width = tile_size + 2*filter_offset;
    int neighborhood_height = tile_size + 2*filter_offset;

    int tile_width = tile_size,
        tile_height = tile_size;
    int filter_width = filter_size,
        filter_height = filter_size;

    char *text = read_text_file(filename);
    if (!text) {
        printf("problem reading <%s>\n", filename);
        fatalError("could not load compute shader");
    }
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    char entry_define[500];
    const GLchar *compute_shader_source = reinterpret_cast<const GLchar*>(text);
    {
#ifdef _WIN32
#define snprintf _snprintf  // Microsoft wart
#endif
        snprintf(entry_define, sizeof(entry_define),
            "#version 430\n"
            "#define %s main\n"
            "#define COMPILE_%s\n"
            "#define NEIGHBORHOOD_WIDTH %d\n"
            "#define NEIGHBORHOOD_HEIGHT %d\n"
            "#define TILE_WIDTH %d\n"
            "#define TILE_HEIGHT %d\n"
            "#define FILTER_WIDTH %d\n"
            "#define FILTER_HEIGHT %d\n%s%s",
            entry, entry,
            neighborhood_width, neighborhood_height,
            tile_width, tile_height,
            filter_width, filter_height,
            nopPhase ? "#define retirePhase nop\n" : "",
            scalarWeights ? "#define SCALAR_WEIGHTS\n" : "");
        GLsizei lengths[2];
        const GLchar *chunks[2];
        chunks[0] = entry_define;
        chunks[1] = compute_shader_source;
        lengths[0] = GLsizei(strlen(entry_define));
        lengths[1] = GLsizei(strlen(text));
        glShaderSource(shader, countof(chunks), chunks, lengths);
        glCompileShader(shader);
    }

    if (dumpShader) {
        const char *dump_filename = "dump.glsl";

        FILE *dump = fopen(dump_filename, "w");
        if (dump) {
            fprintf(dump, "%s%s", entry_define, text);
            fclose(dump);
        } else {
            printf("error: could not fopen %s\n", dump_filename);
        }
    }

    GLint status = GL_FALSE;
    GLint info_log_length = 0;
    GLchar *info_log = NULL;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 1 || !status) {
        info_log = (char *) malloc(info_log_length+1);
        memset(info_log, 0, sizeof(info_log_length+1));
        glGetShaderInfoLog(shader, info_log_length+1, NULL, info_log);
        printf("Shader info log = <%s>\n", info_log);
        if (verbose) {
            printf("Shader source = <%s%s>\n", entry_define, text);
        }
        free(info_log);
        fatalError("compute shader compile failed");
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
    glLinkProgram(program);
    status = GL_FALSE;
    info_log_length = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 1 || !status) {
        info_log = (char *) malloc(info_log_length+1);
        memset(info_log, 0, sizeof(info_log_length+1));
        glGetProgramInfoLog(program, info_log_length+1, NULL, info_log);
        printf("Program info log = <%s>\n", info_log);
        printf("Shader source = <%s%s>\n", entry_define, text);
        free(info_log);
        fatalError("compute shader link failed");
    }

    glDeleteShader(shader);

    glutReportErrors();
    free(text);
    if (verbose) {
        printf("%s: compiled OK\n", entry);
    }
    return program;
}

void gaussWeights(int size, GLfloat *weights, float sigma)
{
    assert(size & 1);
    const int mid = size/2;
    const double sigmaSq_2x = 2.0*sigma*sigma;
    const double scale = 1.0/(sqrt(2*M_PI)*sigma);
    double weight_sum = 0.0;
    for (int i=0; i<mid; i++) {
        int x = i - mid;
        float w = scale * exp(-(x*x)/sigmaSq_2x);
        weights[size-i-1] = weights[i] = w;
        weight_sum += 2*w;
    }
    {
        int x = 0;
        float w = scale * exp(-(x*x)/sigmaSq_2x);
        weights[mid] = w;
        weight_sum += w;
    }
    // Rebalance
    for (int i=0; i<size; i++) {
        weights[i] = weights[i] / weight_sum;
    }
}

const int MAX_FILTER_SIZE = 19;
const int NUM_COMPONENTS = 4;

int image_width = 0;
int image_height = 0;

void loadWeights(GLuint program, FilterType mode, int filterSize, bool scalarWeights)
{
    assert(filterSize & 1);  // should be odd
    const int filterOffset = filterSize/2;

    assert(filterSize <= MAX_FILTER_SIZE);
    static const GLfloat sobel_column_weights[MAX_FILTER_SIZE][NUM_COMPONENTS] = {
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 1,1,1,1 },
        { 2,2,2,2 },
        { 1,1,1,1 },
        { 0,0,0,0 },
        { 0,0,0,0 },
    };
    static const GLfloat sobel_row_weights[MAX_FILTER_SIZE][NUM_COMPONENTS] = {
        { 0,0,0,0 },
        { 0,0,0,0 },
        { -1,-1,-1,-1 },
        { 0,0,0,0 },
        { 1,1,1,1 },
        { 0,0,0,0 },
        { 0,0,0,0 },
    };
    static const GLfloat identity_column_weights[MAX_FILTER_SIZE][NUM_COMPONENTS] = {
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 1,1,1,1 },
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 0,0,0,0 },
    };
    static const GLfloat identity_row_weights[MAX_FILTER_SIZE][NUM_COMPONENTS] = {
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 1,1,1,1 },
        { 0,0,0,0 },
        { 0,0,0,0 },
        { 0,0,0,0 },
    };

    GLfloat tmp_row_weights[MAX_FILTER_SIZE][NUM_COMPONENTS];
    GLfloat tmp_column_weights[MAX_FILTER_SIZE][NUM_COMPONENTS];

    const GLfloat *column_weights, *row_weights;

    switch (mode) {
    case SOBEL_HORIZONTAL:
    case SOBEL_VERTICAL:
        {
            for (int i=0; i<filterSize; i++) {
                if (i == filterOffset-1) {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = -1;
                        tmp_column_weights[i][j] = 1;
                    }
                } else if (i == filterOffset) {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = 0;
                        tmp_column_weights[i][j] = 2;
                    }
                } else if (i == filterOffset+1) {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = +1;
                        tmp_column_weights[i][j] = 1;
                    }
                } else {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = 0;
                        tmp_column_weights[i][j] = 0;
                    }
                }
            }
            if (mode == SOBEL_VERTICAL) {
                for (int i=0; i<filterSize; i++) {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        float tmp = tmp_row_weights[i][j];
                        tmp_row_weights[i][j] = tmp_column_weights[i][j];
                        tmp_column_weights[i][j] = tmp;
                    }
                }
            }
            column_weights = &tmp_column_weights[0][0];
            row_weights = &tmp_row_weights[0][0];
        }
        break;
    case IDENTITY:
        {
            for (int i=0; i<filterSize; i++) {
                if (i == filterOffset) {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = 1;
                        tmp_column_weights[i][j] = 1;
                    }
                } else {
                    for (int j=0; j<NUM_COMPONENTS; j++) {
                        tmp_row_weights[i][j] = 0;
                        tmp_column_weights[i][j] = 0;
                    }
                }
            }
            column_weights = &tmp_column_weights[0][0];
            row_weights = &tmp_row_weights[0][0];
        }
        break;
    case UNIFORM:
        {
            int count = filterSize*filterSize;
            float weight = sqrt(1.0/count);
            for (int i=0; i<filterSize; i++) {
                for (int j=0; j<NUM_COMPONENTS; j++) {
                    tmp_row_weights[i][j] = weight;
                    tmp_column_weights[i][j] = weight;
                }
            }
            column_weights = &tmp_column_weights[0][0];
            row_weights = &tmp_row_weights[0][0];
        }
        break;
    case GAUSSIAN:
        {
            float gen_row_weights[MAX_FILTER_SIZE];
            float gen_column_weights[MAX_FILTER_SIZE];
            gaussWeights(filterSize, gen_row_weights, sigma);
            if (verbose) {
                for (int i=0; i<filterSize; i++) {
                    printf("%2d: %f\n", i-filterOffset, gen_row_weights[i]);
                }
            }
            gaussWeights(filterSize, gen_column_weights, sigma);
            for (int i=0; i<filterSize; i++) {
                for (int j=0; j<NUM_COMPONENTS; j++) {
                    tmp_row_weights[i][j] = gen_row_weights[i];
                    tmp_column_weights[i][j] = gen_column_weights[i];
                }
            }
            column_weights = &tmp_column_weights[0][0];
            row_weights = &tmp_row_weights[0][0];
        }
        break;
    default:
        assert(!"bogus filter mode");
    }

    GLint columnWeight_loc = glGetUniformLocation(program, "columnWeight");
    if (columnWeight_loc >= 0) {
        if (scalarWeights) {
            GLfloat *scalar_weights = new GLfloat[filterSize];
            for (int i=0; i<filterSize; i++) {
                scalar_weights[i] = column_weights[4*i];
            }
            glProgramUniform1fv(program,
                columnWeight_loc,
                filterSize, scalar_weights);
            delete scalar_weights;
        } else {
            glProgramUniform4fv(program,
                columnWeight_loc,
                filterSize, column_weights);
        }
    } else {
        if (verbose) {
            printf("failed to get columnWeight\n");
        }
    }
    GLint rowWeight_loc = glGetUniformLocation(program, "rowWeight");
    if (rowWeight_loc >= 0) {
        if (scalarWeights) {
            GLfloat *scalar_weights = new GLfloat[filterSize];
            for (int i=0; i<filterSize; i++) {
                scalar_weights[i] = row_weights[4*i];
            }
            glProgramUniform1fv(program,
                rowWeight_loc,
                filterSize, scalar_weights);
            delete scalar_weights;
        } else {
            glProgramUniform4fv(program,
                rowWeight_loc,
                filterSize, row_weights);
        }
    } else {
        if (verbose) {
            printf("failed to get rowWeight\n");
        }
    }
    // XXX Revisit if "weight" should work.
    GLint weight_loc = glGetUniformLocation(program, "weight[0]");
    if (weight_loc >= 0) {
        const int elements = filterSize*filterSize;
        if (scalarWeights) {
            GLfloat *scalar_weights = new GLfloat[elements];
            for (int j=0; j<filterSize; j++) {
                for (int i=0; i<filterSize; i++) {
                    scalar_weights[j*filterSize+i] = column_weights[4*j]*row_weights[4*i];
                }
            }
            if (verbose) {
                for (int j=0; j<filterSize; j++) {
                    printf("%d:", j);
                    for (int i=0; i<filterSize; i++) {
                        printf(" %f", scalar_weights[j*filterSize+i]);
                    }
                    printf("\n");
                }
            }
            glProgramUniform1fv(program,
                weight_loc,
                elements, scalar_weights);
            for (int i=0; i<filterSize; i++) {
                glProgramUniform4fv(program,
                    weight_loc + i*filterSize,
                    filterSize, &scalar_weights[i*filterSize]);
            }
            delete scalar_weights;
        } else {
            GLfloat *vector_weights = new GLfloat[4*elements];
            for (int j=0; j<filterSize; j++) {
                for (int i=0; i<filterSize; i++) {
                    for (int c=0; c<4; c++) {
                        vector_weights[4*(j*filterSize+i)+c] =
                            column_weights[4*j+c]*row_weights[4*i+c];
                    }
                }
            }
            if (verbose) {
                for (int j=0; j<filterSize; j++) {
                    printf("%d:", j);
                    for (int i=0; i<filterSize; i++) {
                        const int c = 0;
                        printf(" %f", vector_weights[4*(j*filterSize+i)+c]);
                    }
                    printf("\n");
                }
            }
            for (int i=0; i<filterSize; i++) {
                glProgramUniform4fv(program,
                    weight_loc + i*filterSize,  // location math
                    filterSize, &vector_weights[i*4*filterSize]);
            }
            delete vector_weights;
        }
    } else {
        if (verbose) {
            printf("failed to get weight\n");
        }
    }
    GLint imageBounds_loc = glGetUniformLocation(program, "imageBounds");
    if (imageBounds_loc >= 0) {
        assert(image_width > 0);
        assert(image_height > 0);
        glProgramUniform4i(program, imageBounds_loc, 0, 0, image_width-1, image_height-1 );
    }
}

void getProgramBinary()
{
    GLsizei length = 0;

    glGetProgramiv(active_compute_program, GL_PROGRAM_BINARY_LENGTH, &length);
    if (length > 0) {
        char *buffer = new char[length];
        if (buffer) {
            GLenum format = GL_NONE;
            GLsizei return_length = 0;

            glGetProgramBinary(active_compute_program, length,
                &return_length, &format, buffer);
#define GL_PROGRAM_BINARY_NV                                0x8E21
            if (return_length == length) {
                if (format == GL_PROGRAM_BINARY_NV) {
                    const char *binary_filename = "dump.bin";

                    FILE *binary = fopen(binary_filename, "wb");
                    if (binary) {
                        fwrite(buffer, length, 1, binary);
                        fclose(binary);
                        printf("wrote binary shader to <%s> (format=0x%x,length=%d)\n",
                            binary_filename, format, length);
                    } else {
                        printf("fopen failed on <%s>\n", binary_filename);
                    }

                    char *assembly_start = NULL;
                    ptrdiff_t n = 0;
                    char *end = buffer + length;
                    for (char *p = buffer; p<end-6; p++) {
                        if (p[0] == '!' && p[1] == '!' && p[2] == 'N' && p[3] == 'V') {
                            if (p[5] == 'p' || p[6] == 'p') {
                                assembly_start = p;
                                for (; p<end-4; p++) {
                                    if (p[0] == '\n' && p[1] == 'E' && p[2] == 'N' && p[3] == 'D' && p[4] == '\n') {
                                        const char *assembly_end = p+5;
                                        n = assembly_end-assembly_start;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    if (assembly_start && n > 0) {
                        const char *text_filename = "dump.txt";
                        FILE *text = fopen(text_filename, "w");
                        if (text) {
                            fwrite(assembly_start, n, 1, text);
                            fclose(text);
                            printf("wrote assembly shader to <%s> (length=%d)\n",
                                text_filename, n);
                        } else {
                            printf("fopen failed on <%s>\n", text_filename);
                        }
                    } else {
                        printf("failed to find '!!NV' assembly in binary\n");
                    }
                } else {
                    printf("got binary program but didn't get NV format (format=0x%x,length=%d)\n",
                        format, length);                    
                }
            } else {
                printf("glGetProgramBinary failed to get a binary program\n");
            }
            delete buffer;
        }
    }
}

void doCompute()
{
    glUseProgram(active_compute_program);
    int rows = (image_height + tileSize - 1) / tileSize;
    int columns = (image_width + tileSize - 1) / tileSize;
    glDispatchCompute(rows, columns, 1);
}

void loadImage(const char *filename)
{
    unsigned char *image;
    int width, height, comp;
    image = stbi_load(filename, &width, &height, &comp, 4);
    if (!image) {
        printf("stbi_load could not open/read <%s>\n", filename);
        printf("reason: %s\n", stbi_failure_reason());
        fatalError("could not stbi_load image");
    }
    image_width = width;
    image_height = height;
    loadTexture(texobj, width, height, image);
    GLboolean is_not_layered = GL_FALSE;
    glBindImageTexture(0, texobj, 0, is_not_layered, 0, GL_READ_ONLY, GL_RGBA8);
    free(image);
    loadTexture(texobj+1, width, height, NULL);
    glBindImageTexture(1, texobj+1, 0, is_not_layered, 0, GL_READ_WRITE, GL_RGBA8);
    glutReportErrors();
}

static const char *image_name[] = {
    "man_on_moon.jpg",
    "rooster.tga",
    "valentine_candy.jpg",
    "venice.jpg",
    "head_slice.jpg",
};
int image_ndx = 0;

void initializeSeparableProgram(GLuint &program,
                                const char *filename,
                                const char *entry_function,
                                int tile_size,
                                int filter_size)
{
    if (program) {
        glDeleteProgram(program);
    }
    program = compileComputeProgram(filename,
        entry_function,
        tileSize, filterSize, scalarWeights);
    loadWeights(program, filter_type, filterSize, scalarWeights);
}

void recompile()
{
    const char *filename = "convolution.glsl";

    printf("filterSize = %d, tileSize = %d\n", filterSize, tileSize);
    initializeSeparableProgram(naive_separable_program,
        filename, "naive_separable",
        tileSize, filterSize);
    initializeSeparableProgram(separable_program,
        filename, "separable",
        tileSize, filterSize);
    initializeSeparableProgram(general_program,
        filename, "general",
        tileSize, filterSize);
    initializeSeparableProgram(copy_program,
        filename, "copy",
        tileSize, filterSize);
    initializeSeparableProgram(median_program,
        "median.glsl", "median",
        // Don't let the median filter size get to 7x7 because to slow
        tileSize, filterSize <= 5 ? filterSize : 5);
    initializeSeparableProgram(estimated_median_program,
        "median.glsl", "estimated_median",
        tileSize, filterSize);
    if (!active_compute_program) {
        active_convolve_program = separable_program;
        active_compute_program = separable_program;
    }
    redoConvolve = true;
}

void init()
{
    initOpenGL();

    loadImage(image_name[image_ndx]);

    recompile();

    active_compute_program = separable_program;

    doCompute();

    glutReportErrors();
}

void drawImage(GLuint texobj)
{
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texobj);
    glBegin(GL_POLYGON); {
        glTexCoord2f(0,0);
        glVertex2f(0,0);
        glTexCoord2f(1,0);
        glVertex2f(350,0);
        glTexCoord2f(1,1);
        glVertex2f(350,350);
        glTexCoord2f(0,1);
        glVertex2f(0,350);
    } glEnd();

    glDisable(GL_TEXTURE_2D);
}

void MatrixLoadToGL(Transform3x2 m)
{
  GLfloat mm[16];  /* Column-major OpenGL-style 4x4 matrix. */

  /* First column. */
  mm[0] = m[0][0];
  mm[1] = m[1][0];
  mm[2] = 0;
  mm[3] = 0;

  /* Second column. */
  mm[4] = m[0][1];
  mm[5] = m[1][1];
  mm[6] = 0;
  mm[7] = 0;

  /* Third column. */
  mm[8] = 0;
  mm[9] = 0;
  mm[10] = 1;
  mm[11] = 0;

  /* Fourth column. */
  mm[12] = m[0][2];
  mm[13] = m[1][2];
  mm[14] = 0;
  mm[15] = 1;

  glMatrixLoadfEXT(GL_MODELVIEW, &mm[0]);
}

void display()
{
    if (computeCount == 0) {
        if (redoConvolve) {
            doCompute();
            redoConvolve = false;
        }
    } else {
        for (int i=0; i<computeCount; i++) {
            doCompute();
        }
    }

    glClearColor(0.1, 0.3, 0.6, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (verbose) {
        printf("%f,%f,%f\n%f,%f,%f\n",
            view[0][0], view[0][1], view[0][2],
            view[1][0], view[1][1], view[1][2]);
    }

    glMatrixPushEXT(GL_MODELVIEW); {
        int w = 768, h = 512;
        const int xoffset = (w - 2*350)/3;
        const int yoffset = (h - 350)/2;
        MatrixLoadToGL(view);
        glMatrixTranslatefEXT(GL_MODELVIEW, xoffset, yoffset, 0);
        drawImage(texobj);
        glMatrixPushEXT(GL_MODELVIEW); {
            glMatrixTranslatefEXT(GL_MODELVIEW, xoffset+350, 0, 0);
            drawImage(texobj+1);
        } glMatrixPopEXT(GL_MODELVIEW);
    } glMatrixPopEXT(GL_MODELVIEW);

    glUseProgram(0);
    (void) handleFPS(&gl_fps_context);

    glutSwapBuffers();
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

void updateWeights()
{
    loadWeights(separable_program, filter_type, filterSize, scalarWeights);
    loadWeights(naive_separable_program, filter_type, filterSize, scalarWeights);
    loadWeights(general_program, filter_type, filterSize, scalarWeights);
    redoConvolve = true;
    glutPostRedisplay();
}

void changeWeights(FilterType new_filter_type)
{
    filter_type = new_filter_type;
    updateWeights();
}

void key(unsigned char k, int x, int y)
{
    bool dirty = false;

    switch (k) {
    case 27:
        exit(0);
        break;
    case 'D':
        dumpShader = !dumpShader; // toggle;
        printf("dumpShader = %d\n", dumpShader);
        break;
    case 'W':
        updateWeights();
        redoConvolve = true;
        glutPostRedisplay();
        break;
    case 'w':
        scalarWeights = !scalarWeights; // toggle
        printf("scalarWeights = %d\n", scalarWeights);
        recompile();
        break;
    case 'l':
    case 'L':
        if (texture_filter == GL_NEAREST) {
            texture_filter = GL_LINEAR;
        } else {
            texture_filter = GL_NEAREST;
        }
        setTextureFilter(texobj, texture_filter);
        setTextureFilter(texobj+1, texture_filter);
        break;
    case '1':
    case 'm':
        active_convolve_program = separable_program;
        active_compute_program = separable_program;
        printf("Fast separable convolution\n");
        redoConvolve = true;
        break;
    case '2':
    case 'M':
        active_convolve_program = naive_separable_program;
        active_compute_program = naive_separable_program;
        printf("Naive separable convolution\n");
        redoConvolve = true;
        break;
    case '3':
    case 'g':
        printf("General convolution\n");
        active_convolve_program = general_program;
        active_compute_program = general_program;
        redoConvolve = true;
        break;
    case '9':
        printf("Estimated median filter\n");
        active_compute_program = estimated_median_program;
        redoConvolve = true;
        break;
    case '0':
        printf("Exact median filter\n");
        active_compute_program = median_program;
        redoConvolve = true;
        break;
    case '=':
        printf("Copy...\n");
        active_compute_program = copy_program;
        redoConvolve = true;
        break;
    case 'n':
        nopPhase = !nopPhase; // toggle
        printf("nopPhase = %d\n", nopPhase);
        recompile();
        break;
    case 'k':
        computeCount++;
        printf("computeCount = %d\n", computeCount);
        break;
    case 'K':
        if (computeCount > 0) {
            computeCount--;
        }
        printf("computeCount = %d\n", computeCount);
        break;
    case 'f':
        if (filterSize < MAX_FILTER_SIZE) {
            filterSize += 2;
        }
        recompile();
        break;
    case 'F':
        if (filterSize > 1) {
            filterSize -= 2;
        }
        recompile();
        break;
    case 't':
        tileSize += 1;
        recompile();
        break;
    case 'T':
        if (tileSize > 1) {
            tileSize -= 1;
        }
        recompile();
        break;
    case 'c':
        recompile();
        break;
    case 'r':
        resetSettings();
        break;
    case 'b':
        getProgramBinary();
        break;
    case 'C':
        doCompute();
        break;
    case '4':
        printf("Gaussian weights %f...\n", sigma);
        active_compute_program = active_convolve_program;
        changeWeights(GAUSSIAN);
        break;
    case '5':
        printf("Sobel horizontal weights...\n");
        active_compute_program = active_convolve_program;
        changeWeights(SOBEL_HORIZONTAL);
        break;
    case '6':
        printf("Sobel vertical weights...\n");
        active_compute_program = active_convolve_program;
        changeWeights(SOBEL_VERTICAL);
        break;
    case '7':
        printf("Identity weights...\n");
        active_compute_program = active_convolve_program;
        changeWeights(IDENTITY);
        break;
    case '8':
        printf("Uniform weights...\n");
        active_compute_program = active_convolve_program;
        changeWeights(UNIFORM);
        break;
    case '+':
        sigma += 1.0/16;
        printf("sigma = %f\n", sigma);
        updateWeights();
        break;
    case '-':
        sigma -= 1.0/16;
        if (sigma < 1.0/16) {
            sigma = 1.0/16;
        }
        updateWeights();
        break;
    case 'q':
        exit(0);
        break;
    case 's':
        toggleFPS();
        break;
    case 'v':
        enable_sync = !enable_sync; // toggle
        requestSynchornizedSwapBuffers(enable_sync);
        break;  
    case 'V':
        verbose = !verbose; // toggle
        printf("verbose = %d\n", verbose);
        break;
    case ' ':
        animating = !animating; // toggle
        if (animating) {
            glutIdleFunc(idle);
            enableFPS();
        } else {
            glutIdleFunc(NULL);
            disableFPS();
        }
        break;
    }

    glutPostRedisplay();
}

void specialWithModifier(int key, int x, int y, int modifiers)
{
    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT;
    switch (key) {
    case GLUT_KEY_LEFT:
        break;
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

static void image_menu(int item)
{
    image_ndx = item;
    loadImage(image_name[image_ndx]);
    redoConvolve = true;
    glutPostRedisplay();
}

static void filter_size_menu(int item)
{
    filterSize = item;
    recompile();
    glutPostRedisplay();
}

static void filter_type_menu(int item)
{
    changeWeights(FilterType(item));
}

void buildMenu()
{
    int image_menu_id = glutCreateMenu(image_menu);
    for (int i=0; i<countof(image_name); i++) {
        glutAddMenuEntry(image_name[i], i);
    }
    int filter_size_id = glutCreateMenu(filter_size_menu);
    for (int i=1; i<11; i+=2) {
        char buffer[100];
        _snprintf(buffer, sizeof(buffer), "%dx%d", i, i);
        glutAddMenuEntry(buffer, i);
    }
    int program_type_id = glutCreateMenu(menu);
    glutAddMenuEntry("[1] Separable convolution", '1');
    glutAddMenuEntry("[2] Naive separable convolution", '2');
    glutAddMenuEntry("[3] General convolution", '3');
    glutAddMenuEntry("[9] Estimated median filter", '9');
    glutAddMenuEntry("[0] Exact median filter", '0');
    glutAddMenuEntry("[=] Copy", '=');
    int filter_type_id = glutCreateMenu(filter_type_menu);
    glutAddMenuEntry("[4] Gaussian", GAUSSIAN);
    glutAddMenuEntry("[5] Sobel horizontal (edge detect)", SOBEL_HORIZONTAL);
    glutAddMenuEntry("[6] Sobel vertical (edge detect)", SOBEL_VERTICAL);
    glutAddMenuEntry("[7] Identity", IDENTITY);
    glutAddMenuEntry("[8] Uniform", UNIFORM);
    glutCreateMenu(menu);
    glutAddSubMenu("Images...", image_menu_id);
    glutAddSubMenu("Program type...", program_type_id);
    glutAddSubMenu("Filter type...", filter_type_id);
    glutAddSubMenu("Filter size...", filter_size_id);
    glutAddMenuEntry("[ ] Toggle continuous redraw", ' ');
    glutAddMenuEntry("[s] Toggle frames/second", 's');
    glutAddMenuEntry("[n] Toggle shader barriers", 'n');
    glutAddMenuEntry("[b] Dump binary blob", 'b');
    glutAddMenuEntry("[v] Toggle vsync", 'v');
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
    } else {
      sliding = 0;
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
      angle = 180 * (rotate_x - mouse_space_x)  / window_width;
    }
    if (zooming) {
      zoom = pow(1.003, (mouse_space_y - scale_y) );
    }

    tr3x2_translate(t, anchor_x, anchor_y);
    tr3x2_rotate(r, angle);
    tr3x2_scale(s, zoom, zoom);
    tr3x2_mult(r, r, s);
    tr3x2_mult(m, t, r);
    tr3x2_translate(t, -anchor_x, -anchor_y);
    tr3x2_mult(m, m, t);
    tr3x2_mult(view, m, view);
    rotate_x = mouse_space_x;
    scale_y = mouse_space_y;
    glutPostRedisplay();
  }
  if (sliding) {
    float x_offset = (mouse_space_x - slide_x);
    float y_offset = (mouse_space_y - slide_y);

    Transform3x2 m;

    tr3x2_translate(m, x_offset, y_offset);
    tr3x2_mult(view, m, view);

    slide_x = mouse_space_x;
    slide_y = mouse_space_y;
    glutPostRedisplay();
  }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayString("double rgb samples=8");
    glutCreateWindow("OpenGL 4.3 Compute Shader image filtering example");

    printf("vendor: %s\n", glGetString(GL_VENDOR));
    printf("version: %s\n", glGetString(GL_VERSION));
    printf("renderer: %s\n", glGetString(GL_RENDERER));
    printf("samples per pixel = %d\n", glutGet(GLUT_WINDOW_NUM_SAMPLES));
    printf("Executable: %d bit\n", (int)(8*sizeof(int*)));

    glutDisplayFunc(display);
    glutVisibilityFunc(visible);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    initFPScontext(&gl_fps_context, fps_render_mode);
    colorFPS(0,1,0);
    scaleFPS(3);
    disableFPS();
    init();
    buildMenu();
    initModelAndViewMatrices();
    colorFPS(0,1,0);
    requestSynchornizedSwapBuffers(enable_sync);

    glutMainLoop();

    return 0;
}
