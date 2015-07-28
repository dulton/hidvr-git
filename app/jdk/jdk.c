#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jdk_common.h"
#include "sysconf.h"

#define JDK_ALIGN_2BYTE(x) (2 * ((x) / 2))


lpJDK_CTX s_jdkCtx = NULL;

#include "hisi/jdk_hi_3521.c"


JDK_BOOL jdk_bnc_vin_is_loss(JDK_UINT32 nChn);

#if 0
static inline JDK_SOC_BRAND JDK_get_brand()
{
	return JDK_SOC_BRAND_HISI;
}

static inline JDK_SOC_CHIP JDK_get_soc_chip()
{
#if defined(SDK_PLATFORM_HI3521)
	return JDK_HI_SOC_3521;
#elif defined(SDK_PLATFORM_HI3531)
	return JDK_HI_SOC_3531;
#else
	#error "unknow soc chip"
#endif
}

static inline JDK_UINT32 JDK_get_chn_num()
{
	return MAX_CAM_CH;
}


static inline JDK_UINT32 JDK_get_stream_num()
{
	return 2;
}

static inline JDK_UINT32 JDK_get_audio_num()
{
	return MAX_AUD_CH;
}


static inline JDK_UINT32 JDK_get_sensor_num()
{
	return MAX_SENSOR_CH;
}

static inline JDK_UINT32 JDK_get_alarm_num()
{
	return MAX_ALARM_CH;
}
#endif

/*******************************************************
********************************************************/


static JDK_BOOL jdk_check_iface()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface->deinit != NULL, JDK_FALSE);
	
	jdk_return_val_if_fail(s_jdkCtx->adIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->get_color != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->set_color != NULL, JDK_FALSE);	
	jdk_return_val_if_fail(s_jdkCtx->adIface->deinit != NULL, JDK_FALSE);
	
	jdk_return_val_if_fail(s_jdkCtx->viIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->deinit != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->open != NULL, JDK_FALSE);	
	jdk_return_val_if_fail(s_jdkCtx->viIface->close != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->is_open != NULL, JDK_FALSE);

	jdk_return_val_if_fail(s_jdkCtx->voIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->voIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->voIface->deinit != NULL, JDK_FALSE);
	
	jdk_return_val_if_fail(s_jdkCtx->vencIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vencIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vencIface->deinit != NULL, JDK_FALSE);
	
	jdk_return_val_if_fail(s_jdkCtx->vdecIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface->init != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface->deinit != NULL, JDK_FALSE);

	return JDK_TRUE;
}


JDK_BOOL JDK_new(lpJDK_ARG arg)
{
	JDK_UINT32 ii;
	JDK_UINT32 zeroChnNum = 0;
	
	jdk_return_val_if_fail(arg != NULL, JDK_FALSE);
	jdk_return_val_if_fail((arg->nBncChn + arg->nNetChn) <= JDK_MAX_CHN, JDK_FALSE);
	jdk_return_val_if_fail(arg->nBncChn <= arg->nMaxBncChn, JDK_FALSE);
	jdk_return_val_if_fail(arg->nStream <= JDK_MAX_STREAM, JDK_FALSE);
	jdk_return_val_if_fail(arg->nDdr <= 2, JDK_FALSE);
	jdk_return_val_if_fail(((arg->nBncChn + arg->nNetChn) == 4) ||
		((arg->nBncChn + arg->nNetChn) == 8) ||
		((arg->nBncChn + arg->nNetChn) == 9) ||
		((arg->nBncChn + arg->nNetChn) == 16) ||
		((arg->nBncChn + arg->nNetChn) == 24) ||
		((arg->nBncChn + arg->nNetChn) == 25) ||
		((arg->nBncChn + arg->nNetChn) == 36) ||
		((arg->nBncChn + arg->nNetChn) == 48) ||
		((arg->nBncChn + arg->nNetChn) == 49) ||
		((arg->nBncChn + arg->nNetChn) == 64), JDK_FALSE);

	VO_GRID_init(arg->nBncChn + arg->nNetChn);

	zeroChnNum = VO_GRID_get_max_grids(arg->nBncChn + arg->nNetChn) - (arg->nBncChn + arg->nNetChn);
	jdk_return_val_if_fail((zeroChnNum  == 0) || (zeroChnNum == 1), JDK_FALSE);
	
	if (s_jdkCtx == NULL) {
		s_jdkCtx = (lpJDK_CTX)calloc(1, sizeof(stJDK_CTX));
		jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
		
		s_jdkCtx->brand = arg->brand;
		s_jdkCtx->socChip = arg->soc;
		s_jdkCtx->adChip = arg->ad;
		s_jdkCtx->standard = arg->standard;
		s_jdkCtx->maxBncChnNum = arg->nMaxBncChn;
		s_jdkCtx->bncChnNum = arg->nBncChn;
		s_jdkCtx->netChnNum = arg->nNetChn;
		s_jdkCtx->zeroChnNum = zeroChnNum;
		s_jdkCtx->totalChnNum = arg->nBncChn + arg->nNetChn + zeroChnNum;
		s_jdkCtx->streamNum = arg->nStream;
		s_jdkCtx->audioNum = arg->nAudio;
		s_jdkCtx->sensorNum = arg->nSensor;
		s_jdkCtx->alarmNum = arg->nAlarm;
		s_jdkCtx->maxImgResol = arg->maxImgResol;
		s_jdkCtx->maxBncCamImgResol = arg->maxBncCamImgResol;
		s_jdkCtx->maxNetCamImgResol = arg->maxNetCamImgResol;
		s_jdkCtx->nPlaybackCam = arg->nPlaybackCam;
		s_jdkCtx->maxPlaybackResol = arg->maxPlaybackResol;

		s_jdkCtx->voIsOn = JDK_FALSE;
		s_jdkCtx->nCVBSMode = JDK_DEF_SD_MONITOR_MODE;
		s_jdkCtx->hdDispResol = JDK_VGA_1024x768;
		s_jdkCtx->nVoDev = 1;
		s_jdkCtx->nVoDevList[0] = JDK_MONITOR_VGA;
		s_jdkCtx->nVoDevList[1] = JDK_MONITOR_CVBS;
		s_jdkCtx->bLiveMode = JDK_TRUE;
		s_jdkCtx->bFixWHRatio = JDK_FALSE;
		s_jdkCtx->nMaxGrid = VO_GRID_get_max_grids(JDK_ACTIVE_CAM());
		s_jdkCtx->nMaxRowOrCol = VO_GRID_get_max_row_or_col_by_grids(JDK_ACTIVE_CAM());
		s_jdkCtx->nCurDivision = VO_GRID_get_def_divi(JDK_ACTIVE_CAM());
		s_jdkCtx->nMaxPage = VO_GRID_get_n_pages(s_jdkCtx->nCurDivision);
		s_jdkCtx->nCurPage = 0;

		s_jdkCtx->voSwitchHook = NULL;
		s_jdkCtx->voSwapHook = NULL;

		s_jdkCtx->lossDaemonPid = 0;
		s_jdkCtx->lossLoopTrigger = FALSE;
		s_jdkCtx->videoCheckFreeze = FALSE;
		s_jdkCtx->lostState = 0;
		s_jdkCtx->signalLocked = 0;
		s_jdkCtx->netCamQueryLostState = NULL;
		s_jdkCtx->bncCamQueryLostState = jdk_bnc_vin_is_loss;

		s_jdkCtx->viCheckDaemonPid = 0;
		s_jdkCtx->viCheckLoopTrigger = FALSE;
		s_jdkCtx->viInModeAuto = JDK_FALSE;
		s_jdkCtx->viFormatChangedHook = NULL;
		
		memset(s_jdkCtx->viChnState , 0, sizeof(s_jdkCtx->viChnState));

		for (ii = 0; ii < JDK_MAX_CHN; ii++) {
			s_jdkCtx->viChnState[ii] = JDK_CHN_STATE_NULL;
			s_jdkCtx->viChnMode[ii] = JDK_INPUT_MODE_NULL;
			s_jdkCtx->viVoChnMap[ii] = ii;
			s_jdkCtx->viVencChnMap[ii] = ii;

			s_jdkCtx->viChnMode[ii]  = JDK_INPUT_MODE_NULL;		
			if (ii < 64)
				u64_set_bit(&s_jdkCtx->lostState, ii);
			
			s_jdkCtx->brightness[ii] = -1;
			s_jdkCtx->contrast[ii] = -1;
			s_jdkCtx->saturation[ii] = -1;
			s_jdkCtx->hue[ii] = -1;
			s_jdkCtx->sharpness[ii] = -1;
		}

		// init ad chip interfaces
		if (JDK_AD_CHIP_0 == arg->ad) {
			s_jdkCtx->adIface = &g_jdkAdIface;
		}

		// init soc chip interfaces
		if (arg->brand == JDK_SOC_BRAND_HISI) {
			jdk_goto_if_fail((arg->soc > JDK_HI_SOC_BASE) && (arg->soc < JDK_HI_SOC_END), _fail_return);
			switch(arg->soc) {
				case JDK_HI_SOC_3521:
					s_jdkCtx->sysIface = &g_jdkHi3521SysIface;
					s_jdkCtx->viIface  = &g_jdkHi3521ViIface;
					s_jdkCtx->voIface = &g_jdkHi3521VoIface;
					s_jdkCtx->vencIface = &g_jdkHi3521VencIface;
					s_jdkCtx->vdecIface = &g_jdkHi3521VdecIface;
					break;
				case JDK_HI_SOC_3520D:
					s_jdkCtx->sysIface = &g_jdkHi3521SysIface;
					s_jdkCtx->viIface  = &g_jdkHi3521ViIface;
					s_jdkCtx->voIface = &g_jdkHi3521VoIface;
					s_jdkCtx->vencIface = &g_jdkHi3521VencIface;
					s_jdkCtx->vdecIface = &g_jdkHi3521VdecIface;
					break;
				case JDK_HI_SOC_3531:
					s_jdkCtx->sysIface = &g_jdkHi3521SysIface;
					s_jdkCtx->viIface  = &g_jdkHi3521ViIface;
					s_jdkCtx->voIface = &g_jdkHi3521VoIface;
					s_jdkCtx->vencIface = &g_jdkHi3521VencIface;
					s_jdkCtx->vdecIface = &g_jdkHi3521VdecIface;
					break;
				default:
					goto _fail_return;
			}
		} else {
			goto _fail_return;
		}

		// check interface
		//jdk_goto_if_fail(jdk_check_iface(), _fail_return);
	}
	
	return JDK_TRUE;

_fail_return:
{
	if (s_jdkCtx) free(s_jdkCtx);
	s_jdkCtx = NULL;
	return JDK_FALSE;
}
}

