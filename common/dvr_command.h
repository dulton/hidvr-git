


#ifndef __DVR_COMMAND_H__
#define __DVR_COMMAND_H__

#include <time.h>
#include "conf.h"
#include "JaDaemon.h"
#include "environment.h"
#ifndef REC_SEG_FILE
#define REC_SEG_FILE	"/tmp/search.dat"
#endif

#ifdef __cplusplus
extern	"C"	{
#endif

#define	DVR_CMD_FLAG	(0xfeeffaceUL)

typedef enum
{
	VIDEO_NULL = -1,
	VIDEO_SYNC,
	VIDEO_SWITCH,
	VIDEO_SWAP,
	VIDEO_AUTO_SWITCH,
	VIDEO_SET_COLOR,
	VIDEO_GET_COLOR,
	VIDEO_SET_ALPHA,
	VIDEO_GET_ALPHA,
	VIDEO_POWER,
	VIDEO_DIGITAL_ZOOM,
	VIDEO_TV_ADJUST,
	VIDEO_CTRL_SUBTYPE_CNT,
}VIDEO_CTRL_SUBTYPE;


typedef struct VideoCtrl
{
	VIDEO_CTRL_SUBTYPE enSubType;
	union
	{
		struct
		{
			unsigned char bStart;
			unsigned char bStop;
			unsigned char bReset;
		};
		struct
		{
			int nDivi;
			int nPage;
			int bSkipLoss;
			int voChn1;
			int voChn2;
		};
		struct
		{
			int nChannel;
			int nHue;
			int nBrightness;
			int nContrast;
			int nSaturation;
			int nSharpness;
		};
		// alpha
		struct
		{
			unsigned u8Alpha;
		};
		// output
		struct
		{
			unsigned char bPower;
		};
		// digital zoom
		struct
		{
			int nChn;
			int nRatioX;
			int nRatioY;
			int nRatioW;
			int nRatioH;
		};
		// tv adjust
		struct
		{
			int nPosX;
			int nPosY;
			int nWidth;
			int nHeight;
		};
	};
}VideoCtrl_t;

#define SEARCH_RESULT_DATA "/tmp/search.dat"
#define SEARCH_RESULT_IPCINFO "/tmp/IPCInfo.dat"
#ifndef	DECLARE_ST_REC_SEGMENT_COND
#define DECLARE_ST_REC_SEGMENT_COND
#define ST_REC_SEGMENT_COND_1K (1024)
#define ST_REC_SEGMENT_COND_1M (1048576)
#define ST_REC_SEGMENT_COND_1G (1073741824)
#define ST_REC_SEGMENT_COND_1T (1099511627776LL)
typedef struct _ST_REC_SEGMENT_COND
{
	int disk;
	int seg;
	int chn;
	time_t time_start;
	time_t time_end;
	int type;
	long long frame_total_size;
}ST_REC_SEGMENT_COND;
#endif

// 100929 law
typedef enum
{
	DISK_STATUS_NOT_FORMAT_YET = 0,	//未分区，未格式化
	DISK_STATUS_PARTITION_NUM_OK, 	//分区正确，未对分区进行格式化
	DISK_STATUS_FORMAT_YET,			//分区正确，格式化完毕
	DISK_STATUS_DATABASE_ERROR,		//发现数据库格式错误
	DISK_STATUS_DRIVER_ERROR,		//发现驱动传递的错误
	DISK_STATUS_READY_FOR_WORK,	 	//已正确挂载
}DISK_STATUS;
typedef	struct	HDDInfo
{
	int bHDDExist; //0 不存在 1 存在
	DISK_STATUS enStatus;
	unsigned int nTotalSize;	// 硬盘容量总和，单位G
	unsigned int nUsedSize;	// 硬盘以已用空间，单位G
	unsigned int nFreeSize;	// 硬盘剩余空间，单位G
	char szHddName[64]; // 硬盘名称
}ONEHDDInfo_t;

typedef struct
{
	int nHDDCnt;	// 硬盘数量
	int bHDDFull;
	ONEHDDInfo_t stDiskInfo[MAX_HDD_CNT];
	int bIsDeleteOldFiles;
	int nDaysAgo;
}HDDInfo_t;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// what's blew is added by law for record communication
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


typedef	enum
{
	REC_CTRL_NULL = -1,

	REC_CTRL_GETSTATE,
	REC_CTRL_REQ_MANUL_START_OR_STOP,
}REC_CTRL_SUBTYPE;

typedef enum
{
	NO_RECORDING = 0,
	TIME_RECORDING,
	MOTION_RECORDING,
	SENSOR_RECORDING,
	MANUAL_RECORDING,
}REC_CTRL_REC_TYPE;


typedef	struct RecCtrl
{
	REC_CTRL_SUBTYPE enSubType;
	REC_CTRL_REC_TYPE enRecType[MAX_CAM_CH];
	int bRecording[MAX_CAM_CH];
}RecCtrl_t;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// what's blew is added by law for tde control
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
	TDE_CTRL_NULL = -1,
	TDE_ZOOM_IN,
	TDE_ZOOM_OUT,
}TDE_CTRL_SUBTYPE;

