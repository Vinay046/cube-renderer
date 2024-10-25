#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "drm-common.h"


static const struct egl *egl;
static const struct gbm *gbm;
static const struct drm *drm;

int main()
{
    const char *device = NULL;
    char mode_str[DRM_DISPLAY_MODE_LEN] = "";
    int connector_id = -1;
    unsigned int vrefresh = 0;
    unsigned int count = ~0;
    uint32_t format = DRM_FORMAT_XRGB8888;
    uint64_t modifier = DRM_FORMAT_MOD_LINEAR;
    int samples = 0;

    drm = init_drm_legacy(device, mode_str, connector_id, vrefresh, count);
    if(!drm) {
        printf("failed to initialize DRM\n");
        return -1;
    }
    gbm = init_gbm(drm->fd, drm->mode->hdisplay, drm->mode->vdisplay, format, modifier);
    if(!gbm) {
        printf("failed to initialize GBM\n");
        return -1;
    }
    egl = init_cube_smooth(gbm, samples);
    glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	return drm->run(gbm, egl);
}