void JDK_finalize()
{
	if (s_jdkCtx) {
		free(s_jdkCtx);
		s_jdkCtx = NULL;
	}
}

JDK_BOOL JDK_init()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	if (s_jdkCtx->sysIface) {
		s_jdkCtx->sysIface->init(s_jdkCtx->standard);
	}
	if (s_jdkCtx->adIface) {
		s_jdkCtx->adIface->init(s_jdkCtx->standard, JDK_TRUE);
	}
	if (s_jdkCtx->viIface) {
		s_jdkCtx->viIface->init(s_jdkCtx->standard);
	}
	//if (s_jdkCtx->voIface) {
	//	s_jdkCtx->voIface->init();
	//}
	if (s_jdkCtx->vencIface) {
		s_jdkCtx->vencIface->init();
	}
	if (s_jdkCtx->vdecIface) {
		s_jdkCtx->vdecIface->init();
	}

	return JDK_TRUE;
}

void JDK_deinit()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	
	if (s_jdkCtx->viIface) {
		s_jdkCtx->viIface->deinit();
	}

	if (s_jdkCtx->adIface) {
		s_jdkCtx->adIface->deinit();
	}
	//if (s_jdkCtx->voIface) {
	//	s_jdkCtx->voIface->deinit();
	//}
	if (s_jdkCtx->vencIface) {
		s_jdkCtx->vencIface->deinit();
	}
	if (s_jdkCtx->vdecIface) {
		s_jdkCtx->vdecIface->deinit();
	}
	if (s_jdkCtx->sysIface) {
		s_jdkCtx->sysIface->deinit();
	}
}

JDK_BOOL JDK_set_tv_adjust(JDK_INT32 x, JDK_INT32 y, JDK_INT32 w, JDK_INT32 h, JDK_BOOL bSync)
{
#define __ALLIGNMENT_BY(x, y) {(x) = (x) - (x)%(y);}
	JDK_UINT32 voW, voH;
	JDK_UINT32  nKeepW = 0;
	JDK_UINT32  nKeepH = 0;

	JDK_resol_get_wh(JDK_RESOL_D1, &voW, &voH);
	nKeepW = voW  - JDK_TVSIZE_SCALAR*JDK_TVSIZE_STEPER;
	nKeepH = voH - JDK_TVSIZE_SCALAR*JDK_TVSIZE_STEPER;
	
	if(JDK_TVSIZE_SCALAR < x || x < 0) x = 0;
	if(JDK_TVSIZE_SCALAR < y || y < 0) y = 0;
	if(JDK_TVSIZE_SCALAR < w || w < 0) w = JDK_TVSIZE_SCALAR;
	if(JDK_TVSIZE_SCALAR < h || h < 0) h = JDK_TVSIZE_SCALAR;

	
	s_jdkCtx->voScreenRect[1].x = (x)*JDK_TVSIZE_STEPER;
	s_jdkCtx->voScreenRect[1].y = (y)*JDK_TVSIZE_STEPER;
	s_jdkCtx->voScreenRect[1].width = nKeepW + (w-x)*JDK_TVSIZE_STEPER;
	s_jdkCtx->voScreenRect[1].height = nKeepH + (h-y)*JDK_TVSIZE_STEPER;

	__ALLIGNMENT_BY(s_jdkCtx->voScreenRect[1].x, 2);
	__ALLIGNMENT_BY(s_jdkCtx->voScreenRect[1].y, 2);
	__ALLIGNMENT_BY(s_jdkCtx->voScreenRect[1].width, 2);
	__ALLIGNMENT_BY(s_jdkCtx->voScreenRect[1].height, 2);

	JDK_MIN2(s_jdkCtx->voScreenRect[1].width, 16);
	JDK_MIN2(s_jdkCtx->voScreenRect[1].height, 16);

	if (s_jdkCtx->voIsOn) {
		stJDK_VO_CHN_ARG voChnArg;
		voChnArg.rect.x = s_jdkCtx->voScreenRect[1].x;
		voChnArg.rect.y = s_jdkCtx->voScreenRect[1].y;
		voChnArg.rect.width = s_jdkCtx->voScreenRect[1].width;
		voChnArg.rect.height = s_jdkCtx->voScreenRect[1].height;
		jdk_return_val_if_fail(s_jdkCtx->voIface->close_chn(s_jdkCtx->nVoDevList[1], 0), JDK_FALSE);
		jdk_return_val_if_fail(s_jdkCtx->voIface->open_chn(s_jdkCtx->nVoDevList[1], 0, &voChnArg), JDK_FALSE);
	} else {
		jdk_return_val_if_fail(s_jdkCtx->voIface->init(s_jdkCtx->nVoDevList[1], s_jdkCtx->standard, 0, 
			s_jdkCtx->voScreenRect[1].x, s_jdkCtx->voScreenRect[1].y, s_jdkCtx->voScreenRect[1].width, s_jdkCtx->voScreenRect[1].height), JDK_FALSE);
	}

	return JDK_TRUE;
}