typedef struct _TdeCtrl
{
	TDE_CTRL_SUBTYPE enSubType;
}TdeCtrl_t;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


typedef struct _LogoReload
{
	int bUSBExist;
	char szVGAFile[32];
	int nVGASize;
	char szCVBSFile[32];
	int nCVBSSize;
}LogoReload;

typedef enum
{
	RESOURCE_RET_USB_NOT_FOUND,
	RESOURCE_RET_FILE_NOT_FOUND,
	RESOURCE_RET_SIZE_ERROR,
	RESOURCE_RET_CRC_ERROR,
	RESOURCE_RET_OK,
	RESOURCE_RET_UNKNOWN,
	RESOURCE_RET_CNT,
}RESOURCE_RET;

typedef struct _ResourceReload
{
	RESOURCE_RET enResourceRet;
}ResourceReload;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef struct _SnapShot_t
{
	int chn;
	int quality;
	int force;
//	char file_name[32];
	int ret;
}SnapShot_t;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
typedef enum
{
	VERIFY_AUTHENTICATION = 0,
	VERIFY_DONGLE_ISACTIVE,
}VERIFY_TYPE;

typedef struct _Verify
{
	VERIFY_TYPE enSubType;
	char azText[64];
	int nResult;
}Verify_t;

typedef enum {
	VSUPPORT_NULL = -1,
	VSUPPORT_SD960H  = 0,
	VSUPPORT_HD720P,
	VSUPPORT_HD1080P,
	VSUPPORT_TYPE_CNT,
}VSUPPORT_TYPE;

typedef struct _Vin_t
{
	VSUPPORT_TYPE vsupport;
}Vin_t;

typedef struct _FlvReq_t
{
	int chn;
	time_t now;

	int pal_or_ntsc;
	int frame_rate;
	int width;
	int height;
	int has_audio;
}FlvReq_t;

typedef struct
{
	int autoSet;
	int autoFmtSwitch;
	int Mode[MAX_CAM_CH];
} VideoModeSwitch;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
typedef enum
{
	DISK_OFF_READY,
	POWER_OFF_READY,
} POWER_ACT_TYPE;

typedef struct
{
	POWER_ACT_TYPE type;
	int data1;
	int data2;
} Power_Action_t;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

