/******************************************************************************

  Copyright (C), 2014~, GuangZhou JUAN Electronics Co., Ltd.

 ******************************************************************************
  File Name    : jcm.h
  Version       : Initial Draft
  Author        : wengson@sina.com
  Created       : 2014/12/12
  Last Modified : 2014/12/12
  Description   : 
 
  History       : 
  1.Date        : 2014/12/12
    	Author      : wenson
 	Modification: Created file	
******************************************************************************/

/*****************************************************************************
* Note:
* 1) ID & CHN & INDEX range is [ 0 , MAX]
******************************************************************************/
#ifndef __JCM_H__
#define __JCM_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "jcm_config.h"
#include <glib.h>
#include <glib-object.h>
#include "ja_type.h"

#define JCM_MAX_STREAM_NUM		(32)

typedef enum{
	JCM_CHN_MODE_BNC = 0,
	JCM_CHN_MODE_NET,
	JCM_CHN_MODE_FILE,
	JCM_CHN_MODE_NR
}enJCM_CHN_MODE;

typedef enum{
	JCM_STATE_NULL = 0,
	JCM_STATE_INIT, // all parameters filled & context new
	JCM_STATE_READY, // setup done
	JCM_STATE_PLAYING, // recving stream
	//JCM_STATE_PAUSED,
	JCM_STATE_NR
}enJCM_CHN_STATE;


typedef enum {
	JCM_DATA_NONE = 0,
	JCM_DATA_VIDEO_BASE = 0x1,
	JCM_DATA_H264 = JCM_DATA_VIDEO_BASE,
	JCM_DATA_H265,
	JCM_DATA_MPEG,
	
	JCM_DATA_AUDIO_BASE = 0x20,
	JCM_DATA_ALAW = JCM_DATA_AUDIO_BASE,
	JCM_DATA_ULAW,
	JCM_DATA_G726,
	JCM_DATA_AAC,

	JCM_DATA_USER_BASE = 0x40,
}enJCM_DATA_TYPE;


#define JCM_SRC_BNC		"analog"
#define JCM_SRC_ONVIF	"onvif"
#define JCM_SRC_HICHIP	"hichip"
#define JCM_SRC_RTSP	"rtsp"
#define JCM_SRC_ZAVIO	"zavio"
#define JCM_SRC_TST		"tianshitong"

typedef guint chnID_t;

typedef int (*fJCM_DATA_HOOK)(unsigned int nchn, unsigned int nStream, enJCM_DATA_TYPE dataType, guint64 pts_us, void *pbuf, guint32 dataSize, void *customCtx);
typedef int (*fJCM_EVENT_HOOK)(unsigned int nChn, int eventType, int lparam, void *rparam, void *customCtx);


int JCM_init(int max_chn, int max_stream, 
	fJCM_DATA_HOOK dataHook, void *dataHookCtx,
	fJCM_EVENT_HOOK eventHook, void *eventHookCtx,
	const char *configFile);
void JCM_deinit(void);
void JCM_set_log_level(unsigned int max_log_level /* 1 ~ 7 */);

/*
* load & save chanel
*/
int JCM_load_config(const char *filePath);
int JCM_save_config(const char *filePath);


#define JCM_CHN_FROM_DATAID(id)	((id) / 100 - 1)
#define JCM_STREAM_FROM_DATAID(id)	((id) % 100)

//void JCM_add_event_hook(fJCM_EVENT_HOOK hook, void *customCtx, int customCtxSize);
//void JCM_del_event_hook(fJCM_EVENT_HOOK hook);

//void JCM_set_data_hook(fJCM_DATA_HOOK hook, void *customCtx, int customCtxSize);


/*
* get channel mode
*/
enJCM_CHN_MODE JCM_get_chn_mode(int chn);

/*
* get channel state
*/
enJCM_CHN_STATE JCM_get_chn_state(int chn);
int JCM_get_chn_err_code(int chn);

/*
* set / clear channel config
*/
int JCM_set_chn_src(int chn, const char *srcType, .../* kv_list...*/);
int JCM_set_chn_src2_bnc(int chn,  char *szip, int vin, int ain);
int JCM_set_chn_src2_file(int chn, const char **filePathArray, const char **audioFilePathArray);
int JCM_set_chn_src2_n1(int chn,  char *szip, int port, char *username, char *userpwd, int enableAudio);
int JCM_set_chn_src2_rtsp(int chn, char *username, char *userpwd, unsigned int transportMode, int enableAudio, guint nStream, ... /* url list*/);
int JCM_set_chn_src2_onvif(int chn, char *szip, unsigned int port, char *username, char *userpwd, unsigned int transportMode, int enableAudio);
int JCM_set_chn_src2_zavio(int chn, char *szip, int port, char *username, char *userpwd, int transportMode, int enableAudio);
int JCM_set_chn_src2_tst(int chn, char *szip, int port, char *username, char *userpwd);

