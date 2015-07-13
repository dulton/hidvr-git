/*
 * nvrpc_function.c
 *
 *  Created on: 2012-5-22
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "conf.h"
#include "std.h"
#include "nvrpc_function.h"
#include "environment.h"
#include "dnvr-switch.h"
#ifdef _IPC_CHECK_MODE
#include "net_manage.h"
#endif

#ifdef _NVR

NVR_PROTOCOL_COLLECTION g_prc;
extern Sysenv* g_pstSysEnv;

char* SOCKET_RW_RESULT_STRING[] =
{
	"SOCKET_RW_RESULT_UNKNOWN",
	"SOCKET_RW_RESULT_SUCCESS",
	"SOCKET_RW_RESULT_CLIENT_DISCONNECT",
	"SOCKET_RW_RESULT_ERROR",
	"SOCKET_RW_RESULT_TIMEOUT",
};


#ifdef _DNVR
static int _check_and_update_wan_ipc_mac_filter(int *mac);
static int _check_wan_ipc_ip_filter(int *ip);
#endif

int generate_client_id(char *_client_id)
{
	int fd;
	char buf[MAX_CLIENT_ID_SIZE+1];
	char *id = _client_id;
	char id_one[2];
	int len,i, ret;
	struct timeval tv;

	memset(buf, 0 , sizeof(buf));
	memset(id, 0 , MAX_CLIENT_ID_SIZE);
	fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);
	if(fd < 0)
	{
		goto GENERATE_ID_BY_TIME;
	}
	ret = read(fd, buf, (MAX_CLIENT_ID_SIZE/2));
	close(fd);
	if( ret <= 0)
	{
		goto GENERATE_ID_BY_TIME;
	}

	for (i = 0; i < ret; i++)
	{
		memset(id_one, 0 , sizeof(id_one));
		sprintf(id_one,"%02x", buf[i]);
		strcat(id, id_one);
	}
	len = strlen(id);
	for(i = len; i< MAX_CLIENT_ID_SIZE; i++)
		id[i]='0';
	
	return 0;
	
GENERATE_ID_BY_TIME:
		
	memset(buf, 0 , sizeof(buf));
	gettimeofday(&tv, 0);
	sprintf(buf, "%d", tv.tv_sec);
	len = strlen(buf);
	sprintf(&buf[len], "%d", tv.tv_usec);
	len = strlen(buf);
	for(i = len; i< MAX_CLIENT_ID_SIZE; i++)
		buf[i]='0';
	strcpy(id, buf);
	
	return 0;
}

int print_IPCInfo(IPCDevInfo_t ipc_info)
{
	printf("\r\nprint_IPCInfo-----------------------\r\n");
	printf("id   = %s\r\n", ipc_info.devid);
	printf("ip   = %d.%d.%d.%d\r\n", ipc_info.ip[0], ipc_info.ip[1],
				ipc_info.ip[2], ipc_info.ip[3]);
	printf("mac  = %02X:%02X:%02X:%02X:%02X:%02X\r\n", ipc_info.mac[0], ipc_info.mac[1],
				ipc_info.mac[2], ipc_info.mac[3],
				ipc_info.mac[4], ipc_info.mac[5]);
	printf("port = %d\r\n", ipc_info.port);
	printf("name = %d\r\n", ipc_info.Protocolname);
	printf("-----------------------\r\n");
	return 0;
}


int Write_IPCInfo(char *buf, FILE* file_fd, IPCAM_PROTOCOL_NAME Protocolname)
{
	char *ip;
	char *id;
	char *mac;
	char *port;
	int rlt = 1;
//	char write_buf[512] = {0};
	IPCDevInfo_t ipc_dev_info;

	memset(&ipc_dev_info, 0, sizeof(IPCDevInfo_t));

//	printf("获取的消息\n");
//	printf("%s\n", buf);

//	printf("获取IP地址 \n");
	id = strstr(buf, "Device-ID=");
	id += strlen("Device-ID=");
	ip = strstr(buf, "IP=");
	ip += strlen("IP=");
	mac = strstr(buf, "MAC=");
	mac += strlen("MAC=");
	port = strstr(buf, "Http-Port=");

	port += strlen("Http-Port=");
	*strstr(id, "\r\n") = 0;
	*strstr(ip, "\r\n") = 0;
	*strstr(mac, "\r\n") = 0;
	*strstr(port, "\r\n") = 0;

	//sscanf(ip, "IP=%d.%d.%d.%d", ipc_info.ip);
	strcpy(ipc_dev_info.devid, id);
	IPC_SET_IP(ipc_dev_info.ip, ip);
	IPC_SET_MAC(ipc_dev_info.mac, mac);
	ipc_dev_info.port = atoi(port);
	ipc_dev_info.Protocolname = Protocolname;
	#ifdef _DNVR
		int macaddr[MAC_ADDR_LEN];
		int phy_port = -1;
		
		sscanf(mac, "%x:%x:%x:%x:%x:%x", &macaddr[0], &macaddr[1], &macaddr[2], &macaddr[3], &macaddr[4], &macaddr[5]);
		phy_port = SWT_Mac_hit_port(macaddr);
		if(phy_port == (WAN_PORT_NO-1))
		{
			if(!_check_and_update_wan_ipc_mac_filter(macaddr))
			{
				if(g_prc.dnvr_search.wan_search_count < 255)
				{
					fwrite(&ipc_dev_info, sizeof(IPCDevInfo_t), 1, file_fd);
					rlt = 0;
					g_prc.dnvr_search.wan_search_count++;
				}
			}
		}				
	#else
		fwrite(&ipc_dev_info, sizeof(IPCDevInfo_t), 1, file_fd);
		rlt = 0;
	#endif
	print_IPCInfo(ipc_dev_info);
//	printf("ip : %s\n", ip);
//	printf("id : %s\n", id);
//	printf("mac : %s\n", mac);
//	printf("port : %s\n", port);

//	printf("\n");
	return rlt;
}

/*int Write_IPCInfo_test(FILE* file_fd, IPCAM_PROTOCOL_NAME Protocolname)
{

	IPCInfo_t ipc_info;
	char buf_tmp[32];
	int i ;
	for(i = 0; i < 10; i++){
		sprintf(buf_tmp, "192.168.1.%d", 110+i);
		IPC_SET_IP(ipc_info.ip, buf_tmp);
		sprintf(buf_tmp, "00:01:02:03:04:%0x", 110+i);
		IPC_SET_MAC(ipc_info.mac, buf_tmp);
		ipc_info.http_port = 8888;
		ipc_info.Protocolname = Protocolname;
		fwrite(&ipc_info, sizeof(IPCInfo_t), 1, file_fd);
		print_IPCInfo(ipc_info);
	}
	return i;
//	printf("ip : %s\n", ip);
//	printf("id : %s\n", id);
//	printf("mac : %s\n", mac);
//	printf("port : %s\n", port);

//	printf("\n");
}*/


