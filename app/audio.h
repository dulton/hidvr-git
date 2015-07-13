/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : audio.h
  Version       : Initial Draft
  Author        : Law
  Created       : 2011/5/16
  Last Modified :
  Description   : auto descriptions
  Function List :
  History       :
  1.Date        : 2011/5/16
    Author      : Law
    Modification: Created file

******************************************************************************/

#ifndef __AUDIO_H__
#define __AUDIO_H__

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#if defined(SDK_PLATFORM_HI3515)
#define AIN_DEV (0)
#define AOUT_DEV (0)
#define AOUT_CHN (0)

#elif defined(SDK_PLATFORM_HI3520)
#define AIN_DEV (1)
#define AOUT_DEV (0)
#define AOUT_CHN (0)

#elif defined(SDK_PLATFORM_HI3521)
#if defined(GPIO_PLAT_TYPE7)
#define AIN_DEV (0)
#define AOUT_DEV (0)
#define AOUT_CHN (0)

#define HDMI_AUDIO_SUPPORT 1
#define HDMI_AOUT_DEV (1)
#define HDMI_AOUT_CHN (0)
#else
#define AIN_DEV (1)
#define AOUT_DEV (2)
#define AOUT_CHN (0)

#define HDMI_AUDIO_SUPPORT 1
#define HDMI_AOUT_DEV (3)
#define HDMI_AOUT_CHN (0)
#endif

#elif defined(SDK_PLATFORM_HI3531)
#define AIN_DEV (3)
#define AOUT_DEV (4)
#define AOUT_CHN (0)

#define HDMI_AUDIO_SUPPORT 1
#define HDMI_AOUT_DEV (5)
#define HDMI_AOUT_CHN (0)

#else
#error "Unknown SDK PlatForm!!"
#endif


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

extern int AUDIO_Init();
extern int AUDIO_Destroy();

extern inline unsigned char AUDIO_IsCapLiveLoop();

extern void AUDIO_Enable();
extern void AUDIO_Disable();

extern void AUDIO_Switch(int nChn);

extern int AUDIO_GetChn();

extern int AUDIO_SetVolume(int nVolume);

extern int AUDIO_LiveEn(int enable);
extern int AUDIO_LiveSet(int tunnel);
extern int AUDIO_LivePlay(int tunnel);

extern int AUDIO_PlaybackEn(int enable);
extern int AUDIO_PlaybackSet(int tunnel);
extern int AUDIO_PlaybackPlay(void* data, int size, int block);

extern int AUDIO_BroadcastEn(int enable);
extern int AUDIO_BroadcastSet(int tunnel);
extern int AUDIO_BroadcastPlay(void* data, int size, int block);

extern void AUDIO_Info();

#endif //__AUDIO_H__

