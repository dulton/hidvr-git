#ifndef __JDK_COMMON_H__
#define __JDK_COMMON_H__
	
#ifdef __cplusplus
extern "C" {
#endif

#include "jdk_types.h"
#include "jdk.h"

typedef struct JDK_SYS_IFACE
{
	JDK_BOOL (*init)(JDK_STANDARD_MODE standard);
	JDK_BOOL (*deinit)();
	
	JDK_POINTER (*open_frame)(const char *filePath, JDK_UINT32 width, JDK_UINT32 height);
	JDK_BOOL (*close_frame)(JDK_POINTER handler);
}stJDK_SYS_IFACE, *lpJDK_SYS_IFACE;


typedef struct JDK_VIN_IFACE
{
	JDK_BOOL (*init)(JDK_STANDARD_MODE standard);
	JDK_BOOL (*deinit)();
	JDK_BOOL (*open)(JDK_UINT32 chn, lpJDK_VIN_ARG arg);
	JDK_BOOL (*is_open)(JDK_UINT32 chn);
	JDK_BOOL (*close)(JDK_UINT32 chn);
	JDK_BOOL (*get_vi_size)(JDK_UINT32 chn, lpJDK_SIZE size);
	JDK_BOOL (*bind_output)(JDK_UINT32 vi_chn, JDK_UINT32 vo_chn);
	JDK_BOOL (*unbind_output)(JDK_UINT32 vi_chn, JDK_UINT32 vo_chn);
	JDK_BOOL (*set_digital_zoom)(JDK_UINT32 nChn, JDK_UINT32 nRatioX, JDK_UINT32 nRatioY, JDK_UINT32 nRatioW, JDK_UINT32 nRatioH);
	JDK_BOOL (*clear_digital_zoom)(JDK_UINT32 nChn);
	JDK_BOOL (*bind_enc)(JDK_UINT32 vi_chn, JDK_UINT32 enc_chn);
	JDK_BOOL (*unbind_enc)(JDK_UINT32 vi_chn);
	JDK_BOOL (*set_color)(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT sharpness, JDK_FLOAT hue);
	JDK_BOOL (*get_color)(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *sharpness, JDK_FLOAT *hue);
}stJDK_VI_IFACE, *lpJDK_VI_IFACE;


typedef struct JDK_VO_IFACE
{
	JDK_BOOL (*init)(JDK_MONITOR_TYPE monitor, JDK_STANDARD_MODE standard,
		JDK_VGA_RESOLUTION_TYPE dispResol,
		JDK_UINT32 dispOffsetX, JDK_UINT32 dispOffsetY,
		JDK_UINT32 imgW, JDK_UINT32 imgH);
	JDK_BOOL (*deinit)(JDK_MONITOR_TYPE monitor);
	JDK_BOOL (*open_chn)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn, lpJDK_VO_CHN_ARG arg);
	JDK_BOOL (*chn_is_open)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
	JDK_BOOL (*close_chn)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
	JDK_BOOL (*hide_chn)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
	JDK_BOOL (*show_chn)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);

	JDK_BOOL (*detach_binding)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);

	JDK_BOOL (*enable_user_pic)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
	JDK_BOOL (*show_user_pic)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn, JDK_POINTER picHandler);
	JDK_BOOL (*disable_user_pic)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
	
	//JDK_BOOL (*set_color)(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT sharpness, JDK_FLOAT hue);
	//JDK_BOOL (*get_color)(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *sharpness, JDK_FLOAT *hue);
}stJDK_VO_IFACE, *lpJDK_VO_IFACE;

typedef struct JDK_VENC_IFACE
{
	JDK_BOOL (*init)();
	JDK_BOOL (*deinit)();
	JDK_BOOL (*open_chn)(JDK_UINT32 chn, lpJDK_VENC_ARG arg);
	JDK_BOOL (*close_chn)(JDK_MONITOR_TYPE monitor, JDK_UINT32 chn);
}stJDK_VENC_IFACE, *lpJDK_VENC_IFACE;

typedef struct JDK_VDEC_IFACE
{
	JDK_BOOL (*init)();
	JDK_BOOL (*deinit)();
	JDK_BOOL (*bind_vo)(JDK_UINT32 dec_chn, JDK_MONITOR_TYPE monitor, JDK_UINT32 vo_chn);
	JDK_BOOL (*unbind_vo)(JDK_UINT32 dec_chn, JDK_MONITOR_TYPE monitor, JDK_UINT32 vo_chn);
	JDK_BOOL (*open_chn)(JDK_UINT32 chn, lpJDK_VDEC_ARG arg);
	JDK_BOOL (*close_chn)(JDK_UINT32 chn);
}stJDK_VDEC_IFACE, *lpJDK_VDEC_IFACE;

