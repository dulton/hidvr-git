

#ifndef __JUAN_SDK_H__
#define __JUAN_SDK_H__

#define ENABLE_JDK
	
#ifdef __cplusplus
extern "C" {
#endif

#include "jdk_types.h"
#include "vo_grid.h"

#define JDK_MAX_CHN		(128)
#define JDK_MAX_STREAM	(10)


typedef enum {
	JDK_SOC_BRAND_HISI = 0,
	JDK_SOC_BRAND_TI,
	JDK_SOC_BRAND_GRAIN,

	JDK_SOC_NR
}JDK_SOC_BRAND;

typedef enum {
	JDK_HI_SOC_BASE = 0,
	JDK_HI_SOC_3515,	
	JDK_HI_SOC_3515A,	
	JDK_HI_SOC_3515C,
	JDK_HI_SOC_3520,
	JDK_HI_SOC_3520A,	
	JDK_HI_SOC_3520D,
	JDK_HI_SOC_3521,
	JDK_HI_SOC_3531,
	JDK_HI_SOC_3532,
	JDK_HI_SOC_3535,
	JDK_HI_SOC_END,

	
	JDK_TI_SOC_BASE = 1000,
	JDK_TI_SOC_END,

	JDK_GM_SOC_BASE = 2000,
	JDK_GM_SOC_8120,
	JDK_GM_SOC_END
}JDK_SOC_CHIP;

typedef enum {
	JDK_AD_CHIP_0 = 0, // none or embeded in soc
	
	JDK_AD_CHIP_NR
}JDK_AD_CHIP;


typedef enum {
	JDK_SENSOR_CCD = 0,
	JDK_SENSOR_CMOS,

	JDK_SENSOR_NR
}JDK_SENSOR_TYPE;

typedef enum {
	JDK_MONITOR_VGA = 0x01,
	JDK_MONITOR_CVBS = 0x02,
	JDK_MONITOR_HDMI = 0x04,
}JDK_MONITOR_TYPE;

typedef enum {
	JDK_SD_MODE_DATA_BY_HD_WBC = 0,
	JDK_SD_MODE_DATA_BY_PAINT,
	JDK_SD_MODE_NR
}JDK_SD_MONITOR_MODE;
#define JDK_DEF_SD_MONITOR_MODE	JDK_SD_MODE_DATA_BY_HD_WBC

#define  JDK_TVSIZE_SCALAR (16)
#define  JDK_TVSIZE_STEPER (8)


typedef enum {
	JDK_PIXEL_YUV420 = 0,
	JDK_PIXEL_YUV411,
	JDK_PIXEL_YUV422,
	
	JDK_PIXEL_YUV_NR
}JDK_YUV_PIXEL_FORMAT;

typedef enum {
	JDK_PIXEL_RGB888 = 0,
	JDK_PIXEL_RGB565,
	JDK_PIXEL_RGB555,

	JDK_PIXEL_RGB_NR
}JDK_RGB_PIXEL_FORMAT;

typedef enum {
	JDK_STANDARD_PAL = 0,
	JDK_STANDARD_NTSC,
	JDK_STANDARD_NR
}JDK_STANDARD_MODE;

typedef enum {
	JDK_CHN_STATE_NULL = 0,
	JDK_CHN_STATE_BNC,
	JDK_CHN_STATE_NET,
	JDK_CHN_STATE_NR
}JDK_CHN_STATE;

typedef enum {
	JDK_VI_MUX_X1 = 1,
	JDK_VI_MUX_X2 = 2,
	JDK_VI_MUX_X4 = 4,
}JDK_VI_MUX_MODE;
#define JDK_IS_VALID_VI_MUX(_mux_) (((_mux_) == JDK_VI_MUX_X1) || ((_mux_) == JDK_VI_MUX_X2) || ((_mux_) == JDK_VI_MUX_X4))

typedef enum {
	JDK_VI_CUT_MODE_NONE = 0,
	JDK_VI_CUT_MODE_1_2 = 2, /* 1/2 */
	JDK_VI_CUT_MODE_1_4 = 4,/* 1/4 */
	JDK_VI_CUT_MODE_1_8 = 8,/* 1/8 */
	JDK_VI_CUT_MODE_1_9 = 9,/* 1/9 */
	JDK_VI_CUT_MODE_1_16 = 16,/* 1/16 */
}JDK_VI_CUT_MODE;
#define JDK_IS_VALID_CUT_MUX(_mode_) (((_mode_) == JDK_VI_CUT_MODE_NONE) || ((_mode_) == JDK_VI_CUT_MODE_1_2) \
	|| ((_mode_) == JDK_VI_CUT_MODE_1_4) || ((_mode_) == JDK_VI_CUT_MODE_1_8) || ((_mode_) == JDK_VI_CUT_MODE_1_9) || ((_mode_) == JDK_VI_CUT_MODE_1_16))

