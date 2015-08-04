


#ifndef	__ENVIRONMENT_H__
#define	__ENVIRONMENT_H__

#include "conf.h"

#ifdef	__cplusplus
extern	"C"	{
#endif

#ifndef	IP_ADDR_LEN
#define IP_ADDR_LEN		(4)
#endif
#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN	(6)
#endif
#ifndef ENET_ID_LEN
#define ENET_ID_LEN		(16)
#endif
#ifndef DDNS_URL_LEN
#define DDNS_URL_LEN	(64)
#endif
#ifndef DDNS_USER_LEN
#define DDNS_USER_LEN	(64)
#endif
#ifndef DDNS_PWD_LEN
#define DDNS_PWD_LEN	(64)
#endif
#ifndef SMTP_EDITBOX_LEN
#define SMTP_EDITBOX_LEN	(32)
#endif
#ifndef FTP_EDITBOX_LEN
#define FTP_EDITBOX_LEN	(32)
#endif
#ifndef FTP_FOLDER_LEN
#define FTP_FOLDER_LEN	(200)
#endif
#ifndef NET_EDITBOX_LEN
#define NET_EDITBOX_LEN	(40)
#endif
#ifndef IPC_USER_LEN
#define IPC_USER_LEN	(25)
#endif
#ifndef IPC_PWD_LEN
#define IPC_PWD_LEN	(25)
#endif
#ifndef MAX_IPC_CNT
#define MAX_IPC_CNT	(250)
#endif

#ifndef PPPOE_USER_LEN
#define PPPOE_USER_LEN	(25)
#endif
#ifndef PPPOE_PWD_LEN
#define	PPPOE_PWD_LEN	(25)
#endif
#ifndef CHN_TITLE_LEN
#define	CHN_TITLE_LEN	(16)
#endif
#ifndef MAX_TOUR_POINT
#define MAX_TOUR_POINT (32)
#endif
#ifndef MAX_IPCAM_PRIV_DATA
#define MAX_IPCAM_PRIV_DATA	(128)
#endif	
#ifndef MAX_IPCAM_PROFILE_NR
#define MAX_IPCAM_PROFILE_NR	(2)
#endif

#ifndef ONLINE_UPGRADE_URL_LEN
#define ONLINE_UPGRADE_URL_LEN	(64)
#endif
#ifndef ONLINE_UPGRADE_HOST_LEN
#define ONLINE_UPGRADE_HOST_LEN	(64)
#endif 
#ifndef MAX_FTP_SERVER_PATH_LEN
#define MAX_FTP_SERVER_PATH_LEN (128)
#endif

#define DEFAULT_HUE (32)
#define DEFAULT_BRIGHTNESS (28)
#define DEFAULT_SATURATION (32)
#define DEFAULT_CONTRAST (32)

#ifndef ETH_NETWORK_DEV
#define ETH_NETWORK_DEV "eth0"
#endif

#ifndef	DECLARE_VGA_RESOLUTION
#define	DECLARE_VGA_RESOLUTION

#ifndef ETH_NETWORK_DEV
#define ETH_NETWORK_DEV "eth0"
#endif
#define DEFAULT_IP_ADDR "0.0.0.0"  // "192.168.1.88"
#define DEFAULT_MAC_ADDR "00:00:00:00:00:00" // "00:05:fe:ff:ff:ff"

typedef	enum
{
	VGA_800x600 = 0,
	VGA_1024x768,
	VGA_1280x720,
	VGA_1920x1080P50Hz,
	VGA_1920x1080P60Hz,
	VGA_RESOLUTION_CNT,
}VGA_RESOLUTION;
#endif	//DECLARE_VGA_RESOLUTION

#ifndef DECLARE_DATE_FMT
#define DECLARE_DATE_FMT
typedef	enum
{
	YYYY_MM_DD = 0,
	MM_DD_YYYY,
	DD_MM_YYYY,
	DATE_FMT_CNT,
}DATE_FMT;
#endif	//DECLARE_DATE_FMT

#define DEBUG_ENVIRONMENT
#ifdef	DEBUG_ENVIRONMENT
#define	ENV_TRACE(fmt, args...)		printf("ENVIRONMENT: "fmt"\r\n", ##args)
#else
#define	ENV_TRACE(fmt, args...)
#endif

#ifndef DECLARE_WORLDWIDE_LANGUAGE
#define DECLARE_WORLDWIDE_LANGUAGE
typedef enum
{
	WORLDWIDE_ENGLISH,
	WORLDWIDE_CHINESE_MAINLAND,
	WORLDWIDE_CHINESE_HONGKONG,
	WORLDWIDE_CHINESE_TAIWAN,
	WORLDWIDE_KOREAN,
	WORLDWIDE_JAPANESE,
	WORLDWIDE_ARABIC,
	WORLDWIDE_PERSIAN,
	WORLDWIDE_HEBREW,
	WORLDWIDE_PORTUGUESE,
	WORLDWIDE_SPANISH,
	WORLDWIDE_HUNGARIAN,
	WORLDWIDE_RUSSIAN,
	WORLDWIDE_FRENCH,
	WORLDWIDE_GERMAN,
	WORLDWIDE_THAI,
	WORLDWIDE_VIETNAMESE,
	WORLDWIDE_TURKISH,
	WORLDWIDE_ITALIAN,
	WORLDWIDE_DANISH,
	WORLDWIDE_LANGUAGE_CNT,
}WORLDWIDE_LANGUAGE;
extern char* WORLDWIDE_LANGUAGE_STRING[];
#endif	//DECLARE_LANGUAGE

#ifndef DECLARE_WEEKDAY
#define DECLARE_WEEKDAY
typedef	enum
{
	WEEKDAY_SUNDAY = 0,
	WEEKDAY_MONDAY,
	WEEKDAY_TUESDAY,
	WEEKDAY_WEDNESDAY,
	WEEKDAY_THURSDAY,
	WEEKDAY_FRIDAY,
	WEEKDAY_SATURDAY,
	WEEKDAY_CNT,
}WEEKDAY;
extern char* WEEKDAY_STRING[];
#endif	//DECLARE_WEEKDAY

#ifndef	DECLARE_RANK
#define	DECLARE_RANK
typedef	enum
{
	RANK_HIGHEST = 0,
	RANK_HIGH,
	RANK_MEDIUM,
	RANK_LOW,
	RANK_LOWEST,
	RANK_CNT
}RANK;
typedef enum
{
	MOTION = 0,
	VIDEO_LOSS,
	VIDEO_COVER,
	DETECTIONVAL_CNT
}DETECTIONVAL;

extern char* RANK_STRING[];
#endif	//DECLARE_RANK

typedef	enum
{
	VOUT_PAL = 0,
	VOUT_NTSC,
	VOUT_STANDARD_CNT,
}VOUT_STANDARD;
extern char* VOUT_STANDARD_STRING[];

typedef enum {
	KEYMAP_TYPE0 = 0, //Default Type, Original Keypad Type for Ja7216 Base Platform.
	KEYMAP_TYPE1,     //Original Keypad Type for Ja7204 Base Platform.
	KEYMAP_TYPE2,     //Original Keypad Type for Ja7208 Base Platform.
	KEYMAP_TYPE3,     //Original Keypad Type for Front Panel Base Platform.
	KEYMAP_TYPE_CNT,
}KEYMAP_TYPE;

typedef enum {
	REMOTE_TYPE0 = 0, //Default Type
	REMOTE_TYPE1,     //OEM Specification Type
	REMOTE_TYPE_CNT,
}REMOTE_TYPE;

#if   defined(GPIO_KEYPAD_MATRIX)
#define DEFAULT_KEYMAP_TYPE KEYMAP_TYPE0
#elif defined(GPIO_KEYPAD_PANEL)
#define DEFAULT_KEYMAP_TYPE KEYMAP_TYPE3
#else
#error "Unknown SDK PLATFORMAT!"
#endif

#define DEFAULT_REMOTE_TYPE REMOTE_TYPE0

#ifndef MAX_SUB_STREAM_CNT
#define MAX_SUB_STREAM_CNT (4)
#endif

#ifndef MAX_STREAM_CNT
#define MAX_STREAM_CNT (MAX_SUB_STREAM_CNT + 1)
#endif

typedef enum
{
	MODE_VIDEO = 0,
	MODE_AUDIO_VIDEO,
	ENC_MODE_CNT,
}ENC_MODE;
extern char* ENC_MODE_STRING[];

typedef	enum
{
	ENC_FMT_QCIF = 0,
	ENC_FMT_CIF,
	ENC_FMT_HD1,
	ENC_FMT_D1,
	ENC_FMT_WCIF,
	ENC_FMT_960H,
	ENC_FMT_HD720P,
	ENC_FMT_HD1080P_HALF,
	ENC_FMT_HD1080P,
	ENC_FMT_CNT,
}ENC_FMT;
extern char* ENC_FMT_STRING[];

#define VRES_SIZE_W_SD   704
#define VRES_SIZE_W_EF   960
#define VRES_SIZE_H_PL   576
#define VRES_SIZE_H_NT   480

#define VRES_SIZE_W_HD720P  1280
#define VRES_SIZE_H_HD720P  720
#define VRES_SIZE_W_HD1080P 1920
#define VRES_SIZE_H_HD1080P 1080

typedef struct {
	int VRes_W;
	int VRes_H;
} VRes_Size;

static const VRes_Size VRES_TBL[ENC_FMT_CNT][VOUT_STANDARD_CNT] = {
	{{VRES_SIZE_W_SD/4, VRES_SIZE_H_PL/4}, {VRES_SIZE_W_SD/4, VRES_SIZE_H_NT/4}},
	{{VRES_SIZE_W_SD/2, VRES_SIZE_H_PL/2}, {VRES_SIZE_W_SD/2, VRES_SIZE_H_NT/2}},
	{{VRES_SIZE_W_SD/2, VRES_SIZE_H_PL/1}, {VRES_SIZE_W_SD/2, VRES_SIZE_H_NT/1}},
	{{VRES_SIZE_W_SD/1, VRES_SIZE_H_PL/1}, {VRES_SIZE_W_SD/1, VRES_SIZE_H_NT/1}},
	{{VRES_SIZE_W_EF/2, VRES_SIZE_H_PL/2}, {VRES_SIZE_W_EF/2, VRES_SIZE_H_NT/2}},
	{{VRES_SIZE_W_EF/1, VRES_SIZE_H_PL/1}, {VRES_SIZE_W_EF/1, VRES_SIZE_H_NT/1}},
	{{VRES_SIZE_W_HD720P/1, VRES_SIZE_H_HD720P/1}, {VRES_SIZE_W_HD720P/1, VRES_SIZE_H_HD720P/1}},
	{{VRES_SIZE_W_HD1080P/2, VRES_SIZE_H_HD1080P/1}, {VRES_SIZE_W_HD1080P/2, VRES_SIZE_H_HD1080P/1}},
	{{VRES_SIZE_W_HD1080P/1, VRES_SIZE_H_HD1080P/1}, {VRES_SIZE_W_HD1080P/1, VRES_SIZE_H_HD1080P/1}},
};

enum {
	VIDEO_MODE_960H = 0,
	VIDEO_MODE_720P,
	VIDEO_MODE_HALF1080P,
	VIDEO_MODE_1080P,
	VIDEO_MODE_IPCAM,
	VIDEO_MODE_CNT,
};

// record type
#undef NONE_REC
#undef TIMER_REC
#undef MOTION_REC
#undef SENSOR_REC
#undef MANUAL_REC
#define NONE_REC   (0)
#define TIMER_REC  (1<<0)
#define MOTION_REC (1<<1)
#define SENSOR_REC (1<<2)
#define MANUAL_REC (1<<3)
#define REC_COND_CNT	(4)

#ifndef DECLARE_all_rec_condition_t
#define DECLARE_all_rec_condition_t
typedef struct
{
	int begin_hour; //ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ç€¹ï¿½åºœï¿½çƒ½ï¿½ï¿½ï¿½æ‹·ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·é”ŸèŠ¥ï¿½
	int begin_min;
	int end_hour;   //ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½é”ŸèŠ¥ï¿½ï¿½çƒ½ï¿½æ¿®ï¿½æ‹·ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·é”ŸèŠ¥ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½é”Ÿï¿?
	int end_min;
	int video_type; //ï¿½ï¿½åºœï¿½çƒ½ï¿½ï¿½ã‚†ï¿½é”Ÿæ–¤æ‹·ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½æ¿®ï¿½æ‹·ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½æµ ï¿½æ‹·ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½ï¿½ã‚†ï¿½é”Ÿæ–¤æ‹·ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·ï¿½ã‚†ï¿½é”Ÿæ–¤æ‹·ï¿½ï¿?
}rec_condition_t;  //ï¿½ï¿½åºœï¿½çƒ½ï¿½ï¿½ãƒ¯æ‹·ï¿½ï¿½ï¿½é”ŸèŠ¥ï¿½é”Ÿæ–¤æ‹·ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½é”Ÿï¿?

typedef struct
{
	rec_condition_t channel_conditions[REC_COND_CNT];
}channel_rec_condition_t; //æ¿žï¿½æ‹·ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½é”Ÿæ–¤æ‹·ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·é”ŸèŠ¥ï¿½ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½é”ŸèŠ¥ï¿½ï¿½çƒ½ï¿½æ¿®ï¿½æ‹·ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½æŸ¥ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·é”ŸèŠ¥ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿?

typedef struct
{
	channel_rec_condition_t all_conditions[MAX_CAM_CH];
}all_rec_condition_t;//8ï¿½ï¿½ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½é”ŸèŠ¥ï¿½ï¿½çƒ½ï¿½ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½ï¿½å‡¤æ‹·ï¿½ã‚†ï¿½ï¿½ï¿½ï¿½é”Ÿä»‹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½æ‹?
#endif	//DECLARE_all_rec_condition_t

#ifndef DECLARE_ENC_PIC_LEVEL
#define DECLARE_ENC_PIC_LEVEL
typedef RANK ENC_PIC_LEVEL;
#endif

typedef enum
{
	ENC_BR_VARIABLE = 0,
	ENC_BR_CONSTANT,
	ENC_BR_AVERAGE,
	ENC_BR_MODE_CNT,
}ENC_BR_MODE;
extern char* ENC_BR_MODE_STRING[];

typedef enum
{
	ENC_BR_64KPS = 0,
	ENC_BR_128KPS,
	ENC_BR_256KPS,
	ENC_BR_384KPS,
	ENC_BR_512KPS,
	ENC_BR_768KPS,
	ENC_BR_1MPS,
	ENC_BR_1P5MPS,
	ENC_BR_2MPS,
	ENC_BR_3MPS,
	ENC_BR_4MPS,
	ENC_BR_5MPS,
	ENC_BR_6MPS,
	ENC_BR_8MPS,
	ENC_BR_10MPS,
	ENC_BR_VALUE_CNT,
}ENC_BR_VALUE;
extern char* ENC_BR_VALUE_STRING[];

typedef	enum
{
	FR_1FPS = 0,
	FR_2FPS,
	FR_3FPS,
	FR_4FPS,
	FR_5FPS,
	FR_6FPS,
	FR_7FPS,
	FR_8FPS,
	FR_9FPS,
	FR_10FPS,
	FR_11FPS,
	FR_12FPS,
	FR_13FPS,
	FR_14FPS,
	FR_15FPS,
	FR_16FPS,
	FR_17FPS,
	FR_18FPS,
	FR_19FPS,
	FR_20FPS,
	FR_21FPS,
	FR_22FPS,
	FR_23FPS,
	FR_24FPS,
	FR_25FPS,
	FR_FULLFPS = FR_25FPS,
	FR_30FPS,
	ENC_FRAMERATE_CNT,
}ENC_FRAMERATE;
extern char* ENC_FRAMERATE_STRING[];

//positon mode
typedef	enum
{
	POSITION_CUSTOM = 0,
	POSITION_UPPER_LEFT,
	POSITION_JUST_ABOVE,
	POSITION_UPPER_RIGHT,
	POSITION_BOTTOM_LEFT,
	POSITION_JUST_BELOW,
	POSITION_BOTTOM_RIGHT,
	ENC_POSITION_MODE_CNT,
}ENC_POSITION_MODE;
extern char* ENC_POSITION_MODE_STRING[];

typedef enum
{
	PPPOE_PROTOCOL_AUTO,
	PPPOE_PROTOCOL_PAP,
	PPPOE_PROTOCOL_CHAP,
	PPPOE_PROTOCOL_EAP,
	PPPOE_PROTOCOL_CNT,
}PPPOE_PROTOCOL;

typedef enum
{
	SMTP_ENCRYPT_NONE,
	SMTP_ENCRYPT_SSL,
	SMTP_ENCRYPT_TYPE_CNT,
}SMTP_ENCRYPT_TYPE;
extern char* SMTP_ENCRYPT_TYPE_STRING[];

#ifndef DECLARE_MD_SENSITIVITY
#define DECLARE_MD_SENSITIVITY
typedef RANK MD_SENSITIVITY;
#endif

#ifndef	DECLARE_ALARM_DURATION
#define	DECLARE_ALARM_DURATION
typedef	enum
{
	ALARM_1SEC = 0,
	ALARM_2SEC,
	ALARM_3SEC,
	ALARM_4SEC,
	ALARM_5SEC,
	ALARM_8SEC,
	ALARM_10SEC,
	ALARM_CONT,
	ALARM_DURATION_CNT,
}ALARM_DURATION;
extern char* ALARM_DURATION_STRING[];
#endif

#ifndef DECLARE_AUTOSWI_INTERVAL
#define DECLARE_AUTOSWI_INTERVAL
typedef	enum
{
	AUTOSWI_2SEC,
	AUTOSWI_3SEC,
	AUTOSWI_4SEC,
	AUTOSWI_5SEC,
	AUTOSWI_8SEC,
	AUTOSWI_10SEC,
	AUTOSWI_INTERVAL_CNT,
}AUTOSWI_INTERVAL;
extern char* AUTOSWI_INTERVAL_STRING[];
#endif	//DECLARE_AUTOSWI_INTERVAL

#ifndef DECLEAR_AUTOSWI_MODE
#define DECLEAR_AUTOSWI_MODE
typedef enum
{
	AUTOSWI_X1,
	AUTOSWI_X4,
	AUTOSWI_X9,
	AUTOSWI_X16,
	AUTOSWI_MODE_CNT,
}AUTOSWI_MODE;
extern char* AUTOSWI_MODE_STRING[];
#endif	//DECLEAR_AUTOSWI_MODE

#ifndef TIME_FORMAT_MODE
#define TIME_FORMAT_MODE
typedef enum
{
	TIMEFORMAT_X12 = 0,
	TIMEFORMAT_X24,	
	TIME_FORMAT_CNT,
}TIME_FORMAT;
extern char* TIME_FORMAT_STRING[];
#endif

#ifndef	DECLARE_PTZ_PROTOCAL
#define	DECLARE_PTZ_PROTOCAL
typedef	enum
{
	PELCO_D = 0,
	PELCO_P,
	PTZ_PROTOCAL_CNT,
}PTZ_PROTOCAL;
extern char* PTZ_PROTOCAL_STRING[];
#endif	//DECLARE_PTZ_PROTOCAL

#ifndef	DECLARE_PTZ_BAUDRATE
#define	DECLARE_PTZ_BAUDRATE
typedef	enum
{
	BD_2400 = 0,
	BD_4800,
	BD_9600,
	BD_19200,
	BD_38400,
	BD_57600,
	BD_115200,
	PTZ_BAUDRATE_CNT,
}PTZ_BAUDRATE;
extern char* PTZ_BAUDRATE_STRING[];
#endif	//DECLARE_PTZ_BAUDRATE

typedef enum
{
	IPCAM_HG = 0,
	IPCAM_ONVIF,
	IPCAM_RTSP,
	IPCAM_ZAVIO,
	IPCAM_TST,
	IPCAM_VOC,
	IPCAM_N1_ONVIF,
	IPCAM_PROTOCOL_CNT,
}IPCAM_PROTOCOL_NAME;
extern char* IPCAM_PROTOCOL_NAME_STRING[];

//#ifndef	DECLARE_DATABIT
//#define	DECLARE_DATABIT
//typedef	enum
//{
//	DB_5BIT = 0,
//	DB_6BIT,
//	DB_7BIT,
//	DB_8BIT,
//	PTZ_DATABIT_CNT,
//}PTZ_DATABIT;
//#endif	//DECLARE_DATABIT

//#ifndef	DECLARE_STOPBIT
//#define	DECLARE_STOPBIT
//typedef	enum
//{
//	SB_ONE = 0,
//	SB_ONEPFIVE,
//	SB_TWO,
//	PTZ_STOPBIT_CNT,
//}PTZ_STOPBIT;
//#endif	//DECLARE_STOPBIT

//#ifndef	DECLARE_PARITY
//#define	DECLARE_PARITY
//typedef	enum
//{
//	PB_NONE = 0,
//	PB_ODD,
//	PB_EVEN,
//	PTZ_PARITY_CNT,
//}PTZ_PARITY;
//#endif	//DECLARE_PARITYBIT

#ifndef	DECLARE_SENSOR_WORKMODE
#define	DECLARE_SENSOR_WORKMODE
typedef	enum
{
	SENSOR_DISABLE = 0,
	SENSOR_NOPEN,
	SENSOR_NCLOSE,
	SENSOR_WORKMODE_CNT,
}SENSOR_WORKMODE;
extern char* SENSOR_WORKMODE_STRING[];
#endif	//DECLARE_SENSOR_WORKMODE

typedef enum
{
	ENV_FALSE,
	ENV_TRUE,
	ENV_BOOLEAN_CNT,
}ENV_BOOLEAN;
extern char* ENV_BOOLEAN_STRING[];

typedef enum{
    Maintenance_Never=0,
    Maintenance_Once,
    Maintenance_Everyday,
    Maintenance_Everyweek,
    Maintenance_Test,
    Maintenance_Mode_CNT
}Maintenance_Mode;
extern char* Maintenance_Mode_STRING[];

typedef unsigned char ENV_COLOR_VALUE;
#define ENV_COLOR_VALUE_0 (0)
#define ENV_COLOR_VALUE_CNT (17)

#pragma	pack(4)
typedef struct _SysenvColor
{
	unsigned char au8Hue[MAX_CAM_CH];
	unsigned char au8Contrast[MAX_CAM_CH];
	unsigned char au8Luminance[MAX_CAM_CH];
	unsigned char au8Saturation[MAX_CAM_CH];
}SysenvColor;

#define SUPPORT_WEB_LINK_LEN (256)
typedef struct _SysenvGeneral
{
	DATE_FMT enDateFmt;
	TIME_FORMAT enSysTimeFmt;
	ENV_BOOLEAN bKeyLock;
	ENV_BOOLEAN bKeyBuzzer;
	WORLDWIDE_LANGUAGE enLanguage;
	VOUT_STANDARD enStandard;
	ENV_BOOLEAN ChannelModeAuto;
	int  ChannelModes[MAX_CAM_CH];
	KEYMAP_TYPE   enKey_Type;
	REMOTE_TYPE   enRemote_Type;
	unsigned int nDVRId;
	unsigned int nDeviceMode;
	unsigned int nDeviceModeMax;	
	ENV_BOOLEAN bPersianCalendar;
	ENV_BOOLEAN bGuideUse;
	ENV_BOOLEAN bEseeidUse;
	ENC_POSITION_MODE enTimerPositionMode;
	ENC_POSITION_MODE enEseeidPositionMode;
	int nXCoordinateTimer;
	int nYCoordinateTimer;
	int nXCoordinateEseeid;
	int nYCoordinateEseeid;
	int sum_widthEseeid;
	int sum_widthTimer;
	char Support_Web[SUPPORT_WEB_LINK_LEN];
	char AndroidLink[SUPPORT_WEB_LINK_LEN];
	char IOSLink[SUPPORT_WEB_LINK_LEN];
	ENV_BOOLEAN bTimeUse;
	char azSpecialFTPServerPath[MAX_FTP_SERVER_PATH_LEN+1];
}SysenvGeneral;

typedef struct _SysenvEnc
{
	ENC_MODE aenMode[MAX_CAM_CH];
	ENC_FMT aenFormat[MAX_CAM_CH];
	ENC_PIC_LEVEL aenLevel[MAX_CAM_CH];
	ENC_BR_MODE aenBitrateMode[MAX_CAM_CH];
	ENC_BR_VALUE aenBitrateValue[MAX_CAM_CH];
	ENC_FRAMERATE aenFramerate[MAX_CAM_CH];

	ENC_FMT aenSubFormat[MAX_CAM_CH][MAX_SUB_STREAM_CNT];
	ENC_PIC_LEVEL aenSubLevel[MAX_CAM_CH][MAX_SUB_STREAM_CNT];
	ENC_BR_MODE aenSubBitrateMode[MAX_CAM_CH][MAX_SUB_STREAM_CNT];
	ENC_BR_VALUE aenSubBitrateValue[MAX_CAM_CH][MAX_SUB_STREAM_CNT];
	ENC_FRAMERATE aenSubFramerate[MAX_CAM_CH][MAX_SUB_STREAM_CNT];
	ENV_BOOLEAN bEnableEffioEnhanced;
	ENV_BOOLEAN bFmtManualSwitch;
}SysenvEnc;

//#ifdef _NVR
typedef enum
{
	IPC_VIDEO_ONLY,
//	IPC_AUDIO_ONLY,
	IPC_VIDEO_AUDIO,
	IPC_VIDEO_ALARM,
	IPC_VIDEO_AUDIO_ALARM,
	IPC_STREAM_CHOISE_CNT
}IPCAM_STREAM_CHOISE;

typedef enum
{
	TRANSPORT_AUTO = 0,
	TRANSPORT_UDP,
	TRANSPORT_TCP,
}TRANSPORT_TYPE;

typedef enum
{
	ONVIF_NVT = 0,
	ONVIF_NVD,
	ONVIF_NVS,
	ONVIF_NVA,
	ONVIF_DEV_TYPE_NR
}ONVIF_DEV_TYPE;

typedef struct _SysenvIPCPrivData
{
	union
	{
		char padding[MAX_IPCAM_PRIV_DATA];
		struct
		{
			TRANSPORT_TYPE transport;// rtp over udp  or rtp over rtsp
			char stream_name[MAX_IPCAM_PRIV_DATA-4];
		}rtsp;
		struct{
			TRANSPORT_TYPE transport;
			int timezone;
			int not_synctime;
			ONVIF_DEV_TYPE type;
			char name[32];
			char firmware[32];
			char location[MAX_IPCAM_PRIV_DATA - 32 - 32 - 16];
		}onvif;
	};
}SysenvIPCPrivData;

typedef struct _SysenvIPCam
{
	unsigned char au8IPAddr[4];
	unsigned short u16Port;
	IPCAM_PROTOCOL_NAME azProtocolname;
	IPCAM_STREAM_CHOISE azStreamChoise;
	char azUsername[24];
	char azPassword[24];
	char azModelname[32];
	ENV_BOOLEAN azEnable_ch;
	unsigned char u8MACAddr[MAC_ADDR_LEN];
#define STREAM_ID_START_POINT		101
	int iStreamIndex[MAX_IPCAM_PROFILE_NR];		// if is zero , not set, NVC auto select a stream unsing strategy as below:
		// Mine IPC: 1st stream -> main stream; 2nd stream -> sub stream
		// others IPC: 1st stream -> main stream; last stream -> sub stream
		//if this value is set, it must start from 101, 101 meaning 1st stream, 102 meaning 2nd stream....
	int iStreamNR; // the number of private entry, it's awlays not equeal to the number of stream, only for rtsp yet now,
		// (hichip/zavio/onvif : 1; rtsp : the stream number...)
	SysenvIPCPrivData stPrivData[MAX_IPCAM_PROFILE_NR];//1st for main stream(full view), 2st sub stream(divsion view), other network forword only
	char padding[1]; //use to update inform changed , ignore...
	unsigned char reserved; 
}SysenvIPCam;


typedef enum
{
	PT_1ST = 0,
	PT_2ND,
	PT_3RD,
	PT_4TH,
	PT_5TH,
	PT_ALL = 0xFF
}PROFILE_TYPE;//THE INDEX OF STREAM(VENC PROFILE)


//hvr
typedef struct _SysenvHvrGeneral
{
	int nBnc;
}SysenvHvrGeneral;

//#ifdef _NVR


typedef	enum
{
	CHANNEL_DIRECTLINK = 0,
	CHANNEL_SWITCH,
	CHANNEL_WORKMODE_CNT,
}CHANNEL_WORKMODE;
typedef struct _SysenvIPCamGroup
{
	CHANNEL_WORKMODE azWorkMode;
	ENV_BOOLEAN bMultiNetworkSegment;
	SysenvIPCam astIPCam[MAX_CAM_CH];
}SysenvIPCamGroup;
//#endif

typedef struct _SysenvRec
{
	union
	{
		all_rec_condition_t astRecCond[WEEKDAY_CNT];
		struct
		{
			all_rec_condition_t stRecCondSunday;
			all_rec_condition_t stRecCondMonday;
			all_rec_condition_t stRecCondTuesday;
			all_rec_condition_t stRecCondWednesday;
			all_rec_condition_t stRecCondThursday;
			all_rec_condition_t stRecCondFriday;
			all_rec_condition_t stRecCondSaturday;
		};
	};
}SysenvRec;

typedef struct _SysenvNetwork
{
	ENV_BOOLEAN bDHCPUse;
	unsigned char au8MACAddr[MAC_ADDR_LEN];
	unsigned char au8IPAddr[IP_ADDR_LEN];
	unsigned char au8SubMask[IP_ADDR_LEN];
	unsigned char au8Gateway[IP_ADDR_LEN];
	unsigned char au8DNSAddr[IP_ADDR_LEN];
	unsigned short u16Port[0];
	unsigned short u16PortHTTP;
	unsigned short u16PortClient;
	unsigned short u16PortMobile;
	ENV_BOOLEAN bESeeNetUse;

	ENV_BOOLEAN bPPPoE;
	char szPPPoEUser[PPPOE_USER_LEN +1];
	char szPPPoEPwd[PPPOE_PWD_LEN + 1];
	PPPOE_PROTOCOL enPPPOEProtocol;

	ENV_BOOLEAN bDDNSUse;
	int enDDNSProvider;
	char szDDNSUrl[DDNS_URL_LEN + 1];
	char szDDNSUser[DDNS_USER_LEN + 1];
	char szDDNSPassword[DDNS_PWD_LEN + 1];

	ENV_BOOLEAN b3GUse;
	char sz3GAPN[NET_EDITBOX_LEN +1];
	char sz3GPIN[NET_EDITBOX_LEN +1];
	char sz3GUser[NET_EDITBOX_LEN + 1];
	char sz3GPassword[NET_EDITBOX_LEN + 1];
	char sz3GDIALNum[NET_EDITBOX_LEN +1];

	//SMTP
	ENV_BOOLEAN bSMTPUse;
	char szSMTPServer[SMTP_EDITBOX_LEN+1];
	unsigned short u16SMTPPort;
	ENV_BOOLEAN bSMTPAnonymous;
	char szSMTPUser[SMTP_EDITBOX_LEN+1];
	char szSMTPPwd[SMTP_EDITBOX_LEN+1];
	SMTP_ENCRYPT_TYPE enSMTPEncryptType;
	char szSMTPSender[SMTP_EDITBOX_LEN+1];
	char szSMTPSendee1[SMTP_EDITBOX_LEN+1];
	char szSMTPSendee2[SMTP_EDITBOX_LEN+1];
	char szSMTPSubject[SMTP_EDITBOX_LEN+1];
	unsigned short u16SMTPInterval;
	ENV_BOOLEAN bSMTPHealthEnable;
	unsigned short u16SMTPHealthInterval;

	//FTP
	ENV_BOOLEAN bFTPUse;
	char szFTPServer[FTP_EDITBOX_LEN+1];
	unsigned short u16FTPPort;
	ENV_BOOLEAN bFTPAnonymous;
	char szFTPUser[FTP_EDITBOX_LEN+1];
	char szFTPPwd[FTP_EDITBOX_LEN+1];
	char szFTPRemoteFolder[FTP_FOLDER_LEN+1];
	unsigned long u32FTPFileMaxSize;
	SysenvRec stFTPSchedule;
	
	//Online Upgrade
	ENV_BOOLEAN UsePreHost;
	char UpgradePreHost[ONLINE_UPGRADE_HOST_LEN+1];
	char UpgradePreUrl[ONLINE_UPGRADE_URL_LEN+1];
	unsigned short UpgradePrePort;
}SysenvNetwork;

#define  TVSIZE_SCALAR (16)
#define  TVSIZE_STEPER (8)

typedef enum {
	TV_SIZE_LEFT = 0,
	TV_SIZE_RIGHT,
	TV_SIZE_TOP,
	TV_SIZE_BOTTOM,
	TV_SIZE_CNT,
} TvSize_E;

typedef struct _SysenvScreen
{
	char aszTitle[MAX_CAM_CH][CHN_TITLE_LEN + 1];
	int nAlpha;
	ENV_BOOLEAN bAutoSwi;
	ENV_BOOLEAN bSkipLoss;
	AUTOSWI_INTERVAL enAutoSwiInterval;
	AUTOSWI_MODE enAutoSwiMode;
	VGA_RESOLUTION enVGAResolution;
	int nTvSize[TV_SIZE_CNT];
	ENV_BOOLEAN bCVBSEnable;
}SysenvScreen;

typedef struct _CoverAreaRect
{
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
}CoverAreaRect;

typedef struct _SysenvDetection
{
	MD_SENSITIVITY aenMDSensitivity[MAX_CAM_CH];
	ALARM_DURATION aenMDAlarmDuration[MAX_CAM_CH];
	unsigned char abMDArea[VOUT_STANDARD_CNT][MAX_CAM_CH][18][22];
	CoverAreaRect abVCArea[VOUT_STANDARD_CNT][MAX_CAM_CH];
	ENV_BOOLEAN abMDAlarm[MAX_CAM_CH];
	ENV_BOOLEAN abMDBuzzer[MAX_CAM_CH];
	ENV_BOOLEAN abMDSMTP[MAX_CAM_CH];
	ENV_BOOLEAN abMDFTP[MAX_CAM_CH];

	ALARM_DURATION aenVLAlarmDuration[MAX_CAM_CH];
	ENV_BOOLEAN abVLAlarm[MAX_CAM_CH];
	ENV_BOOLEAN abVLBuzzer[MAX_CAM_CH];
	ENV_BOOLEAN abVLSMTP[MAX_CAM_CH];
	ENV_BOOLEAN abVLFTP[MAX_CAM_CH];
}SysenvDetection;

typedef struct _TourPoint
{
	unsigned char u8Preset;
	unsigned char u8Hour;
	unsigned char u8Min;
	unsigned char u8Sec;
}TourPoint;

typedef struct _Tour
{
	int nTourPointCnt;
	TourPoint astTourPount[MAX_TOUR_POINT];
}Tour;

typedef struct _SysenvPTZ
{
	unsigned char au8Id[MAX_CAM_CH];
	PTZ_PROTOCAL aenProtocal[MAX_CAM_CH];
	PTZ_BAUDRATE aenBaudrate[MAX_CAM_CH];
//	PTZ_DATABIT aenDatabit[MAX_CAM_CH];
//	PTZ_STOPBIT aenStopbit[MAX_CAM_CH];
//	PTZ_PARITY aenParity[MAX_CAM_CH];
	Tour astTour[MAX_CAM_CH];
}SysenvPTZ;

typedef struct _SysenvSensor
{
	SENSOR_WORKMODE aenWorkMode[MAX_SENSOR_CH];
	ALARM_DURATION aenAlarmDuration[MAX_SENSOR_CH];	// 101225 law
	ENV_BOOLEAN abRecord[MAX_SENSOR_CH][MAX_CAM_CH];
	ENV_BOOLEAN abAlarm[MAX_SENSOR_CH];
	ENV_BOOLEAN abBuzzer[MAX_SENSOR_CH];
	ENV_BOOLEAN abSMTP[MAX_SENSOR_CH];
	ENV_BOOLEAN abFTP[MAX_SENSOR_CH];
}SysenvSensor;

#ifndef MAX_TELECTRL_CH
#define MAX_TELECTRL_CH (4)
#endif
typedef struct _SysenvTelecontrol
{
	unsigned int anTeleID[MAX_TELECTRL_CH];
	ENV_BOOLEAN abEnable[MAX_TELECTRL_CH];
	ALARM_DURATION aenAlarmDuration[MAX_TELECTRL_CH];
	ENV_BOOLEAN abRecord[MAX_TELECTRL_CH][MAX_CAM_CH];

	ENV_BOOLEAN abAlarm[MAX_TELECTRL_CH];
	ENV_BOOLEAN abBuzzer[MAX_TELECTRL_CH];
	ENV_BOOLEAN abNetworkNotice[MAX_TELECTRL_CH];
}SysenvTelecontrol;

typedef struct _SysenvUps
{
	ENV_BOOLEAN bEnable;
	ALARM_DURATION enAlarmDuration;
	ENV_BOOLEAN bAlarm;
	ENV_BOOLEAN bBuzzer;
	ENV_BOOLEAN bNetworkNotice;
}SysenvUps;

typedef struct _SysenvHDD
{
	ENV_BOOLEAN bOverwrite;
	ENV_BOOLEAN bDeleteOldFiles;
	int nDaysAgo;
}SysenvHDD;

typedef struct _MaintenanceTime
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
}MaintenanceTime;