typedef struct JDK_AD_IFACE
{
	JDK_BOOL (*init)(JDK_STANDARD_MODE standard, JDK_BOOL bBnc960H);
	JDK_BOOL (*is_loss)(JDK_UINT32 chn);
	JDK_INT32 (*probe_vi_format)(JDK_UINT32 chn);
	JDK_BOOL (*set_color)(JDK_UINT32 chn, JDK_FLOAT brightness, JDK_FLOAT contrast, JDK_FLOAT saturation, JDK_FLOAT hue, JDK_FLOAT sharpness);
	JDK_BOOL (*get_color)(JDK_UINT32 chn, JDK_FLOAT *brightness, JDK_FLOAT *contrast, JDK_FLOAT *saturation, JDK_FLOAT *hue, JDK_FLOAT *sharpness);
	JDK_INT32 (*ad_fix_eq)(JDK_UINT32 chn);
	JDK_BOOL (*deinit)();
}stJDK_AD_IFACE, *lpJDK_AD_IFACE;

typedef struct JDK_CTX
{
	JDK_SOC_BRAND brand;
	JDK_SOC_CHIP socChip;
	JDK_AD_CHIP adChip;
	JDK_STANDARD_MODE standard;
	JDK_UINT32 maxBncChnNum;
	JDK_UINT32 bncChnNum;
	JDK_UINT32 netChnNum;
	JDK_UINT32 zeroChnNum;
	JDK_UINT32 totalChnNum;
	JDK_UINT32 streamNum;
	JDK_UINT32 audioNum;
	JDK_UINT32 sensorNum;
	JDK_UINT32 alarmNum;
	JDK_UINT32 ddrNum;
	JDK_UINT32 ddrSize;
	JDK_UINT32 maxImgResol;
	JDK_CHN_INPUT_MODE maxNetCamImgResol;
	JDK_CHN_INPUT_MODE maxBncCamImgResol;
	JDK_UINT32 nPlaybackCam;
	JDK_UINT32 nCurPlayCam;
	JDK_RESOLUTION_TYPE maxPlaybackResol;
	

	lpJDK_AD_IFACE adIface;
	lpJDK_SYS_IFACE sysIface;
	lpJDK_VI_IFACE viIface;
	lpJDK_VO_IFACE voIface;
	lpJDK_VDEC_IFACE vdecIface;
	lpJDK_VENC_IFACE vencIface;

	stJDK_VIN_ARG viArg[JDK_MAX_CHN];
	JDK_CHN_STATE viChnState[JDK_MAX_CHN];
	JDK_INT32 viChnMode[JDK_MAX_CHN];
	JDK_INT32 viChnModeTemp[JDK_MAX_CHN];
	//JDK_INT32 viChnFormat[JDK_MAX_CHN];
	JDK_UINT8 viVoChnMap[JDK_MAX_CHN];
	JDK_UINT8 voChnMap[JDK_MAX_CHN];
	JDK_UINT8 viVencChnMap[JDK_MAX_CHN];

	// display
	JDK_BOOL voIsOn;
	JDK_VGA_RESOLUTION_TYPE hdDispResol;
	JDK_UINT32 nCVBSMode;
	JDK_UINT32 nVoDev;
	JDK_UINT32 nVoDevList[2];
	JDK_UINT32 voSrcWidth[2];
	JDK_UINT32 voSrcHeight[2];
	stJDK_RECT voScreenRect[2];
	JDK_BOOL bFixWHRatio;
	JDK_BOOL bLiveMode;
	JDK_UINT32 nMaxGrid;
	JDK_UINT32 nMaxRowOrCol;
	JDK_UINT32 nMaxPage;
	JDK_UINT32 nCurDivision;
	JDK_UINT32 nCurPage;
	fJDK_VO_SWITCH_HOOK voSwitchHook;
	fJDK_VO_SWAP_HOOK voSwapHook;

	JDK_FLOAT brightness[JDK_MAX_CHN];
	JDK_FLOAT contrast[JDK_MAX_CHN];
	JDK_FLOAT saturation[JDK_MAX_CHN];
	JDK_FLOAT hue[JDK_MAX_CHN];
	JDK_FLOAT sharpness[JDK_MAX_CHN];

	// chn user picture handler
	JDK_POINTER hChnUserPic[8];
	
	// loss detection
	fJDK_LOSS_HOOK lossHook;
	pthread_t lossDaemonPid;
	JDK_BOOL lossLoopTrigger;
	JDK_BOOL videoCheckFreeze;
	JDK_UINT64 lostState;
	JDK_UINT64 signalLocked;
	fJDK_QUERY_LOST_STATE_HOOK bncCamQueryLostState;
	fJDK_QUERY_LOST_STATE_HOOK netCamQueryLostState;

	// vi format
	JDK_BOOL viInModeAuto;
	pthread_t viCheckDaemonPid;
	JDK_BOOL viCheckLoopTrigger;
	fJDK_VI_FORMAT_CHANGED_HOOK viFormatChangedHook;
}stJDK_CTX, *lpJDK_CTX;