JDK_BOOL JDK_monitor_init(JDK_MONITOR_TYPE monitors, JDK_VGA_RESOLUTION_TYPE dispResol, JDK_RESOLUTION_TYPE maxImgResol, 
	JDK_UINT32 nCVBSOffsetX, JDK_UINT32 nCVBSOffsetY, JDK_UINT32 nCVBSWidth, JDK_UINT32 nCVBSHeight, JDK_UINT32 nCVBSMode)
{
	JDK_INT32 i;
	JDK_UINT32 imgW, imgH, dispW, dispH;

	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	jdk_return_val_if_fail(JDK_vga_resol_get_wh(dispResol, &dispW, &dispH), JDK_FALSE);
	jdk_return_val_if_fail(JDK_resol_get_wh(maxImgResol, &imgW, &imgH), JDK_FALSE);

	s_jdkCtx->nVoDev = 1;

	s_jdkCtx->hdDispResol = dispResol;
	s_jdkCtx->maxImgResol = maxImgResol;
	s_jdkCtx->voSrcWidth[0] =  imgW;
	s_jdkCtx->voSrcHeight[0] = imgH;
	s_jdkCtx->voScreenRect[0].x = 0;
	s_jdkCtx->voScreenRect[0].y = 0;
	s_jdkCtx->voScreenRect[0].width = dispW;
	s_jdkCtx->voScreenRect[0].height = dispH;

	if (JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3531)) {
#if  defined(_EXT_HDDVR_LITE)
		if(s_jdkCtx->voScreenRect[0].width > 1440) {
			s_jdkCtx->voScreenRect[0].width = 1440;
			s_jdkCtx->voScreenRect[0].height = 1080;
		}
#endif
	}

	jdk_return_val_if_fail(s_jdkCtx->voIface->init(s_jdkCtx->nVoDevList[0], s_jdkCtx->standard, dispResol, 0, 0, imgW, imgH), JDK_FALSE);


	if (monitors & JDK_MONITOR_CVBS) {
		s_jdkCtx->nVoDev = 2;
		s_jdkCtx->voSrcWidth[1] =  704;
		s_jdkCtx->voSrcHeight[1] = JDK_STANDARD_IS_PAL() ? 576 : 480;
		s_jdkCtx->voScreenRect[1].x = nCVBSOffsetX;
		s_jdkCtx->voScreenRect[1].y = nCVBSOffsetY;
		s_jdkCtx->voScreenRect[1].width = s_jdkCtx->voSrcWidth[1];
		s_jdkCtx->voScreenRect[1].height = s_jdkCtx->voSrcHeight[1];

		s_jdkCtx->nCVBSMode = nCVBSMode;

		jdk_return_val_if_fail(JDK_set_tv_adjust(nCVBSOffsetX, nCVBSOffsetY, nCVBSWidth, nCVBSHeight, JDK_TRUE), JDK_FALSE);
		//jdk_return_val_if_fail(s_jdkCtx->voIface->init(s_jdkCtx->nVoDevList[1], s_jdkCtx->standard, 0, nCVBSOffsetX, nCVBSOffsetY, 0, 0), JDK_FALSE);
	}
	
	s_jdkCtx->voIsOn = JDK_TRUE;

	return JDK_TRUE;
}


void JDK_monitor_deinit()
{
	JDK_INT32 n, i;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	// close all vo chn
	for (n  = 0; n < JDK_TOTAL_CAM(); n++) {
		for (i = 0; i < s_jdkCtx->nVoDev; i++) {
			jdk_return_if_fail(s_jdkCtx->voIface->close_chn(s_jdkCtx->nVoDevList[i], n));
		}
	}

	for (i = 0; i < s_jdkCtx->nVoDev; i++) {
		jdk_return_if_fail(s_jdkCtx->voIface->deinit(s_jdkCtx->nVoDevList[i]));
	}
	s_jdkCtx->voIsOn = JDK_FALSE;
}

JDK_BOOL JDK_monitor_switch()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	if (s_jdkCtx->voIsOn == JDK_TRUE) {
		JDK_monitor_deinit();
	} else {
		JDK_UINT32 nMonTypes = JDK_MONITOR_VGA;
		if (s_jdkCtx->nVoDev > 1) {
			nMonTypes |= JDK_MONITOR_CVBS;
		}
		if (JDK_monitor_init(nMonTypes, s_jdkCtx->hdDispResol, s_jdkCtx->maxImgResol, 
			s_jdkCtx->voScreenRect[1].x, s_jdkCtx->voScreenRect[1].y, s_jdkCtx->voScreenRect[1].width, s_jdkCtx->voScreenRect[1].height, 
			s_jdkCtx->nCVBSMode) == JDK_TRUE) {
			JDK_set_vo_division(VO_DIVISION_DEF, 0);
		}
	}
	return s_jdkCtx->voIsOn;
}


JDK_BOOL JDK_set_live_mode(void)
{
	int i;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	// clear digital zoom
	for (i = 0; i < JDK_ACTIVE_CAM(); i++) {
		JDK_clear_digital_zoom(i);
	}

	// cleanup vdec for playback
	JDK_vdec_deinit();
	
	// set vo-chn for live
	JDK_set_vo_division(VO_DIVISION_DEF, 0);
	
	// switch to live mode
	s_jdkCtx->bLiveMode = JDK_TRUE;
	
	// reinit vdec for live
	JDK_vdec_init();

	// lass daemon	
	JDK_vo_loss_daemon();
	
	return JDK_TRUE;
}

static JDK_BOOL JDK_show_user_pic(void)
{
	JDK_UINT32 n, nDev, nUserPicIndex, nPageChnBase, nGrids;
	JDK_UINT32 nVoDevSize = s_jdkCtx->nVoDev;
	
	nGrids = VO_GRID_get_grids_of_one_page(s_jdkCtx->nCurDivision);
	nPageChnBase = nGrids * s_jdkCtx->nCurPage;

	if ((s_jdkCtx->nVoDev == 2) && (JDK_CVBS_MONITOR_MODE() == JDK_SD_MODE_DATA_BY_HD_WBC)) {
		nVoDevSize = 1;
	}
	
	if (jdk_is_live_mode()) {
		for (n = 0; (n < nGrids) && (n + nPageChnBase < JDK_ACTIVE_CAM()); n++) {
			for (nDev = 0; nDev < nVoDevSize; nDev++) 
			{
				nUserPicIndex = JCONF_IS_BNC_CAM(n + nPageChnBase) ? JDK_USR_PIC_LOSS: JDK_USR_PIC_IDLE;
				if (JDK_vin_is_loss(nPageChnBase + n) == JDK_TRUE) {
					if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
						s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[nDev], n + nPageChnBase);
					if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
						s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[nDev], n + nPageChnBase, s_jdkCtx->hChnUserPic[nUserPicIndex]);
				}
			}
		}
	} else {
		for (n = 0; (n < nGrids) && (n + nPageChnBase < JDK_ACTIVE_CAM()); n++) {
			for (nDev = 0; nDev < nVoDevSize; nDev++) 
			{
				nUserPicIndex = JDK_USR_PIC_LOGO;
				if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
					s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[nDev], n + nPageChnBase);
				if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
					s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[nDev], n + nPageChnBase, s_jdkCtx->hChnUserPic[nUserPicIndex]);
			}
		}
	}

	return JDK_TRUE;
}

JDK_BOOL JDK_vo_show_picture(JDK_UINT32 chn, JDK_UINT32 nUserPicType)
{
	JDK_UINT32 nDev;
	
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(chn < JCONF_CAM_END(), JDK_FALSE);
	jdk_return_val_if_fail(nUserPicType < 4, JDK_FALSE);

	for (nDev = 0; nDev < 1; nDev++) 
	{
		if (VO_GRID_CHN_IS_IN_PAGE(JDK_VL_CUR_DIVI(),JDK_VL_CUR_PAGE(), chn)) {
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
				s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[nDev], chn);
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
				s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[nDev], chn, s_jdkCtx->hChnUserPic[nUserPicType]);
		}
	}
	return JDK_TRUE;
}


JDK_BOOL JDK_set_play_mode(JDK_UINT32 nPlayCam /* if 0, use default play cam setted by jdk_new */)
{
	JDK_UINT32 n;
	VO_DIVISION_TYPE divi = 0;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	if (nPlayCam == 0) nPlayCam = s_jdkCtx->nPlaybackCam;
	if (nPlayCam == 0) nPlayCam = 1;
	s_jdkCtx->nCurPlayCam = nPlayCam;
	
	// clear digital zoom
	for (n = 0; n < JDK_MAX_PLAYBACK_CAM(); n++) {
		JDK_clear_digital_zoom(n);
	}

	// 1, exit loss daemon
	JDK_exit_vo_loss_daemon();

	// 2, stop net-cam live-view
	

	// 3, cleanup live-mode vdec
	JDK_vdec_deinit();

	// 4, set play-mode	
	s_jdkCtx->bLiveMode = JDK_FALSE;

	// 5, unbind vo
	for ( n = 0; n < JCONF_CAM_END(); n++) {
		if (JCONF_IS_BNC_CAM(n)) {
			s_jdkCtx->viIface->unbind_output(n, n);
		} 
		//else {
		//	s_jdkCtx->vdecIface->unbind_vo(n, JDK_MONITOR_VGA, n);
		//}
		//if (s_jdkCtx->voIface && s_jdkCtx->voIface->detach_binding) {
		//	s_jdkCtx->voIface->detach_binding(JDK_MONITOR_VGA, n);
		//}
	}

	// 7, set vo-chn for playback
	divi = VO_GRID_grids_to_divi(VO_GRID_get_max_grids(nPlayCam));
	//JDK_set_vo_division(divi, 0);
	
	// 6, reinit vdec for playback
	JDK_vdec_init();
	
	//JDK_show_user_pic();

	
	//JDK_set_vo_division(divi, 0);
	
	return JDK_TRUE;

}

