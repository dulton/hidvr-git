
#ifndef __OSD_H__
#define __OSD_H__

#include "conf.h"

#if defined(SDK_PLATFORM_HI3515)
#define FB_GUI0 "/dev/fb0"
#define FB_GUI1 "/dev/fb2"

#elif defined(SDK_PLATFORM_HI3520)
#define FB_GUI0 "/dev/fb0"
#define FB_GUI1 "/dev/fb3"

#elif defined(SDK_PLATFORM_HI3521)
#define FB_GUI0 "/dev/fb0"
#define FB_GUI1 "/dev/fb2"

#elif defined(SDK_PLATFORM_HI3531)
#define FB_GUI0 "/dev/fb1"
#define FB_GUI1 "/dev/fb2"

#else
#error "not HI3520 / HI3515 @ tde"
#endif

#ifndef MAX_OSD_TITLE_LEN
#define MAX_OSD_TITLE_LEN (20)
#endif

//#define OSD_ARGB8888_USE (32)
#define OSD_ARGB1555_USE (16)

#if defined(OSD_ARGB8888_USE)
#define OSD_FONT_FGCOLOR (0xffffff | (0x80 << 24))
#define OSD_FONT_BGCOLOR (0x000000 | (0x60 << 24))
#define OSD_PIXEL_AMASK (0xff000000)
#define OSD_PIXEL_BITSIZE (OSD_ARGB8888_USE)
#elif defined(OSD_ARGB1555_USE)
#define OSD_FONT_FGCOLOR (0xe318 | (1 << 15))
#define OSD_FONT_BGCOLOR (0x2108 | (1 << 15))
#define OSD_PIXEL_AMASK (0x8000)
#define OSD_PIXEL_BITSIZE (OSD_ARGB1555_USE)
#else
#endif
#define OSD_PIXEL_BYTESIZE (((OSD_PIXEL_BITSIZE + 7) & ~7) / 8)

#define OSD_BAR_WEIGHT (3)

#define OSD_NONE_REC   (0)
#define OSD_TIME_REC  (1)
#define OSD_MOTION_REC (2)
#define OSD_SENSOR_REC (3)
#define OSD_MANUAL_REC (4)

#define OSD_CLOCK_POSX	(5)
#define OSD_CLOCK_POSY	(-30)

#define OSD_PATH_PREFIX "/root/dvr_app/"

//
// std: pal / ntsc
// date_fmt: "yyyymmdd" / "mmddyyyy" / "ddmmyyyy"
// time_fmt: "12hour" / "24hour"
//
extern int OSD_Init(int nVGAW, int nVGAH, int nCVBSW, int nCVBSH);
extern void OSD_Destroy();

extern void OSD_PrintStatus(void);

extern int OSD_SetAlphaScale(unsigned char alpha);
extern int OSD_SetAlpha(unsigned char alpha);
extern unsigned char OSD_GetAlpha();

extern  int OSD_Switch(int div, int page);
extern void OSD_Refresh(int nItem);
extern int OSD_SetTitle(int chn, const char* title);
extern int OSD_SetRecState(int chn, int state);

//
// date_fmt: "yyyymmdd" / "mmddyyyy" / "ddmmyyyy"
// time_fmt: "12hour" / "24hour"
//
extern int OSD_SetDateFormat(const char* date_fmt, const char* time_fmt);

extern int OSD_Visable(unsigned char flag);

#endif //__OSD_H__