typedef struct _SysenvMaintenance
{
    ENV_BOOLEAN enable;
    Maintenance_Mode mode;
    int schedule;//ÓÃµÍÆßÎ»À´±íÊ¾Ò»ÖÜÆßÌì
    MaintenanceTime stTime;//²âÊÔÄ£Ê½ÓÃÀ´±£´æÖØÆô¼ä¸ô£¬ÆäËüÄ£Ê½±íÊ¾ÖØÆôµÄÊ±¼äµã
}SysenvMaintenance;

typedef struct _SysenvContent
{
	SysenvColor stColor;
	SysenvGeneral stGeneral;
	SysenvEnc stEncode;
//#ifdef _NVR
	SysenvIPCamGroup stIPCamGroup;
	SysenvHvrGeneral stHvr;
//#endif
	SysenvRec stRecord;
	SysenvNetwork stNetwork;
	SysenvScreen stScreen;
	SysenvDetection stDetection;
	SysenvPTZ stPTZ;
	SysenvSensor stSensor;
	SysenvTelecontrol stTelecontrol;
	SysenvUps stUps;
	SysenvHDD stHDD;
	SysenvMaintenance stMaintenance;
	int res[4];
}SysenvContent;

typedef struct _SysenvAttr
{
	// crc
	unsigned short u16CRC;
	int nSize;
	SysenvContent stContent;
}SysenvAttr;
#pragma	pack()

