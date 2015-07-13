/******************************************************************************
Copyright (c)2013-2023, ZheJiang Dahua Technology Stock CO.LTD.
All Rights Reserved.
******************************************************************************
File Name   : dahua_dh9901.h
Version       : Initial Draft
Author        : 
Created       : 2013/11/29
Last Modified :
Description   : The common defination & API interface
Function List :
History       :
1.Date        : 2013/11/29
2.Author      : 21810
3.Modification: Created file  
******************************************************************************/

#ifndef  __DH9901_API_H__ 
#define __DH9901_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef MODULE_NAME
#undef MODULE_NAME
#endif
#define MODULE_NAME "dh9901.a"

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/
typedef unsigned char           DH_U8;
typedef unsigned short          DH_U16;
typedef unsigned int            DH_U32;

typedef signed char             DH_S8;
typedef short                   DH_S16;
typedef int                     DH_S32;

#ifndef _M_IX86
    typedef unsigned long long  DH_U64;
    typedef long long           DH_S64;
#else
    typedef __int64             DH_U64;
    typedef __int64             DH_S64;
#endif

typedef char                    DH_CHAR;
#define DH_VOID                 void

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/

typedef enum {
    DH_FALSE = 0,
    DH_TRUE  = 1,
} DH_BOOL;

#ifndef NULL
    #define NULL    0L
#endif

#define DH_NULL     0L
#define DH_SUCCESS  0
#define DH_FAILURE  (-1)

/*----------------------------------------------*
 *                           错误码                 *
 *----------------------------------------------*/
#define DH_DH9901_NOT_INIT       (0xFFFFFFFE)
#define DH_DH9901_INVALID_PARAM  (0xFFFFFFFD)
#define DH_DH9901_NULL_POINTER   (0xFFFFFFFC)


/*----------------------------------------------*
 *             DH9901相关结构定义               *
 *----------------------------------------------*/
 
#define DH_CHNS_PER_DH9901     (4)
#define MAX_DH9901_NUM_PER_CPU (4)

typedef struct dh_video_position_s
{
	DH_S32 ucHorizonOffset; // 水平偏移
	DH_S32 ucVerticalOffset;//垂直偏移
	DH_S32 res[2];
}DH_VIDEO_POSITION_S;

typedef enum dh_set_mode_e
{
	DH_SET_MODE_DEFAULT = 0, // 默认配置
	DH_SET_MODE_USER    = 1, // 用户配置
	DH_SET_MODE_NONE,
}DH_SET_MODE_E;

typedef struct dh_video_color_s
{
	DH_U8 ucBrightness;	    //< 亮度，取值0-100。
	DH_U8 ucContrast;		//< 对比度，取值0-100。
	DH_U8 ucSaturation;	    //< 饱和度，取值0-100。
	DH_U8 ucHue;			//< 色调，取值0-100。
	DH_U8 ucGain;			//< 增益，不开放
	DH_U8 ucWhiteBalance;	//< 白电平,不开放
	DH_U8 ucSharpness;	    //< 锐度，取值0-15。
	DH_U8 reserved[1];
}DH_VIDEO_COLOR_S;

typedef enum dh_hd_video_format_e
{
	DH_HD_720P_25HZ = 0, // 720P 25帧
	DH_HD_720P_30HZ,
	DH_HD_720P_50HZ,
	DH_HD_720P_60HZ,
	DH_HD_1080P_25HZ, 
	DH_HD_1080P_30HZ,
	DH_HD_DEFAULT,
}DH_HD_VIDEO_FORMAT_E;

/*AD 视频丢失、视频制式状态*/
typedef struct dh_video_status_s
{
	DH_U8 ucVideoLost; // 1视频丢失，0 视频恢复
	DH_U8 ucVideoFormat; //接入视频制式，取值DH_HD_VIDEO_FORMAT_E
	DH_U8 reserved[2];
}DH_VIDEO_STATUS_S;

typedef struct dh_dh9901_audio_connect_mode_s
{
	DH_BOOL ucCascade;	 /*音频是否级联 ture / false*/
	DH_U8 ucCascadeNum;  /*级联数目1 - 4*/
	DH_U8 ucCascadeStage;/*级联级数0 - 3*/
}DH_AUDIO_CONNECT_MODE_S;

/*音频采样率，9901支持8K(默认)，16K*/
typedef enum dh_audio_samplerate_e
{
	DH_AUDIO_SAMPLERATE_8k  = 0, //默认
	DH_AUDIO_SAMPLERATE_16K = 1,
}DH_AUDIO_SAMPLERATE_E;