JDK_BOOL JDK_set_vo_switch_hook(fJDK_VO_SWITCH_HOOK hook)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	s_jdkCtx->voSwitchHook = hook;
	return JDK_TRUE;
}

JDK_BOOL JDK_set_vo_swap_hook(fJDK_VO_SWAP_HOOK hook)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	s_jdkCtx->voSwapHook = hook;
	return JDK_TRUE;
}


JDK_BOOL JDK_set_vo_division(VO_DIVISION_TYPE div, JDK_UINT32 nPage)
{
	JDK_UINT32 nVoChn, n, nDev, nGrids = 1, nPageChnBase = 0, nMaxPages = 1;
	lpGRID_RECT prect = NULL;
	VO_DIVISION_TYPE oldDiv;
	JDK_UINT32 oldPage;
	JDK_UINT32 nUserPicIndex = 0;
	
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	oldDiv = s_jdkCtx->nCurDivision;
	oldPage = s_jdkCtx->nCurPage;

	if (div == VO_DIVISION_DEF) {
		div = VO_GRID_get_def_divi(JDK_ACTIVE_CAM());
	}

	nMaxPages = VO_GRID_get_n_pages(div);
	jdk_return_val_if_fail(nPage < nMaxPages, JDK_FALSE);
	
	nPageChnBase = VO_GRID_get_grids_of_one_page(div) * nPage;

	// 0, clear all digital zoom
	for (nVoChn = 0; nVoChn < JDK_TOTAL_CAM(); nVoChn++) {
		JDK_clear_digital_zoom(nVoChn);
	}

	for (nDev = 0; nDev < s_jdkCtx->nVoDev; nDev++)
	{
		stJDK_VO_CHN_ARG arg;

		if (nDev == 1 && (JDK_CVBS_MONITOR_MODE() == JDK_SD_MODE_DATA_BY_HD_WBC)) {
			continue;
		}
		
		//1>  get vo chn args
		nGrids = VO_GRID_get_rect_of_grid(div, &s_jdkCtx->voScreenRect[nDev], &prect);
		jdk_return_val_if_fail(prect != NULL, JDK_FALSE);
		
		// 2>  close vo chn
		for (nVoChn = 0; nVoChn < JDK_ACTIVE_CAM(); nVoChn++) {
			if (s_jdkCtx->voIface->hide_chn(s_jdkCtx->nVoDevList[nDev], nVoChn) == JDK_FALSE) 
			{
				JDK_TRACE("hide chn:%u false", nVoChn);
			}
		}

		// 3> open vo chn
		for (nVoChn = 0; (nVoChn < nGrids) && (nPageChnBase + nVoChn < JDK_ACTIVE_CAM()); nVoChn++) {
			memcpy(&arg.rect , prect + nVoChn, sizeof(stJDK_RECT));
			if (s_jdkCtx->voIface->open_chn(s_jdkCtx->nVoDevList[nDev],nPageChnBase + nVoChn, &arg) == JDK_FALSE) 
			{
				JDK_TRACE("open chn:%u false", nPageChnBase + nVoChn);
			}
		}

		// cleanup prect
		free(prect);
		prect = NULL;
	}

	//4,  update current state
	s_jdkCtx->nCurDivision = div;
	s_jdkCtx->nCurPage = nPage;

	// 5, handle main stream vo chn....
	// ....
	
	// 6, show user frame to net vo chn
	nGrids = VO_GRID_get_grids_of_one_page(div);

	//if (jdk_is_live_mode()) {
		JDK_show_user_pic();
	//}


	// reset vivomap
	for (nVoChn = 0; nVoChn < JDK_TOTAL_CAM(); nVoChn++) {
		s_jdkCtx->viVoChnMap[nVoChn] = nVoChn;
	}
	for (nVoChn = 0; nVoChn < nGrids; nVoChn++) {
		s_jdkCtx->voChnMap[nVoChn] = nVoChn;
	}

	// 7, call vo chn switch hook
	if (s_jdkCtx->voSwitchHook)
		s_jdkCtx->voSwitchHook(oldDiv, oldPage, div, nPage);
	
	return JDK_TRUE;
}

JDK_BOOL JDK_swap_vo_chn(JDK_UINT32 chn1, JDK_UINT32 chn2)
{
	JDK_BOOL hasBigGrid = JDK_FALSE;
	JDK_UINT32 div, nPage, nGrids, nDev, nVoChn, nGridSize, npageBase;
	stJDK_SIZE size1, size2;
	lpGRID_RECT prect = NULL;
	stJDK_VO_CHN_ARG arg;
	JDK_UINT32 tmp;
	
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	div = s_jdkCtx->nCurDivision;
	nPage = s_jdkCtx->nCurPage;
	nDev = 0;

	if (div == VO_DIVISION_DEF) {
		div = VO_GRID_get_def_divi(JDK_ACTIVE_CAM());
	}
	if (div == VO_DIVISION_X1 && div >= VO_DIVISION_NR) {
		return JDK_FALSE;
	}
	if (chn2 > JDK_ACTIVE_CAM() || chn1 > JDK_ACTIVE_CAM()) {
		return JDK_FALSE;
	}

	nGridSize = VO_GRID_get_size_of_grid(div, &s_jdkCtx->voScreenRect[nDev], &size1, &size2);
	if (nGridSize == 0) {
		return JDK_FALSE;
	}

	//1>  get vo chn args
	nGrids = VO_GRID_get_rect_of_grid(div, &s_jdkCtx->voScreenRect[nDev], &prect);
	jdk_return_val_if_fail(prect != NULL, JDK_FALSE);

	
	npageBase = VO_GRID_BASE_GRID_OF_PAGE(div, nPage);
	if (!((chn1 >= 0) && (chn1 < nGrids)
		&& (chn2 >= 0) && (chn2 < nGrids))) {
		return JDK_FALSE;
	}
	
	// 2>  close vo chn
	if (s_jdkCtx->voIface->hide_chn(s_jdkCtx->nVoDevList[nDev],  npageBase + s_jdkCtx->voChnMap[chn1]) == JDK_FALSE) 
	{
		JDK_TRACE("hide chn:%u false", npageBase + s_jdkCtx->voChnMap[chn1]);
	}
	if (s_jdkCtx->voIface->hide_chn(s_jdkCtx->nVoDevList[nDev],  npageBase + s_jdkCtx->voChnMap[chn2]) == JDK_FALSE) 
	{
		JDK_TRACE("hide chn:%u false", npageBase + s_jdkCtx->voChnMap[chn2]);
	}

	// 3> open vo chn
	nDev = 0;
	memcpy(&arg.rect , prect + chn2, sizeof(stJDK_RECT));
	if (s_jdkCtx->voIface->open_chn(s_jdkCtx->nVoDevList[nDev],npageBase + s_jdkCtx->voChnMap[chn1], &arg) == JDK_FALSE) 
	{
		JDK_TRACE("open chn:%u false", npageBase + s_jdkCtx->voChnMap[chn1]);
	}
	memcpy(&arg.rect , prect + chn1, sizeof(stJDK_RECT));
	if (s_jdkCtx->voIface->open_chn(s_jdkCtx->nVoDevList[nDev],npageBase + s_jdkCtx->voChnMap[chn2], &arg) == JDK_FALSE) 
	{
		JDK_TRACE("open chn:%u false", npageBase + s_jdkCtx->voChnMap[chn2]);
	}

	// 4> show usr pic
	if (jdk_is_live_mode()) {
		JDK_show_user_pic();
	}

	// 5> cleanup prect
	free(prect);
	prect = NULL;	

	// 6> check and switch main/sub stream
	hasBigGrid = VO_GRID_HAS_BIG_GRID(div);
	if ((hasBigGrid == JDK_TRUE) && (chn1 == 0 || chn2 == 0)) {
		JDK_BOOL fromNeedSwitch = JDK_TRUE, toNeedSwitch = JDK_TRUE;
		if (s_jdkCtx->voSwapHook) s_jdkCtx->voSwapHook(npageBase + s_jdkCtx->voChnMap[chn1], fromNeedSwitch, npageBase + s_jdkCtx->voChnMap[chn2], toNeedSwitch);
	}

	// 7> update vo-chn map
	tmp = s_jdkCtx->voChnMap[chn1];
	s_jdkCtx->voChnMap[chn1] = s_jdkCtx->voChnMap[chn2];
	s_jdkCtx->voChnMap[chn2] = (JDK_UINT8)tmp;


	return JDK_TRUE;
}

