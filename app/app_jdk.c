#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"
#include "app_jdk.h"
#include "jcm.h"
#include "env_common.h"
#include "environment.h"

#include "sysconf.h"
#include "vo_grid.h"
#include "app_debug.h"
#include "jdk_common.h"
#include "sdk/sdk_api.h"
#include "bsp/decoder_drv.h"

extern Sysenv* g_pstSysEnv;

extern void AVENC_chn_open(int chn);
extern void AVENC_chn_close(int chn);


JDK_INT32 app_jdk_pixel2_vi_resolution(JDK_UINT32 pixels, JDK_BOOL isNet)
{
	JDK_INT32 ret = 0;
	if (isNet) {
		switch (pixels) {
			case 1920*1080:
				ret = JDK_INPUT_NET_1080P;
				break;
			case 1280 *960:
				ret = JDK_INPUT_NET_960P;
				break;
			case 1280 * 720:
			default:
				ret = JDK_INPUT_NET_720P;
				break;
		}
	}else {
		switch (pixels) {
			case 1920*1080:
				ret = JDK_INPUT_BNC_1080P;
				break;
			case 960 * 1080:
				ret = JDK_INPUT_BNC_HFHD;
				break;
			case 1280 * 720:
				ret = JDK_INPUT_BNC_720P;
				break;
			case 960 * 576:
				ret = JDK_INPUT_BNC_960H;
				break;
			case 720 * 576:
				ret = JDK_INPUT_BNC_720H;
				break;
			case 704 * 576:
			default:
				ret = JDK_INPUT_BNC_D1;
				break;
		}
	}
	return ret;
}

static JDK_INT32 Be_MainStream_Save[MAX_CAM_CH]= {-1};

JDK_INT32 APP_JDK_clear_stream_index_flag()
{
	memset(Be_MainStream_Save,-1,sizeof(Be_MainStream_Save));
	return 0;
}

static void vo_chn_switch_hook(VO_DIVISION_TYPE divFrom, JDK_UINT32 nPageFrom, VO_DIVISION_TYPE divTo, JDK_UINT32 nPageTo)
{
	int i;
	int isBigFrom = 0, isBigTo = 0;
	int isBigChanged = 0;
	int nFromChnBase = 0, nToChnBase = 0;
	int nFromChnEnd =0, nToChnEnd = 0;
	JDK_INT32 Be_MainStream[MAX_CAM_CH];

	memset(Be_MainStream,0,sizeof(Be_MainStream));

	if(Be_MainStream_Save[0] >= 0 ){
		if (divFrom == divTo && nPageFrom == nPageTo)
			return;
	}
	
	if (jdk_is_live_mode() == JDK_FALSE) {
		return;
	}
	
	isBigFrom = VO_GRID_HAS_BIG_GRID(divFrom);
	isBigTo = VO_GRID_HAS_BIG_GRID(divTo);
	isBigChanged = isBigFrom ^ isBigTo;
	
	nFromChnBase = VO_GRID_BASE_GRID_OF_PAGE(divFrom, nPageFrom);
	nFromChnEnd = VO_GRID_END_GRID_OF_PAGE(divFrom, nPageFrom);
	nToChnBase = VO_GRID_BASE_GRID_OF_PAGE(divTo, nPageTo);
	nToChnEnd = VO_GRID_END_GRID_OF_PAGE(divTo, nPageTo);

	int dec_live_chn = 0;
	if(nToChnEnd < JCONF_NET_CAM_START()){
		dec_live_chn = 0;
	}else if(nToChnBase < JCONF_NET_CAM_START()){
		dec_live_chn = nToChnEnd - JCONF_NET_CAM_START();
	}else{
		dec_live_chn = nToChnEnd - nToChnBase;
	}
	
	int live_max_mainstr_chn = 0;

#ifdef _HVR
	if(isBigTo && divTo > VO_DIVISION_X6){
		live_max_mainstr_chn = 1;
	}else if(divTo <= VO_DIVISION_X9){
		live_max_mainstr_chn = JA_SYS_CONFIG_getMaxMainStream(dec_live_chn,dec_live_chn);
	}
#endif

	for(i = 0; i<MAX_CAM_CH; i++)
	{
		if(i>= nToChnBase && i<nToChnEnd && live_max_mainstr_chn>0){
			Be_MainStream[i]=1;			//main stream
			live_max_mainstr_chn--;
		}else{
			Be_MainStream[i]=0;			//sub stream
		}
	}

	if(Be_MainStream_Save[0]<0){	//first init
		for(i=0;i<MAX_CAM_CH;i++)
		{
		}
	}else{
		for(i=0;i<MAX_CAM_CH;i++)
		{
		}
	}
	memcpy(Be_MainStream_Save,Be_MainStream,sizeof(Be_MainStream_Save));

	//audio switch
	int nBaseChn =  VO_GRID_BASE_GRID_OF_PAGE(divTo, nPageTo);
	if(JCONF_IS_NET_CAM(nBaseChn) || !JDK_VL_IS_LIVE_MODE()){
	}else{
		AUDIO_PlaybackEn(FALSE);
		AUDIO_LiveEn(TRUE);
		AUDIO_LiveSet(nBaseChn);
	}
}