typedef enum {
	JDK_INPUT_MODE_NULL = 0,
	JDK_INPUT_BNC_BASE,
	JDK_INPUT_BNC_D1 = JDK_INPUT_BNC_BASE,
	JDK_INPUT_BNC_720H,
	JDK_INPUT_BNC_960H,
	JDK_INPUT_BNC_720P,
	JDK_INPUT_BNC_HFHD,
	JDK_INPUT_BNC_1080P,
	JDK_INPUT_BNC_END,
	JDK_INPUT_NET_BASE = JDK_INPUT_BNC_END,
	JDK_INPUT_NET_720P = JDK_INPUT_NET_BASE,
	JDK_INPUT_NET_960P,
	JDK_INPUT_NET_1080P,
	JDK_INPUT_NET_END
}JDK_CHN_INPUT_MODE;
#define JDK_INPUT_FORMAT_AUTO		(-2)
#define JDK_INPUT_FORMAT_UNKNOWN	(-1)

typedef enum {
	JDK_RESOL_QCIF = 0,
	JDK_RESOL_QVGA,
	JDK_RESOL_CIF,
	JDK_RESOL_HALF_D1,
	JDK_RESOL_360P,
	JDK_RESOL_VGA,
	JDK_RESOL_D1,
	JDK_RESOL_960H,
	JDK_RESOL_720P,
	JDK_RESOL_960P,
	JDK_RESOL_HFHD,
	JDK_RESOL_1080P,
}JDK_RESOLUTION_TYPE;

typedef enum {
	JDK_VSUPPORT_SD960H  = 0,
	JDK_VSUPPORT_HD720P,
	JDK_VSUPPORT_HD1080P,
}JDK_VSUPPORT_TYPE;


typedef	enum
{
	JDK_VGA_800x600 = 0,
	JDK_VGA_1024x768,
	JDK_VGA_1280x720,
	JDK_VGA_1920x1080P50Hz,
	JDK_VGA_1920x1080P60Hz,
	JDK_VGA_RESOLUTION_CNT,
}JDK_VGA_RESOLUTION_TYPE;


/*
************************************************************************************
*/

typedef enum{
	JDK_VENC_JPEG = 0,
	JDK_VENC_MPEG4,
	JDK_VENC_H264
}JDK_VENC_TYPE;

typedef enum
{
	JDK_AENC_G711 = 0,
	JDK_AENC_G726,
	JDK_AENC_AAC
}JDK_AENC_TYPE;

typedef enum
{
	JDK_QUANT_AUTO = 0,
	JDK_QUANT_CBR,
	JDK_QUANT_VBR,
	JDK_QUANT_NR
}JDK_QUANT_MODE;

typedef enum{
	JDK_MPEG4_PROFILE_SP = 0,
	JDK_MPEG4_PROFILE_ASP,

	JDK_MPEG4_PROFILE_NR
}JDK_MPEG4_PROFILE;

