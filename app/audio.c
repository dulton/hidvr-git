/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : audio.c
  Version       : Initial Draft
  Author        : Law
  Created       : 2011/5/16
  Last Modified :
  Description   : audio routines
  Function List :
  History       :
  1.Date        : 2011/5/16
    Author      : Law
    Modification: Created file

******************************************************************************/

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "jn_sdk/sdk_pri_api.h"

#include "conf.h"
#include "dvr_debug.h"
#include "audio.h"
#include "bsp.h"

#if  defined(_EXT_HDDVR)
#include "hddvr_code.h"
#endif

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

typedef enum _AUDIO_TUNNEL
{
	AUDIO_TUNNEL_LIVE = 0,
	AUDIO_TUNNEL_PLAYBACK,
	AUDIO_TUNNEL_BROADCAST,
	////////////////////////////////////////////////////////////
	AUDIO_TUNNEL_CNT,
}AUDIO_TUNNEL;

typedef struct _Audio
{
	int nFileD;
	int nCurChn;
	unsigned int nLossStat;
	unsigned int nSampleRate;
	unsigned char bEnable;
	unsigned char bLiveLoop;


	unsigned int nModeStat; // live playback broadcast
	unsigned int nTunnel[AUDIO_TUNNEL_CNT];
//	unsigned int nLiveCh;
//	unsigned int nPlaybackCh;
//	unsigned int nBroadcastCh;
}Audio;
static Audio s_stAudio;

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

#define AUDIO_DEVICE     "/dev/audio"

#define DBG_AUDIO
#ifdef DBG_AUDIO
#define	AUDIO_TRACE(fmt,arg...) \
	do{printf("\033[1;35mAUDIO->[%s]:%d ", __FILE__, __LINE__);printf(fmt,##arg);printf("\033[m\r\n");}while(0)
#else
#define	AUDIO_TRACE(fmt, args...)
#endif


/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

static void audio_out_bind_in(int bind)
{
	if(bind){
#if  defined(_EXT_HDDVR)
		HDDVR_AudioLiveLoop(s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE]);
#else
		AUDIODRV_SetLiveLoop(s_stAudio.nFileD, s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE]);
#endif
	}else{
	}
}

static int audio_decode_enable(int tunnel)
{
#define ADEC_G711A_USE (1)
#define ADEC_ADPCM_USE (0)
#define ADEC_LPCM_USE (0)
	ADEC_ATTR_G711_S g711a;
	ADEC_ATTR_ADPCM_S adpcm;
	ADEC_ATTR_LPCM_S lpcm;
	ADEC_CHN_ATTR_S adecattr;
	memset(&g711a, 0, sizeof(g711a));
	memset(&adpcm, 0, sizeof(adpcm));
	memset(&lpcm, 0, sizeof(lpcm));
	memset(&adecattr, 0, sizeof(adecattr));
	if(ADEC_G711A_USE){
		adecattr.enType = PT_G711A;
		adecattr.pValue = &g711a;
	}else if(ADEC_ADPCM_USE){
		adpcm.enADPCMType = ADPCM_TYPE_DVI4;
		adecattr.enType = PT_ADPCMA;
		adecattr.pValue = &adpcm;
	}else if(ADEC_LPCM_USE){
		adecattr.enType = PT_LPCM;
		adecattr.pValue = &lpcm;
	}else{
		AUDIO_TRACE("Audio Decode Format Not Supported");
		return -1;
	}
//	adecattr = MAX_AUDIO_FRAME_NUM;
	adecattr.u32BufSize = 8;
	adecattr.enMode = ADEC_MODE_PACK;
	DVR_ASSERT(HI_MPI_ADEC_CreateChn(tunnel, &adecattr));
	return 0;
#undef ADEC_G711A_USE
#undef ADEC_ADPCM_USE
#undef ADEC_LPCM_USE
}