#define MAX_FRAME_SIZE 10240
static unsigned char s_buf[MAX_FRAME_SIZE];
static int s_buf_size = 0;

JDK_INT32 JDK_load_user_picture(char* filename)
{
	FILE *fp = NULL;
	fp = fopen(filename,"r");

	if(!fp){
		JDK_TRACE("open file faild");
		return -1;
	}

	memset(s_buf,0,sizeof(s_buf));
	s_buf_size = fread(s_buf,1,sizeof(s_buf),fp);
	
	fclose(fp);
	fp = NULL;
	
	return s_buf_size;
}

JDK_BOOL JDK_vo_loss_fill_buffer(JDK_UINT32 chn,JDK_UINT32 nStream, JDK_BOOL isIDR)
{
	return JDK_TRUE;

	if(JDK_IS_BNC_CAM(chn)){
		return JDK_TRUE;
	}

	if(s_buf_size <= 0){
		if(JDK_load_user_picture(JDK_DEF_LOSS_NET_H264)<=0){
			return JDK_FALSE;
		}
	}

	char meida_name[64];
	int key_frame_flag = 0;
	SDK_ENC_BUF_ATTR_t attr;
	memset(&attr, 0, sizeof(attr));
	
	sprintf(meida_name,"ch%d_%d.264", chn, nStream);
	int media_buf_id = MEDIABUF_lookup_byname(meida_name);

	attr.magic = SDK_ENC_BUF_DATA_MAGIC;
	attr.type = SDK_ENC_BUF_DATA_H264;
	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	guint64 aPtsUs = (guint64)tv.tv_sec * 1000000ULL + (guint64)tv.tv_usec;
	attr.timestamp_us = aPtsUs;
	attr.time_us = (unsigned long long)((aPtsUs/1000000ULL) << 32) + (unsigned long long)(aPtsUs%1000000ULL);

	if (nStream == 0) {
		attr.h264.keyframe = isIDR;
		attr.h264.ref_counter = 0;
		attr.h264.fps = 25;
		attr.h264.width = 704;
		attr.h264.height = 480;
		key_frame_flag = isIDR;
	} else {
		attr.h264.keyframe = isIDR;
		attr.h264.ref_counter = 0;
		attr.h264.fps = 25;
		attr.h264.width = 0;
		attr.h264.height = 0;
		key_frame_flag = isIDR;
	}
	
	attr.data_sz = s_buf_size;
	MEDIABUF_in_request(media_buf_id, s_buf_size + sizeof(SDK_ENC_BUF_ATTR_t), key_frame_flag);
	MEDIABUF_in_append(media_buf_id, &attr, sizeof(SDK_ENC_BUF_ATTR_t));
	MEDIABUF_in_append(media_buf_id, s_buf, s_buf_size);
	MEDIABUF_in_commit(media_buf_id);

	return JDK_TRUE;
}

JDK_BOOL JDK_vo_loss_multi_fill_buffer(JDK_UINT32 chn,JDK_UINT32 nStream, JDK_BOOL isIDR,JDK_UINT32 nMultiTimes)
{
	int i;
	for(i = 0;i < nMultiTimes; i++){
		JDK_vo_loss_fill_buffer(chn,nStream,isIDR);
	}
	return JDK_TRUE;
}

JDK_BOOL JDK_vo_fill_buffer_picture(JDK_UINT32 chn)
{
	int i,n;
	int stream_cnt = 2;
	char meida_name[64];
	int media_buf_id;
	const n_max_loop_times = 20;

	for(i=0;i<stream_cnt;i++){
		int loop_time = 0;
		sprintf(meida_name,"ch%d_%d.264", chn, i);
		media_buf_id = MEDIABUF_lookup_byname(meida_name);
//		while(loop_time < n_max_loop_times){
//			++loop_time;
//			usleep(100000);
//		}
		MEDIABUF_empty(media_buf_id);			
		JDK_vo_loss_multi_fill_buffer(chn,i, TRUE,1);
		JDK_vo_loss_multi_fill_buffer(chn,i, FALSE,3);
	}

	return  JDK_TRUE;
}