// 0-config 32k-config-factory 64k-user 96k-user-factory
#ifndef MINGW
#define ENVIRONMENT_DEV "/dev/mtdblock3"
#else
#define ENVIRONMENT_DEV "mtdblock"
#endif
#define ENVIRONMENT_OFFSET (8*1024) // 8k
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define ENVIRONMENT_SIZE (64*1024 - ENVIRONMENT_OFFSET) // 64k
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#define ENVIRONMENT_SIZE (128*1024 - ENVIRONMENT_OFFSET) // 128k
#endif

// system attributions operations
typedef struct _Sysenv
{
	SysenvAttr stAttr;
	char file[100];
	char filefactroy[100];

	// crc
	ENV_BOOLEAN (*IsTheSameAs)(struct _Sysenv* const thiz, struct _Sysenv* const other);
	ENV_BOOLEAN (*Compare)(struct _Sysenv* const thiz, struct _Sysenv* other, const char* item);
	unsigned int (*GetEnvSize)(struct _Sysenv* const thiz);

	// device
	void (*GetDevName)(struct _Sysenv* const thiz, char* ret_szDevName);
	void (*GetDevModel)(struct _Sysenv* const thiz, char* ret_szDevModel);
	void (*GetHWVersion)(struct _Sysenv* const thiz, int* ret_nMaj, int* ret_nMin, int* ret_nRev, char* ret_szExt);
	void (*GetSWVersion)(struct _Sysenv* const thiz, int* ret_nMaj, int* ret_nMin, int* ret_nRev, char* ret_szExt);
	struct tm (*GetRelDateTime)(struct _Sysenv* const thiz);
	unsigned short (*UpdateCRC16)(struct _Sysenv* const thiz);

	// store / restore from file
	int (*Save)(struct _Sysenv* const thiz);
	int (*Load)(struct _Sysenv* const thiz);
	int (*SaveFactory)(struct _Sysenv* const thiz);
	int (*LoadFactory)(struct _Sysenv* const thiz);
	int (*RestoreDefaultColor)(struct _Sysenv* const thiz);
	int (*RestoreDefaultGeneral)(struct _Sysenv* const thiz);
	int (*RestoreDefaultEncode)(struct _Sysenv* const thiz);
	int (*RestoreDefaultIPCam)(struct _Sysenv* const thiz);
	int (*RestoreDefaultHvr)(struct _Sysenv* const thiz);
	int (*RestoreDefaultRecode)(struct _Sysenv* const thiz);
	int (*RestoreDefaultNetwork)(struct _Sysenv* const thiz);
	int (*RestoreDefaultScreen)(struct _Sysenv* const thiz);
	int (*RestoreDefaultMotion)(struct _Sysenv* const thiz);
	int (*RestoreDefaultSensor)(struct _Sysenv* const thiz);
	int (*RestoreDefaultPtz)(struct _Sysenv* const thiz);
	int (*RestoreDefaultTelecontrol)(struct _Sysenv* const thiz);
	int (*RestoreDefaultUps)(struct _Sysenv* const thiz);
	int (*RestoreDefaultHdd)(struct _Sysenv* const thiz);
	int (*RestoreDefault)(struct _Sysenv* const thiz);
	int (*Reset)(struct _Sysenv* const thiz);
	int (*Print)(struct _Sysenv* const thiz);

	// color
	ENV_COLOR_VALUE (*GetHue)(struct _Sysenv* const thiz, int channel);
	void (*SetHue)(struct _Sysenv* const thiz, int channel, ENV_COLOR_VALUE hue);
	ENV_COLOR_VALUE (*GetContrast)(struct _Sysenv* const thiz, int channel);
	void (*SetContrast)(struct _Sysenv* const thiz, int channel, ENV_COLOR_VALUE contrast);
	ENV_COLOR_VALUE (*GetLuminance)(struct _Sysenv* const thiz, int channel);
	void (*SetBrightness)(struct _Sysenv* const thiz, int channel, ENV_COLOR_VALUE luminance);
	ENV_COLOR_VALUE (*GetSaturation)(struct _Sysenv* const thiz, int channel);
	void (*SetSaturation)(struct _Sysenv* const thiz, int channel, ENV_COLOR_VALUE saturation);
	void (*GetDefaultColorValue)(struct _Sysenv* const thiz, int* pHue, int* pContrast, int* pLuminance, int* pSaturation);

	// misc
	DATE_FMT (*GetDateFmt)(struct _Sysenv* const thiz);
	void (*SetDateFmt)(struct _Sysenv* const thiz, DATE_FMT format);
	ENV_BOOLEAN (*IsKeyLock)(struct _Sysenv* const thiz);
	void (*SetKeyLock)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsKeyBuzzer)(struct _Sysenv* const thiz);
	void (*SetKeyBuzzer)(struct _Sysenv* const thiz, ENV_BOOLEAN buzzer);
	WORLDWIDE_LANGUAGE (*GetLang)(struct _Sysenv* const thiz);
	void (*SetLang)(struct _Sysenv* const thiz, WORLDWIDE_LANGUAGE lang);
	VOUT_STANDARD (*GetStandard)(struct _Sysenv* const thiz);
	void (*SetStandard)(struct _Sysenv* const thiz, VOUT_STANDARD standard);
	KEYMAP_TYPE (*GetKey_Type)(struct _Sysenv* const thiz);
	void (*SetKey_Type)(struct _Sysenv* const thiz, KEYMAP_TYPE key_type);
	REMOTE_TYPE (*GetRemote_Type)(struct _Sysenv* const thiz);
	void (*SetRemote_Type)(struct _Sysenv* const thiz, REMOTE_TYPE Remote_type);
	unsigned int (*GetDVRId)(struct _Sysenv* const thiz);
	void (*SetDVRId)(struct _Sysenv* const thiz, unsigned int id);
	void (*SetPersianCalendarUse)(struct _Sysenv* const thiz, ENV_BOOLEAN bPersianCalendar);
	ENV_BOOLEAN (*IsPersianCalendar)(struct _Sysenv* const thiz);
	
	unsigned int (*GetDeviceMode)(struct _Sysenv* const thiz);
	void (*SetDeviceMode)(struct _Sysenv* const thiz, unsigned int index);
	unsigned int (*GetDeviceModeMax)(struct _Sysenv* const thiz);
	void (*SetDeviceModeMax)(struct _Sysenv* const thiz, unsigned int index);
	void (*GetSpecialFTPServerPath)(struct _Sysenv* const thiz, char ret_path[MAX_FTP_SERVER_PATH_LEN+1]);
	void (*SetSpecialFTPServerPath)(struct _Sysenv* const thiz, const char path[MAX_FTP_SERVER_PATH_LEN+1]);

	ENC_POSITION_MODE (*GetTimerPositionCustom)(struct _Sysenv* const thiz);
	void (*SetTimerPositionCustom)(struct _Sysenv* const thiz, ENC_POSITION_MODE positionMode);
	int (*GetTimerXCoordinate)(struct _Sysenv* const thiz);
	void (*SetTimerXCoordinate)(struct _Sysenv* const thiz, int xCoordinate);
	int (*GetTimerYCoordinate)(struct _Sysenv* const thiz);
	void (*SetTimerYCoordinate)(struct _Sysenv* const thiz, int yCoordinate);
	ENC_POSITION_MODE (*GetEseeidPositionCustom)(struct _Sysenv* const thiz);
	void (*SetEseeidPositionCustom)(struct _Sysenv* const thiz, ENC_POSITION_MODE positionMode);
	int (*GetEseeidXCoordinate)(struct _Sysenv* const thiz);
	void (*SetEseeidXCoordinate)(struct _Sysenv* const thiz, int xCoordinate);
	int (*GetEseeidYCoordinate)(struct _Sysenv* const thiz);
	void (*SetEseeidYCoordinate)(struct _Sysenv* const thiz, int yCoordinate);
	void (*GetSupportWeb)(struct _Sysenv* const thiz, char ret_web[SUPPORT_WEB_LINK_LEN]);
	void (*SetSupportWeb)(struct _Sysenv* const thiz, const char web[SUPPORT_WEB_LINK_LEN]);
	void (*GetAndroidLink)(struct _Sysenv* const thiz, char ret_link[SUPPORT_WEB_LINK_LEN]);
	void (*SetAndroidLink)(struct _Sysenv* const thiz, const char link[SUPPORT_WEB_LINK_LEN]);
	void (*GetIOSLink)(struct _Sysenv* const thiz, char ret_link[SUPPORT_WEB_LINK_LEN]);
	void (*SetIOSLink)(struct _Sysenv* const thiz, const char link[SUPPORT_WEB_LINK_LEN]);