extern lpJDK_CTX s_jdkCtx;


#if !(defined (JDK_STMT_START) && defined (JDK_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define JDK_STMT_START	(void) __extension__ (
#    define JDK_STMT_END		)
#  else
#    if (defined (sun) || defined (__sun__))
#      define JDK_STMT_START	if (1)
#      define JDK_STMT_END	else (void)0
#    else
#      define JDK_STMT_START	do
#      define JDK_STMT_END	while (0)
#    endif
#  endif
#endif

#define JDK_DEBUG (1)
#define JDK_SYNTAX "1;32"
#define JDK_DEBUG_SYNTAX "1;34"
#define JDK_TRACE(fmt, arg...) \
	JDK_STMT_START{\
		const char* const bname = __FILE__;\
		printf("\033["JDK_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}JDK_STMT_END
	
#if (JDK_DEBUG	 == 1)
#define JDK_DEBUG(fmt, arg...) \
		JDK_STMT_START{\
			const char* const bname = __FILE__;\
			printf("\033["JDK_DEBUG_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
			printf(fmt, ##arg);\
			printf("\r\n");\
		}JDK_STMT_END
#else
#define JDK_DEBUG(fmt, arg...)
#endif

#define JDK_ASSERT(exp, fmt, arg...) \
	JDK_STMT_START{\
		if(!(exp)){\
			const char* const bname = __FILE__;\
			printf("\033["JDK_SYNTAX"m[%12s:%4d]\033[0m assert(\"%s\") ", bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
			exit(1);\
		}\
	}JDK_STMT_END


#define jdk_return_if_fail(expr)		JDK_STMT_START{			\
     if (expr) { } else       					\
       {								\
	 		JDK_TRACE (#expr);				\
	 		return;							\
       };	\
	 }JDK_STMT_END

#define jdk_goto_if_fail(expr, where)		JDK_STMT_START{			\
		  if (expr) { } else 						 \
			{								 \
				 JDK_TRACE (#expr); 			 \
				 goto where;						 \
			};	 \
		  }JDK_STMT_END
	 

#define jdk_return_val_if_fail(expr,val)	JDK_STMT_START{			\
		if (expr) { } else						\
		{								\
			JDK_TRACE (#expr);				\
			return (val);							\
		};		\
	}JDK_STMT_END

#define jdk_hi_return_if_fail(expr)		JDK_STMT_START{			\
	 if (HI_SUCCESS == (expr)) { } else 						\
	   {								\
			JDK_TRACE (#expr"; errcode: 0x%x", expr);				\
			return; 						\
	   };	\
	 }JDK_STMT_END

#define jdk_hi_goto_if_fail(expr, where)		JDK_STMT_START{			\
		  if (HI_SUCCESS == (expr)) { } else						 \
			{								 \
			  JDK_TRACE (#expr"; errcode: 0x%x", expr);			  \
				 goto where;						 \
			};	 \
		  }JDK_STMT_END
	 

#define jdk_hi_return_val_if_fail(expr,val)	JDK_STMT_START{			\
		if (HI_SUCCESS == (expr)) { } else						\
		{								\
			JDK_TRACE (#expr"; errcode: 0x%x", expr);				\
			return (val);							\
		};		\
	}JDK_STMT_END
		

#define JDK_MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define JDK_MIN2(a, b) (((a) > (b)) ? (b) : (a))
#define JDK_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))


#define JDK_VRES_SIZE_W_HD720P  1280
#define JDK_VRES_SIZE_H_HD720P  720
#define JDK_VRES_SIZE_W_HD1080P 1920
#define JDK_VRES_SIZE_H_HD1080P 1080


#define JDK_HI_SOC_SUPPORTED_HDMI() \
	(JDK_SOC_CHIP_IS(JDK_HI_SOC_3520A) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3520D) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3521) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3531) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3532) || JDK_SOC_CHIP_IS(JDK_HI_SOC_3535))


