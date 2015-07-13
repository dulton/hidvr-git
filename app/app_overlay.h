
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "overlay.h"

#ifndef APP_OVERLAY_H_
#define APP_OVERLAY_H_
#ifdef __cplusplus
extern "C" {
#endif

extern void APP_OVERLAY_task();

extern int APP_OVERLAY_create_title(int vin, int stream, int x, int y, OVERLAY_FONT_SIZE_t font_size);
extern int APP_OVERLAY_create_clock(int vin, int stream, int x, int y, OVERLAY_FONT_SIZE_t font_size);

extern int APP_OVERLAY_release_title(int vin, int stream);
extern int APP_OVERLAY_release_clock(int vin, int stream);

extern int APP_OVERLAY_update_title(int vin, int stream, size_t width, size_t height);
extern int APP_OVERLAY_update_clock(int vin, int stream, size_t width, size_t height);

extern int APP_OVERLAY_show_title(int vin, int stream, bool show);
extern int APP_OVERLAY_show_clock(int vin, int stream, bool show);

extern int APP_OVERLAY_init();
extern void APP_OVERLAY_destroy();


#ifdef __cplusplus
};
#endif
#endif //APP_OVERLAY_H_

