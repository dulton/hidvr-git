/*
 * nvrpc_function.h
 *
 *  Created on: 2012-5-22
 *      Author: root
 */

#ifndef NVRCC_FUNCTION_H_
#define NVRCC_FUNCTION_H_

#include "conf.h"
#include "environment.h"
#include "dvr_command.h"

#define NVRPC_THREAD_RUNNING (1)
#define NVRPC_THREAD_STOP (0)
#define IPC_SEARCH_TIME (4)
#define IPC_WAN_MAX_NUM (32)
#define MAC_ADDR_LEN (6)
#define IP_ADDR_LEN (4)
#define MAX_CLIENT_ID_SIZE (32)

typedef enum
{
	SOCKET_RW_RESULT_UNKNOWN,
	SOCKET_RW_RESULT_SUCCESS,
	SOCKET_RW_RESULT_CLIENT_DISCONNECT,
	SOCKET_RW_RESULT_ERROR,
	SOCKET_RW_RESULT_TIMEOUT,
	SOCKET_RW_RESULT_CNT,
}SOCKET_RW_RESULT;
extern char* SOCKET_RW_RESULT_STRING[];


typedef struct
{
	int socketfd;
	char* buf;
	int request_len;
	int timeout_sec;

	SOCKET_RW_RESULT result;
	int errno_cpy;
	int actual_len;
}SOCKET_RW_CTX;

typedef enum
{
	SOCKET_CONN_RESULT_UNKNOWN,
	SOCKET_CONN_RESULT_SUCCESS,
	SOCKET_CONN_RESULT_FAILED,
	SOCKET_CONN_RESULT_TIMEOUT,
	SOCKET_CONN_RESULT_CNT,
}SOCKET_CONN_RESULT;
extern char* SOCKET_CONN_RESULT_STRING[];

typedef struct
{
	char ip[32];
	int port;
	int timeout_sec;

	SOCKET_CONN_RESULT result;
	int socketfd;
	int errno_cpy;
}SOCKET_CONN_CTX;

typedef enum
{
	NVR_ARG_LIVE_STOP,
	NVR_ARG_LIVE_MAIN,
	NVR_ARG_LIVE_SUB,
	NVR_ARG_LIVE_CNT,
}NVR_ARG_LIVE;

typedef struct
{
	int using_flag; //1 using
	int chn;
	char ip[32];
	int port;
	char username[32];
	char password[32];
	char protocol_name[32];
	int phy_port;
	pthread_t running_id;
	int running;
	NVR_ARG_LIVE live;
	void* private_data;
#ifdef _DNVR
	//int change_flag;
	//int change_chn;
	//char change_ip[32];	
	char dev_id[64];
#endif
#ifdef _IPC_CHECK_MODE
	int stop_flag;
#endif
}NVR_ARG;


typedef int* (*NVR_PROTOCOL_RUN)(NVR_ARG* arg);
typedef int (*NVR_PROTOCOL_CONNECTED)(NVR_ARG* arg);
typedef int (*NVR_PROTOCOL_LIVE_SWITCH)(NVR_ARG* arg, int _is_full);

#define MAX_NVR_PROTOCOL_COUNT (10)
typedef struct
{
	int using_flag;//1 using
	char name[32];
	NVR_PROTOCOL_RUN run_handle;
	NVR_PROTOCOL_CONNECTED connected_handle;
	NVR_PROTOCOL_LIVE_SWITCH live_switch_handle;
}NVR_PROTOCOL_ITEM;
#ifdef _DNVR
typedef struct
{
	char using_flag;
	char mac[MAC_ADDR_LEN];  //DNVR WAN MAC
}NVR_MAC_ITEM;
typedef struct
{
	char using_flag;
	char ip[IP_ADDR_LEN];  //DNVR WAN IP
}NVR_IP_ITEM;
typedef struct
{
	int lan_searching;   //DNVR search ipc from LAN port
	int wan_searching;  //DNVR search IPC from WAN port
	int wan_search_count;  //DNVR the count of search result from WAN port
	NVR_MAC_ITEM mac_filter_list[IPC_WAN_MAX_NUM];  //DNVR WAN IPC MAC
	NVR_IP_ITEM ip_filter_list[IPC_WAN_MAX_NUM];  //DNVR WAN IPC IP
}NVR_SEARCH_ITEM;
#endif
typedef struct
{
	NVR_PROTOCOL_ITEM protocol[MAX_NVR_PROTOCOL_COUNT];
	NVR_ARG args[MAX_CAM_CH];
#ifdef _DNVR
	int dnvr_thread_running;
	char lan_ip[32];
	pthread_t dnvr_thread_id;
	pthread_mutex_t lock_thread_restart;
	NVR_SEARCH_ITEM dnvr_search;
#endif
}NVR_PROTOCOL_COLLECTION;


#define IPC_SET_IP(_target, _default) \
{ \
	int tmp[4]; \
	_target[0] = 0; \
	_target[1] = 0; \
	_target[2] = 0; \
	_target[3] = 0; \
	if(sscanf(_default, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) == 4) \
	{ \
		_target[0] = tmp[0]; \
		_target[1] = tmp[1]; \
		_target[2] = tmp[2]; \
		_target[3] = tmp[3]; \
	} \
}


#define IPC_SET_MAC(_target, _default) \
{ \
	int tmp[6]; \
	_target[0] = 0; \
	_target[1] = 0; \
	_target[2] = 0; \
	_target[3] = 0; \
	_target[4] = 0; \
	_target[5] = 0; \
	if(sscanf(_default, "%x:%x:%x:%x:%x:%x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) == 6) \
	{ \
		_target[0] = tmp[0]; \
		_target[1] = tmp[1]; \
		_target[2] = tmp[2]; \
		_target[3] = tmp[3]; \
		_target[4] = tmp[4]; \
		_target[5] = tmp[5]; \
	} \
}

extern NVR_PROTOCOL_COLLECTION g_prc;

extern void NVRPC_conninit(SOCKET_CONN_CTX* _ctx, char* _ip, int _port, int _timeout_sec);
extern void NVRPC_connect(SOCKET_CONN_CTX* _ctx);
extern void NVRPC_rwinit(SOCKET_RW_CTX* _ctx, int _socketfd, void* _buf, int _request_len, int _timeout_sec);
extern void NVRPC_read(SOCKET_RW_CTX* _ctx);
extern void NVRPC_readn(SOCKET_RW_CTX* _ctx);
extern void NVRPC_write(SOCKET_RW_CTX* _ctx);
extern void NVRPC_writen(SOCKET_RW_CTX* _ctx);
extern char* NVRPC_get_rw_error(SOCKET_RW_CTX* _ctx);
extern unsigned int NVRPC_get_connection();
extern void NVRPC_LiveDecode(int nChn, unsigned char bEnable);
extern void NVRPC_init();
extern void NVRPC_destory();
extern int NVRPC_Search(IPCAM_PROTOCOL_NAME type);
extern int Write_IPCInfo(char *buf, FILE* file_fd, IPCAM_PROTOCOL_NAME Protocolname);
//extern int Write_IPCInfo_test(FILE* file_fd, IPCAM_PROTOCOL_NAME Protocolname);

typedef int (*NVRPC_send_mcas_callback)(void*, int);
int NVRPC_send_mcas(char* _mcasaddr, int _port, NVRPC_send_mcas_callback _on_send, NVRPC_send_mcas_callback _on_recv);
#endif /* NVRCC_FUNCTION_H_ */