#define JDK_MAX_BNC_CAM()		(s_jdkCtx ? s_jdkCtx->maxBncChnNum : 0)
#define JDK_BNC_CAM()		(s_jdkCtx ? s_jdkCtx->bncChnNum : 0)
#define JDK_MAX_PLAYBACK_CAM()		(s_jdkCtx ? s_jdkCtx->nPlaybackCam : 0)
#define JDK_MAX_PLAYBACK_CAM_RESOL()		(s_jdkCtx ? s_jdkCtx->maxPlaybackResol : 0)
#define JDK_IS_BNC_CAM(n)		(s_jdkCtx ? (((n) < s_jdkCtx->bncChnNum) ? JDK_TRUE : JDK_FALSE) : JDK_FALSE)
#define JDK_NET_CAM()		(s_jdkCtx ? s_jdkCtx->netChnNum : 0)
#define JDK_ACTIVE_CAM()	(s_jdkCtx ? (s_jdkCtx->netChnNum + s_jdkCtx->bncChnNum) : 0)
#define JDK_TOTAL_CAM()		(s_jdkCtx ? s_jdkCtx->totalChnNum : 0)
#define JDK_ZERO_CAM()		(s_jdkCtx ? s_jdkCtx->zeroChnNum : 0)
#define JDK_STANDARD_IS_PAL()	(s_jdkCtx ? ((s_jdkCtx->standard == JDK_STANDARD_PAL) ? JDK_TRUE : JDK_FALSE) : JDK_FALSE) 
#define JDK_CHN_STATE(chn)			(s_jdkCtx ? s_jdkCtx->viChnState[chn] : JDK_CHN_STATE_NULL)
#define JDK_CHN_SET_STATE(chn, state)			(s_jdkCtx ? (s_jdkCtx->viChnState[chn] = (state), JDK_TRUE) : JDK_FALSE)
#define JDK_CHN_IN_MODE(chn)			(s_jdkCtx ? s_jdkCtx->viChnMode[chn] : JDK_INPUT_MODE_NULL)
#define JDK_CHN_SET_IN_MODE(chn, mode)			(s_jdkCtx ? (s_jdkCtx->viChnMode[chn] = (mode), JDK_TRUE) : JDK_FALSE)
#define JDK_SOC_BRAND_IS(_brand)		(s_jdkCtx ? (s_jdkCtx->brand == (_brand)) : JDK_FALSE)
#define JDK_SOC_CHIP_IS(soc)		(s_jdkCtx ? (s_jdkCtx->socChip == (soc)) : JDK_FALSE)

#define JDK_VL_MAX_ROW_OR_COL()			(s_jdkCtx ? s_jdkCtx->nMaxRowOrCol : 1)
#define JDK_VL_MAX_GRID()			(s_jdkCtx ? s_jdkCtx->nMaxGrid : 1)
#define JDK_VL_MAX_PAGE(divi)			(s_jdkCtx ? s_jdkCtx->nMaxPage: 1)

#define JDK_CVBS_MONITOR_MODE()				(s_jdkCtx ? s_jdkCtx->nCVBSMode : 0)

JDK_BOOL JDK_inmode_get_wh(JDK_CHN_INPUT_MODE resol, JDK_UINT32 *width, JDK_UINT32 *height);
JDK_BOOL JDK_resol_get_wh(JDK_RESOLUTION_TYPE resol, JDK_UINT32 *width, JDK_UINT32 *height);
JDK_BOOL JDK_vga_resol_get_wh(JDK_VGA_RESOLUTION_TYPE resol, JDK_UINT32 *width, JDK_UINT32 *height);
JDK_BOOL JDK_vi_resize_in_cut_mode(JDK_VI_CUT_MODE cutMode, JDK_INT32 *width, JDK_INT32 *height);
JDK_BOOL JDK_GET_SIBLING_CHN(JDK_UINT32 thiz_chn, JDK_VI_MUX_MODE muxMode,  JDK_UINT32 *sibling_chns, JDK_UINT32 *n_sibling_chns);
JDK_BOOL JDK_is_all_sibling_chns_close(JDK_UINT32 thiz_chn, JDK_VI_MUX_MODE muxMode);
JDK_VI_MUX_MODE JDK_get_mux_mode();

extern char *g_JdkChnInputModeString[];


extern stJDK_AD_IFACE g_jdkAdIface;
extern stJDK_VI_IFACE g_jdkHi3521ViIface;
extern stJDK_VO_IFACE g_jdkHi3521VoIface;
extern stJDK_SYS_IFACE g_jdkHi3521SysIface;
extern stJDK_VENC_IFACE g_jdkHi3521VencIface;
extern stJDK_VDEC_IFACE g_jdkHi3521VdecIface;

#ifdef __cplusplus
}
#endif
#endif /* __JDK_COMMON_H__ */