typedef	enum
{
	CMD_NULL = -1,

	// firmware upgrade request
	CMD_FW_UPGRADE_REQ,
	CMD_FW_UPGRADE_ACK,

	// record added by law 100930
	CMD_REC_CONTROL_REQ,
	CMD_REC_CONTROL_ACK,

	// video control add by law 110330
	CMD_VIDEO_CONTROL,
	CMD_VIDEO_CONTROL_ACK,

	// Power Off Ready add by aklim 20120305
	CMD_POWER_ACTION,
	CMD_POWER_ACTION_ACK,

	// 110317 law
	CMD_PTZ_CTRL,
	CMD_PTZ_CTRL_ACK,

	// 110126 law
	CMD_REC_SEARCH,
	CMD_REC_SEARCH_ACK,
	CMD_REC_PLAYBACK,
	CMD_REC_PLAYBACK_ACK,
	CMD_REC_PLAYBACK_LIST_SEARCH,
	CMD_REC_PLAYBACK_LIST_SEARCH_ACK,

	// 110906
	CMD_TDE_CONTROL,
	CMD_TDE_CONTROL_ACK,

	// 111008
	CMD_LOGO_RELOAD_REQ,
	CMD_LOGO_RELOAD_ACK,
	CMD_RESOURCE_RELOAD_REQ,
	CMD_RESOURCE_RELOAD_ACK,

    // 111014
    CMD_UPNP_REQ,
    CMD_UPNP_ACK,
    CMD_UPNP_UPDATE,

	// 111116
	CMD_VERIFY_REQ,
	CMD_VERIFY_ACK,

	CMD_FLV_REQ,
	CMD_FLV_REQ_ACK,

	CMD_VIDEOMODE_MODIFY_QRY,
	CMD_VIDEOMODE_MODIFY_REQ,
	CMD_VIDEOMODE_MODIFY_ACK,

	CMD_NOTICE,
	GUI_ACK,
	CMD_RELOAD_ENVIRONMENT,    //重新获取环境变量
	CMD_RELOAD_ENVIRONMENT_ACK,//重新获取环境变量返回
	CMD_RELOAD_USERINFO,       //重新获取用户信息
	CMD_RELOAD_USERINFO_ACK,   //重新获取用户信息返回
	CMD_CHECK_HDD_INFO,      //检查硬盘信息
	CMD_CHECK_HDD_INFO_ACK,  //检查硬盘信息返回
	CMD_FORMAT,              //格式化硬盘
	CMD_FORMAT_ACK,         //格式化硬盘返回
	CMD_GET_VSUPPORT,
	CMD_GET_VSUPPORT_ACK,
	CMD_LOG,      //日志记录
	CMD_LOG_ACK,  //日志记录返回
	CMD_BACKUP,     //视频备份
	CMD_BACKUP_ACK, //视频备份返回
	CMD_WATCHDOG,     //看门狗
	CMD_WATCHDOG_ACK, //看门狗返回
	CMD_MODIFY_TIME, //修改系统时间
	CMD_MODIFY_TIME_ACK,
	CMD_CHANGE_VOLUMN, //更改音量
	CMD_CHANGE_VOLUMN_ACK,
	CMD_SNAPSHOT,
	CMD_SNAPSHOT_ACK,
	CMD_TELECONTROL,
	CMD_TELECONTROL_ACK,
	CMD_DDNS_TEST,
	CMD_DDNS_TEST_ACK,
	CMD_SMTP_TEST,
	CMD_SMTP_TEST_ACK,
	CMD_QRCODE_UPDATE,
	CMD_QRCODE_UPDATE_ACK,
	CMD_ONLINE_UPGRADE,
	CMD_ONLINE_UPGRADE_ACK,
//#ifdef _NVR
	CMD_FTP_TEST,
	CMD_FTP_TEST_ACK,
//#ifdef _NVR
	CMD_IPC_SEARCH,
	CMD_IPC_SEARCH_ACK,
	CMD_IPC_MODIFY_DEV,
	CMD_IPC_MODIFY_DEV_ACK,
	CMD_IPC_GETTING_ATTR,
	CMD_IPC_GETTING_ATTR_ACK,
	CMD_IPC_GET_VENC_OPT,
	CMD_IPC_GET_VENC_OPT_ACK,
	CMD_IPC_SETTING_ATTR,
	CMD_IPC_SETTING_ATTR_ACK,
	CMD_IPC_RUNNING_STATUS,
	CMD_IPC_RUNNING_STATUS_ACK,
	CMD_IPC_AUTO_ADD,
	CMD_IPC_AUTO_ADD_ACK,
	CMD_IPC_GET_BANDWIDTH,
	CMD_IPC_GET_BANDWIDTH_ACK,
	CMD_IPC_UPDATE_BANDWIDTH,
	CMD_IPC_UPDATE_BANDWIDTH_ACK,
	CMD_IPC_GET_STATUS,
	CMD_IPC_GET_STATUS_ACK,	
	CMD_IPC_GET_NETWORK_INTERFACE,
	CMD_IPC_GET_NETWORK_INTERFACE_ACK,
//#endif
#ifdef _IPC_CHECK_MODE
	CMD_IPC_RESTORE_DEFAULT,
	CMD_IPC_RESTORE_DEFAULT_ACK,
#endif

	GUI_CMD_RESULT,
}DVR_CMD_TYPE;