JDK_UINT32 jdk_vl_get_cur_page(void)
{
	return (s_jdkCtx ? s_jdkCtx->nCurPage: 0);
}
JDK_UINT32 jdk_vl_get_cur_division(void)
{
	return (s_jdkCtx ? s_jdkCtx->nCurDivision: 0);
}

JDK_UINT32 jdk_vl_get_cur_divi_grids(void)
{
	return (s_jdkCtx ? VO_GRID_get_grids_of_one_page(s_jdkCtx->nCurDivision) : 1);
}
JDK_UINT32 jdk_vl_get_cur_divi_n_pages(void)
{
	return (s_jdkCtx ? VO_GRID_get_n_pages(s_jdkCtx->nCurDivision) : 1);
}
JDK_UINT32 jdk_vl_get_def_divi(void)
{
	return VO_GRID_get_def_divi(JDK_ACTIVE_CAM());
}
JDK_BOOL jdk_is_live_mode(void)
{
	return 	(s_jdkCtx ? s_jdkCtx->bLiveMode : JDK_TRUE);
}

JDK_BOOL JDK_sys_init()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface->init(s_jdkCtx->standard), JDK_FALSE);

 #ifdef _HVR
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_IDLE] = s_jdkCtx->sysIface->open_frame("skin/net_cam.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOSS] = s_jdkCtx->sysIface->open_frame("skin/bnc_cam.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOGO] = s_jdkCtx->sysIface->open_frame("skin/logo.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_RECONNECT] = s_jdkCtx->sysIface->open_frame("skin/reconnect.yuv", 352, 240);
#elif defined(_NVR)
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_IDLE] = s_jdkCtx->sysIface->open_frame("skin/disable.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOSS] = s_jdkCtx->sysIface->open_frame("skin/loss.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOGO] = s_jdkCtx->sysIface->open_frame("skin/logo.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_RECONNECT] = s_jdkCtx->sysIface->open_frame("skin/reconnect.yuv", 352, 240);
#else
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_IDLE] = NULL;
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOSS] = s_jdkCtx->sysIface->open_frame("skin/loss.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOGO] = s_jdkCtx->sysIface->open_frame("skin/logo.yuv", 704, 480);
	s_jdkCtx->hChnUserPic[JDK_USR_PIC_RECONNECT] = NULL;
#endif
	jdk_return_val_if_fail(s_jdkCtx->adIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->init(s_jdkCtx->standard, JDK_TRUE), JDK_FALSE);

	return JDK_TRUE;
}
JDK_BOOL JDK_sys_deinit()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->sysIface->deinit(), JDK_FALSE);

	return JDK_TRUE;

}

JDK_BOOL JDK_vin_init(JDK_BOOL viInModeAuto)
{
	JDK_INT32 i, voDev, voId;
	
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface != NULL, JDK_FALSE);
	
	s_jdkCtx->viInModeAuto = viInModeAuto;
	jdk_return_val_if_fail(s_jdkCtx->viIface->init(s_jdkCtx->standard), JDK_FALSE);
	
#if 0
	for ( i = 0; i < JDK_BNC_CAM(); i++) {
		stJDK_VIN_ARG arg;
		memset(&arg, 0, sizeof(arg));
		
		JDK_inmode_get_wh(s_jdkCtx->maxBncCamImgResol, &arg.rect.width, &arg.rect.height);
		arg.rect.x = 0;
		arg.rect.y = 0;
		arg.inMode = JDK_INPUT_BNC_720P;
		arg.fps = 25;
		arg.flip = JDK_FALSE;
		arg.mirror = JDK_FALSE;
		// open vi 
		jdk_return_val_if_fail(s_jdkCtx->viIface->open(i, &arg), JDK_FALSE);

		//
		/*
		// unbind vi->vo
		if (s_jdkCtx->voIface && s_jdkCtx->voIface->detach_binding) {
			for (voDev = 0; voDev < s_jdkCtx->nVoDev; voDev++) {
				s_jdkCtx->voIface->detach_binding(s_jdkCtx->nVoDevList[voDev], i);
			}
		} else {
			if (s_jdkCtx->viIface && s_jdkCtx->viIface->unbind_output) {
				s_jdkCtx->viIface->unbind_output(i, i);
			}
		}
		*/
		/*
		// set vo chn
		for (voId = 0; voId < s_jdkCtx->nVoDev; voId++) {
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
				s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[voId], i);
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
				s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[voId], i, s_jdkCtx->hChnUserPic[JDK_USR_PIC_LOSS]);
		}
		*/
	}
	for ( ; i < JDK_ACTIVE_CAM(); i++) {
		stJDK_VIN_ARG arg;
		memset(&arg, 0, sizeof(arg));
		
		JDK_inmode_get_wh(s_jdkCtx->maxNetCamImgResol, &arg.rect.width, &arg.rect.height);
		arg.rect.x = 0;
		arg.rect.y = 0;
		arg.inMode = s_jdkCtx->maxNetCamImgResol;
		arg.fps = 30;
		arg.flip = JDK_FALSE;
		arg.mirror = JDK_FALSE;

		// open net vi
		jdk_return_val_if_fail(s_jdkCtx->viIface->open(i, &arg), JDK_FALSE);

		/*
		// set vo chn
		for (voId = 0; voId < s_jdkCtx->nVoDev; voId++) {
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
				s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[voId], i);
			if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
				s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[voId], i, s_jdkCtx->hChnUserPic[JDK_USR_PIC_IDLE]);
		}
		*/
	}
#endif

	return JDK_TRUE;
}

JDK_BOOL JDK_vin_deinit()
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->deinit(), JDK_FALSE);

	return JDK_TRUE;
}

JDK_BOOL jdk_bnc_vin_is_loss(JDK_UINT32 nChn)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_TRUE);
	jdk_return_val_if_fail(s_jdkCtx->adIface != NULL, JDK_TRUE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->is_loss != NULL, JDK_TRUE);

	if (nChn < s_jdkCtx->maxBncChnNum) {
		JDK_BOOL isLoss = s_jdkCtx->adIface->is_loss(nChn);
		if (isLoss == FALSE) {
			u64_set_bit(&s_jdkCtx->signalLocked, nChn);
		} else {
			u64_clear_bit(&s_jdkCtx->signalLocked, nChn);
		}
		return isLoss;
	} else {
		return JDK_TRUE;
	}
}

JDK_BOOL JDK_vin_is_loss(JDK_UINT32 nChn)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_TRUE);

	if (nChn < JDK_ACTIVE_CAM()) {
		return ((u64_get_bit(s_jdkCtx->lostState, nChn) != 0) ? JDK_TRUE : JDK_FALSE);
	} else {
		return JDK_TRUE;
	}
}