static void audio_decode_disable(int tunnel)
{
	DVR_ASSERT(HI_MPI_ADEC_DestroyChn(tunnel));
}
static int audio_check_highest_priority(int tunnel);
static int audio_decode_play(int tunnel, void* data, int size, int block)
{
	if(audio_check_highest_priority(tunnel)){
		AUDIO_STREAM_S astream;
		memset(&astream, 0, sizeof(astream));
		astream.pStream = data;
		astream.u32Len = size;
		astream.u64TimeStamp = 0;
		//AUDIO_TRACE("playback decode size = %d adec = %d", size, tunnel);
		//HI_MPI_ADEC_SendStream(tunnel, &astream, block ? HI_IO_BLOCK : HI_IO_NOBLOCK);
		HI_MPI_ADEC_SendStream(tunnel, &astream, HI_IO_BLOCK);
		return 0;
	}
	return -1;
}

static void audio_out_bind_decode(int bind, int tunnel)
{
	if(bind){
		audio_decode_enable(tunnel);
		DVR_ASSERT(HI_MPI_ADEC_ClearChnBuf(tunnel));
		DVR_ASSERT(HI_MPI_AO_BindAdec(AOUT_DEV, AOUT_CHN, tunnel));
#if  defined(_EXT_HDDVR)
		HDDVR_AudioPlayBack(0);
#else
		AUDIODRV_SetPlayback(s_stAudio.nFileD);
#endif
#ifdef HDMI_AUDIO_SUPPORT
		(HI_MPI_Ao_BindAdecByForce(HDMI_AOUT_DEV, tunnel));
#endif
	}else{
		DVR_ASSERT(HI_MPI_AO_UnBindAdec(AOUT_DEV, AOUT_CHN, tunnel));
		audio_decode_disable(tunnel);
	}
}

static void audio_out_bind_playback(int bind)
{
	audio_out_bind_decode(bind, AUDIO_TUNNEL_PLAYBACK);
}

static void audio_out_bind_broadcast(int bind)
{
	audio_out_bind_decode(bind, AUDIO_TUNNEL_BROADCAST);
}

typedef void (*audio_out_bind_hook)(int bind);
static const audio_out_bind_hook audio_out_bind_ch[] =
{
	[AUDIO_TUNNEL_LIVE] = audio_out_bind_in,
	[AUDIO_TUNNEL_PLAYBACK] = audio_out_bind_playback,
	[AUDIO_TUNNEL_BROADCAST] = audio_out_bind_broadcast,
};

static void audio_mark_in_priority(int tunnel)
{
	s_stAudio.nModeStat |= (1<<tunnel);
}

static void audio_mark_out_priority(int tunnel)
{
	s_stAudio.nModeStat &= ~(1<<tunnel);
}

static int audio_active(int tunnel)
{
	return s_stAudio.nModeStat & (1<<tunnel) ? TRUE : FALSE;
}