int _found_protocol(char* _name)
{
	int ret = -1;
	int i;
	for(i = 0; i < MAX_NVR_PROTOCOL_COUNT; i++)
	{
		if(strcasecmp(_name, g_prc.protocol[i].name) == 0)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

void NVRPC_conninit(SOCKET_CONN_CTX* _ctx, char* _ip, int _port, int _timeout_sec)
{
	memset(_ctx, 0, sizeof(SOCKET_CONN_CTX));
	strcpy(_ctx->ip, _ip);
	_ctx->port = _port;
	_ctx->timeout_sec = _timeout_sec;
}

void NVRPC_connect(SOCKET_CONN_CTX* _ctx)
{
	_ctx->result = SOCKET_CONN_RESULT_UNKNOWN;

	_ctx->socketfd = socket(AF_INET, SOCK_STREAM, 0);


	unsigned long ul = 1;
	ioctl(_ctx->socketfd, FIONBIO, &ul); //设置为非阻塞模式


	/* 设置远程连接的信息*/
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET; /* 注意主机字节顺序*/
	dest_addr.sin_port = htons(_ctx->port); /* 远程连接端口, 注意网络字节顺序*/
	dest_addr.sin_addr.s_addr = inet_addr(_ctx->ip); /* 远程 IP 地址, inet_addr() 会返回网络字节顺序*/
	bzero(&(dest_addr.sin_zero), 8); /* 其余结构须置 0*/


	/* 连接远程主机，出错返回 -1*/
	int ret = connect(_ctx->socketfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr_in));
	if(ret == -1)
	{
		if(errno!=EINPROGRESS)
		{
			close(_ctx->socketfd);
			_ctx->result = SOCKET_CONN_RESULT_FAILED;
			_ctx->errno_cpy = errno;
		}

		struct timeval timeout;
		fd_set r;
		FD_ZERO(&r);
		FD_SET(_ctx->socketfd,&r);
		timeout.tv_sec = _ctx->timeout_sec;
		timeout.tv_usec = 0;
		int retval = select(_ctx->socketfd+1, NULL, &r, NULL, &timeout);
		if(retval==-1)
		{
			close(_ctx->socketfd);
			_ctx->result = SOCKET_CONN_RESULT_FAILED;
			_ctx->errno_cpy = errno;
		}
		else if(retval == 0)
		{
			close(_ctx->socketfd);
			_ctx->result = SOCKET_CONN_RESULT_FAILED;
			_ctx->errno_cpy = ETIMEDOUT;
		}
		else
		{
			unsigned long ul1=0;
			ioctl(_ctx->socketfd, FIONBIO,(unsigned long*)&ul1);

			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			setsockopt(_ctx->socketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
			setsockopt(_ctx->socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			_ctx->result = SOCKET_CONN_RESULT_SUCCESS;
		}
	}
	else
	{

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		setsockopt(_ctx->socketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		setsockopt(_ctx->socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		_ctx->result = SOCKET_CONN_RESULT_SUCCESS;
	}
}

void NVRPC_rwinit(SOCKET_RW_CTX* _ctx, int _socketfd, void* _buf, int _request_len, int _timeout_sec)
{
	memset(_ctx, 0, sizeof(SOCKET_RW_CTX));
	_ctx->socketfd = _socketfd;
	_ctx->buf = _buf;
	_ctx->request_len = _request_len;
	_ctx->timeout_sec = _timeout_sec;
}

void NVRPC_read(SOCKET_RW_CTX* _ctx)
{
	_ctx->result = SOCKET_RW_RESULT_UNKNOWN;
	_ctx->errno_cpy = 0;

	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;

	while(1)
	{
		if(curr_time - begin_time > _ctx->timeout_sec)
		{
			_ctx->result = SOCKET_RW_RESULT_TIMEOUT;
			return;
		}
		_ctx->actual_len = recv(_ctx->socketfd, _ctx->buf, _ctx->request_len, 0);
		_ctx->errno_cpy = errno;
		curr_time = time(NULL);
		if(_ctx->actual_len == -1)
		{
			if(_ctx->errno_cpy == EAGAIN || _ctx->errno_cpy == EINTR || _ctx->errno_cpy == EINPROGRESS)
			{
				continue; //need retry
			}

			TRACE_DEBUG("recv failed errno_cpy=%d", _ctx->errno_cpy);
			_ctx->result = SOCKET_RW_RESULT_ERROR;
			return;
		}
		else if(_ctx->actual_len == 0)
		{
			_ctx->result = SOCKET_RW_RESULT_CLIENT_DISCONNECT;
			return;
		}
		else if(_ctx->actual_len > 0)
		{
			_ctx->result = SOCKET_RW_RESULT_SUCCESS;
			return;
		}
	}
	assert(0);
	return;
}

void NVRPC_readn(SOCKET_RW_CTX* _ctx)
{
	_ctx->result = SOCKET_RW_RESULT_UNKNOWN;

	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;
	int recv_ret;

	while(1)
	{
		ASSERT(_ctx->actual_len <= _ctx->request_len, "_ctx->actual_len=%d, _ctx->request_len=%d", _ctx->actual_len, _ctx->request_len);
		if(_ctx->request_len == _ctx->actual_len)
		{
			_ctx->result = SOCKET_RW_RESULT_SUCCESS;
			return;
		}
		if(curr_time - begin_time > _ctx->timeout_sec)
		{
			_ctx->result = SOCKET_RW_RESULT_TIMEOUT;
			return;
		}

		recv_ret = recv(_ctx->socketfd, _ctx->buf + _ctx->actual_len, _ctx->request_len - _ctx->actual_len, 0);
		_ctx->errno_cpy = errno;
		curr_time = time(NULL);
		if(recv_ret == -1)
		{
			if(_ctx->errno_cpy == EAGAIN || _ctx->errno_cpy == EINTR)
			{
				continue; //need retry
			}

			TRACE_DEBUG("recv failed errno_cpy=%d", _ctx->errno_cpy);
			_ctx->result = SOCKET_RW_RESULT_ERROR;
			return;
		}
		else if(recv_ret == 0)
		{
			_ctx->result = SOCKET_RW_RESULT_CLIENT_DISCONNECT;
			return;
		}
		else if(recv_ret > 0)
		{
			_ctx->actual_len += recv_ret;
		}
	}
	assert(0);
	return;
}

void NVRPC_write(SOCKET_RW_CTX* _ctx)
{

}

void NVRPC_writen(SOCKET_RW_CTX* _ctx)
{
	_ctx->result = SOCKET_RW_RESULT_UNKNOWN;

	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;
//	unsigned char* waitting_buf = _ctx->buf;
//	int waitting_size = _ctx->request_len;
	int send_ret;

	while(1)
	{
		ASSERT(_ctx->actual_len <= _ctx->request_len, "_ctx->actual_len=%d, _ctx->request_len=%d", _ctx->actual_len, _ctx->request_len);
		if(_ctx->request_len == _ctx->actual_len)
		{
			_ctx->result = SOCKET_RW_RESULT_SUCCESS;
			return;
		}
		if(curr_time - begin_time > _ctx->timeout_sec)
		{
			_ctx->result = SOCKET_RW_RESULT_TIMEOUT;
			return;
		}

//		TRACE_DEBUG("waitting_size=%d", waitting_size);
		send_ret = send(_ctx->socketfd, _ctx->buf + _ctx->actual_len, _ctx->request_len - _ctx->actual_len, 0);
		_ctx->errno_cpy = errno;
		curr_time = time(NULL);
		if(send_ret == -1)
		{
			if(_ctx->errno_cpy == EAGAIN || _ctx->errno_cpy == EINTR)
			{
				continue;//need retry
			}
			TRACE_DEBUG("send failed errno_cpy=%d", _ctx->errno_cpy);
			_ctx->result = SOCKET_RW_RESULT_ERROR;
			return;
		}
		else if(send_ret >= 0)
		{
			_ctx->actual_len += send_ret;
			if(send_ret == 0)
			{
				TRACE_DEBUG("send_ret is zero, errno_cpy=%d", _ctx->errno_cpy);
			}
		}
	}
	assert(0);
	return;
}

char* NVRPC_get_rw_error(SOCKET_RW_CTX* _ctx)
{
	return SOCKET_RW_RESULT_STRING[_ctx->result];
}

unsigned int NVRPC_get_connection()
{
	unsigned int ret = 0;
	int protocol_index = -1;
	int i;
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			protocol_index = _found_protocol(g_prc.args[i].protocol_name);
			if(protocol_index != -1)
			{
				ret |= (g_prc.protocol[protocol_index].connected_handle(&g_prc.args[i]) << i);
			}
			else
			{
				ret |= (1 << i);
			}
		}
		else
		{
			ret |= (1 << i);
		}
	}
//	TRACE_DEBUG("ret=%d", ret);
	return ret;
}

void NVRPC_LiveDecode(int nChn, unsigned char bEnable)
{
	TRACE_DEBUG("nChn=%d, bEnable=%d", nChn, bEnable);
	if(nChn >= 0 && nChn <= MAX_CAM_CH)
	{
		if(bEnable == TRUE)
		{
			if(g_prc.args[nChn].live == NVR_ARG_LIVE_STOP)
			{
				g_prc.args[nChn].live = NVR_ARG_LIVE_SUB;
			}
		}
		else
		{
			g_prc.args[nChn].live = NVR_ARG_LIVE_STOP;
		}
	}
}

void NVRPC_switch_full(int chn)
{
	TRACE_DEBUG("NVRPC_hg_live_switch chn=%d", chn);
	int protocol_index = -1;
	int i;
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			protocol_index = _found_protocol(g_prc.args[i].protocol_name);
			if(protocol_index != -1)
			{
				g_prc.protocol[protocol_index].live_switch_handle(&g_prc.args[i], i==chn?1:0);
			}
		}
	}
}

#ifdef _DNVR
#pragma pack(4)

#define CONNECT_FIALED_SLEEP (3)
#define RW_TIMEOUT (5)
#define MCASTADDR     "239.255.255.250"
#define MCASTPORT      8002
#define BUFSIZE        2048
#define DEFAULT_COUNT  500

typedef struct
{
	int chn;
	char id[64];
	int ip[4];
	int port;
	int mac[6];
	char client_id[MAX_CLIENT_ID_SIZE+1];
}MODIFY_CHANNEL_PARAMS;

typedef struct
{
	int sock;
	int timeout;

	MODIFY_CHANNEL_PARAMS params;
}_DNVR_SEARCH_CTX;

#pragma pack()


static int _start_dnvr_channel(int _chn, MODIFY_CHANNEL_PARAMS* _params)
{
	pthread_mutex_lock(&g_prc.lock_thread_restart);

	int protocol_index = IPCAM_HG;
	g_prc.args[_chn].using_flag = 1;
	g_prc.args[_chn].chn = _chn;
	sprintf(g_prc.args[_chn].ip, "%d.%d.%d.%d", _params->ip[0], _params->ip[1], _params->ip[2], _params->ip[3]);
	g_prc.args[_chn].port = _params->port;
	sprintf(g_prc.args[_chn].username, "admin");
	sprintf(g_prc.args[_chn].password, "admin");
	sprintf(g_prc.args[_chn].protocol_name, IPCAM_PROTOCOL_NAME_STRING[protocol_index]);
	g_prc.args[_chn].phy_port = _chn;
	g_prc.args[_chn].running_id = 0;
	g_prc.args[_chn].running = NVRPC_THREAD_STOP;
	g_prc.args[_chn].live = NVR_ARG_LIVE_SUB;
	strcpy(g_prc.args[_chn].dev_id,_params->id);

	TRACE_INFO("chn=%d, start", _chn);
	g_prc.args[_chn].running = NVRPC_THREAD_RUNNING;
	pthread_create(&(g_prc.args[_chn].running_id), NULL, g_prc.protocol[protocol_index].run_handle, &g_prc.args[_chn]);

	pthread_mutex_unlock(&g_prc.lock_thread_restart);
	return 0;
}

static int _start_nvr_channel(int _chn)
{
	pthread_mutex_lock(&g_prc.lock_thread_restart);

	ENV_BOOLEAN using = g_pstSysEnv->IsEnableIPCChannel(g_pstSysEnv, _chn);
	int protocol_index = g_pstSysEnv->GetIPCamProtocolname(g_pstSysEnv, _chn);
	unsigned char ip[4];
	g_pstSysEnv->GetIPCamAddr(g_pstSysEnv, _chn, ip);

	if(using == ENV_TRUE)
	{
		g_prc.args[_chn].using_flag = using == ENV_TRUE ? 1 : 0;
		g_prc.args[_chn].chn = _chn;
		sprintf(g_prc.args[_chn].ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		g_prc.args[_chn].port = (int)g_pstSysEnv->GetIPCamPort(g_pstSysEnv, _chn);
		g_pstSysEnv->GetIPCamUsername(g_pstSysEnv, _chn, g_prc.args[_chn].username);
		g_pstSysEnv->GetIPCamPassword(g_pstSysEnv, _chn, g_prc.args[_chn].password);
		sprintf(g_prc.args[_chn].protocol_name, IPCAM_PROTOCOL_NAME_STRING[protocol_index]);
		g_prc.args[_chn].phy_port = MAX_CAM_CH;
		g_prc.args[_chn].running_id = 0;
		g_prc.args[_chn].running = NVRPC_THREAD_STOP;
		g_prc.args[_chn].live = NVR_ARG_LIVE_SUB;
		strcpy(g_prc.args[_chn].dev_id, "");

		TRACE_INFO("chn=%d, start", _chn);
		g_prc.args[_chn].running = NVRPC_THREAD_RUNNING;
		pthread_create(&(g_prc.args[_chn].running_id), NULL, g_prc.protocol[protocol_index].run_handle, &g_prc.args[_chn]);
	}
	pthread_mutex_unlock(&g_prc.lock_thread_restart);
	return 0;
}

#ifdef _DNVR_VLAN
static int _lock_ip_mac_relation(char* _ip, char* _mac)
{
	return 0;
}

static int _unlock_ip_mac_relation(char* _ip)
{
	return 0;
}
#else
//Lock the relation between IP and MAC to ensure that LAN IPC is correct
static int _lock_ip_mac_relation(char* _ip, char* _mac)
{
	char cmd_buf[200];
	
	sprintf(cmd_buf, "arp -s %s %s", _ip, _mac);
	TRACE_DEBUG("%s", cmd_buf);
	system(cmd_buf);	
}

//Lock the relation between IP and MAC if IP isn't used
static int _unlock_ip_mac_relation(char* _ip)
{
	char cmd_buf[200];
	
	sprintf(cmd_buf, "arp -d %s", _ip);
	TRACE_DEBUG("%s", cmd_buf);
	system(cmd_buf);
}
#endif

static void _stop_channel(int _chn)
{
	pthread_mutex_lock(&g_prc.lock_thread_restart);
	if(g_prc.args[_chn].using_flag == 1)
	{
		g_prc.args[_chn].running = NVRPC_THREAD_STOP;
		pthread_join(g_prc.args[_chn].running_id, NULL);
		_unlock_ip_mac_relation(g_prc.args[_chn].ip);
		memset(&g_prc.args[_chn], 0, sizeof(NVR_ARG));
		TRACE_INFO("chn=%d stop", _chn);
	}
	pthread_mutex_unlock(&g_prc.lock_thread_restart);
}

static int _found_ipc_ip(void)
{
	int n,i;
	int is_used = 0;
	int tmp_lan_ip[4];
	int tmp_ip[4];
	int tmp_wan_ip[4];

	sscanf(g_prc.lan_ip, "%d.%d.%d.%d", &tmp_lan_ip[0], &tmp_lan_ip[1], &tmp_lan_ip[2], &tmp_lan_ip[3]);
	for(i = 0; i < 4; i++)
		tmp_wan_ip[i] = tmp_lan_ip[i];
	for(n = 11; n < 255; n++)
	{
		is_used = 0;
		if(n == tmp_lan_ip[3])
			continue;
		for(i = 0; i < MAX_CAM_CH; i++)
		{
			sscanf(g_prc.args[i].ip, "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
			if( n == tmp_ip[3] )
			{
				is_used = 1;
				break;
			}
		}
		if(is_used == 0)
		{
			tmp_wan_ip[3] = n;
			is_used = _check_wan_ipc_ip_filter(tmp_wan_ip);
		}
		if(is_used==0)
			break;
	}

	return n;
}


static int _modify_channel(MODIFY_CHANNEL_PARAMS* _params)
{
	_stop_channel(_params->chn);
	_start_dnvr_channel(_params->chn, _params);
	return 0;
}

static int _init_search_request(_DNVR_SEARCH_CTX* _ctx)
{
	//TRACE_DEBUG("%s begin", __FUNCTION__);
	memset(_ctx, 0, sizeof(_DNVR_SEARCH_CTX));
	_ctx->timeout = 1;

//	unsigned char recvbuf[BUFSIZE];
	unsigned char sendbuf[BUFSIZE];

	int ret;

	_ctx->sock = socket(AF_INET, SOCK_DGRAM, 0);

	struct ip_mreq mreq;
	bzero(&mreq, sizeof(struct ip_mreq));
	inet_pton(AF_INET, MCASTADDR, &mreq.imr_multiaddr.s_addr);
	/* 设置发送组播消息的源主机的地址信息 */
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	ret = setsockopt(_ctx->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port   = htons(MCASTPORT);
	local.sin_addr.s_addr = INADDR_ANY;
	ret = bind(_ctx->sock, (struct sockaddr *)&local, sizeof(local));
	if(ret != 0)
	{
//		ASSERT(ret == sent_count, "ret=%d, errno=%d", ret, errno);
		TRACE_DEBUG("bind ret=%d, errno=%d", ret, errno);
		close(_ctx->sock);
		return -1;
	}

	generate_client_id(_ctx->params.client_id);
	memset(sendbuf, 0, sizeof(sendbuf));
	snprintf(sendbuf, BUFSIZE, "SEARCH * HDS/1.0\r\n"
			"CSeq:1\r\n"
			"Client-ID:%s\r\n"
			"Accept-Type:text/HDP\r\n"
			"Content-Length:0\r\n"
			"X-Search-Type:DNVR\r\n"
			"\r\n\r\n",
			_ctx->params.client_id);
//	TRACE_DEBUG("sendbuf=\"%s\", len=%d", sendbuf, strlen(sendbuf));

	struct sockaddr_in remote;
	remote.sin_family      = AF_INET;
	remote.sin_port        = htons(MCASTPORT);
	remote.sin_addr.s_addr = inet_addr(MCASTADDR);
	int sent_count = strlen(sendbuf);
	ret = sendto(_ctx->sock, (char *)sendbuf, sent_count, 0,(struct sockaddr *)&remote, sizeof(remote));
	if(ret != sent_count)
	{
//		ASSERT(ret == sent_count, "ret=%d, errno=%d", ret, errno);
		TRACE_DEBUG("sendto ret=%d, errno=%d", ret, errno);
		close(_ctx->sock);
		return -1;
	}

//	TRACE_DEBUG("%s end", __FUNCTION__);
	return 0;
}

static int _add_ipc_search_result(char *search_buf)
{
	FILE *fd_IPCInfo = NULL;

	fd_IPCInfo = fopen(SEARCH_RESULT_IPCINFO, "ab+");
	assert(fd_IPCInfo);

	Write_IPCInfo(search_buf, fd_IPCInfo, IPCAM_HG);

	if(fd_IPCInfo){
		fclose(fd_IPCInfo);
		fd_IPCInfo = NULL;
	}
	return 0;
}

static void _reset_wan_ipc_mac_filter(void)
{
	int i;
	NVR_MAC_ITEM *mac_item;
	
	for(i = 0;i < IPC_WAN_MAX_NUM; i++)
	{
		mac_item = &g_prc.dnvr_search.mac_filter_list[i];
		mac_item->using_flag = 0;
	}
}

static void _reset_wan_ipc_ip_filter(void)
{
	int i;
	NVR_IP_ITEM *ip_item;
	
	for(i = 0;i < IPC_WAN_MAX_NUM; i++)
	{
		ip_item = &g_prc.dnvr_search.ip_filter_list[i];
		memset(ip_item,0,sizeof(NVR_IP_ITEM));
	}
}


static int _check_and_update_wan_ipc_mac_filter(int *mac)
{
	int i, j;
	int rlt = 0;
	NVR_MAC_ITEM *mac_item;

	#if 0
	printf("%s:mac is ",__FUNCTION__);
	for(j = 0; j < MAC_ADDR_LEN; j++)
		printf("%02x", mac[j]);
	printf("\n");	
	#endif
	for(i = 0; i < IPC_WAN_MAX_NUM; i++)
	{
		mac_item = &g_prc.dnvr_search.mac_filter_list[i];
		#if 0
		printf("%s:mac item,used_flag=%d, mac is ",__FUNCTION__, mac_item->using_flag);
		for(j = 0; j < MAC_ADDR_LEN; j++)
			printf("%02x", mac_item->mac[j] );
		printf("\n");	
		#endif
		if(mac_item->using_flag == 0)
		{
			mac_item->using_flag = 1;
			for(j = 0; j < MAC_ADDR_LEN; j++)
				mac_item->mac[j] = mac[j];
			rlt = 0;
			break;
		}
		if(mac[0] == mac_item->mac[0]
		&& mac[1] == mac_item->mac[1]
		&& mac[2] == mac_item->mac[2]
		&& mac[3] == mac_item->mac[3]
		&& mac[4] == mac_item->mac[4]
		&& mac[5] == mac_item->mac[5])
		{
			rlt = 1;
			break;
		}
	}

	return rlt;
}

static int _check_wan_ipc_ip_filter(int *ip)
{
	int i, j;
	int rlt = 0;
	NVR_IP_ITEM *ip_item;

	#if 0
	printf("%s:IP is ",__FUNCTION__);
	for(j = 0; j < IP_ADDR_LEN; j++)
		printf("%d.", ip[j]);
	printf("\n");	
	#endif
	for(i = 0; i < IPC_WAN_MAX_NUM; i++)
	{
		ip_item = &g_prc.dnvr_search.ip_filter_list[i];
		#if 0
		printf("%s:IP item,used_flag=%d, IP is ",__FUNCTION__,ip_item->using_flag);
		for(j = 0; j < IP_ADDR_LEN; j++)
			printf("%d.", ip_item->ip[j] );
		printf("\n");	
		#endif
		if(ip_item->using_flag == 0)
		{
			rlt = 0;
			break;
		}
		if(ip[0] == ip_item->ip[0]
		&& ip[1] == ip_item->ip[1]
		&& ip[2] == ip_item->ip[2]
		&& ip[3] == ip_item->ip[3])
		{
			rlt = 1;
			break;
		}
	}

	return rlt;
}

static int _check_and_update_wan_ipc_ip_filter(int *ip)
{
	int i, j;
	int rlt = 0;
	NVR_IP_ITEM *ip_item;

	#if 0
	printf("%s:IP is ",__FUNCTION__);
	for(j = 0; j < IP_ADDR_LEN; j++)
		printf("%d.", ip[j]);
	printf("\n");	
	#endif
	for(i = 0; i < IPC_WAN_MAX_NUM; i++)
	{
		ip_item = &g_prc.dnvr_search.ip_filter_list[i];
		#if 0
		printf("%s:IP item,used_flag=%d, IP is ",__FUNCTION__,ip_item->using_flag);
		for(j = 0; j < IP_ADDR_LEN; j++)
			printf("%d.", ip_item->ip[j] );
		printf("\n");	
		#endif
		if(ip_item->using_flag == 0)
		{
			ip_item->using_flag = 1;
			for(j = 0; j < IP_ADDR_LEN; j++)
				ip_item->ip[j] =ip[j];
			rlt = 0;
			break;
		}
		if(ip[0] == ip_item->ip[0]
		&& ip[1] == ip_item->ip[1]
		&& ip[2] == ip_item->ip[2]
		&& ip[3] == ip_item->ip[3])
		{
			rlt = 1;
			break;
		}
	}

	return rlt;
}



#define GET_IPC_RESULT_SUCCESS (0)
#define GET_IPC_RESULT_NOT_A_IPC (-1) //not a ipc, need get next
#define GET_IPC_RESULT_TIMEOUT (-2) //no more result, no need get more
#define GET_IPC_RESULT_ERROR (-3) //no more result, no need get more
#define GET_IPC_RESULT_UNKNOWN (-4) //no more result, no need get more
static int _get_ipc_result(_DNVR_SEARCH_CTX* _ctx)
{
	int returnret = GET_IPC_RESULT_UNKNOWN;

	unsigned char recvbuf[BUFSIZE];
	unsigned char savebuf[BUFSIZE];
//	unsigned char sendbuf[BUFSIZE];
	int ret;

	fd_set r;
	struct timeval t;

	t.tv_sec = _ctx->timeout;
	t.tv_usec = 0;


	FD_ZERO(&r);
	FD_SET(_ctx->sock, &r);

	ret = select(_ctx->sock + 1, &r, NULL, NULL, &t);
	if (ret > 0)
	{
		struct sockaddr_in from;
		socklen_t len = sizeof(from);
		memset(recvbuf, 0, sizeof(recvbuf));
		ret = recvfrom(_ctx->sock, recvbuf, BUFSIZE, 0, &from, &len);
		int errno_cpy = errno;
		if(ret < 0)
		{
			TRACE_ERROR("ret=%d, errno_cpy=%d receive %s", ret, errno_cpy, recvbuf);
		}
		#ifdef SDK_PLATFORM_HI3515
			#ifdef _DNVR_VLAN
				usleep(200*1000);
			#endif
		#endif

		if (strstr(recvbuf, "HDS/1.0 200 OK") && strstr(recvbuf,_ctx->params.client_id))
		{
			char *ip;
			char *id;
			char *mac;
			char *port;
	
			id = strstr(recvbuf, "Device-ID=");
			ip = strstr(recvbuf, "IP=");
			mac = strstr(recvbuf, "MAC=");
			port = strstr(recvbuf, "Http-Port=");

			if(id && ip && mac && port)
			{
				if (g_prc.dnvr_search.wan_searching)
				{
					memset(savebuf, 0, sizeof(savebuf));
					strcpy(savebuf,recvbuf);
					_add_ipc_search_result(savebuf);
				}
				
				id += strlen("Device-ID=");
				ip += strlen("IP=");
				mac += strlen("MAC=");
				port += strlen("Http-Port=");

				*strstr(id, "\r\n") = 0;
				*strstr(ip, "\r\n") = 0;
				*strstr(mac, "\r\n") = 0;
				*strstr(port, "\r\n") = 0;
//					TRACE_DEBUG("parse params success, id=%s, ip=%s, mac=%s, port=%s", id, ip, mac, port);

				strcpy(_ctx->params.id, id);
				sscanf(ip, "%d.%d.%d.%d", &_ctx->params.ip[0], &_ctx->params.ip[1], &_ctx->params.ip[2], &_ctx->params.ip[3]);
				_ctx->params.port = atoi(port);
				sscanf(mac, "%x:%x:%x:%x:%x:%x", &_ctx->params.mac[0], &_ctx->params.mac[1], &_ctx->params.mac[2], &_ctx->params.mac[3], &_ctx->params.mac[4], &_ctx->params.mac[5]);
				//TRACE_INFO("ip=%s,mac=%s,id=%s", ip,mac,id);

				returnret = GET_IPC_RESULT_SUCCESS;
			}
			else
			{
				TRACE_INFO("parse params error, buf=\"%s\"", recvbuf);
				returnret = GET_IPC_RESULT_ERROR;
			}
		}
		else
		{
			returnret = GET_IPC_RESULT_NOT_A_IPC;
		}
	}
	else if(ret == 0)
	{
		returnret = GET_IPC_RESULT_TIMEOUT;
	}
	else
	{
		TRACE_INFO("select error");
		returnret = GET_IPC_RESULT_ERROR;
	}

	return returnret;
}


static int _check_ipc_ip(MODIFY_CHANNEL_PARAMS* _params)
{
	int rlt = 0;
	int i = 0;
	int tmp_ip[4];
	char tmp_ip_str[32];

	//IP Area
	sprintf(tmp_ip_str, "%d.%d.%d.%d", _params->ip[0], _params->ip[1], _params->ip[2], _params->ip[3]);
	sscanf(g_prc.lan_ip, "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
	if( (_params->ip[0]!=tmp_ip[0]) || (_params->ip[1]!=tmp_ip[1]) || (_params->ip[2]!=tmp_ip[2]) || (_params->ip[3] <= 10) )
	{
		TRACE_DEBUG("%s: IP(%s) is not in valid area", __FUNCTION__, tmp_ip_str);
		return 1;
	}
	//LAN IP
	if(_params->ip[3]==tmp_ip[3])
	{
		TRACE_DEBUG("%s: IP(%s) is same with NVR", __FUNCTION__, tmp_ip_str);
		return 1;
	}
	//IP List
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			if(strcmp(tmp_ip_str,g_prc.args[i].ip)==0 && strcasecmp(g_prc.args[i].dev_id, _params->id) != 0)
			{
				rlt = 1;
				TRACE_DEBUG("%s: IP(%s) is same with other IPC(%d,%d)", __FUNCTION__, tmp_ip_str,_params->chn,i);
				break;
			}
		}
	}	
	
	if(rlt == 0)
		rlt = _check_wan_ipc_ip_filter(_params->ip);

	return rlt;
}


static int _check_switch_link(unsigned long current_link,unsigned long last_link)
{
	int i;
	unsigned long active_link;
	unsigned long check_bit;

	active_link = current_link ^ last_link;
	//TRACE_INFO("link active=0x%x,current=0x%x,last=0x%x", active_link, current_link, last_link);
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		check_bit = (1 << i);
		//TRACE_INFO("check bit=0x%x,using flag=%d", check_bit, g_prc.args[i].using_flag);
		if( (active_link & check_bit) && (!(current_link & check_bit)) && (g_prc.args[i].using_flag) )
		{
			TRACE_INFO("IPC(%d) Stop 1!",i);
//			pthread_mutex_lock(&g_prc.lock_thread_restart);
//			g_prc.args[i].using_flag = 0;
//			g_prc.args[i].running = NVRPC_THREAD_STOP;
//			pthread_join(g_prc.args[i].running_id, NULL);
//			memset(&g_prc.args[i], 0, sizeof(NVR_ARG));
//			pthread_mutex_unlock(&g_prc.lock_thread_restart);
			_stop_channel(i);
			#ifdef _IPC_CHECK_MODE	
				DVRCommand_t cmd;
				cmd.type = CMD_NOTICE;
				cmd.flag = DVR_CMD_FLAG;
				cmd.notice_info.enSubType = NOTICE_CHECKINFO_CLEAR;
				cmd.notice_info.chk_chn = i;
				SEND_DVRCMD_ACK(cmd, socket_index_gui);
				printf("IPC(channel %d) clear check information\n", i);
				g_prc.args[i].stop_flag = 0;
			#endif
			_start_nvr_channel(i);
		}
	}
}

static int _stop_all_channel(void)
{
	int i;

	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if( g_prc.args[i].using_flag )
		{
			TRACE_INFO("IPC(%d) Stop 3!",i);
//			pthread_mutex_lock(&g_prc.lock_thread_restart);
//			g_prc.args[i].using_flag = 0;
//			g_prc.args[i].running = NVRPC_THREAD_STOP;
//			pthread_join(g_prc.args[i].running_id, NULL);
//			memset(&g_prc.args[i], 0, sizeof(NVR_ARG));
//			pthread_mutex_unlock(&g_prc.lock_thread_restart);
			_stop_channel(i);
		}
	}
	
	return 0;
}

static void _update_lan_ipaddr(int update_mode)
{
	unsigned char lan_ipaddr[4]={0},ipaddr[4]={0};	
	unsigned char bDHCPUse;
	char tmp_ipaddr[64];

	bDHCPUse = g_pstSysEnv->IsDHCPUse(g_pstSysEnv);
	if(bDHCPUse == 1)  //DHCP MODE
	{
		if( (update_mode & 0x2) != 0x2) //If not dhcp enable,then return
			return;
		#if 1
			if( (update_mode & 0x1) != 0x1 )  //If not all enable then disable continue to check ip
				set_lan_ip_status(0);
			if(!get_ipaddr(ETH_NETWORK_DEV, ipaddr)) //Get WAN IP
			{				
				sprintf(tmp_ipaddr, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
				TRACE_DEBUG("WAN IP 2 is %s", tmp_ipaddr);
			}
			else
			{
				TRACE_DEBUG("Get WAN IP is fail (DHCP)");	
			}
			generate_lan_ipaddr(ipaddr,lan_ipaddr);
			sprintf(g_prc.lan_ip, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
			unsigned char ipaddr1[4]={0};
			if(!get_ipaddr(ETH_NETWORK_DEV1, ipaddr1))  //Get LAN IP
			{
				//If lan ip is same then don't change
				if( (lan_ipaddr[0]==ipaddr1[0]) && (lan_ipaddr[1]==ipaddr1[1]) && (lan_ipaddr[2]==ipaddr1[2]) && (lan_ipaddr[3]==ipaddr1[3]) )
				{
					TRACE_DEBUG("LAN IP(%s) is same with old", g_prc.lan_ip);
					return;
				}
			}			
			//Change lan ip avoid confilct
			sprintf(g_prc.lan_ip, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
			TRACE_DEBUG("LAN IP become %s", g_prc.lan_ip);
			set_ipaddr(ETH_NETWORK_DEV1, g_prc.lan_ip);
			add_multicast_route(3);
			_stop_all_channel();			
		#else
			if(get_ipaddr(ETH_NETWORK_DEV, ipaddr))
				g_pstSysEnv->GetIPAddr(g_pstSysEnv, ipaddr);
			char tmp_ipaddr[64];
			sprintf(tmp_ipaddr, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
			TRACE_DEBUG("WAN IP 2 is %s", tmp_ipaddr);	
			generate_lan_ipaddr(ipaddr,lan_ipaddr);
			sprintf(g_prc.lan_ip, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
			TRACE_DEBUG("LAN IP 2 is %s",g_prc.lan_ip);
			set_lan_ip_status(0);
		#endif
	}
	else  //Static IP Mode
	{
		if( (update_mode & 0x1) != 0x1 ) //if not static ip enable,then return
			return;
		if(get_ipaddr(ETH_NETWORK_DEV, ipaddr))
			g_pstSysEnv->GetIPAddr(g_pstSysEnv, ipaddr);
		sprintf(tmp_ipaddr, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
		TRACE_DEBUG("WAN IP 2 is %s", tmp_ipaddr);			
		generate_lan_ipaddr(ipaddr,lan_ipaddr);
		sprintf(g_prc.lan_ip, "%d.%d.%d.%d", lan_ipaddr[0], lan_ipaddr[1], lan_ipaddr[2], lan_ipaddr[3]);
		TRACE_DEBUG("LAN IP 2 is %s",g_prc.lan_ip);
		set_lan_ip_status(0);
	}

}

static int _check_param(MODIFY_CHANNEL_PARAMS* _params)
{
	int i;
	char ip_tmp[64];
//	char mac_tmp[64];
	sprintf(ip_tmp, "%d.%d.%d.%d", _params->ip[0], _params->ip[1], _params->ip[2], _params->ip[3]);
//	sprintf(mac_tmp, "%02x-%02x-%02x-%02x-%02x-%02x", _params->mac[0], _params->mac[1], _params->mac[2], _params->mac[3], _params->mac[4], _params->mac[5]);

	int old_running = -1;
	for(i = 0; i < MAX_CAM_CH; i++)
	{
//		TRACE_DEBUG("i=%d, using_flag=%d, ip=%s, port=%d", i, g_prc.args[i].using_flag, g_prc.args[i].ip, g_prc.args[i].port);
		if(g_prc.args[i].using_flag == 1)
		{
			if(strcasecmp(g_prc.args[i].ip, ip_tmp) == 0 && g_prc.args[i].port == _params->port && strcasecmp(g_prc.args[i].dev_id, _params->id) == 0)
			{
				old_running = i;
			}
		}
	}
	#ifdef _IPC_CHECK_MODE
		if(old_running == -1)
		{
			for(i = 0; i < MAX_CAM_CH; i++)
			{
				//TRACE_DEBUG("i=%d, using_flag=%d, ip=%s, port=%d", i, g_prc.args[i].using_flag, g_prc.args[i].ip, g_prc.args[i].port);
				if(g_prc.args[i].stop_flag == 1)
				{
					if(strcasecmp(g_prc.args[i].dev_id, _params->id) == 0)
					{
						old_running = i;
					}
				}
			}				
		}
	#endif

	return old_running;
}

static int _destory_search_request(_DNVR_SEARCH_CTX* _ctx)
{
//	TRACE_DEBUG("%s begin", __FUNCTION__);
	close(_ctx->sock);
	_ctx->sock = 0;
//	TRACE_DEBUG("%s end", __FUNCTION__);
	return 0;
}

static void* dnvr_thread_func(void* _args)
{
	SWT_Init();

//	int sock;
	int ret,i;
//	int s;
	_DNVR_SEARCH_CTX search_ctx;
//	MAC_NOTIFY_MSG msg;

	unsigned long last_loss = 0, last_loss_all = 0;
	unsigned long loss_change_count = 0;
	time_t last_time = time(NULL);
	time_t last_keep_time = time(NULL);
//	int is_finding = 0;
	int search_time = 60;

	g_prc.dnvr_search.lan_searching = 0;
	g_prc.dnvr_search.wan_search_count = 0;
	while(g_prc.dnvr_thread_running == 1)
	{
		time_t tmp_time = time(NULL);
		unsigned long tmp_loss, tmp_loss_all;

		tmp_loss_all = SWT_phy_port_getloss();
		#if(WAN_PORT_NO == 1)
			tmp_loss = (tmp_loss_all >> 1);   //WAN Port is lowest bit;
		#else
			tmp_loss = (tmp_loss_all & LAN_PORT_BITMASK);  //WAN Port is highest bit;
		#endif
		if(tmp_loss_all != last_loss_all)
		{
			unsigned long tmp_change = (tmp_loss_all ^ last_loss_all);
			//Start search if only WAN Port is not changed
			TRACE_INFO("found a loss event(now=0x%x,last=0x%x)", tmp_loss_all, last_loss_all);
			if( tmp_change != WAN_PORT_BITMASK )
			{
				if( (tmp_change & tmp_loss_all) == 0)
				{
					TRACE_INFO("it's only unplug line from lan port!");
				}
			}
			else
			{
				TRACE_INFO("it's changed only wan port!");
			}
			last_loss_all = tmp_loss_all;
		}
		if(tmp_loss != last_loss)
		{
			TRACE_INFO("found a lan loss event(now=0x%x,last=0x%x)", tmp_loss, last_loss);
			#ifndef _IPC_CHECK_MODE
			//Check two times as circuit is perhaps unstable
			if(loss_change_count++ < 2)
			{
				sleep(1);
				continue;
			}
			else
			{
				loss_change_count = 0;
			}
			#endif
			_check_switch_link(tmp_loss,last_loss); //Disconnect video channel;
			last_time = tmp_time;
			last_loss = tmp_loss;
		}

		if(tmp_time - last_time > 30)
		{
			if(get_lan_ip_status())
			{
				_update_lan_ipaddr(0x2);
			}
		}

		if(abs(tmp_time - last_keep_time) >= 3600)
		//if(abs(tmp_time - last_keep_time) >= 120)
		{
			last_keep_time = tmp_time;
			SWT_keep_config();
		}
		
		if(tmp_time - last_time > search_time)
		{
			search_time = 60;
			if(g_prc.dnvr_search.lan_searching)
			{
				g_prc.dnvr_search.lan_searching = 0;
				_reset_wan_ipc_ip_filter();
			}
			if(g_prc.dnvr_search.wan_searching == 0)
				g_prc.dnvr_search.wan_search_count = 0;
			set_lan_multicast_status(0);
			add_multicast_route(0);
			//TRACE_DEBUG("search not running, tmp=%ld, last=%ld", tmp_time, last_time);
			#if 1
			if(++i >= 60)
			{
				TRACE_DEBUG("search not running, tmp=%ld, last=%ld", tmp_time, last_time);
				i = 0;
			}
			#endif
			sleep(1);
			continue;
		}
		TRACE_DEBUG("search running, tmp=%ld, last=%ld", tmp_time, last_time);

		if(get_lan_ip_status())
		{
			_update_lan_ipaddr(0x1);
		}

		if(g_prc.dnvr_search.wan_searching == 1)
		{
			g_prc.dnvr_search.lan_searching = 0;
			search_time += 2;
			sleep(1);
			continue;
		}
		
		//FIXME:UPNP_SERVER also use MCASTADDR("239.255.255.250") temporarily and need to avoid confilct.
		add_multicast_route(1);
		g_prc.dnvr_search.lan_searching = 1;
		ret = _init_search_request(&search_ctx);
		if(ret != 0)
		{
			TRACE_ERROR("_init_search_request failed");
			sleep(1);  //decrease try number if error
			continue;
		}

		while(1)
		{
			ret = _get_ipc_result(&search_ctx);
			if(ret == GET_IPC_RESULT_SUCCESS)
			{
				int old_running = _check_param(&search_ctx.params);
				int phy_port = SWT_Mac_hit_port(search_ctx.params.mac);
				int tmp_ip[4];
				TRACE_INFO("got a ipc, id=%s ip=%03d.%03d.%03d.%03d, port=%05d, mac=%02x-%02x-%02x-%02x-%02x-%02x, phy_port=%02d, old_running=%02d",
						search_ctx.params.id,
						search_ctx.params.ip[0], search_ctx.params.ip[1], search_ctx.params.ip[2], search_ctx.params.ip[3], search_ctx.params.port,
						search_ctx.params.mac[0], search_ctx.params.mac[1], search_ctx.params.mac[2], search_ctx.params.mac[3], search_ctx.params.mac[4], search_ctx.params.mac[5],
						phy_port, old_running);
				if(phy_port < 0)  //Not find port
				{

				}
				else if(phy_port == (WAN_PORT_NO-1))  //wan devices
				{
					sscanf(g_prc.lan_ip, "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
					if(search_ctx.params.ip[0] == tmp_ip[0] 
						&& search_ctx.params.ip[1] == tmp_ip[1]
						&& search_ctx.params.ip[2] == tmp_ip[2])
					{
						_check_and_update_wan_ipc_ip_filter(search_ctx.params.ip);					
					}
				}
				else if (phy_port <= MAX_CAM_CH)
				{			
					#if(WAN_PORT_NO == 1)
						phy_port = phy_port -1;
					#endif
					if(old_running != phy_port)
					{
						char ip_str[32];
						search_ctx.params.chn = phy_port;
						int is_need_change_ip = _check_ipc_ip(&search_ctx.params);	
						if(is_need_change_ip)
						{
							sscanf(g_prc.lan_ip, "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
							search_ctx.params.ip[0] = tmp_ip[0];
							search_ctx.params.ip[1] = tmp_ip[1];
							search_ctx.params.ip[2] = tmp_ip[2];
							search_ctx.params.ip[3] = _found_ipc_ip();
							sprintf(g_prc.args[phy_port].ip, "%d.%d.%d.%d", search_ctx.params.ip[0], search_ctx.params.ip[1],
								search_ctx.params.ip[2], search_ctx.params.ip[3]);
							//TRACE_INFO("Free IP is %s",g_prc.args[phy_port].ip);
							sleep(1);
							//_modify_dev_hg(search_ctx.params.id,g_prc.args[phy_port].ip, "",0,"admin", "admin");							
							sprintf(ip_str, "%d.%d.%d.%d", search_ctx.params.ip[0], search_ctx.params.ip[1], search_ctx.params.ip[2], search_ctx.params.ip[3]);
							//char mac_str[32];
							//sprintf(mac_str, "%02x.%02x.%02x.%02x.%02x.%02x", search_ctx.params.mac[0], search_ctx.params.mac[1], search_ctx.params.mac[2], search_ctx.params.mac[3], search_ctx.params.mac[4], search_ctx.params.mac[5]);
							char netmask[32];
							sprintf(netmask, "%s", "255.255.255.0");
							char gateway[32];
							sprintf(gateway, "%d.%d.%d.1", search_ctx.params.ip[0], search_ctx.params.ip[1], search_ctx.params.ip[2]);
							_modify_dev_hg(search_ctx.params.id, ip_str, "", netmask, gateway, search_ctx.params.port, "admin", "admin", "soft");
							//TRACE_INFO("Modify IP is completed!");
						}
						else
							sprintf(ip_str, "%d.%d.%d.%d", search_ctx.params.ip[0], search_ctx.params.ip[1], search_ctx.params.ip[2], search_ctx.params.ip[3]);												
						_modify_channel(&search_ctx.params);
						char mac_str[20];
						sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", search_ctx.params.mac[0], search_ctx.params.mac[1], search_ctx.params.mac[2], 
							search_ctx.params.mac[3], search_ctx.params.mac[4], search_ctx.params.mac[5]);
						_lock_ip_mac_relation(ip_str,mac_str);						
					}
				}
			}
			else if(ret == GET_IPC_RESULT_NOT_A_IPC)
			{
				TRACE_DEBUG("not a ipc, continue;");
				continue;
			}
			else if(ret == GET_IPC_RESULT_TIMEOUT)
			{
				TRACE_DEBUG("no more ipc, break;");
				break;
			}
			else if(ret == GET_IPC_RESULT_ERROR)
			{
				TRACE_DEBUG("ipc search fail, break;");
				break;
			}
		}

		_destory_search_request(&search_ctx);
	}

//dnvr_thread_exit:
//	close(sockfd);
	SWT_Destroy();
	return NULL;
}

#endif

void NVRPC_init()
{
	sleep(2);
	memset(&g_prc, 0, sizeof(NVR_PROTOCOL_COLLECTION));

#ifdef _DNVR
	pthread_mutex_init(&g_prc.lock_thread_restart, NULL);
	_update_lan_ipaddr(0x3);
	add_multicast_route(3);
#endif

	extern int* NVRPC_hg_run(NVR_ARG* arg);
	extern int  NVRPC_hg_connected(NVR_ARG* arg);
	extern int  NVRPC_hg_live_switch(NVR_ARG* arg, NVR_ARG_LIVE live);
	g_prc.protocol[0].using_flag = 1;
	strcpy(g_prc.protocol[0].name, IPCAM_PROTOCOL_NAME_STRING[IPCAM_HG]);
	g_prc.protocol[0].run_handle = NVRPC_hg_run;
	g_prc.protocol[0].connected_handle = NVRPC_hg_connected;
	g_prc.protocol[0].live_switch_handle = NVRPC_hg_live_switch;

//	extern int* NVRPC_voc_run(NVR_ARG* arg);
//	extern int  NVRPC_voc_connected(NVR_ARG* arg);
//	g_prc.protocol[1].using_flag = 1;
//	strcpy(g_prc.protocol[1].name, "voc");
//	g_prc.protocol[1].run_handle = NVRPC_voc_run;
//	g_prc.protocol[1].connected_handle = NVRPC_voc_connected;

	ENV_BOOLEAN azEnable_ch[MAX_CAM_CH] = {0};
	unsigned char au8IPAddr[MAX_CAM_CH][4] = {0};
	unsigned short au16Port[MAX_CAM_CH] = {0};
	IPCAM_PROTOCOL_NAME azProtocolname[MAX_CAM_CH] = {0};
	char azPassword[MAX_CAM_CH][24] = {0};
	char azUsername[MAX_CAM_CH][24] = {0};
	int i;

	for(i = 0; i < MAX_CAM_CH; ++i)
	{
		if(i == -1)
		{
			azEnable_ch[i] = ENV_TRUE;
			au8IPAddr[i][0] = 192;
			au8IPAddr[i][1] = 168;
			au8IPAddr[i][2] = 1;
			au8IPAddr[i][3] = 78;
			au16Port[i] = 9080;
			azProtocolname[i] = 0;
			strcpy(&azUsername[i][0], "admin");
			strcpy(&azPassword[i][0], "admin");
		}
		else
		{
			azEnable_ch[i] = g_pstSysEnv->IsEnableIPCChannel(g_pstSysEnv, i);
			g_pstSysEnv->GetIPCamAddr(g_pstSysEnv, i, au8IPAddr[i]);
			au16Port[i] = g_pstSysEnv->GetIPCamPort(g_pstSysEnv, i);
			azProtocolname[i] = g_pstSysEnv->GetIPCamProtocolname(g_pstSysEnv, i);
			g_pstSysEnv->GetIPCamUsername(g_pstSysEnv, i, azUsername[i]);
			g_pstSysEnv->GetIPCamPassword(g_pstSysEnv, i, azPassword[i]);
		}
	}

	for(i = 0; i < MAX_CAM_CH; ++i)
	{
		g_prc.args[i].using_flag = azEnable_ch[i];
		g_prc.args[i].chn = i;
		sprintf(g_prc.args[i].ip, "%d.%d.%d.%d", au8IPAddr[i][0], au8IPAddr[i][1], au8IPAddr[i][2], au8IPAddr[i][3]);
		g_prc.args[i].port = (int)au16Port[i];
		strcpy(g_prc.args[i].username, azUsername[i]);
		strcpy(g_prc.args[i].password, azPassword[i]);
		strcpy(g_prc.args[i].protocol_name, IPCAM_PROTOCOL_NAME_STRING[azProtocolname[i]]);
		g_prc.args[i].phy_port = MAX_CAM_CH;
		g_prc.args[i].running_id = 0;
//		g_prc.args[i].connected = NVR_ARG_CONNECTED_FALSE;
		g_prc.args[i].live = NVR_ARG_LIVE_SUB;
		printf("-------------------------------\r\n");
		printf("using = %d\r\n", g_prc.args[i].using_flag);
		printf("chn = %d\r\n", g_prc.args[i].chn);
		printf("ip = %s\r\n", g_prc.args[i].ip);
		printf("port = %d\r\n", g_prc.args[i].port);
		printf("username = %s\r\n", g_prc.args[i].username);
		printf("password = %s\r\n", g_prc.args[i].password);
		printf("protocol name = %s\r\n", g_prc.args[i].protocol_name);
		printf("-------------------------------\r\n");
	}


#ifdef _DNVR
	pthread_mutex_lock(&g_prc.lock_thread_restart);
#endif
	int protocol_index = -1;
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			protocol_index = _found_protocol(g_prc.args[i].protocol_name);
			if(protocol_index != -1)
			{
				TRACE_INFO("chn %d protocol %s init", i, g_prc.args[i].protocol_name);
				g_prc.args[i].running = NVRPC_THREAD_RUNNING;
				pthread_create(&(g_prc.args[i].running_id), NULL, g_prc.protocol[protocol_index].run_handle, &g_prc.args[i]);
			}
		}
	}
#ifdef _DNVR
	pthread_mutex_unlock(&g_prc.lock_thread_restart);
#endif

#ifdef _DNVR
	g_prc.dnvr_thread_running = 1;
	pthread_create(&g_prc.dnvr_thread_id, NULL, dnvr_thread_func, NULL);
#endif
}


void NVRPC_destory()
{
	int i;
#ifdef _DNVR
	pthread_mutex_lock(&g_prc.lock_thread_restart);
#endif
	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			g_prc.args[i].running = NVRPC_THREAD_STOP;
		}
	}

	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if(g_prc.args[i].using_flag == 1)
		{
			TRACE_INFO("stoping i=%d, running_id=%ld", i, g_prc.args[i].running_id);
			pthread_join(g_prc.args[i].running_id, NULL);
			#ifdef _DNVR
			_unlock_ip_mac_relation(g_prc.args[i].ip);
			#endif
			TRACE_INFO("chn %d protocol %s destory", i, g_prc.args[i].protocol_name);
		}
	}
#ifdef _DNVR
	pthread_mutex_unlock(&g_prc.lock_thread_restart);
#endif

#ifdef _DNVR
	g_prc.dnvr_thread_running = NVRPC_THREAD_STOP;
	pthread_join(g_prc.dnvr_thread_id, NULL);
#endif

#ifdef _DNVR
	pthread_mutex_destroy(&g_prc.lock_thread_restart);
#endif
}


int NVRPC_Search(IPCAM_PROTOCOL_NAME type)
{
	int count = 0, wait_time = 0;
	FILE *fd_IPCInfo = NULL;
	time_t last_time = 0, current_time = 0;

	#ifdef _DNVR
	#if 0
	_reset_wan_ipc_mac_filter();
	if(g_prc.dnvr_search.lan_searching)
	{
		g_prc.dnvr_search.wan_searching = 1;
		sleep(IPC_SEARCH_TIME);
		count = g_prc.dnvr_search.wan_search_count;
		g_prc.dnvr_search.wan_searching = 0;
		sleep(1);  //wait thread to stop add wan ipc
		g_prc.dnvr_search.wan_search_count = 0;
		return count;
	}
	#else
	g_prc.dnvr_search.wan_searching = 1;
	last_time = time(NULL);
	while(g_prc.dnvr_search.lan_searching)
	{
		usleep(100*1000);
		current_time = time(NULL);
		if( abs(current_time - last_time) > IPC_SEARCH_TIME)
			break;
	}
	_reset_wan_ipc_mac_filter();
	#endif
	#endif
	
	fd_IPCInfo = fopen(SEARCH_RESULT_IPCINFO, "wb+");
	assert(fd_IPCInfo);
	printf("protocal type = %s\r\n", IPCAM_PROTOCOL_NAME_STRING[type]);
	switch(type)
	{
		case IPCAM_HG:
				count += Search_hg(fd_IPCInfo);
			break;
		case IPCAM_VOC:
			break;
		case IPCAM_RTSP:
			break;
		case IPCAM_PROTOCOL_CNT:
		default:
				count += Search_hg(fd_IPCInfo);
			break;
	}

	if(fd_IPCInfo){
		fclose(fd_IPCInfo);
		fd_IPCInfo = NULL;
	}
	#ifdef _DNVR
		g_prc.dnvr_search.wan_searching = 0;
	#endif
	return count;
}

#undef BUFSIZE
#define BUFSIZE (2048)
int NVRPC_send_mcas(char* _mcasaddr, int _port, NVRPC_send_mcas_callback _on_send, NVRPC_send_mcas_callback _on_recv)
{
	struct ip_mreq mreq;
	int count = 0;
	unsigned char sendbuf[BUFSIZE];

	struct sockaddr_in local;
	struct sockaddr_in remote;
	struct sockaddr_in from;

	int sock;
	int len = sizeof(struct sockaddr_in);
	int ret;

	memset(&mreq, 0, sizeof(struct ip_mreq));
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	local.sin_family = AF_INET;
	local.sin_port   = htons(_port);
	local.sin_addr.s_addr = INADDR_ANY;

	inet_pton(AF_INET, _mcasaddr, &mreq.imr_multiaddr.s_addr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	bind(sock, (struct sockaddr *)&local, sizeof(local));

	remote.sin_family      = AF_INET;
	remote.sin_port        = htons(_port);
	remote.sin_addr.s_addr = inet_addr(_mcasaddr);

	ret = _on_send(sendbuf, BUFSIZE);
	TRACE_DEBUG("_on_send ret=%d", ret);
	if(ret <= 0)
	{
		return -1;
	}
	ret = sendto(sock, sendbuf, ret, 0,(struct sockaddr *)&remote, sizeof(remote));
	TRACE_DEBUG("sendto ret=%d", ret);

	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;
	while(1)
	{
		fd_set r;
		struct timeval t;

		t.tv_sec = 0;
		t.tv_usec = 200;

		FD_ZERO(&r);
		FD_SET(sock, &r);

		ret = select(sock + 1, &r, NULL, NULL, &t);
		if (ret > 0)
		{
			memset(sendbuf, 0, BUFSIZE);
			ret = recvfrom(sock, sendbuf, BUFSIZE, 0, (struct sockaddr *)&from, &len);
			TRACE_DEBUG("recvfrom ret=%d", ret);

			ret = _on_recv(sendbuf, ret);
			TRACE_DEBUG("_on_recv ret=%d", ret);
			if(ret <= 0)
			{
			}
			else
			{
				count++;
			}
		}
		curr_time = time(NULL);
		if(curr_time - begin_time > IPC_SEARCH_TIME){
			break;
		}
	}
	close(sock);
	TRACE_DEBUG("count=%d", count);
	return count;
}

#endif