JDK_BOOL JDK_vin_open(JDK_UINT32 chn, lpJDK_VIN_ARG arg)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	if (arg->inMode <  JDK_INPUT_BNC_END)
		jdk_return_val_if_fail(JDK_IS_VALID_VI_MUX(arg->muxMode), JDK_FALSE);
	
	if (chn < JDK_ACTIVE_CAM()) {
		if ((JDK_IS_BNC_CAM(chn) == JDK_TRUE)) {
#if defined VI_DEV_2MUX_CHNL

			if(arg->inMode == JDK_INPUT_FORMAT_AUTO){
				if(s_jdkCtx->adIface && s_jdkCtx->adIface->probe_vi_format){
					arg->inMode = s_jdkCtx->adIface->probe_vi_format(chn);
				}else{
					arg->inMode = s_jdkCtx->maxBncCamImgResol;
				}
			}
			if (arg->inMode == JDK_INPUT_FORMAT_UNKNOWN) {
				JDK_TRACE("--> chn %u probe mode is %s", chn, "UNKNOWN");
				arg->inMode = s_jdkCtx->maxBncCamImgResol;
			} else {
				JDK_TRACE("--> chn %u probe mode is %s", chn, g_JdkChnInputModeString[arg->inMode]);
			}
			if (((arg->inMode > s_jdkCtx->maxBncCamImgResol) && (arg->cutMode == JDK_VI_CUT_MODE_NONE))
				|| (arg->inMode == JDK_INPUT_BNC_HFHD)) {
				JDK_TRACE("exceed maxBncCamImgResol, chn %u set cut-mode: 1/2", chn);
				arg->cutMode = JDK_VI_CUT_MODE_1_2;
			}
#else
			if ((chn % arg->muxMode) == 0) {
				if(arg->inMode == JDK_INPUT_FORMAT_AUTO){
					if(s_jdkCtx->adIface && s_jdkCtx->adIface->probe_vi_format){
						arg->inMode = s_jdkCtx->adIface->probe_vi_format(chn);
					}else{
						arg->inMode = s_jdkCtx->maxBncCamImgResol;
					}
				}
				if (arg->inMode == JDK_INPUT_FORMAT_UNKNOWN) {
					JDK_TRACE("--> chn %u probe mode is %s", chn, "UNKNOWN");
					arg->inMode = s_jdkCtx->maxBncCamImgResol;
				} else {
					JDK_TRACE("--> chn %u probe mode is %s", chn, g_JdkChnInputModeString[arg->inMode]);
				}
				if (((arg->inMode > s_jdkCtx->maxBncCamImgResol) && (arg->cutMode == JDK_VI_CUT_MODE_NONE))
					|| (arg->inMode == JDK_INPUT_BNC_HFHD)) {
					JDK_TRACE("exceed maxBncCamImgResol, chn %u set cut-mode: 1/2", chn);
					arg->cutMode = JDK_VI_CUT_MODE_1_2;
				}
			} else {
				if (s_jdkCtx->viChnMode[chn-chn%arg->muxMode] != JDK_INPUT_MODE_NULL) {
					arg->inMode = s_jdkCtx->viChnMode[chn-chn%arg->muxMode];
					arg->cutMode = s_jdkCtx->viArg[chn-chn%arg->muxMode].cutMode;
					arg->muxMode = s_jdkCtx->viArg[chn-chn%arg->muxMode].muxMode;
				} else {
					arg->inMode = s_jdkCtx->maxBncCamImgResol;
				}
			}
#endif
		}
		sleep_ms_c(10); // ---necessary ---
		JDK_TRACE("chn %u probe mode is %s , mux:%u cutmode:%u", chn, g_JdkChnInputModeString[arg->inMode], arg->muxMode, arg->cutMode);
		jdk_return_val_if_fail(s_jdkCtx->viIface->open(chn, arg), JDK_FALSE);
		s_jdkCtx->viChnMode[chn] = arg->inMode;
		s_jdkCtx->viChnModeTemp[chn] = arg->inMode;

		JDK_ad_set_color(chn, s_jdkCtx->brightness[chn], s_jdkCtx->contrast[chn], s_jdkCtx->saturation[chn], s_jdkCtx->hue[chn]);
		return JDK_TRUE;
	} else {
		return JDK_FALSE;
	}
}

JDK_BOOL JDK_vin_close(JDK_UINT32 chn)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	if (chn < JDK_ACTIVE_CAM()) {
		return s_jdkCtx->viIface->close(chn);
	} else {
		return JDK_FALSE;
	}
}

JDK_UINT32 JDK_vin_get_max_support(void)
{
	JDK_UINT32 vsupport;

	switch(HDDVR_VSupportCheck()){
	case HDDVR_VSUPPORT_SD960H:
		vsupport = JDK_VSUPPORT_SD960H;
		break;
	case HDDVR_VSUPPORT_HD720P:
		vsupport = JDK_VSUPPORT_HD720P;
		break;
	case HDDVR_VSUPPORT_HD1080P:
		vsupport = JDK_VSUPPORT_HD1080P;
		break;
	default:
		vsupport = JDK_VSUPPORT_SD960H;
		break;
	}
	
	return vsupport;
}

JDK_BOOL JDK_vin_get_size(JDK_UINT32 chn, lpJDK_SIZE size)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	if (chn < JDK_ACTIVE_CAM()) {
		return s_jdkCtx->viIface->get_vi_size(chn, size);
	} else {
		return JDK_FALSE;
	}
}

JDK_BOOL JDK_vin_set_input_format(JDK_UINT32 chn, JDK_INT32 format)
{
	JDK_INT32 voId =0;
	stJDK_VIN_ARG arg;
	lpJDK_CTX thiz = s_jdkCtx;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);

	memset(&arg, 0, sizeof(arg));
	
	if (JDK_IS_BNC_CAM(chn)) {
		if ((format >= JDK_INPUT_BNC_BASE && format < JDK_INPUT_BNC_END) 
			|| format == JDK_INPUT_FORMAT_AUTO) {
			if (format != thiz->viChnMode[chn]) {
				thiz->videoCheckFreeze = TRUE;
				
				// ------------------------open vi 
				JDK_inmode_get_wh(thiz->maxBncCamImgResol, &arg.rect.width, &arg.rect.height);
				arg.inMode = format;
				arg.rect.x = 0;
				arg.rect.y = 0;
				// FIX ME  cutmode & muxMode
				arg.cutMode = JDK_VI_CUT_MODE_NONE;
				if (((arg.inMode > s_jdkCtx->maxBncCamImgResol) && (arg.cutMode == JDK_VI_CUT_MODE_NONE))
					|| (arg.inMode == JDK_INPUT_BNC_HFHD)) {
					JDK_TRACE("exceed maxBncCamImgResol, chn %u set cut-mode: 1/2", chn);
					arg.cutMode = JDK_VI_CUT_MODE_1_2;
				}
				arg.muxMode = JDK_get_mux_mode();
				jdk_goto_if_fail(thiz->viIface->close(chn), _fail_exit);
				jdk_goto_if_fail(JDK_vin_open(chn, &arg), _fail_exit);

				
				//
				// -----------------------reset to loss state
				// unbind vi->vo
				if (thiz->viIface && thiz->viIface->unbind_output) {
					thiz->viIface->unbind_output(chn, chn);
				}
				// bind vdec -> vo
				for (voId = 0; voId < thiz->nVoDev; voId++) {
					thiz->vdecIface->bind_vo(chn, thiz->nVoDevList[voId], chn);
				}
				
				// set vo chn
				for (voId = 0; voId < thiz->nVoDev; voId++) 
				{
					if (thiz->voIface && thiz->voIface->enable_user_pic)
						thiz->voIface->enable_user_pic(thiz->nVoDevList[voId], chn);
					if (thiz->voIface && thiz->voIface->show_user_pic) {
						JDK_UINT32 usrPicIndex = JDK_USR_PIC_LOSS;
						thiz->voIface->show_user_pic(thiz->nVoDevList[voId], chn, thiz->hChnUserPic[usrPicIndex]);
					}
				}	
				
				u64_set_bit(&thiz->lostState, chn);
				u64_clear_bit(&thiz->signalLocked, chn);
				
				thiz->videoCheckFreeze = FALSE;

				JDK_TRACE("chn %d format changed", chn);
			}
			return JDK_TRUE;
		} else {
			return JDK_FALSE;
		}
	} else {
		return JDK_FALSE;
	}

_fail_exit:
{
	thiz->videoCheckFreeze = FALSE;
	return JDK_FALSE;
}
}

JDK_BOOL JDK_ad_set_color(int nChn, float nBrightness, float nContrast, float nSaturation, float nHue)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->adIface->set_color != NULL, JDK_FALSE);

	if(JCONF_IS_BNC_CAM(nChn)){
		int nItem = 0;
		//printf("Set Color [%d] (%f,%f,%f,%f)\n", nChn, nBrightness, nContrast, nSaturation, nHue);
		
		if(nHue >= 0 && nHue <= 1){
			nItem++;
			s_jdkCtx->hue[nChn] = nHue;
		}
		if(nBrightness >= 0 && nBrightness <= 1){
			nItem++;
			s_jdkCtx->brightness[nChn] = nBrightness;
		}
		if(nContrast >= 0 && nContrast <= 1){
			nItem++;
			s_jdkCtx->contrast[nChn] = nContrast;
		}
		if(nSaturation >= 0 && nSaturation <= 1){
			nItem++;
			s_jdkCtx->saturation[nChn] = nSaturation;
		}
		if(nItem > 0){
			s_jdkCtx->adIface->set_color((JDK_UINT32)nChn, nBrightness, nContrast, nSaturation, nHue, -1);
		}
	}
	return JDK_TRUE;
}