//timer&eseeid width
	int (*GetEseeidWidth)(struct _Sysenv* const thiz);
	void (*SetEseeidWidth)(struct _Sysenv* const thiz, int sum_width);
	int (*GetTimerWidth)(struct _Sysenv* const thiz);
	void (*SetTimerWidth)(struct _Sysenv* const thiz, int tsum_width);
	void (*SetTimeUse)(struct _Sysenv* const thiz, ENV_BOOLEAN bTimeUse);
	ENV_BOOLEAN (*IsTimeUse)(struct _Sysenv* const thiz);

	int  (*GetChannelModeAuto)(struct _Sysenv* const thiz);
	void (*SetChannelModeAuto)(struct _Sysenv* const thiz, int Value);
	int  (*GetChannelModes)(struct _Sysenv* const thiz, int Chnl);
	void (*SetChannelModes)(struct _Sysenv* const thiz, int Chnl, int Value);

	TIME_FORMAT (*GetSysTimeFmt)(struct _Sysenv* const thiz);
	void (*SetSysTimeFmt)(struct _Sysenv* const thiz, TIME_FORMAT fmt);


	// encode
	ENC_MODE (*GetEncMode)(struct _Sysenv* const thiz, int channel);
	void (*SetEncMode)(struct _Sysenv* const thiz, int channel, ENC_MODE mode);
	ENC_FMT (*GetEncFmt)(struct _Sysenv* const thiz, int channel);
	void (*SetEncFmt)(struct _Sysenv* const thiz, int channel, ENC_FMT format);
	ENC_PIC_LEVEL (*GetEncPicLevel)(struct _Sysenv* const thiz, int channel);
	void (*SetEncPicLevel)(struct _Sysenv* const thiz, int channel, ENC_PIC_LEVEL level);
	ENC_BR_MODE (*GetBitrateMode)(struct _Sysenv* const thiz, int channel);
	void (*SetBitrateMode)(struct _Sysenv* const thiz, int channel, ENC_BR_MODE mode);
	ENC_BR_VALUE (*GetBitrateValue)(struct _Sysenv* const thiz, int channel);
	void (*SetBitrateValue)(struct _Sysenv* const thiz, int channel, ENC_BR_VALUE bitrate);
	ENC_FRAMERATE (*GetEncFramerate)(struct _Sysenv* const thiz, int channel);
	void (*SetEncFramerate)(struct _Sysenv* const thiz, int channel, ENC_FRAMERATE framerate);

	ENC_MODE (*GetEncSubMode)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetEncSubMode)(struct _Sysenv* const thiz, int channel, int sub, ENC_MODE mode);
	ENC_FMT (*GetEncSubFmt)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetEncSubFmt)(struct _Sysenv* const thiz, int channel, int sub, ENC_FMT format);
	ENC_PIC_LEVEL (*GetEncSubPicLevel)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetEncSubPicLevel)(struct _Sysenv* const thiz, int channel, int sub, ENC_PIC_LEVEL level);
	ENC_BR_MODE (*GetSubBitrateMode)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetSubBitrateMode)(struct _Sysenv* const thiz, int channel, int sub, ENC_BR_MODE mode);
	ENC_BR_VALUE (*GetSubBitrateValue)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetSubBitrateValue)(struct _Sysenv* const thiz, int channel, int sub, ENC_BR_VALUE bitrate);
	ENC_FRAMERATE (*GetEncSubFramerate)(struct _Sysenv* const thiz, int channel, int sub);
	void (*SetEncSubFramerate)(struct _Sysenv* const thiz, int channel, int sub, ENC_FRAMERATE framerate);
	ENV_BOOLEAN (*IsFmtManualSwitch)(struct _Sysenv* const thiz);
	void (*SetFmtManualSwitch)(struct _Sysenv* const thiz, ENV_BOOLEAN bManual);
	
	// ipcam
	void (*GetIPCamAddr)(struct _Sysenv* const thiz, int channel, unsigned char* ret_ipaddr);
	void (*SetIPCamAddr)(struct _Sysenv* const thiz, int channel, unsigned char* ipaddr);
	unsigned short (*GetIPCamPort)(struct _Sysenv* const thiz, int channel);
	void (*SetIPCamPort)(struct _Sysenv* const thiz, int channel, unsigned short port);
	IPCAM_PROTOCOL_NAME (*GetIPCamProtocolname)(struct _Sysenv* const thiz, int channel);
	void (*SetIPCamProtocolname)(struct _Sysenv* const thiz, int channel, IPCAM_PROTOCOL_NAME protocol_name);
	void (*GetIPCamUsername)(struct _Sysenv* const thiz, int channel, const char* ret_username);
	void (*SetIPCamUsername)(struct _Sysenv* const thiz, int channel, char* username);
	void (*GetIPCamPassword)(struct _Sysenv* const thiz, int channel, const char* ret_password);
	void (*SetIPCamPassword)(struct _Sysenv* const thiz, int channel, char* password);
	void (*GetIPCamModelname)(struct _Sysenv* const thiz, int channel, const char* ret_modelname);
	void (*SetIPCamModelname)(struct _Sysenv* const thiz, int channel, char* modelname);
	ENV_BOOLEAN (*IsEnableIPCChannel)(struct _Sysenv* const thiz, int channel);
	void (*SetEnableIPCChannel)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	void (*GetIPCMACAddr)(struct _Sysenv* const thiz,int channel, unsigned char ret_mac[MAC_ADDR_LEN]);
	void (*SetIPCMACAddr)(struct _Sysenv* const thiz,int channel, const unsigned char mac[MAC_ADDR_LEN]);
	int (*GetIPCamStreamNR)(struct _Sysenv* const thiz, int channel);
	void (*SetIPCamStreamNR)(struct _Sysenv* const thiz, int channel, int nr);
	void (*GetIPCamPrivData)(struct _Sysenv* const thiz,int channel, int stream, void *ret_buf,int ret_size);
	void (*SetIPCamPrivData)(struct _Sysenv* const thiz,int channel, int stream, void *data,int size);	
	void (*GetIPCRtspStream)(struct _Sysenv* const thiz,int channel,char *m_stream,char *s_stream,int *m_tran,int *s_tran);
	void (*SetIPCRtspStream)(struct _Sysenv* const thiz,int channel,char *m_stream,char *s_stream,int m_tran,int s_tran);
	CHANNEL_WORKMODE (*GetChannelWorkMode)(struct _Sysenv* const thiz);
	void (*SetChannelWorkMode)(struct _Sysenv* const thiz, CHANNEL_WORKMODE mode);
	ENV_BOOLEAN (*IsMultiNetworkSegment)(struct _Sysenv* const thiz);	
	void (*SetIsMultiNetworkSegment)(struct _Sysenv* const thiz, ENV_BOOLEAN set);	
	IPCAM_STREAM_CHOISE (*GetIPCamStreamChoise)(struct _Sysenv* const thiz, int chn);
	 void (*SetIPCamStreamChoise)(struct _Sysenv* const thiz, int chn, IPCAM_STREAM_CHOISE choise);	 
	 void (*UpdateIPcam)(struct _Sysenv * const thiz, struct _Sysenv * target);
	 int (*GetIPCamStreamIndex)(struct _Sysenv * const thiz, int chn, int stream);
	 void (*SetIPCamStreamIndex)(struct _Sysenv * const thiz, int chn, int stream, int index);

	//hvr
	void (*SetHvrBncChn)(struct _Sysenv * const thiz,int nbnc);
	int (*GetHvrBncChn)(struct _Sysenv * const thiz);

	// record
	all_rec_condition_t (*GetSchedule)(struct _Sysenv* const thiz, WEEKDAY weekday);
	void (*SetSchedule)(struct _Sysenv* const thiz, WEEKDAY weekday, int channel, int item, rec_condition_t cond);

	// network
	ENV_BOOLEAN (*IsDHCPUse)(struct _Sysenv* const thiz);
	void (*SetDHCPUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*GetMACAddr)(struct _Sysenv* const thiz, unsigned char ret_mac[MAC_ADDR_LEN]);
	void (*SetMACAddr)(struct _Sysenv* const thiz, const unsigned char mac[MAC_ADDR_LEN]);
	void (*GetIPAddr)(struct _Sysenv* const thiz, unsigned char ret_ip[IP_ADDR_LEN]);
	void (*SetIPAddr)(struct _Sysenv* const thiz, const unsigned char ret_ip[IP_ADDR_LEN]);
	void (*GetSubMask)(struct _Sysenv* const thiz, unsigned char ret_ip[IP_ADDR_LEN]);
	void (*SetSubMask)(struct _Sysenv* const thiz, const unsigned char ret_ip[IP_ADDR_LEN]);
	void (*GetGateway)(struct _Sysenv* const thiz, unsigned char ret_ip[IP_ADDR_LEN]);
	void (*SetGateway)(struct _Sysenv* const thiz, const unsigned char ret_ip[IP_ADDR_LEN]);
	void (*GetDNSAddr)(struct _Sysenv* const thiz, unsigned char ret_dns[IP_ADDR_LEN]);
	void (*SetDNSAddr)(struct _Sysenv* const thiz, const unsigned char dns[IP_ADDR_LEN]);
	unsigned short (*GetHTTPPort)(struct _Sysenv* const thiz);
	void (*SetHTTPPort)(struct _Sysenv* const thiz, unsigned short port);
	unsigned short (*GetClientPort)(struct _Sysenv* const thiz);
	void (*SetClientPort)(struct _Sysenv* const thiz, unsigned short port);
	unsigned short (*GetMobilePort)(struct _Sysenv* const thiz);
	void (*SetMobilePort)(struct _Sysenv* const thiz, unsigned short port);
	ENV_BOOLEAN (*IsESeeNetUse)(struct _Sysenv* const thiz);
	void (*SetESeeNetUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsDDNSUse)(struct _Sysenv* const thiz);
	void (*SetDDNSUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	int (*GetDDNSProvider)(struct _Sysenv* const thiz);
	void (*SetDDNSProvider)(struct _Sysenv* const thiz, int provider);
	void (*GetDDNSUrl)(struct _Sysenv* const thiz, char ret_url[DDNS_URL_LEN]);
	void (*SetDDNSUrl)(struct _Sysenv* const thiz, const char url[DDNS_URL_LEN]);
	void (*GetDDNSUser)(struct _Sysenv* const thiz, char ret_user[DDNS_URL_LEN]);
	void (*SetDDNSUser)(struct _Sysenv* const thiz, const char user[DDNS_URL_LEN]);
	void (*GetDDNSPwd)(struct _Sysenv* const thiz, char ret_pwd[DDNS_URL_LEN]);
	void (*SetDDNSPwd)(struct _Sysenv* const thiz, const char pwd[DDNS_URL_LEN]);
	ENV_BOOLEAN (*IsPPPoE)(struct _Sysenv* const thiz);
	void (*SetPPPoE)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*GetPPPoEUser)(struct _Sysenv* const thiz, char ret_user[DDNS_URL_LEN]);
	void (*SetPPPoEUser)(struct _Sysenv* const thiz, const char user[DDNS_URL_LEN]);
	void (*GetPPPoEPwd)(struct _Sysenv* const thiz, char ret_pwd[DDNS_URL_LEN]);
	void (*SetPPPoEPwd)(struct _Sysenv* const thiz, const char pwd[DDNS_URL_LEN]);
	PPPOE_PROTOCOL (*GetPPPOEProtocol)(struct _Sysenv* const thiz);
	void (*SetPPPOEProtocol)(struct _Sysenv* const thiz, PPPOE_PROTOCOL protocol);
	//guide	
	ENV_BOOLEAN (*IsGuideUse)(struct _Sysenv* const thiz);	
	void (*SetIsGuideUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsEseeidUse)(struct _Sysenv* const thiz);	
	void (*SetIsEseeidUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	

	//CVBS
	ENV_BOOLEAN (*IsCVBSEnable)(struct _Sysenv* const thiz);
	void (*SetCVBSEnable)(struct _Sysenv* const thiz, ENV_BOOLEAN set);

	ENV_BOOLEAN (*Is3GUse)(struct _Sysenv* const thiz);
	void (*Set3GUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*Get3GAPN)(struct _Sysenv* const thiz, char ret_APN[NET_EDITBOX_LEN]);
	void (*Set3GAPN)(struct _Sysenv* const thiz, const char APN[NET_EDITBOX_LEN]);
	void (*Get3GPIN)(struct _Sysenv* const thiz, char ret_PIN[NET_EDITBOX_LEN]);
	void (*Set3GPIN)(struct _Sysenv* const thiz, const char ret_PIN[NET_EDITBOX_LEN]);
	void (*Get3GUser)(struct _Sysenv* const thiz, char ret_user[NET_EDITBOX_LEN]);
	void (*Set3GUser)(struct _Sysenv* const thiz, const char user[NET_EDITBOX_LEN]);
	void (*Get3GPwd)(struct _Sysenv* const thiz, char ret_pwd[NET_EDITBOX_LEN]);
	void (*Set3GPwd)(struct _Sysenv* const thiz, const char pwd[NET_EDITBOX_LEN]);
	void (*Get3GDIALNum)(struct _Sysenv* const thiz,char ret_DIALNum [ NET_EDITBOX_LEN ]);
	void (*Set3GDIALNum)(struct _Sysenv* const thiz, const char DIALNum[NET_EDITBOX_LEN]);
	//Network -> SMTP
	ENV_BOOLEAN (*IsSMTPUse)(struct _Sysenv* const thiz);
	void (*SetSMTPUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*GetSMTPServer)(struct _Sysenv* const thiz, char ret_server[SMTP_EDITBOX_LEN]);
	void (*SetSMTPServer)(struct _Sysenv* const thiz, const char server[SMTP_EDITBOX_LEN]);
	unsigned short (*GetSMTPPort)(struct _Sysenv* const thiz);
	void (*SetSMTPPort)(struct _Sysenv* const thiz, unsigned short port);	
	ENV_BOOLEAN (*IsSMTPAnonymous)(struct _Sysenv* const thiz);
	void (*SetSMTPAnonymous)(struct _Sysenv* const thiz, ENV_BOOLEAN anonymous);	
	void (*GetSMTPUser)(struct _Sysenv* const thiz, char ret_user[SMTP_EDITBOX_LEN]);
	void (*SetSMTPUser)(struct _Sysenv* const thiz, const char user[SMTP_EDITBOX_LEN]);	
	void (*GetSMTPPwd)(struct _Sysenv* const thiz, char ret_pwd[SMTP_EDITBOX_LEN]);
	void (*SetSMTPPwd)(struct _Sysenv* const thiz, const char pwd[SMTP_EDITBOX_LEN]);		
	SMTP_ENCRYPT_TYPE (*GetSMTPEncryptType)(struct _Sysenv* const thiz);
	void (*SetSMTPEncryptType)(struct _Sysenv* const thiz, SMTP_ENCRYPT_TYPE type);
	void (*GetSMTPSender)(struct _Sysenv* const thiz, char ret_sender[SMTP_EDITBOX_LEN]);
	void (*SetSMTPSender)(struct _Sysenv* const thiz, const char sender[SMTP_EDITBOX_LEN]);			
	void (*GetSMTPSendee1)(struct _Sysenv* const thiz, char ret_sendee1[SMTP_EDITBOX_LEN]);
	void (*SetSMTPSendee1)(struct _Sysenv* const thiz, const char sendee1[SMTP_EDITBOX_LEN]);				
	void (*GetSMTPSendee2)(struct _Sysenv* const thiz, char ret_sendee2[SMTP_EDITBOX_LEN]);
	void (*SetSMTPSendee2)(struct _Sysenv* const thiz, const char sendee2[SMTP_EDITBOX_LEN]);				
	void (*GetSMTPSubject)(struct _Sysenv* const thiz, char ret_subject[SMTP_EDITBOX_LEN]);
	void (*SetSMTPSubject)(struct _Sysenv* const thiz, const char subject[SMTP_EDITBOX_LEN]);				
	unsigned short (*GetSMTPInterval)(struct _Sysenv* const thiz);
	void (*SetSMTPInterval)(struct _Sysenv* const thiz, unsigned short interval);		
	ENV_BOOLEAN (*IsSMTPHealthEnable)(struct _Sysenv* const thiz);
	void (*SetSMTPHealthEnable)(struct _Sysenv* const thiz, ENV_BOOLEAN enable);		
	unsigned short (*GetSMTPHealthInterval)(struct _Sysenv* const thiz);
	void (*SetSMTPHealthInterval)(struct _Sysenv* const thiz, unsigned short interval);
	// Network --> FTP
	ENV_BOOLEAN (*IsFTPUse)(struct _Sysenv* const thiz);
	void (*SetFTPUse)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*GetFTPServer)(struct _Sysenv* const thiz, char ret_server[FTP_EDITBOX_LEN]);
	void (*SetFTPServer)(struct _Sysenv* const thiz, const char server[FTP_EDITBOX_LEN]);
	unsigned short (*GetFTPPort)(struct _Sysenv* const thiz);
	void (*SetFTPPort)(struct _Sysenv* const thiz, unsigned short port);	
	ENV_BOOLEAN (*IsFTPAnonymous)(struct _Sysenv* const thiz);
	void (*SetFTPAnonymous)(struct _Sysenv* const thiz, ENV_BOOLEAN anonymous);	
	void (*GetFTPUser)(struct _Sysenv* const thiz, char ret_user[FTP_EDITBOX_LEN]);
	void (*SetFTPUser)(struct _Sysenv* const thiz, const char user[FTP_EDITBOX_LEN]);	
	void (*GetFTPPwd)(struct _Sysenv* const thiz, char ret_pwd[FTP_EDITBOX_LEN]);
	void (*SetFTPPwd)(struct _Sysenv* const thiz, const char pwd[FTP_EDITBOX_LEN]);		
	void (*GetFTPRemoteFolder)(struct _Sysenv* const thiz, char ret_folder[FTP_FOLDER_LEN]);
	void (*SetFTPRemoteFolder)(struct _Sysenv* const thiz, const char folder[FTP_FOLDER_LEN]);	
	unsigned long (*GetFTPFileMaxSize)(struct _Sysenv* const thiz);
	void (*SetFTPFileMaxSize)(struct _Sysenv* const thiz, unsigned long size);		
	// Network --> FTP --> FTPSchedule
	all_rec_condition_t (*GetFTPSchedule)(struct _Sysenv* const thiz, WEEKDAY weekday);
	void (*SetFTPSchedule)(struct _Sysenv* const thiz, WEEKDAY weekday, int channel, int item, rec_condition_t cond);
	//Online Upgrade
	ENV_BOOLEAN (*IsUsePreHost)(struct _Sysenv* const thiz);
	void (*SetUsePreHost)(struct _Sysenv* const thiz, ENV_BOOLEAN Use);
	void (*GetUpgradePreHost)(struct _Sysenv* const thiz, char ret_Host[ONLINE_UPGRADE_HOST_LEN]);
	void (*SetUpgradePreHost)(struct _Sysenv* const thiz, const char Host[ONLINE_UPGRADE_HOST_LEN]);
	void (*GetUpgradePreUrl)(struct _Sysenv* const thiz, char ret_Url[ONLINE_UPGRADE_URL_LEN]);
	void (*SetUpgradePreUrl)(struct _Sysenv* const thiz, const char Url[ONLINE_UPGRADE_URL_LEN]);
	unsigned short (*GetUpgradePrePort)(struct _Sysenv* const thiz);
	void (*SetUpgradePrePort)(struct _Sysenv* const thiz, unsigned short Port);
	
	// screen
	void (*GetTitle)(struct _Sysenv* const thiz, int channel, char title[CHN_TITLE_LEN + 1]);
	const char* (*GetTitleEx)(struct _Sysenv* const thiz, int channel);
	void (*SetTitle)(struct _Sysenv* const thiz, int channel, char title[CHN_TITLE_LEN + 1]);
	int (*GetAlpha)(struct _Sysenv* const thiz);
	void (*SetAlpha)(struct _Sysenv* const thiz, int alpha);
	ENV_BOOLEAN (*IsAutoSwi)(struct _Sysenv* const thiz);
	ENV_BOOLEAN (*IsSkipLoss)(struct _Sysenv* const thiz);
	void (*SetAutoSwi)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	void (*SetSkipLoss)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	AUTOSWI_INTERVAL (*GetAutoSwiInterval)(struct _Sysenv* const thiz);
	void (*SetAutoSwiInterval)(struct _Sysenv* const thiz, AUTOSWI_INTERVAL interval);
	AUTOSWI_MODE (*GetAutoSwiMode)(struct _Sysenv* const thiz);
	void (*SetAutoSwiMode)(struct _Sysenv* const thiz, AUTOSWI_MODE mode);	
	VGA_RESOLUTION (*GetVGAResolution)(struct _Sysenv* const thiz);
	void (*SetVGAResolution)(struct _Sysenv* const thiz, VGA_RESOLUTION resolution);
	int (*GetTvSize)(struct _Sysenv* const thiz, TvSize_E nType);
	void (*SetTvSize)(struct _Sysenv* const thiz, TvSize_E nType, int nVal);

	// detection
	MD_SENSITIVITY (*GetMDSensitivity)(struct _Sysenv* const thiz, int channel);
	void (*SetMDSensitivity)(struct _Sysenv* const thiz, int channel, MD_SENSITIVITY sensitivity);
	ALARM_DURATION (*GetMDAlarmDuration)(struct _Sysenv* const thiz, int channel);
	void (*SetMDAlarmDuration)(struct _Sysenv* const thiz, int channel, ALARM_DURATION duration);
	void (*GetMDArea)(struct _Sysenv* const thiz, int channel, unsigned char area[18][22]);
	void (*SetMDArea)(struct _Sysenv* const thiz, int channel, unsigned char area[18][22]);
	void (*GetVCArea)(struct _Sysenv* const thiz, int channel, CoverAreaRect *ret_area);
	void (*SetVCArea)(struct _Sysenv* const thiz, int channel, CoverAreaRect *ret_area);
	ENV_BOOLEAN (*IsMDAlarm)(struct _Sysenv* const thiz, int channel);
	void (*SetMDAlarm)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsMDBuzzer)(struct _Sysenv* const thiz, int channel);
	void (*SetMDBuzzer)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsMDSMTP)(struct _Sysenv* const thiz, int channel);
	void (*SetMDSMTP)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsMDFTP)(struct _Sysenv* const thiz, int channel);
	void (*SetMDFTP)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ALARM_DURATION (*GetVLAlarmDuration)(struct _Sysenv* const thiz, int channel);
	void (*SetVLAlarmDuration)(struct _Sysenv* const thiz, int channel, ALARM_DURATION duration);
	ENV_BOOLEAN (*IsVLAlarm)(struct _Sysenv* const thiz, int channel);
	void (*SetVLAlarm)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsVLBuzzer)(struct _Sysenv* const thiz, int channel);
	void (*SetVLBuzzer)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsVLSMTP)(struct _Sysenv* const thiz, int channel);
	void (*SetVLSMTP)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsVLFTP)(struct _Sysenv* const thiz, int channel);
	void (*SetVLFTP)(struct _Sysenv* const thiz, int channel, ENV_BOOLEAN set);

	// ptz
	unsigned char (*GetPTZId)(struct _Sysenv* const thiz, int ptz_channel);
	void (*SetPTZId)(struct _Sysenv* const thiz, int ptz_channel, unsigned char id);
	PTZ_PROTOCAL (*GetPTZProtocal)(struct _Sysenv* const thiz, int ptz_channel);
	void (*SetPTZProtocal)(struct _Sysenv* const thiz, int ptz_channel, PTZ_PROTOCAL protocal);
	PTZ_BAUDRATE (*GetPTZBaudrate)(struct _Sysenv* const thiz, int ptz_channel);
	void (*SetPTZBaudrate)(struct _Sysenv* const thiz, int ptz_channel, PTZ_BAUDRATE baudrate);
