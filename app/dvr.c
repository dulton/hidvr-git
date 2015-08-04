

#ifndef	__DVR_C__
#define	__DVR_C__

#include <unistd.h>
#include <signal.h>

#include "conf.h"
#include "std.h"
#include "stime.h"
#include "environment.h"
#include "avenc.h"
#include "video.h"
#include "timer.h"
#include "system.h"
#include "gpio.h"
#include "osd.h"
#include "dvr.h"
#include "syslog.h"
#include "timetick.h"
#include "audio.h"
#include "env_action.h"
#include "gpio_manage.h"
#include "user.h"
#include "bsp.h"

#include "sdk/sdk_api.h"
#include "media_buf.h"
#include "spook.h"
#include "avenc_map.h"
#include "envfix.h"

#include "app_debug.h"
#include "overlay.h"
#include "app_overlay.h"
#include "sysconf.h"
#include "app_jdk.h"
#include "vo_grid.h"

#include "ticker.h"

#if  defined(_EXT_HDDVR)
#include "hddvr_code.h"
#endif

#include "app_jdk.h"
#define DEBUG_DVR
#ifdef DEBUG_DVR
#define DVR_TRACE(fmt...)	\
	do{printf("\033[0;37mDVR->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define DVR_TRACE(fmt...)
#endif

Sysenv* g_pstSysEnv = NULL;

static void mediabuf_hock_user_changed(const char *name, uint32_t user_number, uint32_t user_max)
{
	int vin_ch = -1, stream_ch = -1;
	int const mediabuf_id = MEDIABUF_lookup_byname(name);

	if (sdk_enc) {
		sdk_enc->lookup_stream_byname(name, &vin_ch, &stream_ch);
		
		if(vin_ch >= 0 && stream_ch >= 0){
			if(0 == user_number){
				APP_TRACE("Media Buf(%s) users gone!", name);
				sdk_enc->stop_h264_stream(vin_ch, stream_ch);
			}else if(user_number > 0){
				APP_TRACE("Media Buf(%s) users attached(%d/%d)", name, user_number, user_max);
				sdk_enc->start_h264_stream(vin_ch, stream_ch);
			}
		}
	}
}

static void mediabuf_hock_on_pool_exhausted(const char *name)
{
	int const mediabuf_id = MEDIABUF_lookup_byname(name);
	if(mediabuf_id >= 0){
		APP_TRACE("Media Pool(%s) emptying!", name);
		MEDIABUF_empty(mediabuf_id);
	}
}

# define PLATFORM_REF_MODE_DEFAULT (SDK_ENC_H264_REF_MODE_4X)

#include "jcm.h"
#include "env_common.h"
void APP_GLIB_init(void)
{
	g_type_init();

	slog_set_level(2);
	
	if (g_thread_supported ()) {
		g_log("JCM", G_LOG_LEVEL_MESSAGE, "g_thread_supported!");
	} else {
		g_log("JCM", G_LOG_LEVEL_MESSAGE, "g_thread_supported not yet!!!");
		g_thread_init (NULL);
	}
	setenv("G_SLICE", "always-malloc", TRUE);
	setenv("G_DEBUG", "gc-friendly", TRUE);
}

static HMUTEX s_encMutex = NULL;

void AVENC_chn_open(int chn)
{
	int i = chn,ii = 0, n=0;
	char title[32] = {""};

	if (JCONF_BNC_CAM() <= 0) return;
	if (JCONF_IS_BNC_CAM(chn) == FALSE) return;
	if (sdk_enc == NULL) return;

	g_pstSysEnv->GetTitle(g_pstSysEnv, i, title);

	mutex_lock(s_encMutex);
	
	for(ii = 0; ii < 2; ++ii){
		int buf_id = 0;
		char stream_name[32];

		sprintf(stream_name, "ch%d_%d.264", i, ii);
		buf_id = MEDIABUF_lookup_byname(stream_name);

		if(buf_id >= 0){
			const unsigned int height_map_pal[] = {
				[ENC_FMT_QCIF] = 144,
				[ENC_FMT_CIF] = 288,
				[ENC_FMT_HD1] = 576,
				[ENC_FMT_D1] = 576,
				[ENC_FMT_WCIF] = 288,
				[ENC_FMT_960H] = 576,
				[ENC_FMT_HD720P] = 720,
				[ENC_FMT_HD1080P_HALF] = 1080,
				[ENC_FMT_HD1080P] = 1080,
			};
			const unsigned int height_map_ntsc[] = {
				[ENC_FMT_QCIF] = 120,
				[ENC_FMT_CIF] = 240,
				[ENC_FMT_HD1] = 480,
				[ENC_FMT_D1] = 480,
				[ENC_FMT_WCIF] = 240,
				[ENC_FMT_960H] = 480,
				[ENC_FMT_HD720P] = 720,
				[ENC_FMT_HD1080P_HALF] = 1080,
				[ENC_FMT_HD1080P] = 1080,
			};
			
			const unsigned int width_map[] = {
				[ENC_FMT_QCIF] = 176,
				[ENC_FMT_CIF] = 352,
				[ENC_FMT_HD1] = 704,
				[ENC_FMT_D1] = 704,
				[ENC_FMT_WCIF] = 480,
				[ENC_FMT_960H] = 960,
				[ENC_FMT_HD720P] = 1280,
				[ENC_FMT_HD1080P_HALF] = 960,
				[ENC_FMT_HD1080P] = 1920,
			};
			const unsigned int* height_map = g_pstSysEnv->GetStandard(g_pstSysEnv) == VOUT_PAL ? height_map_pal : height_map_ntsc;
			const SDK_ENC_H264_MODE_t enc_mode_map[] = {
				[ENC_BR_VARIABLE] = SDK_ENC_H264_MODE_VBR,
				[ENC_BR_CONSTANT] = SDK_ENC_H264_MODE_CBR,
				[ENC_BR_AVERAGE] = SDK_ENC_H264_MODE_ABR,
			};

			ENC_FMT fmt = (0 == ii)
						? g_pstSysEnv->GetEncFmt(g_pstSysEnv, i)
						: g_pstSysEnv->GetEncSubFmt(g_pstSysEnv, i, 0);
			if(0 == ii && fmt == ENC_FMT_QCIF)
			{
				fmt = ENC_FMT_CIF;
			}
			int const vin_fps = g_pstSysEnv->GetStandard(g_pstSysEnv) == VOUT_PAL ? 25 : 30;
			int fps = (0 == ii) ? FRAMERATE_MAP[g_pstSysEnv->GetEncFramerate(g_pstSysEnv, i)] : FRAMERATE_MAP[g_pstSysEnv->GetEncSubFramerate(g_pstSysEnv, i, 0)];
			unsigned int width = width_map[fmt];
			unsigned int height = height_map[fmt];
			int bps = (0 == ii)
							? BITRATE_MAP[g_pstSysEnv->GetBitrateValue(g_pstSysEnv, i)]
							: BITRATE_MAP[g_pstSysEnv->GetSubBitrateValue(g_pstSysEnv, i, 0)];

			//printf("venc ii:%d fmt:%d size:%dx%d fps:%d(%d)\n", ii, fmt, width, height, fps, vin_fps);
			
#if   defined(_HI3520D_RAM_LITE)
			if((ii == 0) && (bps > BITRATE_MAP[ENC_BR_1MPS])) {
				bps = BITRATE_MAP[ENC_BR_1MPS];
			}
			if((ii != 0) && (bps > BITRATE_MAP[ENC_BR_256KPS])) {
				bps = BITRATE_MAP[ENC_BR_256KPS];
			}
#endif
			if(((0 == ii) && (g_pstSysEnv->GetEncFramerate(g_pstSysEnv, i) >= FR_FULLFPS))
				|| (g_pstSysEnv->GetEncSubFramerate(g_pstSysEnv, i, 0) >= FR_FULLFPS)) {
				fps = vin_fps;
			}
			
			SDK_ENC_H264_MODE_t mode = (0 == ii) ? enc_mode_map[g_pstSysEnv->GetBitrateMode(g_pstSysEnv, i)] : enc_mode_map[g_pstSysEnv->GetSubBitrateMode(g_pstSysEnv, i, 0)];
			SDK_ENC_H264_STREAM_ATTR_t video_stream_attr;

			stJDK_SIZE viSize = {0, 0};
			if (JDK_vin_get_size(i, &viSize) == TRUE) {
				if (width * height > viSize.width * viSize.height) {
					APP_TRACE("WARN: venc size(%dx%d) is bigger than vin(%ux%u)", width, height, viSize.width, viSize.height);
					width = viSize.width;
					height = viSize.height;
				} else {
					if ((width > viSize.width) || (height > viSize.height)) {
						APP_TRACE("WARN: --venc size(%dx%d) is bigger than vin(%ux%u)", width, height, viSize.width, viSize.height);
						width = viSize.width;
						height = viSize.height;
					}
				}
			}
			
			if (width == 0 || height == 0) {
				goto __exit_clean;
			}

			// get auto-fix fmt
			for (n =0 ; n < ENC_FMT_CNT; n++) {
				if (height_map[n] * width_map[n] == width *height) {
					break;
				}
				if (height_map[n] * width_map[n] > width *height) {
					if (n > 0) n--;
					break;
				}
			}
			fmt = n;

			// auto -fix fps
			int MaxFpsLite = JA_SYS_CONFIG_getEncMaxFps(fmt, ii);
			if (MaxFpsLite == 0) MaxFpsLite = 5;
			if(fps > MaxFpsLite) {
				fps = MaxFpsLite;
			}
			
			APP_TRACE("venc %s(%dx%d) %dfps %dbps fmt:%d  open", stream_name, width, height, fps, bps, fmt);
			memset(&video_stream_attr, 0, sizeof(video_stream_attr));


			video_stream_attr.vin_fps = vin_fps;
			video_stream_attr.width = width;
			video_stream_attr.height = height;
			video_stream_attr.fps = fps;
			video_stream_attr.gop = 2 * fps;
			video_stream_attr.profile = SDK_ENC_H264_PROFILE_DEFAULT;
			video_stream_attr.mode = mode;
			video_stream_attr.ref_mode = (0 == ii) ? PLATFORM_REF_MODE_DEFAULT : SDK_ENC_H264_REF_MODE_1X;
			video_stream_attr.bps = bps;
			video_stream_attr.quality = 0;
			video_stream_attr.frame_limit = 0;
			video_stream_attr.buf_id = buf_id;
			video_stream_attr.start = (0 == i) ? true : false;

			SDK_ENC_G711A_STREAM_ATTR_t audio_stream_attr;
			memset(&audio_stream_attr, 0, sizeof(audio_stream_attr));
			audio_stream_attr.sample_rate = SDK_ENC_AUDIO_SAMPLE_RATE_8KBPS;
			audio_stream_attr.bit_width = SDK_ENC_AUDIO_BITWIDTH_16BITS;
			audio_stream_attr.packet_size = 320;
			audio_stream_attr.start = true;

			// create stream
			ENC_MODE enc_mode = g_pstSysEnv->GetEncMode(g_pstSysEnv, i);
			sdk_enc->create_h264_stream(stream_name, i, ii, &video_stream_attr);
			if(ii == 0 && enc_mode == MODE_AUDIO_VIDEO){
				sdk_enc->create_g711a_stream(i, i, &audio_stream_attr);
			}

			// overlay
			if(width > 352){
				APP_OVERLAY_create_clock(i, ii, 10, 4, OVERLAY_FONT_SIZE_20);
				APP_OVERLAY_create_title(i, ii, 10, 28, OVERLAY_FONT_SIZE_20);
			}else{
				APP_OVERLAY_create_clock(i, ii, 10, 4, OVERLAY_FONT_SIZE_16);
				APP_OVERLAY_create_title(i, ii, 10, 24, OVERLAY_FONT_SIZE_16);
			}

			APP_OVERLAY_update_clock(i, ii, 0, 0);
			APP_OVERLAY_show_clock(i, ii, true);

			APP_OVERLAY_update_title(i, ii, 0, 0);
			APP_OVERLAY_show_title(i, ii, true);

			if(1){//if(0 == ii){
				// main stream encode all the time
				sdk_enc->start_h264_stream(i, ii);
			}else{
				// media buf hock
				MEDIABUF_hock_user_changed(buf_id, mediabuf_hock_user_changed);
			}
		}
	}

__exit_clean:	
	mutex_unlock(s_encMutex);
}

void AVENC_chn_close(int chn)
{
	int i = chn;
	int ii;

	if (sdk_enc == NULL) return;
	if (JCONF_IS_BNC_CAM(chn) == FALSE) return;
	
	mutex_lock(s_encMutex);

	for(ii = 1; ii >= 0; --ii){
		APP_OVERLAY_release_clock(i, ii);
		APP_OVERLAY_release_title(i, ii);
		sdk_enc->release_h264_stream(i, ii);
		APP_TRACE("venc %d,%d close", i, ii);
	}
	
	mutex_unlock(s_encMutex);
}

void AVENC_Init2()
{
	int i = 0;

	if (s_encMutex == NULL) s_encMutex = mutex_create();
	if (JCONF_BNC_CAM() <= 0) return;
	if (sdk_enc != NULL) return;
	
	SDK_init_enc();
	APP_OVERLAY_init();
	
	sdk_enc->do_buffer_request = MEDIABUF_in_request;
	sdk_enc->do_buffer_attach = MEDIABUF_in_append;
	sdk_enc->do_buffer_commit = MEDIABUF_in_commit;

	for(i = JCONF_BNC_CAM_START(); i < JCONF_BNC_CAM_END(); ++i){
		AVENC_chn_open(i);
	}

	sdk_enc->start();
	TIMETICK_AddTask(APP_OVERLAY_task, 1); // update per second
	
}

void AVENC_Destroy2()
{
	int i;

	if (sdk_enc == NULL) return;

	// remove the timer task, it's very importatn
	TIMETICK_RemoveTask(APP_OVERLAY_task);
	sdk_enc->stop();
	for(i = JCONF_BNC_CAM_START(); i < JCONF_BNC_CAM_END(); ++i){
		AVENC_chn_close(i);
	}
	APP_OVERLAY_destroy();
	SDK_destroy_enc();
	if (s_encMutex)  {
		mutex_destroy(s_encMutex);
		s_encMutex = NULL;
	}
}

#if  defined(_HVR)
#define MEDIA_POOL_SIZE_MAIN (1024*1024)
#define MEDIA_POOL_SIZE_SUB  (MEDIA_POOL_SIZE_MAIN/3)
#define FRAME_BUFF_NUM_IDR   (2)
#define FRAME_BUFF_NUM_NOR   (400)
#define STREAM_USER_MAIN     (4)
#define STREAM_USER_SUB      (8)
#elif  defined(_EXT_HDDVR)
#define MEDIA_POOL_SIZE_MAIN (8*1024*1024/8)
#define MEDIA_POOL_SIZE_SUB  (MEDIA_POOL_SIZE_MAIN/6)
#define FRAME_BUFF_NUM_IDR   (2)
#define FRAME_BUFF_NUM_NOR   (400)
#define STREAM_USER_MAIN     (4)
#define STREAM_USER_SUB      (8)
#elif defined(_HI3520D_RAM_LITE) || defined(SDK_PLATFORM_HI3531)
#define MEDIA_POOL_SIZE_MAIN (256*1024*1.2)
#define MEDIA_POOL_SIZE_SUB  (MEDIA_POOL_SIZE_MAIN/4)
#define FRAME_BUFF_NUM_IDR   (1)
#define FRAME_BUFF_NUM_NOR   (340)
#define STREAM_USER_MAIN     (2)
#define STREAM_USER_SUB      (4)
#elif (MAX_CAM_CH > 16)
#define MEDIA_POOL_SIZE_MAIN (256*1024*1.2)
#define MEDIA_POOL_SIZE_SUB  (MEDIA_POOL_SIZE_MAIN/4)
#define FRAME_BUFF_NUM_IDR   (2)
#define FRAME_BUFF_NUM_NOR   (320)
#define STREAM_USER_MAIN     (4)
#define STREAM_USER_SUB      (8)
#else
#define MEDIA_POOL_SIZE_MAIN (0)
#define MEDIA_POOL_SIZE_SUB  (0)
#define FRAME_BUFF_NUM_IDR   (2)
#define FRAME_BUFF_NUM_NOR   (400)
#define STREAM_USER_MAIN     (4)
#define STREAM_USER_SUB      (8)
#endif

static int dvr_init_prepare(int *state)
{
	int rlt = 0;
	int wdt_count = 0;
	time_t time_start = 0;
	pthread_t pid = NULL;

	//Set Signal
    struct sigaction sa; 
    sa.sa_handler = SIG_IGN; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, 0);

	TRACE_DEBUG("end");
	return 0;
}

int DVR_Init()
{	
	int init_state = FALSE;
	g_pstSysEnv = PSysenvStruct(NULL, NULL, NULL); //初始化环境变量
	
    setenv("DEF_ETHER", ETH_NETWORK_DEV, 1);

	//Prepare
	dvr_init_prepare(&init_state);

#if defined(_NVR)	
	JA_SYS_CONFIG_init0(0, MAX_CAM_CH, 2);
#elif defined(_HVR)
	{
		int nBNG = g_pstSysEnv->GetHvrBncChn(g_pstSysEnv);
		JA_SYS_CONFIG_init0(nBNG, MAX_CAM_CH  - nBNG, 2);
	}
#else
	JA_SYS_CONFIG_init0(MAX_CAM_CH, 0, 2);
#endif
	
	int nVGAW = 0;
	int nVGAH = 0;
	int nCVBSW = 704;
	int nCVBSH = g_pstSysEnv->GetStandard(g_pstSysEnv) == VOUT_PAL ? 576 : 480;
	VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);
	setenv("DEF_ETHER", ETH_NETWORK_DEV, 1);
	switch(enRes)
	{
	default:
	case VGA_800x600:
		nVGAW = 800;
		nVGAH = 600;
		break;
#if defined(_JA7216NC)
	case VGA_1024x768:
	case VGA_1280x1024:
		nVGAW = 1024;
		nVGAH = 768;
		break;
//	case VGA_1280x1024:
//		nVGAW = 1280;
//		nVGAH = 1024;
//		break;
#else
	case VGA_1024x768:
		nVGAW = 1024;
		nVGAH = 768;
		break;
	case VGA_1280x720:
		nVGAW = 1280;
		nVGAH = 720;
		break;
#endif
	case VGA_1920x1080P50Hz:
	case VGA_1920x1080P60Hz:
		nVGAW = 1920;
		nVGAH = 1080;
		break;
	}

	srand((int)time(NULL));
	DVR_TRACE("vga [%d,%d] cvbs [%d,%d]", nVGAW, nVGAH, nCVBSW, nCVBSH);

	TICKER_init();
	TIMETICK_Init(); // 时钟节拍线程初始化
	SYSTEM_Init(); // CPU初始化，与SDK相关
	SYSLOG_Init(NULL, 0); // 系统UDP调试日志

	GPIO_INIT();
#if  defined(_EXT_HDDVR)
	HDDVR_Init(VIDEO_IS_PAL() ?0 :1);
#endif

	MEDIABUF_init();
	int i = 0,ii = 0;
	for(i = 0; i < MAX_CAM_CH; ++i){
		for(ii = 0; ii < 2; ++ii){
			char stream_name[32];
			size_t media_pool_size = 0;
			int const keyframe_available = FRAME_BUFF_NUM_IDR;
			int const frame_available	 = FRAME_BUFF_NUM_NOR;
			int user_available = 0;
			if(0 == ii){
				user_available = STREAM_USER_MAIN;
			}else{
				user_available = STREAM_USER_SUB;
			}
			media_pool_size = (0 == (ii % 2))
						?MEDIA_POOL_SIZE_MAIN
						:MEDIA_POOL_SIZE_SUB;
			sprintf(stream_name, "ch%d_%d.264", i, ii);
			MEDIABUF_new(media_pool_size, stream_name, NULL, frame_available, keyframe_available, user_available);
			MEDIABUF_hock_pool_exhausted(MEDIABUF_lookup_byname(stream_name), mediabuf_hock_on_pool_exhausted);
#if defined(_NVR) || defined(_HVR) || defined(_DNVR) || defined(_JAMEDIA)
			JDK_vo_loss_multi_fill_buffer(i, ii, TRUE, 1);
			JDK_vo_loss_multi_fill_buffer(i, ii, FALSE, 3);
#endif
		}
	}

#ifdef ENABLE_JDK
	APP_JDK_init();
#endif

#ifndef ENABLE_JDK
	video_ScreenInit();
#endif
	OSD_INIT(nVGAW, nVGAH, nCVBSW, nCVBSH);

	OSD_RELOAD(FALSE, FALSE);
#ifndef ENABLE_JDK	
	video_StandardInit();
	VIDEO_INIT();
#endif
	sleep(4);
	AUDIO_Init();

	SDK_VIN_HW_SPEC_t vin_hw_spec = SDK_VIN_HW_SPEC_INVALID;
	switch(GPIODRV_GetVerFix()){
		case GPIO_PROBE_VER0:
			vin_hw_spec = SDK_VIN_HW_SPEC_HI35XX_4D1X2_CH0_CH2;
			break;
		case GPIO_PROBE_VER1:
			vin_hw_spec = SDK_VIN_HW_SPEC_HI35XX_4D1X2_CH2_CH0;
			break;
		default:
			break;
    }

	XENVACT_init_volumn();
#if defined(_NVR) || defined(_HVR) || defined(_DNVR) || defined(_JAMEDIA)	
	APP_GLIB_init();
#endif

	system("kill -9 `pidof rcS`");
	init_state = TRUE;

	return 0;
}