static int audio_check_highest_priority(int tunnel)
{
	int i = 0;
	if(audio_active(tunnel)){
		for(i = tunnel + 1; i < AUDIO_TUNNEL_CNT; ++i){
			if(audio_active(i)){
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

static void audio_out_bind(int bind, int tunnel)
{
	int i = 0;
	if(bind){
		if(audio_check_highest_priority(tunnel)){
			// prevent doule bind
			AUDIO_TRACE("tunnel = %d active", tunnel);
			return;
		}
		audio_mark_in_priority(tunnel);
		if(audio_check_highest_priority(tunnel)){
			for(i = 0; i < tunnel; ++i){
				if(audio_active(i)){
					audio_out_bind_ch[i](FALSE);
				}
			}
			// bind the current audio tunnel
			audio_out_bind_ch[tunnel](TRUE);
		}
	}else{
		if(audio_check_highest_priority(tunnel)){
			audio_out_bind_ch[tunnel](FALSE);
			for(i = tunnel - 1; i >= 0; --i){
				if(audio_active(i)){
					audio_out_bind_ch[i](TRUE);
					break;
				}
			}
		}
		audio_mark_out_priority(tunnel);
	}
}

int AUDIO_Init()
{
	int i = 0;
	AIO_ATTR_S stAioAttr = {0};
	// open the driver node
	s_stAudio.nFileD = open(AUDIO_DEVICE, O_RDWR);
	// get live loop capability
#if  defined(_EXT_HDDVR)
	s_stAudio.bLiveLoop = TRUE;
#else
	s_stAudio.bLiveLoop = AUDIODRV_IsCapLiveLoop(s_stAudio.nFileD) ? TRUE : FALSE;
#endif
	AUDIO_TRACE("Live Loop Capability = %s", s_stAudio.bLiveLoop ? "Yes" : "No");
	// get sample rate
	s_stAudio.nSampleRate = AUDIODRV_SetSampleRate(s_stAudio.nFileD, SAMPLE_RATE_8000); // set to 8k samplerate
	AUDIO_TRACE("Set Sample Rate = %d", s_stAudio.nSampleRate);
	// config aio dev attr
    memset(&stAioAttr, 0, sizeof(stAioAttr));
	stAioAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32EXFlag = 1;
	//stAioAttr.u32FrmNum = MAX_AUDIO_FRAME_NUM; // ver 050 faq 1.6.3
	stAioAttr.u32FrmNum = 32;
	//stAioAttr.u32PtNumPerFrm = 480;
	stAioAttr.u32PtNumPerFrm = 320; // ver 050 faq 1.6.3
	stAioAttr.u32ClkSel = 0;

#if defined(SDK_PLATFORM_HI3515)
	if(GPIO_PROBE_VER1 == GPIODRV_GetVerFix()) {
		stAioAttr.u32ClkSel = 1;
	}
#endif

#if defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#if !(defined(GPIO_PLAT_TYPE7) || defined(GPIO_PLAT_TYPE12))
	stAioAttr.u32ClkSel = 1;
#endif
#endif

#if defined(_EXT_HDDVR)
	stAioAttr.u32FrmNum = MAX_AUDIO_FRAME_NUM;
#endif

//Hisilicon BUG: Wrong AO/AI Orders Cause Audio Missing or Crossing.
#if defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
	// enable AO tunnel
	stAioAttr.u32ChnCnt = 2;
	DVR_ASSERT(HI_MPI_AO_SetPubAttr(AOUT_DEV, &stAioAttr));
	DVR_ASSERT(HI_MPI_AO_Enable(AOUT_DEV));
	DVR_ASSERT(HI_MPI_AO_EnableChn(AOUT_DEV, AOUT_CHN));

	// enable AI channle
	stAioAttr.u32ChnCnt = 16;
	DVR_ASSERT(HI_MPI_AI_SetPubAttr(AIN_DEV, &stAioAttr));
	DVR_ASSERT(HI_MPI_AI_Enable(AIN_DEV));
	// if with live loop
	// ai could be init when avenc init
	if(!AUDIO_IsCapLiveLoop()){
		//for(i = 0; i < stAioAttr.u32ChnCnt; ++i){
		for(i = 0; i < MAX_CAM_CH; ++i){
			DVR_ASSERT(HI_MPI_AI_EnableChn(AIN_DEV, i));
		}
	}
#else
	// enable AI channle
	stAioAttr.u32ChnCnt = 16;
	DVR_ASSERT(HI_MPI_AI_SetPubAttr(AIN_DEV, &stAioAttr));
	DVR_ASSERT(HI_MPI_AI_Enable(AIN_DEV));
	// if with live loop
	// ai could be init when avenc init
	if(!AUDIO_IsCapLiveLoop()){
		//for(i = 0; i < stAioAttr.u32ChnCnt; ++i){
		for(i = 0; i < MAX_CAM_CH; ++i){
			DVR_ASSERT(HI_MPI_AI_EnableChn(AIN_DEV, i));
		}
	}

	// enable AO tunnel
	stAioAttr.u32ChnCnt = 2;
	DVR_ASSERT(HI_MPI_AO_SetPubAttr(AOUT_DEV, &stAioAttr));
	DVR_ASSERT(HI_MPI_AO_Enable(AOUT_DEV));
	DVR_ASSERT(HI_MPI_AO_EnableChn(AOUT_DEV, AOUT_CHN));
#endif

#ifdef HDMI_AUDIO_SUPPORT
	// enable AO tunnel
	stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
	stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
	stAioAttr.u32PtNumPerFrm = 320*6;
	stAioAttr.u32ChnCnt = 2;
	stAioAttr.u32ClkSel = 0;

	DVR_ASSERT(HI_MPI_AO_SetPubAttr(HDMI_AOUT_DEV, &stAioAttr));
	DVR_ASSERT(HI_MPI_AO_Enable(HDMI_AOUT_DEV));
	DVR_ASSERT(HI_MPI_AO_EnableChn(HDMI_AOUT_DEV, HDMI_AOUT_CHN));
	{
		AUDIO_RESAMPLE_ATTR_S tmpASR;
		tmpASR.u32InPointNum   = 320;
		tmpASR.enInSampleRate  = AUDIO_SAMPLE_RATE_8000;
		tmpASR.enReSampleType  = AUDIO_RESAMPLE_1X6;
		DVR_ASSERT(HI_MPI_AO_DisableReSmp(HDMI_AOUT_DEV,HDMI_AOUT_CHN));
		DVR_ASSERT(HI_MPI_AO_EnableReSmp(HDMI_AOUT_DEV, HDMI_AOUT_CHN, &tmpASR));
	}
#endif

//	AUDIO_LiveEn(TRUE);
//	AUDIO_LiveSet(0);
//	AUDIO_PlaybackEn(TRUE);
//	AUDIO_BroadcastEn(TRUE);


	// bind AI to AO channle
	// reset audio tunnel
	s_stAudio.nCurChn = 0;
	s_stAudio.bEnable = TRUE;

//	ai_bind_ao(s_stAudio.nCurChn, TRUE);
	return 0;
}

int AUDIO_Destroy()
{
	int i = 0;
	AUDIO_LiveEn(FALSE);
	// disable vo
//	ai_bind_ao(s_stAudio.nCurChn, FALSE);
#ifdef HDMI_AUDIO_SUPPORT
	DVR_ASSERT(HI_MPI_AO_DisableChn(HDMI_AOUT_DEV, HDMI_AOUT_CHN));
	DVR_ASSERT(HI_MPI_AO_Disable(HDMI_AOUT_DEV));
#endif

	DVR_ASSERT(HI_MPI_AO_DisableChn(AOUT_DEV, AOUT_CHN));
	DVR_ASSERT(HI_MPI_AO_Disable(AOUT_DEV));

	// disable vi
	if(!AUDIO_IsCapLiveLoop()){
		for(i = 0; i < MAX_CAM_CH; ++i){
			DVR_ASSERT(HI_MPI_AI_DisableChn(AIN_DEV, i));
		}
	}
	DVR_ASSERT(HI_MPI_AI_Disable(AIN_DEV));
	if(s_stAudio.nFileD){
		close(s_stAudio.nFileD);
		s_stAudio.nFileD = -1;
	}
	return 0;
}

inline unsigned char AUDIO_IsCapLiveLoop()
{
	return s_stAudio.bLiveLoop;
}

void AUDIO_Enable()
{
//	s_stAudio.bEnable = TRUE;
//	ai_bind_ao(s_stAudio.nCurChn, TRUE);
}

void AUDIO_Disable()
{
//	s_stAudio.bEnable = FALSE;
//	ai_bind_ao(s_stAudio.nCurChn, FALSE);
//	AUDIODRV_SetPlayback(s_stAudio.nFileD);
}

void AUDIO_Switch(int nChn)
{
	if(audio_active(AUDIO_TUNNEL_BROADCAST)){
		AUDIO_BroadcastSet(nChn);
	}else if(audio_active(AUDIO_TUNNEL_PLAYBACK)){
		AUDIO_PlaybackSet(nChn);
	}else if(audio_active(AUDIO_TUNNEL_LIVE)){
		AUDIO_LiveSet(nChn);
	}
}

int AUDIO_GetChn()
{
	int i = 0;
	for(i = AUDIO_TUNNEL_CNT - 1; i >= 0; --i){
		if(audio_active(i)){
			return s_stAudio.nTunnel[i];
		}
	}
	return 0;
}

// 0 - 7
int AUDIO_SetVolume(int nVolume)
{
#if  defined(_EXT_HDDVR)
	HDDVR_AudioVolume(0, nVolume);
	return 0;
#else
	if(s_stAudio.nFileD){
		AUDIODRV_SetVolume(s_stAudio.nFileD, nVolume);
		return 0;
	}
#endif
	return -1;
}

unsigned int AUDIO_GetSampleRate()
{
	return s_stAudio.nSampleRate;
}

int AUDIO_LiveEn(int enable)
{
	audio_out_bind(enable, AUDIO_TUNNEL_LIVE);
	return 0;
}

int AUDIO_LiveSet(int tunnel)
{
	if(AUDIO_IsCapLiveLoop()){
		s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE] = tunnel;
#if  defined(_EXT_HDDVR)
		HDDVR_AudioLiveLoop(s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE]);
#else
		AUDIODRV_SetLiveLoop(s_stAudio.nFileD, s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE]);
#endif
#if 0 //enable resample
#ifdef HDMI_AUDIO_SUPPORT
		if(tunnel >= 16) {
			DVR_ASSERT(HI_MPI_Sys_UnBindByForce(HDMI_AOUT_DEV, AIN_DEV, 0));
		}
		else {
			DVR_ASSERT(HI_MPI_Ao_BindAiByForce(HDMI_AOUT_DEV, AIN_DEV, tunnel));
		}
#endif
#endif 
	}else{
//		audio_in_bind(FALSE, s_stAudio.nLiveCh);
//		s_stAudio.nLiveCh = tunnel;
//		audio_in_bind(TRUE, s_stAudio.nLiveCh);
	}
	return 0;
}

int AUDIO_LiveHdmiGet()
{
#ifdef HDMI_AUDIO_SUPPORT	
	if(audio_active(AUDIO_TUNNEL_PLAYBACK))
	{
		return -1;
	}
	else if(audio_active(AUDIO_TUNNEL_LIVE))
	{
		return s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE];
	}
#endif

	return -1;

}