static void vo_chn_swap_hook(JDK_UINT32 fromVoChn, JDK_BOOL fromNeedSwitch, JDK_UINT32 toVoChn, JDK_BOOL toNeedSwitch)
{
	if(Be_MainStream_Save[0]<0 || (Be_MainStream_Save[fromVoChn] == Be_MainStream_Save[toVoChn])){
		return ;
	}

	if (fromNeedSwitch) {
		if(JCONF_IS_NET_CAM(fromVoChn)){
		}
		Be_MainStream_Save[fromVoChn] = !Be_MainStream_Save[fromVoChn];
	}
	if (toNeedSwitch) {
		if(JCONF_IS_NET_CAM(toVoChn)){
		}
		Be_MainStream_Save[toVoChn] = !Be_MainStream_Save[toVoChn];
	}
}

static void vi_format_changed_hook(JDK_UINT32 viChn, JDK_UINT32 fromFormat, JDK_UINT32 toFormat)
{
	ENC_FMT enc_fmt = g_pstSysEnv->GetEncFmt(g_pstSysEnv, viChn);

	APP_TRACE("vi(%u) ==== changed ==== %u->%u", viChn, fromFormat, toFormat);
#if 0
	if ((fromFormat == JDK_INPUT_BNC_720P || fromFormat == JDK_INPUT_BNC_HFHD)
		&& (toFormat == JDK_INPUT_BNC_720P || toFormat == JDK_INPUT_BNC_HFHD)
		&& (enc_fmt == ENC_FMT_HD720P || enc_fmt == ENC_FMT_HD1080P_HALF)) {
		AVENC_chn_close(viChn);
		AVENC_chn_open(viChn);
	} else if (toFormat > fromFormat) {
		AVENC_chn_close(viChn);
		AVENC_chn_open(viChn);
	} else if (toFormat < fromFormat) {
		AVENC_chn_close(viChn);
		AVENC_chn_open(viChn);
	}
#else
	if (toFormat != fromFormat) {
		AVENC_chn_close(viChn);
		AVENC_chn_open(viChn);
	}
#endif
}


static int app_jdk_vin_chn_init(lpJDK_ARG s_jdkCtx)
{
	int i;
	int bAuto = g_pstSysEnv->GetChannelModeAuto(g_pstSysEnv);
	for ( i = 0; i < JCONF_BNC_CAM(); i++) {
		stJDK_VIN_ARG arg;
		int inmode = bAuto ? JDK_INPUT_FORMAT_AUTO : JDK_INPUT_BNC_720P;

		if (inmode == JDK_INPUT_FORMAT_AUTO) {
			;
		} else {
			int channel_mode = g_pstSysEnv->GetChannelModes(g_pstSysEnv, i);
			switch(channel_mode){
				case VIDEO_MODE_960H:
					inmode = JDK_INPUT_BNC_960H;
					break;
				case VIDEO_MODE_720P:
					inmode = JDK_INPUT_BNC_720P;
					break;
				case VIDEO_MODE_HALF1080P:
					inmode = JDK_INPUT_BNC_HFHD;
					break;
				case VIDEO_MODE_1080P:
					inmode = JDK_INPUT_BNC_1080P;
					break;
				default:
					inmode = JDK_INPUT_FORMAT_AUTO;
					break;
			}	
		}
		
		memset(&arg, 0, sizeof(arg));
		
		JDK_inmode_get_wh(s_jdkCtx->maxBncCamImgResol, &arg.rect.width, &arg.rect.height);
		arg.rect.x = 0;
		arg.rect.y = 0;
		arg.inMode = inmode;
		arg.cutMode = JDK_VI_CUT_MODE_NONE;
		arg.muxMode = JDK_get_mux_mode();
		arg.fps = 25;
		arg.flip = JDK_FALSE;
		arg.mirror = JDK_FALSE;
		// open vi 
		if (JDK_vin_open(i, &arg) == JDK_FALSE)
			return -1;

	}
	for ( ; i < JCONF_NET_CAM_END(); i++) {
		stJDK_VIN_ARG arg;
		memset(&arg, 0, sizeof(arg));
		
		JDK_inmode_get_wh(s_jdkCtx->maxNetCamImgResol, &arg.rect.width, &arg.rect.height);
		arg.rect.x = 0;
		arg.rect.y = 0;
		arg.inMode = s_jdkCtx->maxNetCamImgResol;
		arg.fps = 30;
		arg.flip = JDK_FALSE;
		arg.mirror = JDK_FALSE;
		arg.cutMode = 0;
		arg.muxMode = 0;

		// open net vi
		if (JDK_vin_open(i, &arg) == JDK_FALSE)
			return -1;
	}
	return 0;
}

int APP_JDK_init(void)
{		
	JDK_UINT32 n;
	stJDK_ARG m_jdkArg;
	VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);
	int bAuto = g_pstSysEnv->GetChannelModeAuto(g_pstSysEnv);

	int nCvbsX, nCvbsY, nCvbsW, nCvbsH;

	nCvbsX = g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_LEFT);
	nCvbsY = g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_TOP);
	nCvbsW = g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_RIGHT);
	nCvbsH = g_pstSysEnv->GetTvSize(g_pstSysEnv, TV_SIZE_BOTTOM);

	m_jdkArg.brand = JDK_SOC_BRAND_HISI;
