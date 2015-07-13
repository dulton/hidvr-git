/*
 * 2011/03/12
 * auther: Law
 */

#ifndef	__VIDEO_H__
#define	__VIDEO_H__

#include "conf.h"
#include "bsp.h"

#ifdef _NVR
#define VO_DEV_CNT (1)
#else
#if   defined(FORCE_NORMAL_CVBS_OUTPUT) \
		|| defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define VO_DEV_CNT (2)
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#define VO_DEV_CNT (1)
#endif
#endif

#if defined(SDK_PLATFORM_HI3515)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3520)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3521)
#define VO_VGA_DEVICE  (0)
#define VO_CVBS_DEVICE (2)

#elif defined(SDK_PLATFORM_HI3531)
#define VO_VGA_DEVICE  (1)
#define VO_CVBS_DEVICE (2)

#else
#error "null vo device"
#endif

#define HI_VO_DEV(i) (0==i?VO_VGA_DEVICE:VO_CVBS_DEVICE)
#define VO_SYNC_GROUP (0)

extern int VIDEO_GetMaxGrid();
extern int VIDEO_GetMaxDiv();

extern void video_ScreenInit(void);
extern void video_ScreenExit(void);
extern int  video_StandardInit(void);

extern void VIDEO_Init();
extern void VIDEO_Destroy();

extern void* VIDEO_GetUserPic(int _chn);
extern unsigned char VIDEO_IsLoss(int nChn);

extern int VIDEO_GetCurDivi();
extern int VIDEO_GetCurPage();

extern void VIDEO_SetLiveMode();
extern void VIDEO_SetPlaybackMode();
extern unsigned char VIDEO_IsLiveMode();

extern int VIDEO_Switch(int nDiv, int nPage);
extern int VIDEO_SwitchFull(int nPage);
extern int VIDEO_SwitchQuad(int nPage);
extern int VIDEO_SwitchX9(int nPage);
extern int VIDEO_SwitchX16(int nPage);
extern int VIDEO_SwitchDefault();

// nValue: 0 - 15 other: ignore
extern int VIDEO_SetColor(int nChn, int nHue, int nBrightness, int nContrast, int nSaturation);
// nValue: 0 - 15
extern int VIDEO_SetHue(int nChn, int nValue);
extern int VIDEO_SetBrightness(int nChn, int nValue);
extern int VIDEO_SetContrast(int nChn, int nValue);
extern int VIDEO_SetSaturation(int nChn, int nValue);

// auto switching
extern int VIDEO_StartAutoSwitch(int nDivi, int nTickSec);
extern void VIDEO_StopAutoSwitch();
extern void VIDEO_ResetAutoSwitch();

// output control
extern unsigned char VIDEO_IsPower();
extern void VIDEO_PowerOff();
extern void VIDEO_PowerOn();
extern void VIDEO_Power();

// digital zoom
extern int VIDEO_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH);
extern void VIDEO_ClearDigitalZoom(int nChn);

// tv adjust
extern void VIDEO_GetTvSize(int* ret_nSrcX, int* ret_nSrcY, int* ret_nWidth, int* ret_nHeight);
extern void VIDEO_SetTvSize(bSync);
extern void video_TvAdjust(int x, int y, int w, int h, int bSync);
extern void video_WbcAdjust(int x, int y, int w, int h);

#define VO_SPEED_RATE_X0_5 0
#define VO_SPEED_RATE_X1_0 1
#define VO_SPEED_RATE_X2_0 2

void video_ChnSpeedRate(int Chn, int nRate);
void video_ChnFrameRate(int Chn, int nFrameRate);
void video_VoSpeedRate(int nRate);
void video_VoOutputOpa(int nEnable);

#endif	//__VIDEO_H__