/*音频时钟主从模式*/
typedef enum dh_audio_encclk_mode_e
{
	DH_AUDIO_ENCCLK_MODE_SLAVE  = 0, //默认
	DH_AUDIO_ENCCLK_MODE_MASTER = 1,
}DH_AUDIO_ENCCLK_MODE_E;

/*音频数据输出模式*/
typedef enum dh_audio_sync_mode_e
{
	DH_AUDIO_SYNC_MODE_I2S = 0, //默认
	DH_AUDIO_SYNC_MODE_DSP = 1,
}DH_AUDIO_SYNC_MODE_E;

/*音频工作时钟模式*/
typedef enum dh_audio_encclk_sel_e
{
	DH_AUDIO_ENCCLK_MODE_108M = 0,//默认
	DH_AUDIO_ENCCLK_MODE_54M  = 1,
	DH_AUDIO_ENCCLK_MODE_27M  = 2, 
}DH_AUDIO_ENCCLK_SEL_E;

/*音频属性配置 : (1)连接方式:级联非级联
** (2) 采样率 (3) 主从模式
*/
typedef struct dh_dh9901_audio_attr_s
{
	DH_BOOL bAudioEnable;
	/*级联模式 用户需配置*/
	DH_AUDIO_CONNECT_MODE_S stAudioConnectMode;
	/*DH_SET_MODE_DEFAULT默认配置8K,I2S 从模式，108M*/
	DH_SET_MODE_E enSetAudioMode; //默认配置，用户配置
	DH_AUDIO_SAMPLERATE_E enAudioSampleRate; // 0:8K (default) 1:16k
	DH_AUDIO_ENCCLK_MODE_E enAudioEncClkMode; //0:slave mode(defualt) 1: master mode
	DH_AUDIO_SYNC_MODE_E enAudioSyncMode;   //0:I2S format(defualt) 1:DSP format
	DH_AUDIO_ENCCLK_SEL_E enAudioEncClkSel;  //系统时钟0:27M 1:54M 2:108M(defualt)
	DH_U8 reserved[5];
}DH_DH9901_AUDIO_ATTR_S;

/*视频属性，(1) 通道顺序选择
** (2) 输出数据复用方式
*/
typedef struct dh_dh9901_video_attr_s
{
	DH_SET_MODE_E enSetVideoMode; //默认配置，用户配置
	DH_U8 ucChnSequence[DH_CHNS_PER_DH9901]; //通道输出映射:0 - 3 
	DH_U8 ucVideoOutMux; // 0: 8bit 4路高清(default) 1: 16bit两路高清复合
	DH_U8 reserved[7];
}DH_DH9901_VIDEO_ATTR_S;

typedef struct dh_dh9901_ptz485attr_s
{
	DH_SET_MODE_E enSetVideoMode; //默认配置，用户配置
	DH_U8 ucProtocolLen; //485 协议长度,默认7字节
	DH_U8 reserved[3];
}DH_DH9901_PTZ485_ATTR_S;


/*内部自检线程属性*/
typedef struct dh_auto_detect_attr_s
{
	DH_SET_MODE_E enDetectMode;//默认模式50ms
	//内部需要自适应均衡，不建议间隔时间设置过大。
	DH_U8 ucDetectPeriod;//检测周期，单位ms
	DH_U8 reserved[3];
}DH_AUTO_DETECT_ATTR_S;


typedef struct dh_dh9901_attr_s
{
	DH_U8 ucChipAddr; //I2C地址

	DH_DH9901_VIDEO_ATTR_S   stVideoAttr;
	DH_DH9901_AUDIO_ATTR_S   stAuioAttr;
	DH_DH9901_PTZ485_ATTR_S  stPtz485Attr;
	DH_U32 reserved[4];
}DH_DH9901_ATTR_S;

typedef struct dh_dh9901_init_attr_s
{
	DH_U8 ucAdCount; // 主控下AD 数量
	
	DH_DH9901_ATTR_S stDh9901Attr[MAX_DH9901_NUM_PER_CPU];// 每一片DH9901配置信息
	DH_AUTO_DETECT_ATTR_S    stDetectAttr;

	/*读写寄存器方法，初始化时需要注册*/
	DH_S32 (* Dh9901_WriteByte)(DH_U8 ucChipAddr, DH_U8 ucRegAddr, DH_U8 ucRegValue);
	DH_U8 (* Dh9901_ReadByte)(DH_U8 ucChipAddr, DH_U8 ucRegAddr);

	DH_U32 reserved[4];
}DH_DH9901_INIT_ATTR_S;