#if defined(SDK_PLATFORM_HI3531)
	m_jdkArg.soc = JDK_HI_SOC_3531;
#elif defined(SDK_PLATFORM_HI3521) 
#if defined(_HI3520D_EXT)
	m_jdkArg.soc = JDK_HI_SOC_3520D;
#elif defined(_HI3515A_EXT)
m_jdkArg.soc = JDK_HI_SOC_3515A;
#else
m_jdkArg.soc = JDK_HI_SOC_3521;
#endif
#endif
	m_jdkArg.ad = JDK_AD_CHIP_0;
	m_jdkArg.standard = g_pstSysEnv->GetStandard(g_pstSysEnv);
	m_jdkArg.nMaxBncChn = MAX_CAM_CH;
	m_jdkArg.nNetChn = JCONF_NET_CAM();
	m_jdkArg.nBncChn = JCONF_BNC_CAM();
	m_jdkArg.nStream = JCONF_STREAM_NUM();
	m_jdkArg.nAudio = 1;
	m_jdkArg.nAlarm = MAX_ALARM_CH;
	m_jdkArg.nSensor = MAX_SENSOR_CH;
	m_jdkArg.nDdr = 2;
	m_jdkArg.ddrSize = 0;
	
	JDK_CHN_INPUT_MODE maxBNCResol = app_jdk_pixel2_vi_resolution(MAX_BNC_1ST_RESOL, FALSE);
	if(JDK_VSUPPORT_HD1080P == JDK_vin_get_max_support()){
		m_jdkArg.maxBncCamImgResol = maxBNCResol;
	}else{
		m_jdkArg.maxBncCamImgResol = maxBNCResol > JDK_INPUT_BNC_720P ? JDK_INPUT_BNC_720P : maxBNCResol;
	}

	m_jdkArg.maxNetCamImgResol = app_jdk_pixel2_vi_resolution(MAX_NET_1ST_RESOL, TRUE);
	m_jdkArg.maxImgResol = JDK_RESOL_720P; //FIXME: JDK_RESOL_1280x1080
	m_jdkArg.nPlaybackCam = MAX_PLAYBACK_CH;
	m_jdkArg.maxPlaybackResol = JDK_RESOL_1080P;
	JDK_new(&m_jdkArg);

	JDK_sys_init();
	JDK_vin_init(bAuto);
	app_jdk_vin_chn_init(&m_jdkArg);

	//CVBS
	if(g_pstSysEnv->IsCVBSEnable(g_pstSysEnv)){
		JDK_monitor_init(JDK_MONITOR_VGA | JDK_MONITOR_CVBS, enRes, JDK_RESOL_720P, nCvbsX, nCvbsY, nCvbsW, nCvbsH, JDK_DEF_SD_MONITOR_MODE);
	}else{
		JDK_monitor_init(JDK_MONITOR_VGA, enRes, JDK_RESOL_720P, nCvbsX, nCvbsY, nCvbsW, nCvbsH, JDK_DEF_SD_MONITOR_MODE);
	}
	
	JDK_set_vo_switch_hook(vo_chn_switch_hook);
	JDK_set_vo_swap_hook(vo_chn_swap_hook);
	
	JDK_vdec_init();
	JDK_set_vloss_hook(JDK_vo_fill_buffer_picture);
	JDK_set_vi_format_changed_hook(vi_format_changed_hook);
	JDK_vo_loss_daemon();
	JDK_vi_format_check_daemon();
	JDK_set_vo_division(VO_DIVISION_DEF, 0);
	// set color
	for (n = JCONF_BNC_CAM_START(); n < JCONF_BNC_CAM_END(); n++) {
		JDK_INT32 nHue = g_pstSysEnv->GetHue(g_pstSysEnv, n);
		JDK_INT32 nBrightness = g_pstSysEnv->GetLuminance(g_pstSysEnv, n);
		JDK_INT32 nContrast = g_pstSysEnv->GetContrast(g_pstSysEnv, n);
		JDK_INT32 nSaturation = g_pstSysEnv->GetSaturation(g_pstSysEnv, n);

		JDK_ad_set_color(n, (float)nBrightness/VIDEO_MAX_COLOR_GAIN, (float)nContrast/VIDEO_MAX_COLOR_GAIN,
			(float)nSaturation/VIDEO_MAX_COLOR_GAIN, (float)nHue/VIDEO_MAX_COLOR_GAIN);
	}
	JDK_set_tv_adjust(nCvbsX, nCvbsY, nCvbsW, nCvbsH, TRUE);
		
	return 0;
}

void APP_JDK_deinit(void)
{
}