typedef enum{
	JDK_H264_PROFILE_BASELINE = 0,
	JDK_H264_PROFILE_MAIN,
	JDK_H264_PROFILE_EXTENDED,
	JDK_H264_PROFILE_HIGH,

	JDK_H264_PROFILE_NR
}JDK_H264_PROFILE;

typedef struct JDK_POINT
{
	JDK_INT32 x;
	JDK_INT32 y;
}stJDK_POINT, *lpJDK_POINT;
#define JDK_SET_POINT(P,X,Y) (P)->x = (X); (P)->y = (Y)

typedef struct JDK_SIZE
{
	JDK_UINT32 width;
	JDK_UINT32 height;
}stJDK_SIZE, *lpJDK_SIZE;
#define JDK_SET_SIZE(P,W,H) (P)->width = (W); (P)->height = (H)

typedef struct JDK_RECT
{
	JDK_INT32 x, y;
	JDK_UINT32 width, height;
}stJDK_RECT, *lpJDK_RECT;
#define JDK_SET_RECT(P,X,Y,W,H) (P)->width = (W); (P)->height = (H); (P)->x = (X); (P)->y = (Y)

typedef struct JDK_SYS_ARG
{
	JDK_STANDARD_MODE standard;
	JDK_UINT32 chnNum;
	JDK_UINT32 streamNum;
	stJDK_SIZE resol[JDK_MAX_CHN];
}stJDK_SYS_ARG, *lpJDK_SYS_ARG;

typedef struct JDK_VIN_ARG
{
	stJDK_RECT rect;
	JDK_INT32 inMode;
	JDK_UINT32 fps;
	JDK_BOOL mirror;
	JDK_BOOL flip;
	JDK_VI_MUX_MODE muxMode;
	JDK_VI_CUT_MODE cutMode;
}stJDK_VIN_ARG, *lpJDK_VIN_ARG;

typedef struct JDK_VENC_ARG
{
	JDK_VENC_TYPE enc_type;
	stJDK_SIZE enc_resolution;
	JDK_UINT32 enc_profile;
	JDK_QUANT_MODE enc_brc;
	JDK_UINT32 enc_bps;
	JDK_UINT32 enc_fps;
}stJDK_VENC_ARG, *lpJDK_VENC_ARG;

typedef struct JDK_VO_ARG
{
	stJDK_SIZE resolution;
}stJDK_VO_ARG, *lpJDK_VO_ARG;

typedef struct JDK_VO_CHN_ARG
{
	stJDK_RECT rect;
}stJDK_VO_CHN_ARG, *lpJDK_VO_CHN_ARG;


typedef struct JDK_VDEC_ARG
{
	stJDK_SIZE max_resolu;
}stJDK_VDEC_ARG, *lpJDK_VDEC_ARG;


/*
************************************************************************************
*/

typedef struct JDK_ARG
{
	JDK_SOC_BRAND brand;
	JDK_SOC_CHIP soc;
	JDK_AD_CHIP ad;
	JDK_STANDARD_MODE standard;
	JDK_UINT32 nMaxBncChn;
	JDK_UINT32 nBncChn;
	JDK_UINT32 nNetChn;
	JDK_UINT32 nStream;
	JDK_UINT32 nAudio;
	JDK_UINT32 nSensor;
	JDK_UINT32 nAlarm;
	JDK_UINT32 nDdr;
	JDK_UINT32 ddrSize;
	JDK_CHN_INPUT_MODE maxNetCamImgResol;
	JDK_CHN_INPUT_MODE maxBncCamImgResol;
	JDK_RESOLUTION_TYPE maxImgResol;

	JDK_UINT32 nPlaybackCam;
	JDK_RESOLUTION_TYPE maxPlaybackResol;
}stJDK_ARG, *lpJDK_ARG;

JDK_BOOL JDK_new(lpJDK_ARG arg);
JDK_BOOL JDK_init();
void JDK_deinit();
void JDK_finalize();

