
#ifndef __HICHIP_H__
#define __HICHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvp_define.h"

struct hichip_ctx;
typedef struct hichip_ctx stHICHIP, *lpHICHIP;

/*
* minirtsp result of call ...
*/
#define HICHIP_OK						(0)
#define HICHIP_FAIL					(-1)
#define HICHIP_INVALID_PARAM			(-2)
#define HICHIP_NOT_ENOUGH_MEM			(-3)

#define HICHIP_MULTICAST_PORT (8002)
#define HICHIP_MULTICAST_IPADDR "239.255.255.251"


void
HICHIP_delete(lpHICHIP thiz);


typedef struct hichip_data_property
{
	int mediaType;
	union
	{
		struct{
			int width, height, fps, bps;
		}h264;		
		struct{
			int sampleRate, sampleSize, channel;
		}g711;
	};
}stHICHIP_DATA_PROPERTY, *lpHICHIP_DATA_PROPERTY;

int
HICHIP_lookup_data(lpHICHIP thiz, int mediaType, 
	lpHICHIP_DATA_PROPERTY dataProperties);

/*
* function: HICHIP_set_data_hook
* description: rtsp set a hook to handle data
* parameter:
*		-- thiz: minirtsp context
*		-- hook: type of fHICHIP_DATA_HOOK
*		-- customCtx: user-defined context
*		-- int CustomCtxSize: the size of customCtx Size, if size given ( > 0), it would malloc cache to store custome context,
*				else it would only store the pointer of custom context
* return: none
* note:
*		-- fHICHIP_DATA_HOOK:
*			--pdata: cache to store data
*			-- dataSize: size of data
*			--timestamp: timestamp of data in milisecond
*			-- dataType: type of data , detail see definition of media data type as follows
*			--customCtx: custom context given by HICHIP_set_event_hook
*/

/*
* media data type : corresponding to rtplib.h
*/
#define HICHIP_MD_TYPE_H264		(96)
#define HICHIP_MD_TYPE_G711		(8)
#define HICHIP_MD_TYPE_G726		(97)
#define HICHIP_MD_TYPE_DATA		(100)

typedef void (*fHICHIP_DATA_HOOK)(int streamId, void *pdata, unsigned int dataSize, unsigned int timestamp,
	int dataType, void *customCtx);
void
HICHIP_set_data_hook(lpHICHIP thiz, fHICHIP_DATA_HOOK hook, void *customCtx, int CustomCtxSize);

typedef enum{
	HICHIP_ERR_OK = 0,
	HICHIP_ERR_FAULT,
	HICHIP_ERR_INVALID_ARG,
	HICHIP_ERR_CONNECT_FAILED,
	HICHIP_ERR_SOCK_ERROR,
	HICHIP_ERR_AUTH_FAILED,
	HICHIP_ERR_SERVER_NOT_FOUND,
	HICHIP_ERR_PEER_ABORT_SERVER,
}enHICHIP_ERROR;
/*
* hichip event , origin from rtsplib.h
*/
enum {
	HICHIP_EVENT_CONNECTED = 0,
	HICHIP_EVENT_ERROR_OCCUR,
	HICHIP_EVENT_DISCONNECTED,
	HICHIP_EVENT_DESTROYED,
	HICHIP_EVENT_PLAY,   // only for server
	HICHIP_EVENT_DATA_RECEIVED, // only for client, lparam[OUT] is MediadataType
	HICHIP_EVENT_AUTH_REQUIRE,  // only for client, set username in rparam[IN] with formated "username:userpwd"
	HICHIP_EVENT_MD,
	
	HICHIP_EVENT_NR
};

typedef void (*fHICHIP_EVENT_HOOK)(int eventType, int lParam, void *rParam, void *customCtx);
void
HICHIP_set_event_hook(lpHICHIP thiz, fHICHIP_EVENT_HOOK hook, void *customCtx, int CustomCtxSize);


/*
* additional interfaces for JCM
* new0 -> do_loop(exited: cleanup) -> loop_exit-> destroy
*/
lpHICHIP 
HICHIP_client_new0(const char *szip, int port, const char *userName, const char *userPwd, int enableAudio, int autoReconnect, int id);

void *
HICHIP_client_do_loop(lpHICHIP thiz);

#define HICHIP_AUTH_NONE		(0)
#define HICHIP_AUTH_BASIC		(1)
#define HICHIP_AUTH_DIGEST		(2)

void HICHIP_set_auth_mode(lpHICHIP thiz, unsigned int authMode);
void
HICHIP_client_loop_exit(lpHICHIP thiz);

void
HICHIP_client_cleanup(lpHICHIP thiz);



int HICHIP_client_get_id(lpHICHIP thiz);

void HICHIP_set_loglevel(lpHICHIP thiz, int level);


/*
*
*/
void HICHIP_get_device_id(char *devid);
const char *HICHIP_get_ether_token();


typedef void (*fHichipSearchFoundHook)(unsigned char *ip, unsigned *mask, unsigned short port, unsigned char *mac, unsigned char* id, 
	unsigned char* version, unsigned char* model, unsigned char *nonce,void *customCtx);
extern int HICHIP_search(int device_type, int recv_timeout_s, 
	fHichipSearchFoundHook hook,  const char *bind_host, 
	void *customCtx);
int HG_set_network_interface(lpNVP_ARGS args, lpNVP_ETHER_CONFIG ether);


typedef struct _HichipSearchAckData
{
	char szip[64];
	int port;
	char netmask[64];
	char eseeId[64];
	char gateway[64];
	char mac[32];
	char devname[64];
	char devModel[64];
	char swVersion[64];

}stHichipSearchAckData, *lpHichipSearchAckData;

typedef int (*fHICHIP_PROBE_HOOK)(lpHichipSearchAckData data);

int HICHIP_init(fHICHIP_PROBE_HOOK hook, const char *etherToken, int bDaemonBindHost);
void HICHIP_destroy();

void HICHIP_daemon_start();
void HICHIP_daemon_stop();

#define HICHIP_MODEL_JUAN			"IPCAM"
#define HICHIP_MODEL_XINGWEI		"QV_IPC"
#define HICHIP_MODEL_UNKNOW		"MODEL-NAME"

void HICHIP_get_device_id(char *devid);



#ifdef __cplusplus
}
#endif

#endif



