#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "esUtil.h"

static struct {
    struct egl egl;
    GLfloat aspect;
    GLuint program;
    GLint modelviewprojectionmatrix;
    GLint translationLocation;  // New uniform location for translation
    GLuint redSquareVBO;
    GLuint blueSquareVBO;
} gl;

// Red square vertices
GLfloat redSquareVertices[] = {
    // Positions          // Colors
    -1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-left
    -1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left
     1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-right
     1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-right
    -1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f   // Bottom-right
};

// Blue square vertices (now without translation)
GLfloat blueSquareVertices[] = {
    // Positions          // Colors
    -0.1f,  0.1f, 0.0f,  0.0f, 0.0f, 1.0f,  // Top-left
    -0.1f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f,  // Bottom-left
     0.1f,  0.1f, 0.0f,  0.0f, 0.0f, 1.0f,  // Top-right
     0.1f,  0.1f, 0.0f,  0.0f, 0.0f, 1.0f,  // Top-right
    -0.1f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f,  // Bottom-left
     0.1f, -0.1f, 0.0f,  0.0f, 0.0f, 1.0f   // Bottom-right
};

// Updated vertex shader with translation uniform
static const char *triangle_vertex_shader_source =
    "#version 300 es\n"
    "in vec3 vPosition;\n"
    "in vec3 vColor;\n"
    "uniform mat4 projection;\n"
    "uniform vec2 translation;\n"  // New uniform for translation
    "out vec3 fColor;\n"
    "void main() {\n"
    "    vec4 pos = vec4(vPosition.xy + translation, vPosition.z, 1.0);\n"
    "    gl_Position = projection * pos;\n"
    "    fColor = vColor;\n"
    "}\n";

static const char *triangle_fragment_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 fColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(fColor, 1.0);\n"
    "}\n";

static void draw_hello_triangle(unsigned i)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Calculate animation parameters
    float time = i * 0.05f;
    float radius = 0.5f;
    float centerX = radius * cos(time);
    float centerY = radius * sin(time);

    ESMatrix modelview;
    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.0f, 0.0f, -8.0f);

    ESMatrix projection;
    esMatrixLoadIdentity(&projection);
    esFrustum(&projection, -2.8f, +2.8f, -2.8f * gl.aspect, +2.8f * gl.aspect, 6.0f, 10.0f);

    ESMatrix modelviewprojection;
    esMatrixLoadIdentity(&modelviewprojection);
    esMatrixMultiply(&modelviewprojection, &modelview, &projection);

    glUniformMatrix4fv(gl.modelviewprojectionmatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw red square with no translation
    glUniform2f(gl.translationLocation, 0.0f, 0.0f);
    glBindBuffer(GL_ARRAY_BUFFER, gl.redSquareVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Draw blue square with animated translation
    glUniform2f(gl.translationLocation, centerX, centerY);
    glBindBuffer(GL_ARRAY_BUFFER, gl.blueSquareVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

const struct egl * init_hello_triange(const struct gbm *gbm, int samples)
{
    int ret;

    ret = init_egl(&gl.egl, gbm, samples);
    if (ret)
        return NULL;

    gl.aspect = (GLfloat)(gbm->height) / (GLfloat)(gbm->width);

    ret = create_program(triangle_vertex_shader_source, triangle_fragment_shader_source);
    if (ret < 0)
        return NULL;

    gl.program = ret;

    ret = link_program(gl.program);
    if (ret)
        return NULL;

    glUseProgram(gl.program);

    gl.modelviewprojectionmatrix = glGetUniformLocation(gl.program, "projection");
    gl.translationLocation = glGetUniformLocation(gl.program, "translation");  // Get translation uniform location

    glViewport(0, 0, gbm->width, gbm->height);

    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Setup red square
    glGenBuffers(1, &gl.redSquareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gl.redSquareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(redSquareVertices), redSquareVertices, GL_STATIC_DRAW);

    // Setup blue square
    glGenBuffers(1, &gl.blueSquareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gl.blueSquareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blueSquareVertices), blueSquareVertices, GL_STATIC_DRAW);

    gl.egl.draw = draw_hello_triangle;

    return &gl.egl;
}

const struct egl * init_cube_smooth(const struct gbm *gbm, int samples)
{
    return init_hello_triange(gbm, samples);
}