//	PTZ_DATABIT (*GetPTZDatabit)(struct _Sysenv* const thiz, int ptz_channel);
//	void (*SetPTZDatabit)(struct _Sysenv* const thiz, int ptz_channel, PTZ_DATABIT datebit);
//	PTZ_STOPBIT (*GetPTZStopbit)(struct _Sysenv* const thiz, int ptz_channel);
//	void (*SetPTZStopbit)(struct _Sysenv* const thiz, int ptz_channel, PTZ_STOPBIT stopbit);
//	PTZ_PARITY (*GetPTZParity)(struct _Sysenv* const thiz, int ptz_channel);
//	void (*SetPTZParity)(struct _Sysenv* const thiz, int ptz_channel, PTZ_PARITY parity);
	Tour (*GetPTZTour)(struct _Sysenv* const thiz, int ptz_channel);
	void (*SetPTZTour)(struct _Sysenv* const thiz, int ptz_channel, Tour tour);
	void (*PrintPTZTour)(struct _Sysenv* const thiz, int ptz_channel);

	// sensor
	SENSOR_WORKMODE (*GetSensorWorkMode)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorWorkMode)(struct _Sysenv* const thiz, int sensor, SENSOR_WORKMODE mode);
	ALARM_DURATION (*GetSensorAlarmDuration)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorAlarmDuration)(struct _Sysenv* const thiz, int sensor, ALARM_DURATION duration);
	ENV_BOOLEAN (*IsSensorCamRec)(struct _Sysenv* const thiz, int sensor, int camera);
	void (*SetSensorCamRec)(struct _Sysenv* const thiz, int sensor, int camera, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsSensorAlarm)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorAlarm)(struct _Sysenv* const thiz, int sensor, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsSensorBuzzer)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorBuzzer)(struct _Sysenv* const thiz, int sensor, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsSensorSMTP)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorSMTP)(struct _Sysenv* const thiz, int sensor, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsSensorFTP)(struct _Sysenv* const thiz, int sensor);
	void (*SetSensorFTP)(struct _Sysenv* const thiz, int sensor, ENV_BOOLEAN set);


	// telecontrol
	unsigned int (*GetTelecontrolId)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolId)(struct _Sysenv* const thiz, int telecontrol, unsigned int id);
	ENV_BOOLEAN (*IsTelecontrolEnable)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolEnable)(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable);
	ALARM_DURATION (*GetTelecontrolAlarmDuration)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolAlarmDuration)(struct _Sysenv* const thiz, int telecontrol, ALARM_DURATION duration);
	ENV_BOOLEAN (*IsTelecontrolRecord)(struct _Sysenv* const thiz, int telecontrol, int camera);
	void (*SetTelecontrolRecord)(struct _Sysenv* const thiz, int telecontrol, int camera, ENV_BOOLEAN record);
	ENV_BOOLEAN (*IsTelecontrolAlarm)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolAlarm)(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable);
	ENV_BOOLEAN (*IsTelecontrolBuzzer)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolBuzzer)(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable);
	ENV_BOOLEAN (*IsTelecontrolNetworkNotice)(struct _Sysenv* const thiz, int telecontrol);
	void (*SetTelecontrolNetworkNotice)(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable);

	// ups
	ENV_BOOLEAN (*IsUpsEnable)(struct _Sysenv* const thiz);
	void (*SetUpsEnable)(struct _Sysenv* const thiz, ENV_BOOLEAN enable);
	ALARM_DURATION (*GetUpsAlarmDuration)(struct _Sysenv* const thiz);
	void (*SetUpsAlarmDuration)(struct _Sysenv* const thiz, ALARM_DURATION duration);
	ENV_BOOLEAN (*IsUpsAlarm)(struct _Sysenv* const thiz);
	void (*SetUpsAlarm)(struct _Sysenv* const thiz, ENV_BOOLEAN enable);
	ENV_BOOLEAN (*IsUpsBuzzer)(struct _Sysenv* const thiz);
	void (*SetUpsBuzzer)(struct _Sysenv* const thiz, ENV_BOOLEAN enable);
	ENV_BOOLEAN (*IsUpsNetworkNotice)(struct _Sysenv* const thiz);
	void (*SetUpsNetworkNotice)(struct _Sysenv* const thiz, ENV_BOOLEAN enable);

	// hdd
	ENV_BOOLEAN (*IsHDDOverwrite)(struct _Sysenv* const thiz);
	void (*SetHDDOverwrite)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	ENV_BOOLEAN (*IsHDDDeleteOldFiles)(struct _Sysenv* const thiz);
	void (*SetHDDDeleteOldFiles)(struct _Sysenv* const thiz, ENV_BOOLEAN set);
	int (*GetHDDDaysAgo)(struct _Sysenv* const thiz);
	void (*SetHDDDaysAgo)(struct _Sysenv* const thiz, int set);

	//maintenance
    ENV_BOOLEAN (*IsMaintenance)(struct _Sysenv* const thiz);
    Maintenance_Mode (*GetMaintenanceMode)(struct _Sysenv* const thiz);
    void (*SetMaintenanceMode)(struct _Sysenv* const thiz, Maintenance_Mode mode);
    void (*GetMaintenanceSchedule)(struct _Sysenv* const thiz, int* schedule);
    void (*SetMaintenanceSchedule)(struct _Sysenv* const thiz, int schedule);
    void (*GetMaintenanceDate)(struct _Sysenv* const thiz, int *year, int *month, int *day);
    void (*SetMaintenanceDate)(struct _Sysenv* const thiz, int year, int month, int day);
    void (*GetMaintenanceTime)(struct _Sysenv* const thiz, int *hour, int *min, int *sec);
    void (*SetMaintenanceTime)(struct _Sysenv* const thiz, int hour, int min, int sec);
}Sysenv;

#define	PSYSTEMENV(ptr)	((const Sysenv*)ptr)


extern Sysenv* PSysenvStruct(const char* pathFile, const char* pathFileFactory, void* arg);
extern int PSysenvDestruct(Sysenv* thiz, void* arg);

extern struct tm GetBuildDateTime();

extern int SetIrdaBuzzer(int SetOn);
extern ENV_BOOLEAN IsEffioSupport(void);

extern int Plat_CPU_Check(void);

#ifdef	__cplusplus
}
#endif
#endif	//__ENVIRONMENT_H__