typedef struct
{
	int channels;
	int types;
	time_t begin;
	time_t end;
	int session_count;
}RecSearch_t;

//#ifdef _NVR
typedef struct
{
	IPCAM_PROTOCOL_NAME ipc_type;
	int ipc_count;
	int is_multi_net_segment;
}IPCSearch_t;

typedef struct
{
	int protocal;
	char dev_id[64];
	char src_ip[20];
	int src_port;
	char usr[32];
	char pwd[32];
	//modify param
	char ip[20];
	char netmask[20];
	//
	int error; // -1, failed, 0 ok, 1, ok with reboot
}IPCModifyDev_t;

typedef struct
{
	int chn;
	char protocol_name[32];
	char ip[32];
	int port;
	int phy_port;
	int connected;
	int video_width;
	int video_height;
	int video_fps;
	int video_bps;
	int status;
	int errcode;
	int enable; // true or false 
}IPCRunningStatusDetail_t;

typedef struct
{
	IPCRunningStatusDetail_t status[MAX_CAM_CH];
}IPCRunningStatus_t;

typedef struct _range
{
	int min;
	int max;
}Range_t;

typedef struct _size
{
	unsigned short width;
	unsigned short height;
}RectSize_t;

typedef enum{
	H264_P_BASELINE = 0,
	H264_P_MAIN,
	H264_P_EXTENDED,
	H264_P_HIGH
}H264_PROFILE;

typedef struct 
{
	Range_t enc_quality;
	Range_t enc_fps;
	Range_t enc_bps; //kbps
	Range_t enc_gov;
	Range_t enc_interval;
	int resolution_nr;	// how many resolution supported
	RectSize_t resolution[20];
	int profile_nr;
	int profile[5];
}IPCVencOptions_t;

typedef struct
{
	int isValid;
	ENC_BR_MODE BitrateCtrlMode;//码率控制方式
	int video_width;
	int video_height;
	int video_bps;
	int video_fps;
	int video_gop;
	int video_quality;
	int venc_interval;
	int venc_profile;
}IPCStreamAttr_t;

typedef struct
{
	int chn;
	int isSetTitle;
	int chnBandWidth;
	char channel_name[32];
	int streamCnt;
	int stream_index[MAX_STREAM_CNT];
	char profile_token[MAX_STREAM_CNT][64];
	char stream_name[MAX_STREAM_CNT][64];
	char stream_token[MAX_STREAM_CNT][64];
	IPCStreamAttr_t stStreamAttr[MAX_STREAM_CNT];
	ENC_MODE encode_mode;
	int act_success;
}IPCVideoAttr_t;

typedef struct
{
	int chn;
	int streamCnt;
	int isvalid[MAX_STREAM_CNT];
	char profile_token[MAX_STREAM_CNT][64];
	char stream_name[MAX_STREAM_CNT][64];
	char stream_token[MAX_STREAM_CNT][64];
	IPCVencOptions_t stStreamOptions[MAX_STREAM_CNT];
}IPCVideoOptions_t;

//#endif

#ifdef _IPC_CHECK_MODE
typedef struct
{
	unsigned long result;
}IPCRestoreDefault_t;
#endif