///\dh9901初始化
/// 
///\param [in] 9901
///\retval 其他值 错误码
///\retval 0 初始化成功
DH_S32 DH9901_API_Init(DH_DH9901_INIT_ATTR_S *pDh9901InitAttrs);

///\dh9901去初始化
/// 
///\param [in] 9901
///\retval 其他值 错误码
///\retval 0 去初始化成功
DH_S32 DH9901_API_DeInit(void);

///\dh9901设置图像色彩
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] 9901 pVideoColor 色彩参数，enColorMode 色彩模式
///\retval 其他值 错误码
///\retval 0 设置成功
DH_S32 DH9901_API_SetColor(DH_U8 ucChipIndex, DH_U8 ucChn, DH_VIDEO_COLOR_S *pVideoColor, DH_SET_MODE_E enColorMode);

///\dh9901读取图像色彩
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] 9901 pVideoColor 色彩参数，enColorMode 色彩模式
///\retval 其他值 错误码
///\retval 0 读取成功
DH_S32 DH9901_API_GetColor(DH_U8 ucChipIndex, DH_U8 ucChn, DH_VIDEO_COLOR_S *pVideoColor, DH_SET_MODE_E enColorMode);


///\ dh9901清除图像均衡
///\ 注意 : 图像均衡清除阻塞调用，
///\正确返回后才可进行色彩读取。
///\param [in]ucChipIndex芯片索引，ucChn通道号
///\retval 其他值 错误码
///\retval 0清除成功
DH_S32 DH9901_API_ClearEq(DH_U8 ucChipIndex, DH_U8 ucChn);

///\dh9901 485功能使能，单独通道操作
///\每次操作485前使能，操作完毕后关闭，
///\注意: 不要多个通道同时使能
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] bEnable DH_TRUE/ DH_FALSE
///\retval 其他值 错误码
///\retval 0 使能成功
DH_S32 DH9901_API_Contrl485Enable(DH_U8 ucChipIndex, DH_U8 ucChn, DH_BOOL bEnable);


///\写485buffer
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] ucBuf 数据指针ucLenth 数据长度
///\retval 其他值 错误码
///\retval 0  写成功
DH_S32 DH9901_API_Send485Buffer(DH_U8 ucChipIndex, DH_U8 ucChn, DH_U8 *ucBuf, DH_U8 ucLenth);


///\图像偏移调节
///\ 设置前先读取当前位置信息，根据当前位置调整偏移
///\调整范围0 - 15
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] pVideoPos 偏移参数
///\retval 其他值 错误码
///\retval 0 成功
DH_S32 DH9901_API_SetVideoPosition(DH_U8 ucChipIndex, DH_U8 ucChn, DH_VIDEO_POSITION_S *pVideoPos);

///\获取图像偏移位置信息
/// 
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] pVideoPos 偏移参数
///\retval 其他值 错误码
///\retval 0 成功
DH_S32 DH9901_API_GetVideoPosition(DH_U8 ucChipIndex, DH_U8 ucChn, DH_VIDEO_POSITION_S *pVideoPos);

///\设置同轴输入音频音量
/// 
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\param [in] ucVolume 音量值0 - 100
///\retval -1 
///\retval 0 成功
DH_S32 DH9901_API_SetAudioInVolume(DH_U8 ucChipIndex, DH_U8 ucChn, DH_U8 ucVolume);

///\ 获取AD芯片丢失和视频制式
/// 
///\param [in] 9901 ucChipIndex芯片索引，ucChn通道号
///\retval 其他值 错误码
///\retval 0 成功
DH_S32 DH9901_API_GetVideoStatus(DH_U8 ucChipIndex, DH_U8 ucChn, DH_VIDEO_STATUS_S *pVideoStatus);

///\9901写寄存器
/// 
///\param [in] 9901 ucChipIndex芯片索引，ucPage 页索引
///\param [in] 9901 ucRegAddr寄存器值，pucValue 寄存器值指针
///\retval 其他值 错误码
///\retval 0 成功
DH_S32 DH9901_API_WriteReg(DH_U8 ucChipIndex, DH_U8 ucPage, DH_U8 ucRegAddr, DH_U8 *pucValue);

///\9901读寄存器
/// 
///\param [in] 9901 ucChipIndex芯片索引，ucPage 页索引
///\param [in] 9901 ucRegAddr寄存器值，pucValue 寄存器值指针
///\retval 其他值 错误码
///\retval 0  读取成功
DH_S32 DH9901_API_ReadReg(DH_U8 ucChipIndex, DH_U8 ucPage, DH_U8 ucRegAddr, DH_U8 *pucValue);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