JDK_BOOL JDK_vdec_init(void)
{
	JDK_UINT32 n, dec_chn, vo_chn, voDev;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface->init(), JDK_FALSE);

	s_jdkCtx->vdecIface->init();

	// open vdec chn
	if (jdk_is_live_mode()) {
	} else {
		//for (n = 0; n < s_jdkCtx->nCurPlayCam; n++) {
		//	DECODE_chn_open(n, JDK_FALSE);
		//}
	}

	// bind vdec -> vo
	//if (jdk_is_live_mode()) {
	for (n = 0; n < JDK_ACTIVE_CAM(); n++) {
		for (voDev = 0; voDev < s_jdkCtx->nVoDev; voDev++) {
			s_jdkCtx->vdecIface->bind_vo(n, s_jdkCtx->nVoDevList[voDev], n);
		}
	}
	//}

	// show user frame
	if (jdk_is_live_mode()) {
		for (n = 0; n < JDK_ACTIVE_CAM(); n++) {
			for (voDev = 0; voDev < s_jdkCtx->nVoDev; voDev++) {
				JDK_UINT32 nUserPicIndex = JDK_IS_BNC_CAM(n) ? JDK_USR_PIC_LOSS : JDK_USR_PIC_IDLE;
				if (s_jdkCtx->voIface && s_jdkCtx->voIface->enable_user_pic)
					s_jdkCtx->voIface->enable_user_pic(s_jdkCtx->nVoDevList[voDev], n);
				if (s_jdkCtx->voIface && s_jdkCtx->voIface->show_user_pic)
					s_jdkCtx->voIface->show_user_pic(s_jdkCtx->nVoDevList[voDev], n, s_jdkCtx->hChnUserPic[nUserPicIndex]);
			}
		}
	}
}

JDK_BOOL JDK_vdec_deinit(void)
{
	JDK_UINT32 n, dec_chn, vo_chn, voDev;
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->vdecIface->deinit != NULL, JDK_FALSE);


	// open vdec chn
	if (jdk_is_live_mode()) {
	} else {
	}
	
	s_jdkCtx->vdecIface->deinit();

	return JDK_FALSE;
}

JDK_BOOL JDK_set_digital_zoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH)
{
	jdk_return_val_if_fail(s_jdkCtx != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface != NULL, JDK_FALSE);
	jdk_return_val_if_fail(s_jdkCtx->viIface->set_digital_zoom != NULL, JDK_FALSE);

	//if (JDK_vin_is_loss(nChn) == JDK_FALSE) {
		return s_jdkCtx->viIface->set_digital_zoom(nChn, nRatioX, nRatioY, nRatioW, nRatioH);
	//}
	//return JDK_FALSE;
}

void JDK_clear_digital_zoom(int nChn)
{
	jdk_return_if_fail(s_jdkCtx != NULL);
	jdk_return_if_fail(s_jdkCtx->viIface != NULL);
	jdk_return_if_fail(s_jdkCtx->viIface->clear_digital_zoom != NULL);
	
	s_jdkCtx->viIface->clear_digital_zoom(nChn);
}


void JDK_set_hook_to_query_lost(fJDK_QUERY_LOST_STATE_HOOK bncCamQueryLostState, fJDK_QUERY_LOST_STATE_HOOK netCamQueryLostState)
{
	jdk_return_if_fail(s_jdkCtx != NULL);

	if (bncCamQueryLostState)
		s_jdkCtx->bncCamQueryLostState = bncCamQueryLostState;
	if (netCamQueryLostState)
		s_jdkCtx->netCamQueryLostState = netCamQueryLostState;
}

void JDK_set_vloss_hook(fJDK_LOSS_HOOK vloss_hock)
{
	jdk_return_if_fail(s_jdkCtx != NULL);

	if (vloss_hock)
		s_jdkCtx->lossHook = vloss_hock;
}

void *JDK_vo_loss_loop(void *param)
{
	JDK_UINT32 n = 0, voId = 0;
	lpJDK_CTX thiz = (lpJDK_CTX)param;
	JDK_UINT64 curLossState = 0;
	JDK_UINT32 nVoDevSize = s_jdkCtx->nVoDev;
	
	if ((s_jdkCtx->nVoDev == 2) && (JDK_CVBS_MONITOR_MODE() == JDK_SD_MODE_DATA_BY_HD_WBC)) {
		nVoDevSize = 1;
	}
	
	jdk_return_if_fail(thiz != NULL);
	
	for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
		u64_set_bit(&thiz->lostState, n);
	}
	
	JDK_TRACE("JDK Loss Loop....");
	while (thiz->lossLoopTrigger) {
		JDK_UINT64 changed = 0;

		if (thiz->videoCheckFreeze == TRUE) {
			sleep_ms_c(200);
			continue;
		}

		// get state
		curLossState = 0;
		for ( n =0; (n < JDK_ACTIVE_CAM()) && ( n < 64 ); n++) {
			JDK_BOOL bloss = JDK_TRUE;
			if (JDK_IS_BNC_CAM(n)) {
				if (thiz->bncCamQueryLostState)
					bloss = thiz->bncCamQueryLostState(n);
			} else {
				if (thiz->netCamQueryLostState)
					bloss = thiz->netCamQueryLostState(n);
			}
			if (bloss == JDK_TRUE) {
				u64_set_bit(&curLossState, n);
			}
		}
		
		//JDK_TRACE("curLossState %llu (%llu)", curLossState, lastLossState);

		changed = u64_xor(curLossState, thiz->lostState);
		if (changed != 0)
		{
			JDK_TRACE("VLOSS %llu, curState:%llu", changed, curLossState);
			for (n = 0; (n < JDK_ACTIVE_CAM()) && ( n < 64 ) && (thiz->videoCheckFreeze == FALSE); n++) {
				int cbit = u64_get_bit(changed, n);
				int new_bit = u64_get_bit(curLossState, n);
				if (cbit != 0) 
				{
					if (new_bit != 0) {
						if (JDK_IS_BNC_CAM(n)) {
							// unbind vi->vo
							if (thiz->viIface && thiz->viIface->unbind_output) {
								thiz->viIface->unbind_output(n, n);
							}
							// bind vdec -> vo
							for (voId = 0; voId < nVoDevSize; voId++) {
								s_jdkCtx->vdecIface->bind_vo(n, s_jdkCtx->nVoDevList[voId], n);
							}
							s_jdkCtx->adIface->ad_fix_eq(n);
						}
						
						// set vo chn
						for (voId = 0; voId < nVoDevSize; voId++) 
						{
							if (thiz->voIface && thiz->voIface->enable_user_pic)
								thiz->voIface->enable_user_pic(thiz->nVoDevList[voId], n);
							if (thiz->voIface && thiz->voIface->show_user_pic) {
								JDK_UINT32 usrPicIndex = JDK_IS_BNC_CAM(n) ? JDK_USR_PIC_LOSS : JDK_USR_PIC_IDLE;
								thiz->voIface->show_user_pic(thiz->nVoDevList[voId], n, thiz->hChnUserPic[usrPicIndex]);
							}
						}
							
						// notify loss signal
						if (thiz->lossHook) thiz->lossHook(n);
					} else {
						if (JDK_IS_BNC_CAM(n)) {
							// unbind vdec->vo
							for (voId = 0; voId < nVoDevSize; voId++) {
								s_jdkCtx->vdecIface->unbind_vo(n, s_jdkCtx->nVoDevList[voId], n);
							}
							
							// bind vi->vo
							if (thiz->viIface && thiz->viIface->bind_output) {
								thiz->viIface->bind_output(n, n);
							}
							// set vo chn
							for (voId = 0; voId < nVoDevSize; voId++) {
								if (thiz->voIface && thiz->voIface->disable_user_pic)
									thiz->voIface->disable_user_pic(thiz->nVoDevList[voId], n);
							}
							s_jdkCtx->adIface->ad_fix_eq(n);
						}
					}
				}
			}
		}

		// backup current state
		thiz->lostState = curLossState;
		
		// vi format changed setup
		for (n = 0; (n < JDK_ACTIVE_CAM()) && ( n < 64 ); n++) {
			if (thiz->viInModeAuto && (thiz->viChnModeTemp[n] != thiz->viChnMode[n])) {
				JDK_UINT32 fTmp = thiz->viChnMode[n];
				if (JDK_vin_set_input_format(n, thiz->viChnModeTemp[n]) == JDK_TRUE) {
					if (thiz->viFormatChangedHook) thiz->viFormatChangedHook(n, fTmp, thiz->viChnModeTemp[n]);
				}
			}
		}
		
		//
		#if 0
		for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
			JDK_TRACE("curLossState%llu nbit(%d, %d)", curLossState, n, u64_get_bit(curLossState, n));
			HDDVR_InputFormatSetting(n, HDVIDEO_SD960H25FPS);
			HDDVR_VideoLossFix(n, u64_get_bit(curLossState, n));
			HDDVR_VideoStVideoFix(n);
		}
		#endif
		
		// sleep
		sleep_ms_c(300);
	}
	
	JDK_TRACE("JDK_vo_loss_loop exit!!!!!!!!!!!!!!!!!!");
}