typedef void (*fJDK_LOSS_HOOK)(JDK_UINT32 chn);
void JDK_set_vloss_hook(fJDK_LOSS_HOOK hook);
typedef JDK_BOOL (*fJDK_QUERY_LOST_STATE_HOOK)(JDK_UINT32 chn);
void JDK_set_hook_to_query_lost(fJDK_QUERY_LOST_STATE_HOOK bncCamQueryLostState, fJDK_QUERY_LOST_STATE_HOOK netCamQueryLostState);
JDK_BOOL JDK_vin_is_loss(JDK_UINT32 nChn);

JDK_BOOL JDK_sys_init();
JDK_BOOL JDK_sys_deinit();

JDK_BOOL JDK_vin_init(JDK_BOOL viInModeAuto);
JDK_BOOL JDK_vin_deinit();
JDK_BOOL JDK_vin_open(JDK_UINT32 chn, lpJDK_VIN_ARG arg);
JDK_BOOL JDK_vin_close(JDK_UINT32 chn);
JDK_BOOL JDK_vin_is_loss(JDK_UINT32 nChn);
JDK_BOOL JDK_vin_get_size(JDK_UINT32 chn, lpJDK_SIZE size);
JDK_BOOL JDK_vin_set_input_format(JDK_UINT32 chn, JDK_INT32 format);

JDK_BOOL JDK_venc_init(void);
JDK_BOOL JDK_venc_deinit(void);
JDK_BOOL JDK_venc_chn_open(JDK_UINT32 chn, lpJDK_VENC_ARG arg);
JDK_BOOL JDK_venc_chn_close(JDK_UINT32 chn);

JDK_BOOL JDK_vdec_init(void);
JDK_BOOL JDK_vdec_deinit(void);
JDK_BOOL JDK_vdec_chn_open(JDK_UINT32 chn, lpJDK_VDEC_ARG arg);
JDK_BOOL JDK_vdec_chn_close(JDK_UINT32 chn);

/* value  0 ~ 1.0 */
extern JDK_BOOL JDK_set_color(JDK_UINT32 nChn, JDK_FLOAT nHue, JDK_FLOAT nBrightness, JDK_FLOAT nContrast, JDK_FLOAT nSaturation, JDK_FLOAT nSharpness);
extern JDK_BOOL JDK_get_color(JDK_UINT32 nChn, JDK_FLOAT *nHue, JDK_FLOAT *nBrightness, JDK_FLOAT *nContrast, JDK_FLOAT *nSaturation, JDK_FLOAT *nSharpness);
extern JDK_BOOL JDK_set_hue(JDK_UINT32 nChn, JDK_FLOAT nValue);
extern JDK_BOOL JDK_set_brightness(JDK_UINT32 nChn, JDK_FLOAT nValue);
extern JDK_BOOL JDK_set_contrast(JDK_UINT32 nChn, JDK_FLOAT nValue);
extern JDK_BOOL JDK_set_saturation(JDK_UINT32 nChn, JDK_FLOAT nValue);
extern JDK_BOOL JDK_set_sharpness(JDK_UINT32 nChn, JDK_FLOAT nSharpness);

JDK_BOOL JDK_monitor_init(JDK_MONITOR_TYPE monitors, JDK_VGA_RESOLUTION_TYPE dispResol, JDK_RESOLUTION_TYPE maxImgResol, 
	JDK_UINT32 nCVBSOffsetX, JDK_UINT32 nCVBSOffsetY, JDK_UINT32 nCVBSWidth, JDK_UINT32 nCVBSHeight, JDK_UINT32 nCVBSMode);
void JDK_monitor_deinit();
JDK_BOOL JDK_monitor_switch();
JDK_BOOL JDK_set_tv_adjust(JDK_INT32 x, JDK_INT32 y, JDK_INT32 w, JDK_INT32 h, JDK_BOOL bSync);


