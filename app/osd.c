
//#define	NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>

#include "jn_sdk/sdk_pri_api.h"

#include "dvr_debug.h"
//#include "stime.h"
#include "timetick.h"
#include "osd.h"
#include "environment.h"

#define DEBUG_OSD
#ifdef DEBUG_OSD
#define OSD_TRACE(fmt...)	\
	do{printf("\033[1;31mOSD->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define OSD_TRACE(fmt, args...)
#endif

//#define AUTO_FLIP_THREAD_USE
#ifdef AUTO_FLIP_THREAD_USE
#define AUTO_FLIP_PERIOD_US (1000000)
#endif

//#define OSD_PUT_SYNC // div bar must be put sync, if this open , title & icon also sync too

#ifndef MAX_TITLE_LEN
#define MAX_TITLE_LEN	(30)
#endif

typedef enum
{
	FMT_YYYY_MM_DD = 0,
	FMT_MM_DD_YYYY,
	FMT_DD_MM_YYYY,
	OSD_DATE_FMT_CNT,
}OSD_DATE_FMT;

typedef enum
{
	FMT_12HOUR = 0,
	FMT_24HOUR,
	OSD_TIME_FMT_CNT,
}OSD_TIME_FMT;

typedef struct OSDSurface
{
	unsigned int u32Width;
	unsigned int u32Height;
	unsigned int u32Stride;
	void* pVirAddr;
}OSDSurface;
#define SRFC(s) (s)

typedef struct OSDRect
{
	int s32Xpos;
	int s32Ypos;
	unsigned int u32Width;
	unsigned int u32Height;
}OSDRect;

#if defined(OSD_ARGB8888_USE)
typedef unsigned int OSDPixel;
#elif defined(OSD_ARGB1555_USE)
typedef unsigned short OSDPixel;
#else
#endif

typedef struct _OSD
{
	// element
	OSDSurface* srfcVGAOut;
	OSDSurface* srfcCVBSOut;

	// about border
	int nCurDiv;
	int nCurPage;
	unsigned char bVisable;

	// osd clock
	struct tm tmClock;
	OSD_DATE_FMT enDateFmt; //
	OSD_TIME_FMT enTimeFmt; // 12 24
	OSDRect rcDateTime;
	OSDSurface* srfcDateTime;

	pthread_mutex_t stMutex;
	OSDSurface* asrfcTitle[MAX_CAM_CH];
	char aszTitle[2][MAX_CAM_CH][MAX_OSD_TITLE_LEN];
	unsigned int anRecState[2][MAX_CAM_CH];
	unsigned char bFlip;
	unsigned char bAutoFlip;
	#ifdef AUTO_FLIP_THREAD_USE
	pthread_t tAutoFlip;
	#endif

	// some image
	OSDSurface* asrfcIcon[0];
	OSDSurface* srfcNoneRecIcon;
	OSDSurface* srfcTimeRecIcon;
	OSDSurface* srfcMotionRecIcon;
	OSDSurface* srfcSensorRecIcon;
	OSDSurface* srfcManualRecIcon;

	// interfaces
	void (*PrintStatus)(struct _OSD* const thiz);
	OSDSurface* (*CreateFrameBuf)(struct _OSD* const thiz, const char* dev, int w, int h);
	void (*FreeFrameBuf)(struct _OSD* const thiz, OSDSurface** surface);
	OSDSurface* (*CreateSurface)(struct _OSD* const thiz, int w, int h);
	void (*FreeSurface)(struct _OSD* const thiz, OSDSurface** surface);
	OSDSurface* (*LoadBMP)(struct _OSD* const thiz, const char* bmp);
	int (*GetSurfaceWidth)(struct _OSD* const thiz, OSDSurface* surface);
	int (*GetSurfaceHeight)(struct _OSD* const thiz, OSDSurface* surface);

	int (*PutPixel)(struct _OSD* const thiz, OSDSurface* surface, int x, int y, OSDPixel color);
	int (*FillRect)(struct _OSD* const thiz, OSDSurface* surface, OSDRect* rect, OSDPixel color);
	int (*ClearRect)(struct _OSD* const thiz, OSDSurface* surface, OSDRect* rect);
	void(*FreeBMP)(struct _OSD* const thiz, OSDSurface** surface);
	int (*PutString)(struct _OSD* const thiz, OSDSurface* surface, int x, int y, const char* text);
	int (*BitBlit)(struct _OSD* const thiz, OSDSurface* src, OSDRect* src_rc, OSDSurface* dst, OSDRect* dst_rc);

	// genal operations
//	int (*PutDivBar)(struct _OSD* const thiz, int div, unsigned char flag);
//	int (*PutTitle)(struct _OSD* const thiz, int div, int page, unsigned char all, unsigned char flag);
//	int (*PutRecState)(struct _OSD* const thiz, int div, int page, unsigned char all, unsigned char flag);

}OSD;
static OSD s_stOSD;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
static const int s_anGrid[] =
{
	[0] = 0,
	[1] = 0,
	[2] = 1,
	[3] = 1,
	[4] = 1,
	[5] = 2,
	[6] = 2,
	[7] = 2,
	[8] = 2,
	[9] = 2,
	[10] = 3,
	[11] = 3,
	[12] = 3,
	[13] = 3,
	[14] = 3,
	[15] = 3,
	[16] = 3,
	[17] = 4,
	[18] = 4,
	[19] = 4,
	[20] = 4,
	[21] = 4,
	[22] = 4,
	[23] = 4,
	[24] = 4,
	[25] = 4,
};
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void osd_PrintStatus(struct _OSD* const thiz)
{
}

static OSDSurface* osd_CreateFrameBuf(struct _OSD* const thiz, const char* dev, int w, int h)
{
	return NULL;
}

static void osd_FreeFrameBuf(struct _OSD* const thiz, OSDSurface** surface)
{
}

static OSDSurface* osd_CreateSurface(struct _OSD* const thiz, int w, int h)
{
	return NULL;
}

static void osd_FreeSurface(struct _OSD* const thiz, OSDSurface** surface)
{
}

static inline OSDPixel osd_PixelConvert(unsigned char* data, int bitcount)
{
	return 0;
}

static OSDSurface* osd_LoadBMP(struct _OSD* const thiz, const char* bmp)
{
	return NULL;
}


static int osd_GetSurfaceWidth(struct _OSD* const thiz, OSDSurface* surface)
{
	return SRFC(surface)->u32Width;
}

static int osd_GetSurfaceHeight(struct _OSD* const thiz, OSDSurface* surface)
{
	return SRFC(surface)->u32Height;
}

static int osd_PutPixel(struct _OSD* const thiz, OSDSurface* surface, int x, int y, OSDPixel color)
{
	return 0;
}

static int osd_FillRect(struct _OSD* const thiz, OSDSurface* surface, OSDRect* rect, OSDPixel color)
{
	return 0;
}

static int osd_ClearRect(struct _OSD* const thiz, OSDSurface* surface, OSDRect* rect)
{
	return thiz->FillRect(thiz, surface, rect, 0);
}

static void osd_FreeBMP(struct _OSD* const thiz, OSDSurface** surface)
{
	return thiz->FreeSurface(thiz, surface);
}

static int osd_PutString(struct _OSD* const thiz, OSDSurface* surface, int x, int y, const char* text)
{
	return 0;
}

// ignore dst_rect->u32Width/h
static int osd_BitBlit(struct _OSD* const thiz, OSDSurface* src, OSDRect* src_rc, OSDSurface* dst, OSDRect* dst_rc)
{
	return 0;
}

static int osd_PutDivBar(int div, unsigned char flag)
{
	return 0;
}

static int osd_PutTitle(int div, int page, unsigned char all, unsigned char flag)
{
	return 0;
}

static int osd_PutRecState(int div, int page, unsigned char all, unsigned char flag)
{
	return 0;
}

static int osd_PutDateTime(unsigned char flag)
{
	return 0;
}

static void osd_RequestFlip()
{
}

static void osd_Flip()
{
}

int OSD_Switch(int div, int page)
{
	return 0;
}

void OSD_Refresh(int nItem)
{
}

int OSD_SetTitle(int chn, const char* title)
{
	return 0;
}

int OSD_SetRecState(int chn, int state)
{
	return 0;
}

static int osd_AutoFlipStart()
{
	return 0;
}

static int osd_AutoFlipStop()
{
	return 0;
}

int OSD_SetDateFormat(const char* date_fmt, const char* time_fmt)
{
	return 0;
}

int OSD_Visable(unsigned char flag)
{
	return 0;
}

int OSD_Init(int nVGAW, int nVGAH, int nCVBSW, int nCVBSH)
{
	return 0;
}

void OSD_Destroy()
{
}

void OSD_PrintStatus(void)
{
}

// 0 - 16
int OSD_SetAlphaScale(unsigned char scale)
{
	const unsigned char ALPHA_SCALE[] =
	{
		127,
		135, 143, 151, 159, 167, 175, 183, 191,
		199, 207, 215, 223, 231, 239, 247, 255,
	};
	if(!(scale <= 16)){
		scale = 16;
	}
	return OSD_SetAlpha(ALPHA_SCALE[scale]);
}

unsigned char s_u8Alpha = 0;
int OSD_SetAlpha(unsigned char alpha)
{
	int i = 0;
	HIFB_ALPHA_S stAlpha;
	int fd[2];
	fd[0] = open(FB_GUI0, O_RDWR); assert(fd[0] > 0);
	fd[1] = open(FB_GUI1, O_RDWR);

	// gui
	stAlpha.bAlphaEnable = 1;
	stAlpha.bAlphaChannel = 1;
	stAlpha.u8Alpha0 = 0;
	stAlpha.u8Alpha1 = 255;
	stAlpha.u8GlobalAlpha = alpha;
	for(i = 0; (i < sizeof(fd) / sizeof(fd[0])) && (fd[i] > 0); ++i){
		if(0 != ioctl(fd[i], FBIOPUT_ALPHA_HIFB, &stAlpha)){
			perror("FBIOGET_ALPHA_HIFB");
			assert(0);
		}
		close(fd[i]);
		fd[i] = -1;
	}
	s_u8Alpha = alpha;
	return 0;
}

unsigned char OSD_GetAlpha()
{
	return s_u8Alpha;
}

//#define TEST_OSD
#ifdef TEST_OSD
int one_test_task(char* input)
{
	int i = 0;
	int div = 0;
	int page = 0;
	int r_data = 0;
	int div_map[] = {1, 4, 9};
	int page_mode[] = {MAX_CAM_CH, 2, 0};
	#if 1
	fgets(input, 100, stdin);
	if(0 == strncasecmp(input, "exit", strlen("exit"))){
		return -1;
	}else if(0 == strncasecmp(input, "von", strlen("von"))){
		OSD_Visable(TRUE);
	}else if(0 == strncasecmp(input, "voff", strlen("voff"))){
		OSD_Visable(FALSE);
	}else if(0 == strncasecmp(input, "yyyymmdd", strlen("yyyymmdd"))){
		OSD_SetDateFormat("yyyymmdd", NULL);
	}else if(0 == strncasecmp(input, "mmddyyyy", strlen("mmddyyyy"))){
		OSD_SetDateFormat("mmddyyyy", NULL);
	}else if(0 == strncasecmp(input, "ddmmyyyy", strlen("ddmmyyyy"))){
		OSD_SetDateFormat("ddmmyyyy", NULL);
	}else if(0 == strncasecmp(input, "12hour", strlen("12hour"))){
		OSD_SetDateFormat(NULL, "12hour");
	}else if(0 == strncasecmp(input, "24hour", strlen("24hour"))){
		OSD_SetDateFormat(NULL, "24hour");
	}else if(0 == strncasecmp(input, "rand", strlen("rand"))){
		for(i = 0; i < MAX_CAM_CH; ++i){
			OSD_SetRecState(i, rand() % (OSD_MANUAL_REC + 1));
		}
		for(i = 0; i < MAX_CAM_CH; ++i){
			FILE* f = fopen("/dev/urandom", "rb");
			int rand_len = rand() % (MAX_TITLE_LEN - 1);
			char title[MAX_TITLE_LEN] = {""};
			fread(title, 1, rand_len, f);
			fclose(f);
			f = NULL;
			OSD_SetTitle(i, title);
		}
	}else
	#endif
	{
		r_data = rand() % 3;
		div = div_map[r_data];
		if(9 == div){
			page = 0;
		}else{
			page = rand() % page_mode[r_data];
		}
		OSD_TRACE("switch div=%d page=%d", div, page);
		OSD_Switch(div, page);
	}
	return 0;
}

int main(int argc, char** argv)
{
	char input[100] = {""};
	int i = 0;
	OSDRect rect;
	OSDSurface* surface = NULL;
	OSDSurface* bmp = NULL;
	int nDivMap[] = {1, 4, 9};
	int nPageMap[] = {MAX_CAM_CH, 2, 0};
	TIMETICK_Init();
	FONT_Init();
	OSD_Init(800, 600, 720, 576);
	OSD_TRACE("clicked test start");
	osd_FillRect(&s_stOSD, s_stOSD.srfcVGAOut, NULL, 0x10ff00ff);

	for(i = 0; i < MAX_CAM_CH; ++i){
		OSD_SetRecState(i, rand() % (OSD_MANUAL_REC + 1));
	}
	for(i = 0; i < MAX_CAM_CH; ++i){
		char title[MAX_TITLE_LEN] = {""};
		sprintf(title, "CAM%d", i + 1);
		OSD_SetTitle(i, title);
	}
	while(1){
		if(one_test_task(input)){
			break;
		}
	}
	OSD_Destroy();
	FONT_Destroy();
	TIMETICK_Destroy();
	return 0;
}
#endif //TEST_OSD