void JDK_vo_loss_daemon(void)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	JDK_INT32 n;
	
	jdk_return_if_fail(thiz != NULL);
	jdk_return_if_fail(thiz->lossDaemonPid == 0);
	
	thiz->lossLoopTrigger = JDK_TRUE;
	for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
		u64_set_bit(&thiz->lostState, n);
	}
	initThread_c(&thiz->lossDaemonPid, JDK_vo_loss_loop, thiz);
}

void JDK_exit_vo_loss_daemon(void)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	JDK_INT32 n;
	
	if (thiz) {
		thiz->lossLoopTrigger = JDK_FALSE;
		if (thiz->lossDaemonPid != 0) {
			joinThread_c(thiz->lossDaemonPid);
			thiz->lossDaemonPid = 0;
			//for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
			//	u64_set_bit(&thiz->lostState, n);
			//}
		}
	}
}

void *jdk_vi_format_check_proc(void *param)
{
	JDK_UINT32 n;
	lpJDK_CTX thiz = (lpJDK_CTX)param;
	JDK_BOOL isLoss;
	JDK_UINT32 mux_mode = JDK_get_mux_mode();
	VI_SCAN_MODE_E getScanMode, setScanMode;
	
	JDK_TRACE("jdk_vi_format_check_proc....");
	while (thiz->viCheckLoopTrigger) 
	{
		if (thiz->viInModeAuto == TRUE) 
		{
			//JDK_TRACE("jdk vi mode checking....");
			for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ) && (thiz->viCheckLoopTrigger) && (thiz->viInModeAuto == TRUE); n++) {
				JDK_INT32 format = JDK_INPUT_FORMAT_UNKNOWN;
				//if ((n%2 == 0) && (JDK_CHN_IN_MODE(n) == JDK_INPUT_FORMAT_AUTO)) {
					//VideoModeSwitchAutoCheck(void)
				//}
				if(mux_mode == JDK_VI_MUX_X1){
					format = thiz->adIface->probe_vi_format(n);
					if ((format != JDK_CHN_IN_MODE(n)) && (format != JDK_INPUT_FORMAT_UNKNOWN)) {
						JDK_TRACE("%u ]] chn mode changed %s->>%s", n, g_JdkChnInputModeString[JDK_CHN_IN_MODE(n)], g_JdkChnInputModeString[format]);
						// mode changed
						thiz->viChnModeTemp[n] = format;
					}
				}else if ((n % 2) == 0) {
				#if defined VI_DEV_2MUX_CHNL
						format = thiz->adIface->probe_vi_format(n);
						if ((format != JDK_CHN_IN_MODE(n)) && (format != JDK_INPUT_FORMAT_UNKNOWN)) {
							JDK_TRACE("%u ]] chn mode changed %s->>%s", n, g_JdkChnInputModeString[JDK_CHN_IN_MODE(n)], g_JdkChnInputModeString[format]);
							// mode changed
							thiz->viChnModeTemp[n] = format;
						}
				#else
					isLoss = jdk_bnc_vin_is_loss(n + 1); 
					if (isLoss == TRUE) {
						format = thiz->adIface->probe_vi_format(n);
						if ((format != JDK_CHN_IN_MODE(n)) && (format != JDK_INPUT_FORMAT_UNKNOWN)) {
							JDK_TRACE("%u ]] chn mode changed %s->>%s", n, g_JdkChnInputModeString[JDK_CHN_IN_MODE(n)], g_JdkChnInputModeString[format]);
							// mode changed
							thiz->viChnModeTemp[n] = format;
							thiz->viChnModeTemp[n + 1] = format;
						}
					}
				#endif
				}else {
					if(JDK_VSUPPORT_HD1080P == JDK_vin_get_max_support()){
					#if defined VI_DEV_2MUX_CHNL
							format = thiz->adIface->probe_vi_format(n);
							if ((format != JDK_CHN_IN_MODE(n)) && (format != JDK_INPUT_FORMAT_UNKNOWN)) {
								JDK_TRACE("%u ]] chn mode changed %d->%d", n, JDK_CHN_IN_MODE(n), format);
								// mode changed
								thiz->viChnModeTemp[n] = format;
							}
					#else
						isLoss = jdk_bnc_vin_is_loss(n - 1);
						if (isLoss == TRUE) {
							format = thiz->adIface->probe_vi_format(n);
							if ((format != JDK_CHN_IN_MODE(n)) && (format != JDK_INPUT_FORMAT_UNKNOWN)) {
								JDK_TRACE("%u ]] chn mode changed %d->%d", n, JDK_CHN_IN_MODE(n), format);
								// mode changed
								thiz->viChnModeTemp[n] = format;
								thiz->viChnModeTemp[n - 1] = format;
							}
						}
					#endif
					}
				}
				//if (isLoss == FALSE) {
				//	sleep_ms_c(100);
				//}
			}
		}

		sleep_ms_c(500);
	}

	return NULL;
}


void JDK_set_auto_vi_format_check(JDK_BOOL bAuto)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	int n;
	
	jdk_return_if_fail(thiz != NULL);
	if (bAuto == TRUE) {
		//JDK_exit_vo_loss_daemon();
		JDK_exit_vi_format_check_daemon();
		//for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
			//JDK_vin_close(n);
			//u64_set_bit(&thiz->lostState, n);

			//thiz->viChnModeTemp[n] = JDK_INPUT_MODE_NULL;
			//thiz->viChnMode[n] = JDK_INPUT_MODE_NULL;
		//}
		//JDK_vo_loss_daemon();
		JDK_vi_format_check_daemon();
	} else {	
		//JDK_exit_vo_loss_daemon();
		JDK_exit_vi_format_check_daemon();

		//for ( n =0; (n < JDK_BNC_CAM()) && ( n < 64 ); n++) {
		//	u64_set_bit(&thiz->lostState, n);
		//}
		//JDK_vo_loss_daemon();
	}
	thiz->viInModeAuto = bAuto;
}

void JDK_set_vi_format_changed_hook(fJDK_VI_FORMAT_CHANGED_HOOK hook)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	
	jdk_return_if_fail(thiz != NULL);
	thiz->viFormatChangedHook = hook;
}

void JDK_vi_format_check_daemon(void)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	
	jdk_return_if_fail(thiz != NULL);
	jdk_return_if_fail(thiz->viCheckDaemonPid == 0);

	if (thiz->viCheckDaemonPid == 0) {
		thiz->viCheckLoopTrigger = JDK_TRUE;
		initThread_c(&thiz->viCheckDaemonPid, jdk_vi_format_check_proc, thiz);
	}
}

void JDK_exit_vi_format_check_daemon(void)
{
	lpJDK_CTX thiz = (lpJDK_CTX)s_jdkCtx;
	
	if (thiz) {
		thiz->viCheckLoopTrigger = JDK_FALSE;
		if (thiz->viCheckDaemonPid != 0) {
			joinThread_c(thiz->viCheckDaemonPid);
			thiz->viCheckDaemonPid = 0;
		}
	}
}

