#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "esUtil.h"


static struct {
	struct egl egl;

	GLfloat aspect;

	GLuint program;
	GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
	GLuint vbo;
	GLuint positionsoffset, colorsoffset, normalsoffset;
} gl;


GLfloat vertices[] = {
        // Positions          // Colors
	// First triangle
	-0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-left
	-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left
	0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-right
	// Second triangle
		0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top-right
	-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f   // Bottom-right
};

GLuint indices[] = {
	0, 1, 2,  // First triangle
	3, 4, 5   // Second triangle
};

static const char *triangle_vertex_shader_source =
		"#version 300 es\n"
		"in vec3 vPosition;\n"
		"in vec3 vColor;\n"
		"uniform mat4 projection;\n"  // Add projection matrix uniform
		"out vec3 fColor;\n"
		"void main() {\n"
		"    gl_Position = projection * vec4(vPosition, 1.0);\n"  // Apply projection matrix
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

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

	glViewport(0, 0, gbm->width, gbm->height);

	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO);

	gl.egl.draw = draw_hello_triangle;

	return &gl.egl;
}