JDK_BOOL JDK_set_live_mode(void);
JDK_BOOL JDK_set_play_mode(JDK_UINT32 nPlayCam /* if 0, use default play cam setted by jdk_new */);
typedef void (*fJDK_VO_SWITCH_HOOK)(VO_DIVISION_TYPE divFrom, JDK_UINT32 nPageFrom, VO_DIVISION_TYPE divTo, JDK_UINT32 nPateTo);
typedef void (*fJDK_VO_SWAP_HOOK)(JDK_UINT32 fromVoChn, JDK_BOOL fromNeedSwitch, JDK_UINT32 toVoChn, JDK_BOOL toNeedSwitch);
JDK_BOOL JDK_set_vo_switch_hook(fJDK_VO_SWITCH_HOOK hook);
JDK_BOOL JDK_set_vo_division(VO_DIVISION_TYPE div, JDK_UINT32 nPage);
JDK_BOOL JDK_set_vo_swap_hook(fJDK_VO_SWAP_HOOK hook);
JDK_BOOL JDK_swap_vo_chn(JDK_UINT32 chn1, JDK_UINT32 chn2);

void JDK_vo_loss_daemon(void);
void JDK_exit_vo_loss_daemon(void);

void JDK_vi_format_check_daemon(void);
void JDK_exit_vi_format_check_daemon(void);
void JDK_set_auto_vi_format_check(JDK_BOOL bAuto);
typedef void (*fJDK_VI_FORMAT_CHANGED_HOOK)(JDK_UINT32 viChn, JDK_UINT32 fromFormat, JDK_UINT32 toFormat);
void JDK_set_vi_format_changed_hook(fJDK_VI_FORMAT_CHANGED_HOOK hook);

JDK_UINT32 JDK_get_max_vsupport(void);

extern int JDK_set_digital_zoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH);
extern void JDK_clear_digital_zoom(int nChn);



#define JDK_USR_PIC_IDLE		(0)
#define JDK_USR_PIC_LOSS		(1)
#define JDK_USR_PIC_LOGO		(2)
#define JDK_USR_PIC_RECONNECT	(3)
#define JDK_DEF_IDLE_CHN_PIC		"disable.yuv"
#define JDK_DEF_LOSS_CHN_PIC		"loss.yuv"
#define JDK_DEF_LOGO_CHN_PIC		"logo.yuv"
#define JDK_DEF_RECONNECT_CHN_PIC	"reconnect.yuv"
#define JDK_DEF_LOSS_BNC_H264		"skin/bnc_cam.h264"
#define JDK_DEF_LOSS_NET_H264		"skin/net_cam.h264"
JDK_BOOL JDK_vo_show_picture(JDK_UINT32 chn, JDK_UINT32 nUserPicType);
JDK_BOOL JDK_vo_show_text(JDK_UINT32 chn, const char *text, stJDK_POINT position);

JDK_BOOL JDK_ad_set_color(int nChn, float nBrightness, float nContrast, float nSaturation, float nHue);


JDK_UINT32 jdk_vl_get_def_divi(void);
JDK_UINT32 jdk_vl_get_cur_page(void);
JDK_UINT32 jdk_vl_get_cur_divi_n_pages(void);
JDK_UINT32 jdk_vl_get_cur_division(void);
JDK_UINT32 jdk_vl_get_cur_divi_grids(void);
JDK_BOOL jdk_is_live_mode(void);

#define JDK_VL_DEF_DIVI()				jdk_vl_get_def_divi()
#define JDK_VL_CUR_DIVI()				jdk_vl_get_cur_division()
#define JDK_VL_CUR_DIVI_GRIDS()		jdk_vl_get_cur_divi_grids()	
#define JDK_VL_CUR_PAGE()				jdk_vl_get_cur_page()
#define JDK_VL_IS_LIVE_MODE()			jdk_is_live_mode()
#define JDK_VL_CUR_DIVI_PAGES() 		VO_GRID_get_n_pages(JDK_VL_CUR_DIVI())
#define JDK_VL_PAGES(divi) 			VO_GRID_get_n_pages(divi)



#ifdef __cplusplus
}
#endif
#endif /* __JUAN_SDK_H__ */