int DVR_Destroy()
{
	TIMETICK_Destroy();

	pthread_gpio_manage_deinti();

	AUDIO_Destroy();

	OSD_Destroy();

#if  defined(_EXT_HDDVR)
	HDDVR_Exit();
#endif
	GPIO_Destroy();

	PSysenvDestruct(g_pstSysEnv, NULL);
	g_pstSysEnv = NULL;

	SYSLOG_Destroy();
	SYSTEM_Destroy();

	return 0;
}


// return argc, argv
#define MAX_INPUT_PARAM (31)
static int dvr_exec_parse_cmdline(char* szLine, char* ret_argv[MAX_INPUT_PARAM])
{
	int argc = 0;
	int* argv[MAX_INPUT_PARAM];
	char* p = NULL;
	memset(argv, 0, sizeof(argv));
	//DVR_TRACE("input length = %d\r\n", nInLength);
	if((p = strpbrk(szLine, "\r\n"))){
		*p = '\0';
	}
	p = szLine;
	do
	{
		p = strtok(p, ", \t");
		if(p){
			ret_argv[argc++] = p;
			p += strlen(p) + 1;
			//DVR_TRACE("arg[%d] = %s", argc - 1, ret_argv[argc - 1]);
		}
	}while(p && argc < MAX_INPUT_PARAM);
	return argc;
}

