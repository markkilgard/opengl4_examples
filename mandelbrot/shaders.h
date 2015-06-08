#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <assert.h>

void printInfoLog(GLuint program, bool shader)
{
    GLint logLength = 0;
    char *infoLog;

    if (shader)
    {
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logLength);
        assert(logLength != 0);

        infoLog = new char[logLength];
        glGetShaderInfoLog(program, logLength, &logLength, infoLog);
    }
    else
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        assert(logLength != 0);

        infoLog = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, infoLog);
    }

    if (strlen(infoLog) > 0)
        printf("%s\n", infoLog);

    delete [] infoLog;
}

void addShader(GLuint program, GLenum shaderType, std::string source)
{
    assert(program != 0);
    assert(shaderType != 0);
    assert(source.length() > 0);

    GLuint shader = glCreateShader(shaderType);
    glAttachShader(program, shader);

    const GLcharARB *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);

    glCompileShader(shader);

    printInfoLog(shader, true);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        printf("Failed to compile shader\n");
        exit(-1);
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        printf("OpenGL error: %s\n", gluErrorString(err));
}

void linkProgram(GLuint program)
{
    glLinkProgram(program);

    printInfoLog(program, false);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        printf("Failed to link program\n");
        exit(-1);
    }
}

void setUniform3f(GLuint program, const char *name, float x, float y, float z)
{
    GLint location = glGetUniformLocation(program, name);
    if (location != -1)
        glUniform3f(location, x, y, z);
}

#endif