int JCM_clear_chn(unsigned int chn);
int JCM_clear_all_chn(void);

/*
* set /get properties
*/
#define JCM_PROPERTY_SRC_STREAM_NUM			"srcStreamNum"		// uint
#define JCM_PROPERTY_ACTIVE_STREAM_NUM		"activeStreamNum" 	// uint
#define JCM_PROPERTY_STREAM_INDEX			"streamIndex"		// uint
#define JCM_PROPERTY_SUPPORT_AUDIO			"supportAudio"		// boolean
#define JCM_PROPERTY_ENABLE_AUDIO			"enableAudio"		// boolean
#define JCM_PROPERTY_ENABLE_ALARM			"enableAlarm"		// boolean
#define JCM_PROPERTY_TIME_FIX_HOUR			"timeFixHour"		// int
#define JCM_PROPERTY_STREAM_BANDWIDTH		"streamBandwidth" // int, kbps

int JCM_set_chn_property(int chn, int id, const char *propertyName, GValue *propertyVal);
int JCM_get_chn_property(int chn, int id, const char *propertyName, GValue *propertyVal);


/*
* start /stop channel
*/
int JCM_start_chn(int chn);
int JCM_restart_chn(int chn);
int JCM_stop_chn(int chn);
int JCM_start_all_chn();
int JCM_stop_all_chn();
	

/*
* key/value, control
*/
#define JCM_SET			"[set]"
#define JCM_GET			"[get]"
#define JCM_CTL			"[ctl]"
#define JCM_INFO		"/kvc/sys/info"
#define JCM_PROFILE		"/kvc/sys/profile"		// single channel profile
#define JCM_PROFILES	"/kvc/sys/profiles"			// all channels profiles
#define JCM_DTIME		"/kvc/sys_date_time"
#define JCM_NET			"/kvc/sys/net/basic"
#define JCM_PPPOE		"/kvc/sys/net/PPPoE"
#define JCM_UPNP		"/kvc/sys/net/UPnP"
#define JCM_ESEE		"/kvc/sys/net/ESEE"
#define JCM_VENCS		"/kvc/sys/video_encodes"
#define JCM_VENC		"/kvc/sys/video_encode"
#define JCM_VENCOPT		"/kvc/sys/video_encode_opt"
#define JCM_VSRC		"/kvc/sys/video_source"
#define JCM_VIN			"/kvc/sys/video_source_conf"
#define JCM_IMG			"/kvc/sys/image"
#define JCM_COLOR		"/kvc/sys/color"
#define JCM_AENC		"/kvc/sys/audio_encode"
#define JCM_AIN			"/kvc/sys/audio_source"
#define JCM_MD			"/kvc/sys/md"
#define JCM_SENSOR		"/kvc/sys/sensor-in"
#define JCM_ALARM		"/kvc/sys/alarm-out"
#define JCM_OSDTITLE	"/kvc/sys/osd-title"
#define JCM_OSDDTIME	"/kvc/sys/osd-datetime"
#define JCM_AND			"|"
//
#define JCM_REBOOT		"/kvc/reboot"
#define JCM_SYS_RESET	"/kvc/factory_default"
#define JCM_PTZ			"/kvc/ptz_control"

#define JCM_MATCH(src, target) (strncmp(src, target, strlen(target)) == 0)
int JCM_set_kv(int chn, int id, const char *propertyName, void *propertyVal, int valSize);
int JCM_get_kv(int chn, int id, const char *propertyName, void *propertyVal, int valSize);
int JCM_control(int chn, const char *cmdName, void *cmdArg, int argSize);
/*
* set /get properties
*/
/*
extern int JCM_kvc_load(lpNVP_ENV nvp, const char *module, int keyid);
extern int JCM_kvc_save(lpNVP_ENV nvp, const char *module, int keyid);
extern int JCM_kvc_cmd(lpNVP_CMD cmd, const char *module, int keyid);
*/

/*
* ENV
*/
#define JCM_ENV_TIME_SYNC_PERIOD			"JCM_ENV_TIME_SYNC_PERIOD"
#define JCM_ENV_EVENT_RENEW_PERIOD			"JCM_ENV_EVENT_RENEW_PERIOD"

#ifdef __cplusplus
}
#endif
#endif