#define MAX_STREAM_NAME		(MAX_IPCAM_PRIV_DATA-4)
typedef struct _ipc_dev_info
{
	IPCAM_PROTOCOL_NAME Protocolname;//frist item ,must be IPCAM_PROTOCOL_NAME
	unsigned char ip[4];
	unsigned char netmask[4];
	unsigned short port;
	unsigned char mac[6];
	int stream_nr; // stream entries, almost is not equal to the number of stream
	char username[24];
	char password[24];
	char model_name[32];	
	unsigned char software_version[32];
	union
	{
		struct{
			char devid[64];
			char nonce[64];
		}hichip;		
		struct{
			int transport;
			char main_stream[MAX_STREAM_NAME];
			char sub_stream[MAX_STREAM_NAME];
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
}IPCDevInfo_t;

#define PARAMENT_FILE_SMTPINFO "/tmp/SMTPInfo.dat"
typedef struct
{
	ENV_BOOLEAN bSMTPUse;
	char szServer[SMTP_EDITBOX_LEN + 1];// include '\0'
	unsigned short u16Port;
	ENV_BOOLEAN bAnonymous;
	char szUsername[SMTP_EDITBOX_LEN + 1];
	char szPassword[SMTP_EDITBOX_LEN + 1];
	SMTP_ENCRYPT_TYPE enEncryptType;
	char szSender[SMTP_EDITBOX_LEN + 1];
	char szSendee1[SMTP_EDITBOX_LEN + 1];
	char szSendee2[SMTP_EDITBOX_LEN + 1];
	char szSubject[SMTP_EDITBOX_LEN + 1];
	unsigned short u16Interval;
	ENV_BOOLEAN bHealthEnable;
	unsigned short u16HealthInterval;
}SMTP_INFO;

#if 0
#define PARAMENT_FILE_FTPINFO "/tmp/FTPInfo.dat"
typedef struct
{
	ENV_BOOLEAN bFTPUse;
	char szServer[SMTP_EDITBOX_LEN + 1];// include '\0'
	unsigned short u16Port;
	ENV_BOOLEAN bAnonymous;
	char szUsername[SMTP_EDITBOX_LEN + 1];
	char szPassword[SMTP_EDITBOX_LEN + 1];
	SMTP_ENCRYPT_TYPE enEncryptType;
	char szSender[SMTP_EDITBOX_LEN + 1];
	char szSendee1[SMTP_EDITBOX_LEN + 1];
	char szSendee2[SMTP_EDITBOX_LEN + 1];
	char szSubject[SMTP_EDITBOX_LEN + 1];
	unsigned short u16Interval;
	ENV_BOOLEAN bHealthEnable;
	unsigned short u16HealthInterval;
}FTP_INFO;
#endif

#define PARAMENT_FILE_FTPINFO "/tmp/FTPInfo.dat"
typedef struct
{
	ENV_BOOLEAN bFTPUse;
	char szServer[FTP_EDITBOX_LEN + 1];// include '\0'
	unsigned short u16Port;
	ENV_BOOLEAN bAnonymous;
	char szUsername[FTP_EDITBOX_LEN + 1];
	char szPassword[FTP_EDITBOX_LEN + 1];
	char szRemoteFolder[FTP_FOLDER_LEN + 1];
	unsigned long u32FileMaxSize;
}FTP_INFO;

#define QR_CODE_FOLDER		"/tmp/qrcode/"
#define QR_CODE_CONF_PATH	"/root/dvr_app/config/"
#define QR_CODE_TOOL			"/root/zint"
#define QR_CODE_ODM			"/tmp/dvr_resource_dir/QRCodeODM.ini"

enum
{
	WECHAT_QR_CODE = 0,
	ANDROID_QR_CODE,
	IOS_QR_CODE,
	LOCALIP_QR_CODE,
	ESEE_QR_CODE,
	WEB_QR_CODE,
	QR_CODE_TYPE_NUM,
};

typedef struct _QRcodeODM
{
	int guide_Android;
	int guide_IOS;
	int guide_Esee;
	int Network_Esee;
	int sysinfo_Android;
	int sysinfo_IOS;
	int sysinfo_SupportWeb;
	char QRCodeText[QR_CODE_TYPE_NUM][128];
}QRcodeODM;

typedef struct
{
	int update_code[QR_CODE_TYPE_NUM];
	int biggerQRcode;
	char eseeid[64];
	char localip[64];
	char localport[64];
	char username[64];
	char LANG[64];
	int update_res;
}QRCode_INFO;

// 110126 law
typedef enum
{
	REC_PLAYBACK_START = 0,
	REC_PLAYBACK_CANCEL,
	REC_PLAYBACK_PLAY,
	REC_PLAYBACK_PAUSE,
	REC_PLAYBACK_FAST,
	REC_PLAYBACK_PROGRESS,
	REC_PLAYBACK_REWIND,
	REC_PLAYBACK_PREV,
	REC_PLAYBACK_NEXT,
	REC_PLAYBACK_JUMP,
	REC_PLAYBACK_STOP,
}REC_PLAYBACK_SUBTYPE;

typedef struct RecPlayback
{
	REC_PLAYBACK_SUBTYPE enSubType;
	int channels;
	int types;
	int fast;
	time_t begin;
	time_t end;
	time_t now;
	double percent;
	int action_ret;
}RecPlayback_t;

typedef struct RecPlaybackListSearch
{
	int channels;
	int types;
	time_t begin;
	time_t end;
	int ret_count;
}RecPlaybackListSearch_t;

typedef struct RecPlaybackListSearchItem
{
	int disk;
	int begin_fsindex;
	int end_fsindex;
}RecPlaybackListSearchItem_t;

// 110316 law
typedef struct PTZCtrl
{
	int nChn;
	int nCmd;
	unsigned char u8Val;
}PTZCtrl_t;

typedef enum
{
	BACKUP_RESULT_RUNNING,
	BACKUP_RESULT_ZERO_SIZE,
	BACKUP_RESULT_NODEV,
	BACKUP_RESULT_NOSPACE,
	BACKUP_RESULT_FS_ERROR,
	BACKUP_RESULT_FINISH,
}BACKUP_RESULT;

typedef enum
{
    BACKUP_SUBTYPE_CHECK_DEV,
	BACKUP_SUBTYPE_BACKUP_START,
	BACKUP_SUBTYPE_QUERY,
    BACKUP_SUBTYPE_CLOSE_DEV,
}BACKUP_SUBTYPE;

typedef enum
{
	BACKUP_TARGET_DEVICE_USB_STORAGE,
	BACKUP_TARGET_DEVICE_USB_BURRER,
}BACKUP_TARGET_DEVICE;

typedef struct
{
	BACKUP_SUBTYPE enSubType;
	union
	{
        struct
        {
            long long totalSize;
            BACKUP_TARGET_DEVICE dev;
            BACKUP_RESULT ret;
        };
		struct
		{
			int disk;
			int session_id;
			time_t start;
			time_t end;
			BACKUP_TARGET_DEVICE device;
		};
		struct
		{
			BACKUP_RESULT result;
			int progress;
		};
	};
}Backup_t;

typedef	enum
{
	FW_UPGRADE_NULL = -1,
	FW_UPGRADE_CHECK_USBDEV,
	FW_UPGRADE_SCAN_FILE,
	FW_UPGRADE_CHECK_FILE,
	FW_UPGRADE_IMPORT_FILE,
	FW_UPGRADE_START,
	FW_UPGRADE_QUERY,
	FW_UPGRADE_FINISH,
}FW_UPGRADE_SUBTYPE;

typedef	struct FWUpgrade
{
	FW_UPGRADE_SUBTYPE enSubType;
	int action_finish;
}FWUpgrade_t;

#define OPERATE_SUCCEED			101
#define HAS_NEW_FIRMWARE			102
#define BROKEN_FIRMWARE			103
#define NO_NEW_FIRMWARE			104
#define GET_IMCOMPLETE_DATA		105
#define SAVE_FRIMWARE_ERROR		106
#define CHECK_FRIMWARE_TIMEOUT	107
#define EXTRACT_FIRMWARE			108
#define CHECK_FRIMWARE_TOOFAST	109
#define NO_DEVICE_SN				110
#define ONLINE_UPGRADE_ODM		"/tmp/dvr_resource_dir/OnlineUpgradeODM"
typedef	enum
{
	ONLINE_CHECK_FIRMWARE = 1,
	ONLINE_DOWNLOAD_START,
	ONLINE_DOWNLOAD_PROG,
	ONLINE_CHECK_INTEGRITY,
	ONLINE_EXTRACT_UPGRADE,
	ONLINE_EXTRACT_UPGRADE_PROG,
	ONLINE_FINISH,
}ONLINE_UPGRADE_SUBTYPE;

typedef	struct OnlineUpgrade
{
	ONLINE_UPGRADE_SUBTYPE enSubType;
	int action_finish;
}OnlineUpgrade_t;

//#ifndef	DECLARE_DVR_NOTICE
//#define	DECLARE_DVR_NOTICE
typedef enum
{
	NOTICE_RELOAD_ENV,
	NOTICE_RECORD_SET,
	NOTICE_PLAYBACK_STATUS,
	NOTICE_RELOAD_USERINFO,
	NOTICE_HDD_ERROR,
#ifdef _IPC_CHECK_MODE
	NOTICE_CHECKINFO_SHOW,
	NOTICE_CHECKINFO_CLEAR,
#endif
}NOTICE_SUBTYPE;
//#endif	//DECLARE_DVR_NOTICE

typedef enum
{
	FROMAT_SUBTYPE_START,
	FROMAT_SUBTYPE_QUERY,
}FROMAT_SUBTYPE;

typedef enum
{
	FORMAT_RESULT_RUNNING,
	FORMAT_RESULT_FINISH,
}FORMAT_RESULT;

typedef struct
{
	FROMAT_SUBTYPE enSubType;
	union
	{
		struct
		{
			int format_index;
		};
		struct
		{
			int progress;
			FORMAT_RESULT result;
		};
	};
}Format_t;

typedef enum
{
	LOG_ADD = 0,
	LOG_SEARCH,
}LOG_SUBTYPE;

typedef struct
{
	LOG_SUBTYPE enSubType;
	union
	{
		//add
		struct
		{
			int add_type;
			char msg[128];
		};
		//search
		struct
		{
			int search_type;
			time_t begin;
			time_t end;
			int row_count;
		};
	};
}Log_t;

typedef enum
{
	PLAYBACK_STATUS_STOP,
	PLAYBACK_STATUS_PLAYING,
	PLAYBACK_STATUS_BEYONDCAP,
	PLAYBACK_CNT,
}PLAYBACK_STATUS;

typedef struct NOTICE
{
	NOTICE_SUBTYPE enSubType;
	union
	{
		//NOTICE_RELOAD_ENV
		struct //NOTICE_RECORD_SET
		{
			int chn;
			int type;
		};
		struct //NOTICE_PLAYBACK_STATUS
		{
			int pb_chn;
			PLAYBACK_STATUS pb_status;
		};

		//NOTICE_RELOAD_USERINFO
		//NOTICE_HDD_ERROR
		#ifdef _IPC_CHECK_MODE
		struct //NOTICE_IPC_CHECKINFO
		{
			int chk_chn;
			int chk_type;
			int chk_state;
			char chk_desc[64];
		};
		#endif
	};
}NOTICE_t;

typedef struct
{
	int volume;
	int misc;
	int encode;
	int record;
	int network_ip;
	int network_web;
	int network_client;
	int network_ddns;
	int network_pppoe;
	int network_3g;
	int network_smtp;
	int network_ftp;
	int screen;
	int detection;
	int ptz;
	int sensor;
	int telecontrol;
	int ups;
	int hdd;
	int color;
	int ipcam;
	int maintenance;
}ReloadEnv_t;

typedef enum
{
	WATCHDOG_SUBTYPE_ENABLE,
	WATCHDOG_SUBTYPE_DISABLE,
	WATCHDOG_SUBTYPE_FEED,
	WATCHDOG_SUBTYPE_SET_TIMEOUT,
}WATCHDOG_SUBTYPE;

typedef struct
{
	WATCHDOG_SUBTYPE enSubType;
	int param;
}Watchdog_t;

typedef struct
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
	int ret;
}ModifyTime_t;

typedef struct
{
	int volumn;
}ChangeVolumn_t;

typedef enum _TELECONTROL_SUBTYPE
{
	TELECONTROL_GET_LAST_ACTIVE = 0,
	TELECONTROL_CLEAR_LAST_ACTIVE,
}TELECONTROL_SUBTYPE;

typedef struct _Telecontrol
{
	TELECONTROL_SUBTYPE subtype;
	int lastid;
	int lastdata;
}Telecontrol_t;

typedef struct _Smtptest
{
	int testresult;
}Smtptest_t;

typedef struct _FtpTest
{
	int test_result;
}FtpTest_t;

typedef struct _DDNStest
{
	char User[DDNS_USER_LEN + 1];
	char Password[DDNS_PWD_LEN + 1];
	char DDNSUrl[DDNS_URL_LEN + 1];
	int ddns_provider;
	int testresult;
}DDNStest_t;

#pragma pack(4)
typedef	struct	DVRCommand
{
	unsigned int flag;	// always "0xfeefface"
	DVR_CMD_TYPE type;	// command type
	union
	{
		VideoCtrl_t video_ctrl; // video control
		FWUpgrade_t fw_upgrade;	// firmware upgrade
		HDDInfo_t hdd_info;	// hard disk driver info 100929 law
		RecCtrl_t rec_ctrl;	// record added by law 100930
		NOTICE_t notice_info;
		RecPlayback_t rec_playback; // 110126 added by law for dvr playback
		RecPlaybackListSearch_t rec_playback_search; // 110126 added by law for dvr playback
		RecSearch_t rec_search;
		DDNStest_t ddnstest;
		Format_t format_info;
		Log_t log_info;
		PTZCtrl_t ptz_ctrl; // 110316 law for ptz remote request control
		Backup_t backup;
		ReloadEnv_t reload;
		Watchdog_t watchdog;
		ModifyTime_t modify_time;
		ChangeVolumn_t change_volumn;
		TdeCtrl_t tde_ctrl;
		LogoReload logo_reload;
		ResourceReload resource_reload;
		SnapShot_t  snap_shot;
		_XCHG_STRUCT  xstruct;
		Verify_t verify;
		FlvReq_t flv_req;
		VideoModeSwitch VideoMode_Switch_Req;
		Power_Action_t power_act;
		Telecontrol_t telecontrol;
		Smtptest_t smtptest;
		QRCode_INFO qrcode_info;
		OnlineUpgrade_t online_upgrade;
		Vin_t vin_property;
//#ifdef _NVR
		FtpTest_t ftp_test;
//#ifdef _NVR
		IPCSearch_t ipc_search;
		IPCModifyDev_t ipc_modify_dev;
		IPCRunningStatus_t ipc_running_status;
		IPCVideoAttr_t ipc_video_attr;
		IPCVideoOptions_t ipc_video_options;
//#endif
#ifdef _IPC_CHECK_MODE
		IPCRestoreDefault_t ipc_restore_default;
#endif

	};
}DVRCommand_t;
#pragma pack()


#define NET_ID_LEN (8)
#define NET_ID_GUI      "GUI     "
#define NET_ID_CGI      "CGI     "
#define NET_ID_AVSTREAM "AVSTREAM"
#define NET_ID_UPNPSVR  "UPNPSVR "

#define COMM_TCP_USE_UNIX_SOCK
#define COMM_UNIX_SOCK_SERVER	"/tmp/comm_server.sock"
#define COMM_GUI_UNIX_SOCK_CLIENT	"/tmp/comm_gui_client.sock"
#define COMM_CGI_UNIX_SOCK_CLIENT	"/tmp/comm_cgi_client.sock"
#define COMM_BUBBLE_UNIX_SOCK_CLIENT	"/tmp/comm_bubble_client.sock"
#define COMM_UPGRADE_UNIX_SOCK_CLIENT	"/tmp/comm_upgrade_client.sock"
#define COMM_UPNP_UNIX_SOCK_CLIENT	"/tmp/comm_upnp_client.sock"


#ifdef _IPC_CHECK_MODE
#define	IPC_CHECK_CONNECT	0
#define	IPC_CHECK_TIME		1
#define	IPC_CHECK_VERSION	2
#define	IPC_CHECK_SNO		3
#define	IPC_CHECK_AUTO	4
#define	IPC_CHECK_RESTORE	5

#define CHECKRESULT_HIDE 0
#define CHECKRESULT_SUCCESS 1
#define CHECKRESULT_FAIL 2
#endif
#ifdef __cplusplus
}
#endif
#endif	//__DVR_COMMAND_H__

