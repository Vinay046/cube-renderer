// glm_wrapper.cpp
#include "glm_wrapper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
    void glm_translate(float* matrix, float x, float y, float z) {
        glm::mat4 mat = glm::make_mat4(matrix);
        mat = glm::translate(mat, glm::vec3(x, y, z));
        const float* ptr = glm::value_ptr(mat);
        for (int i = 0; i < 16; ++i) {
            matrix[i] = ptr[i];
        }
    }

    void glm_rotate(float* matrix, float angle, float x, float y, float z) {
        glm::mat4 mat = glm::make_mat4(matrix);
        mat = glm::rotate(mat, glm::radians(angle), glm::vec3(x, y, z));
        const float* ptr = glm::value_ptr(mat);
        for (int i = 0; i < 16; ++i) {
            matrix[i] = ptr[i];
        }
    }

    void glm_perspective(float* matrix, float fovy, float aspect, float near, float far) {
        glm::mat4 mat = glm::perspective(glm::radians(fovy), aspect, near, far);
        const float* ptr = glm::value_ptr(mat);
        for (int i = 0; i < 16; ++i) {
            matrix[i] = ptr[i];
        }
    }
}