int AUDIO_LivePlay(int tunnel)
{
	return 0;
}

int AUDIO_PlaybackEn(int enable)
{
	audio_out_bind(enable, AUDIO_TUNNEL_PLAYBACK);
	return 0;
}

int AUDIO_PlaybackPlay(void* data, int size, int block)
{
	return audio_decode_play(AUDIO_TUNNEL_PLAYBACK, data, size, block);
}

int AUDIO_PlaybackSet(int tunnel)
{
	s_stAudio.nTunnel[AUDIO_TUNNEL_PLAYBACK] = tunnel;
	return 0;
}

int AUDIO_BroadcastEn(int enable)
{
	audio_out_bind(enable, AUDIO_TUNNEL_BROADCAST);
	return 0;
}

int AUDIO_BroadcastSet(int tunnel)
{
	s_stAudio.nTunnel[AUDIO_TUNNEL_BROADCAST] = tunnel;
	return 0;
}

int AUDIO_BroadcastPlay(void* data, int size, int block)
{
	return audio_decode_play(AUDIO_TUNNEL_BROADCAST, data, size, block);
}

void AUDIO_Info()
{
	printf("Audio Information:\r\n");
	printf("Mode Status = %x\r\n", s_stAudio.nModeStat);
	printf("Live Channel = %d\r\n", s_stAudio.nTunnel[AUDIO_TUNNEL_LIVE]);
	printf("Playback Channel = %d\r\n", s_stAudio.nTunnel[AUDIO_TUNNEL_PLAYBACK]);
	printf("Broadcast Channel = %d\r\n", s_stAudio.nTunnel[AUDIO_TUNNEL_BROADCAST]);
}





