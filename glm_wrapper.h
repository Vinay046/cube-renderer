// glm_wrapper.h
#ifndef GLM_WRAPPER_H
#define GLM_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void glm_translate(float* matrix, float x, float y, float z);
void glm_rotate(float* matrix, float angle, float x, float y, float z);
void glm_perspective(float* matrix, float fovy, float aspect, float near, float far);

#ifdef __cplusplus
}
#endif

#endif // GLM_WRAPPER_H