static void dvr_exec_help()
{
	return;
	printf("\033[1;33m");
	printf("usage\r\n");
	printf("\033[m");
}

int DVR_Exec()
{
	int i = 0;
	int argc = 0;
	char* argv[MAX_INPUT_PARAM];
	char szInput[128];
	DVR_TRACE("DVR Init Finished......");

#define OPT_MATCH(opt,m) ((0==strncasecmp(opt,m,strlen(m)))&&(strlen(opt)==strlen(m)))
#define CMD_DETECT(cmd) (OPT_MATCH(argv[0],cmd))

	while(fgets(szInput, sizeof(szInput) - 1, stdin)){
		printf(szInput);
		if(!((argc = dvr_exec_parse_cmdline(szInput, argv)) > 0)){
			dvr_exec_help();
			continue;
		}
		else if(CMD_DETECT("video"))
		{
			if(!(argc >= 2)){
				continue;
			}

			if(OPT_MATCH(argv[1], "--color") || OPT_MATCH(argv[1], "-c")){
				if(argc >= 3){
					unsigned char u8Value = (unsigned char)(atoi(argv[2]));
					DVR_TRACE("set color %d", u8Value);
					for(i = 0; i < MAX_CAM_CH; ++i){
						VIDEO_SetColor(i, u8Value, u8Value, u8Value, u8Value);
					}
				}
			}
			else if(OPT_MATCH(argv[1], "--auto") || OPT_MATCH(argv[1], "-a")){
				VIDEO_ResetAutoSwitch();
			}


		}
		else if(CMD_DETECT("vo"))
		{
			switch(argc)
			{
			default:
			case 4:
				{
					int chn1, chn2;
					chn1 = atoi(argv[2]);
					chn2 = atoi(argv[3]);
					JDK_swap_vo_chn(chn1, chn2);
				}
				break;
			case 1:
				JDK_set_vo_division(VO_DIVISION_DEF, 0);
			case 2:
				switch(atoi(argv[1]))
				{
				default:
				case 0:
					{
						int n;
						for ( n =0 ; n < 2000; n++) {
							printf("vdec dynamic switch test @%d\n", n);
							JDK_set_vo_division(VO_DIVISION_X8 , 0); 
							usleep(20*1000);
							JDK_set_vo_division(VO_DIVISION_X9, 0); 
						}
					}
					break;
				case 1:
					JDK_set_vo_division(VO_DIVISION_X1, 0); break;
				case 4:
					JDK_set_vo_division(VO_DIVISION_X4 , 0); break;
				case 6:
					JDK_set_vo_division(VO_DIVISION_X6 , 0); break;
				case 8:
					JDK_set_vo_division(VO_DIVISION_X8 , 0); break;
				case 9:
					JDK_set_vo_division(VO_DIVISION_X9, 0); break;
				case 16:
					JDK_set_vo_division(VO_DIVISION_X16,  0); break;
				case 25:
					JDK_set_vo_division(VO_DIVISION_X25,  0); break;
				case 36:
					JDK_set_vo_division(VO_DIVISION_X36,  0); break;
				}
				break;
			case 3:
				switch(atoi(argv[1]))
				{
				default:
				case 1:
					JDK_set_vo_division(VO_DIVISION_X1, atoi(argv[2]) % MAX_CAM_CH); break;
				case 4:
					JDK_set_vo_division(VO_DIVISION_X4, atoi(argv[2]) % 2); break;
#if (MAX_CAM_CH >= 8)
				case 9:
					JDK_set_vo_division(VO_DIVISION_X9, 0); break;
#endif
				}
				break;
			}
		}
	}
#undef CMD_DETECT
	return 0;
}


#endif	//__DVR_C__

