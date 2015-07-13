/*
 * nvrpc_protocol_hg.c
 *
 *  Created on: 2012-5-22
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#include "conf.h"
#include "std.h"
#include "avenc_buf.h"
#include "avenc.h"
#include "nvrpc_function.h"
#include "sdk/sdk_api.h"
#ifdef _IPC_CHECK_MODE
#include "net_manage.h"
#endif

#ifdef _NVR

#define CONNECT_FIALED_SLEEP (3)
#define RW_TIMEOUT 	(5)
#define MCASTADDR     "239.255.255.250"
#define MCASTPORT      8002
#define BUFSIZE        2048
#define DEFAULT_COUNT  500

/*------------------- define ---------------*/
#define PT_H264 96
#define PT_G726 97
#define PT_G711 8
#define PT_DATA 100

#define CHANNEL1 11 //1280*720
#define CHANNEL2 12 //640*352
#define STREAM_TYPE1 "video_audio_data"
#define STREAM_TYPE2 "video_audio"
#define STREAM_TYPE3 "video_data"
#define STREAM_TYPE4 "audio_data"
#define STREAM_TYPE5 "video"
#define STREAM_TYPE6 "audio"

typedef struct sRTP_HDR_S
{
	/* byte 0 */
	unsigned short cc :4; /* CSRC count */
	unsigned short x :1; /* header extension flag */
	unsigned short p :1; /* padding flag */
	unsigned short version :2; /* protocol version */
	/* byte 1 */
	unsigned short pt :7; /* payload type */
	unsigned short marker :1; /* marker bit */
	/* bytes 2, 3 */
	unsigned short seqno :16; /* sequence number */
	/* bytes 4-7 */
	unsigned int ts; /* timestamp in ms */
	/* bytes 8-11 */
	unsigned int ssrc; /* synchronization source */
} RTP_HDR_S;

typedef struct sRTSP_ITLEAVED_HDR_S
{
	unsigned char daollar; /*8, $:dollar sign(24 decimal)*/
	unsigned char channelid; /*8, channel id*/
	unsigned short resv; /*16, reseved*/
	unsigned int payloadLen; /*32, payload length*/
	RTP_HDR_S rtpHead; /*rtp head*/
} RTSP_ITLEAVED_HDR_S;

typedef struct
{
	int main_sock;
	int main_connected;
	time_t main_last_recv;
	int sub_sock;
	int sub_connected;
	time_t sub_last_recv;
}HG_PRIVATE_DATA;

typedef struct
{
		struct
		{
#define HG_V_TYPE_UNKNOWN (0)
#define HG_V_TYPE_H264 (1)
			int type;
			int width;
			int height;
		}v;
		struct
		{
#define HG_A_TYPE_UNKNOWN (0)
			int type;
		}a;
}HG_AV_PARAMS;

//#include <asm/ioctls.h>
#include <sys/ioctl.h>
#define TIMEOUT (5000)
#define BUF_LEN 2048
void base64Encode(char *intext, char *output)
{
	char table64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/@";
	unsigned char ibuf[3];
	unsigned char obuf[4];
	int i;
	int inputparts;

	while(*intext) {
		for (i = inputparts = 0; i < 3; i++) {
			if(*intext) {
				inputparts++;
				ibuf[i] = *intext;
				intext++;
			}
			else
				ibuf[i] = 0;
		}

		obuf [0] = (ibuf [0] & 0xFC) >> 2;
		obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
		obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
		obuf [3] = ibuf [2] & 0x3F;

		switch(inputparts) {
		case 1: /* only one byte read */
			sprintf(output, "%c%c==",
					table64[obuf[0]],
					table64[obuf[1]]);
			break;
		case 2: /* two bytes read */
			sprintf(output, "%c%c%c=",
					table64[obuf[0]],
					table64[obuf[1]],
					table64[obuf[2]]);
			break;
		default:
			sprintf(output, "%c%c%c%c",
					table64[obuf[0]],
					table64[obuf[1]],
					table64[obuf[2]],
					table64[obuf[3]] );
			break;
		}
		output += 4;
	}
	*output=0;
}

int ConnectIpCam(char *pServer, unsigned short Port)
{
	struct sockaddr_in SockAddr;
	int sock = -1;
	int s32Ret;

	struct timeval tv;
	fd_set writefds;
	int len, error;
	unsigned long non_blocking = 1;
	unsigned long blocking = 0;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket Error!\n");
		return -1;
	}

	tv.tv_sec = TIMEOUT/1000;
	tv.tv_usec = TIMEOUT%1000;

//	ioctlsocket(sock, FIONBIO, &non_blocking);
	ioctl(sock, FIONBIO, &non_blocking);

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(pServer);
	SockAddr.sin_port = htons(Port);
	s32Ret = connect(sock, (struct sockaddr *)&SockAddr, sizeof(SockAddr));
	if (s32Ret < 0)
	{
		FD_ZERO(&writefds);
		FD_SET((unsigned int)sock, &writefds);
		if(select(sock+1, NULL, &writefds, NULL, &tv) != 0)
		{
			if(FD_ISSET(sock, &writefds))
			{
				len = sizeof(error);

				if(getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
					goto error_ret;
				if(error != 0)
					goto error_ret;
			}
			else
				goto error_ret; //timeout or error happen
		}
		else
			goto error_ret;

//		ioctlsocket(sock, FIONBIO, &blocking);
		ioctl(sock, FIONBIO, &blocking);
	}
	else
	{
		error_ret:
		close(sock);
		sock = -1;
		printf("Connect Fail!\n");
		return -1;
	}

	return sock;
}


int SendData(int sock, unsigned char* pu8Buff, unsigned int u32DataLen)
{
	int s32Ret = 0;
	unsigned int u32RemSize =0;
	int s32Size    = 0;
	char*  ps8BufferPos = NULL;
	fd_set write_fds;
	struct timeval TimeoutVal;
	int s32Errno = 0;

	memset(&TimeoutVal,0,sizeof(struct timeval));

	u32RemSize = u32DataLen;
	ps8BufferPos = (char*)pu8Buff;
	while(u32RemSize > 0)
	{
		FD_ZERO(&write_fds);
		FD_SET(sock, &write_fds);
		TimeoutVal.tv_sec = TIMEOUT/1000;
		TimeoutVal.tv_usec = TIMEOUT%1000;

		s32Ret = select(sock + 1, NULL, &write_fds, NULL, &TimeoutVal);
		if (s32Ret > 0 )
		{
			if( FD_ISSET(sock, &write_fds))
			{
				s32Size = send(sock, ps8BufferPos, u32RemSize, 0);
				if (s32Size < 0)
				{
					if (errno != EAGAIN  && errno != EINTR)
						return -1;

					continue;
				}

				u32RemSize -= s32Size;
				ps8BufferPos += s32Size;
			}
			else
				return -1;

		}
		else if( s32Ret <= 0 )
			return -1;
	}

	return 0;
}

int ReadCmdInfo(int Sock, char* Ipaddr, char* Cgi, char* Buf, int* Buf_len, char* Auth)
{
	int s32Ret;
	int len = 0;
	struct timeval struTimeout;
	fd_set struReadSet;

	memset(Buf, 0, *Buf_len);
	sprintf(Buf, "GET %s HTTP/1.1\r\n", Cgi);
	strcpy(Buf+strlen(Buf), "Accept: */*\r\n");
	strcpy(Buf+strlen(Buf), "Accept-Language: zh-cn\r\n");
	strcpy(Buf+strlen(Buf), "Accept-Encoding: gzip, deflate\r\n");
	strcpy(Buf+strlen(Buf), "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\r\n");
	sprintf(Buf+strlen(Buf), "Host: %s\r\n", Ipaddr);
	strcpy(Buf+strlen(Buf), "Connection: Keep-Alive\r\n");
	sprintf(Buf+strlen(Buf), "Authorization: Basic %s\r\n", Auth);
	strcpy(Buf+strlen(Buf), "\r\n");
	len = strlen(Buf);

//	printf("%s\n", Buf);
	s32Ret = SendData(Sock, (unsigned char *)Buf, len);
	if(0 != s32Ret)
	{
		printf("Send Error!\n");
		return -1;
	}

	//如果CGI没有返回值，下面代码就不用，直接返回 HI_SUCCESS
	memset(Buf, 0, *Buf_len);
	len = 0;

	struTimeout.tv_sec = TIMEOUT/1000;
	struTimeout.tv_usec = TIMEOUT%1000;
	FD_ZERO(&struReadSet);
	FD_SET((unsigned int)Sock, &struReadSet);

	s32Ret = select(Sock + 1, &struReadSet, NULL, NULL, &struTimeout);
	if(s32Ret <= 0 )
	{
		printf("select Error!\n");
		return -1;
	}

	//memset(Buf, 0, *Buf_len);
	len = recv(Sock, Buf, *Buf_len, 0);
	if (len <= 0)
	{
		printf("Recv Error!\n");
		return -1;
	}

//	printf("%s\n", Buf);

	return 0;
}

int _send_cmd(char* _ip, int _port, char* _usr, char* _pwd, char* _cmd)
{
	char sAuth[128] = {0};
	char sTemp[128] = {0};
	sprintf(sTemp, "%s:%s", _usr, _pwd);
	base64Encode(sTemp, sAuth);

	char sBuf[BUF_LEN] = {0};
	int s32Len = BUF_LEN;

	int sock = ConnectIpCam(_ip, _port);
	if(sock == -1)
	{
		TRACE_ERROR("ConnectIpCam error");
		return -1;
	}

	int s32Ret = ReadCmdInfo(sock, _ip, _cmd, sBuf, &s32Len, sAuth);
	close(sock);
	sock = -1;
	if(s32Ret != 0)
	{
		TRACE_ERROR("ReadCmdInfo error");
		return -1;
	}
	return 0;
}

int __HG_send_cmd(char* _cmd, int _chn)
{
//	printf("%s\n", __FUNCTION__);
	unsigned int connection = NVRPC_get_connection();
//	printf("connection=%d\n", connection);

	if((connection & (1 << _chn)) == 0)
	{
//		printf("%d, connection ok\n", _chn);
		_send_cmd(g_prc.args[_chn].ip, g_prc.args[_chn].port, g_prc.args[_chn].username, g_prc.args[_chn].password, _cmd);
	}

	return 0;
}

int _login(char* _ip, int _port, char* _usr, char* _pwd, int _sockfd, int _main_or_sub, HG_AV_PARAMS* _av_params) //0 main, 1 sub
{

	char buf[1024];
	char tmp_buf[256];

	memset(tmp_buf, 0, 256);
	memset(buf, 0, 1024);

	sprintf(buf, "GET http://%s:%d/livestream/%d?action=play&media=%s HTTP/1.1\r\n", _ip, _port, _main_or_sub == 0 ? CHANNEL1 : CHANNEL2, STREAM_TYPE5);
	strcpy(buf + strlen(buf), "User-Agent: HiIpcam/V100R003 VodClient/1.0.0\r\n");
	strcpy(buf + strlen(buf), "Connection: Keep-Alive\r\n");
	strcpy(buf + strlen(buf), "Cache-Control: no-cache\r\n");
	sprintf(buf + strlen(buf), "Authorization: %s %s\r\n", _usr, _pwd);
	strcpy(tmp_buf, "Cseq: 1\r\n");
	strcpy(tmp_buf + strlen(tmp_buf), "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n\r\n");
	sprintf(buf + strlen(buf), "Content-Length: %d\r\n", strlen(tmp_buf));
	strcpy(buf + strlen(buf), "\r\n");
	strcpy(buf + strlen(buf), tmp_buf);

	//printf("_main_or_sub=%d,buf=%s\n", _main_or_sub,buf);

	SOCKET_RW_CTX ctx;
	NVRPC_rwinit(&ctx, _sockfd, buf, strlen(buf), RW_TIMEOUT);
	NVRPC_writen(&ctx);
	if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
	{
		printf("send request error \n");
		return -1;
	}

	char head_code[16] = {0};
	char head_session[16] = {0};
	char head_cseq[16] = {0};
	char head_video[128] = {0};
	char head_audio[128] = {0};
	int  head_finish = 0;

	char* line_head = buf;
	int read_count = 0;
	while(read_count < sizeof(buf))
	{
		NVRPC_rwinit(&ctx, _sockfd, buf + read_count, 1, RW_TIMEOUT);
		NVRPC_readn(&ctx);
		if(ctx.result !=  SOCKET_RW_RESULT_SUCCESS)
		{
			TRACE_INFO("ip:%s %s, ctx.actual_len=%d, ctx.errno_cpy=%d",
					_ip, NVRPC_get_rw_error(&ctx), ctx.actual_len, ctx.errno_cpy);
			return -1;
		}
//		TRACE_DEBUG("ctx.actual_len=%d", ctx.actual_len);

		int i;
		for(i = 0; i < ctx.actual_len; i++)
		{
//			TRACE_DEBUG("buf[read_count + i]=\"%c\", read_count=%d, i=%d", buf[read_count + i], read_count, i);
			if(buf[read_count + i] == '\n')
			{
				buf[read_count + i] = 0;
				if(buf[read_count + i - 1] == '\r')
				{
					buf[read_count + i - 1] = 0;
				}
//				TRACE_DEBUG("line_head=\"%s\"", line_head);

				if(strcmp(head_code, "200") != 0)
				{
					//HTTP/1.1 200 OK
					sscanf(line_head, "HTTP/1.1 %s OK", head_code);
				}
				else if(strlen(head_session) == 0)
				{
					//Session: 26649504
					sscanf(line_head, "Session: %s", head_session);
				}
				else if(strlen(head_cseq) == 0)
				{
					//Cseq: 1
					sscanf(line_head, "Cseq: %s", head_cseq);
				}
				else if(strlen(head_video) == 0)
				{
					//m=video 96 H264/90000/1280/720
					sscanf(line_head, "m=video %s H264/90000/%d/%d", head_video, &_av_params->v.width, &_av_params->v.height);
				}
				else if(strlen(head_audio) == 0)
				{
					//m=audio 97 G726/8000/1
					sscanf(line_head, "m=audio %s", head_audio);
				}
				else if(head_finish == 0 && strlen(line_head) == 0)
				{
					head_finish = 1;

//					TRACE_INFO("head_code=%s", head_code);
//					TRACE_INFO("head_session=%s", head_session);
//					TRACE_INFO("head_cseq=%s", head_cseq);
//					TRACE_INFO("head_video=%s, width=%d, height=%d", head_video, _av_params->v.width, _av_params->v.height);
//					TRACE_INFO("head_audio=%s", head_audio);

					return 0;
				}
				line_head = &buf[read_count + i + 1];
			}
		}
		read_count += ctx.actual_len;
	}

	TRACE_INFO("read_count=%d", read_count);
	return -1;
}

//int _get_frame(int* _running, int _sockfd,
//		unsigned char* _buf, int* _buf_len, int* _is_key, unsigned int* _timestamp,
//		RTSP_ITLEAVED_HDR_S* _header)
//{
////	RTSP_ITLEAVED_HDR_S header;
//	SOCKET_RW_CTX ctx;
//
//	int packet_len = 0;
//	int total_len = 0;
//	unsigned int timestamp = 0;
//	unsigned int last_timestamp = 0;
//	unsigned short last_pt = 0;
//	int need_head = 0;
//
//	while (*_running == 1)
//	{
//		if(_header->rtpHead.ts == 0 || need_head == 1)
//		{
//			NVRPC_rwinit(&ctx, _sockfd, _header, sizeof(RTSP_ITLEAVED_HDR_S), RW_TIMEOUT);
//			NVRPC_readn(&ctx);
//			if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
//			{
//				TRACE_INFO("recv header error, errno_cpy=%d", ctx.errno_cpy);
//				return -1;
//			}
//		}
//
//		need_head = 1;
//
//		packet_len = ntohl(_header->payloadLen) - sizeof(RTP_HDR_S);
//		timestamp = ntohl(_header->rtpHead.ts);
////		TRACE_INFO("packet_len=%d, timestamp=%ld", packet_len, timestamp);
//
//		if(last_timestamp != timestamp && last_timestamp != 0)
//		{
//			if(last_pt == PT_H264 && total_len > 0)
//			{
//
//				int nalu_type = _buf[4] & 0x1f;
//				if(nalu_type == 1)
//				{
//					*_is_key = 0;
//				}
//				else
//				{
//					*_is_key = 1;
//				}
//
//				*_timestamp = last_timestamp;
//				*_buf_len = total_len;
////				TRACE_DEBUG("got a frame len=%d, key=%d, timestamp=%u", total_len, *_is_key, *_timestamp);
//				return 0;
//			}
//		}
//
//		if(last_timestamp == timestamp)
//		{
//			NVRPC_rwinit(&ctx, _sockfd, _buf + total_len, packet_len, RW_TIMEOUT);
//			NVRPC_readn(&ctx);
//			if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
//			{
//				TRACE_INFO("recv body1 error, packet_len=%d, errno_cpy=%d", packet_len, ctx.errno_cpy);
//				return -1;
//			}
//			total_len += packet_len;
//		}
//		else
//		{
//			NVRPC_rwinit(&ctx, _sockfd, _buf, packet_len, RW_TIMEOUT);
//			NVRPC_readn(&ctx);
//			if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
//			{
//				TRACE_INFO("recv body2 error, packet_len=%d, errno_cpy=%d", packet_len, ctx.errno_cpy);
//				return -1;
//			}
//			total_len = packet_len;
//		}
//		if (ctx.actual_len != packet_len)
//		{
//			TRACE_INFO("recv data error, errno_cpy=%d", ctx.errno_cpy);
//			return -1;
//		}
//
//		last_pt = _header->rtpHead.pt;
//		last_timestamp = timestamp;
//	}
//
//
//	TRACE_INFO("running is stop");
//	return -1;
//}

#define PRIVATE_DATA() ((HG_PRIVATE_DATA*)arg->private_data)
#define CLEAR() \
{ \
	if(PRIVATE_DATA()->main_connected == 1) \
	{ \
		close(PRIVATE_DATA()->main_sock); \
		PRIVATE_DATA()->main_sock = 0; \
		PRIVATE_DATA()->main_connected = 0; \
	} \
	if(PRIVATE_DATA()->sub_connected == 1) \
	{ \
		close(PRIVATE_DATA()->sub_sock); \
		PRIVATE_DATA()->sub_sock = 0; \
		PRIVATE_DATA()->sub_connected = 0; \
	} \
	AVENC_rec_stop(arg->chn); \
}
#define TEST_EXIT() \
{ \
	if(arg->running != NVRPC_THREAD_RUNNING) \
	{ \
		goto clear_all; \
	} \
}

int NVRPC_hg_connected(NVR_ARG* arg)
{
	int ret = 1;
	if(arg->private_data != NULL)
	{
		if(PRIVATE_DATA()->main_connected == 1 && PRIVATE_DATA()->sub_connected == 1)
		{
			ret = 0;
		}
	}
//	TRACE_DEBUG("ret=%d", ret);
	return ret;
}

int NVRPC_hg_live_switch(NVR_ARG* arg, int _is_full)
{
	if(arg->live != NVR_ARG_LIVE_STOP)
	{
		if(_is_full == 1)
		{
			arg->live = NVR_ARG_LIVE_MAIN;
		}
		else
		{
			arg->live = NVR_ARG_LIVE_SUB;
		}
	}
	return 0;
}

#define BUF_SIZE (8*128*1024)
typedef struct
{
	RTSP_ITLEAVED_HDR_S hbuf;
	int hlen;

	unsigned int last_timestamp;


	int is_key;
	unsigned int time_stamp;
	unsigned char fbuf[BUF_SIZE];
	int ftmplen;
	int flen;


	time_t last_socket_read;
}_HG_GET_FRAME_CTX;

#define	MIN(a,b) (((a)<(b))?(a):(b))
#define	MAX(a,b) (((a)>(b))?(a):(b))
int _read_frame(int _sockfd, _HG_GET_FRAME_CTX* _ctx)
{
	int total;
//	TRACE_DEBUG("reading total");
	ioctl(_sockfd, FIONREAD, &total);
//	TRACE_DEBUG("total=%d", total);

	if(total == 0 && _ctx->last_socket_read > 0)
	{
		time_t now_t = time(NULL);
		if(now_t - _ctx->last_socket_read > RW_TIMEOUT)
		{
			TRACE_INFO("total==0 continue %d secs", RW_TIMEOUT);
			return -1;
		}
	}

	int checked = 0;
	int need_read;
	SOCKET_RW_CTX ctx;
	while(checked < total)
	{
		if(_ctx->hlen < sizeof(RTSP_ITLEAVED_HDR_S))
		{
			need_read = MIN(sizeof(RTSP_ITLEAVED_HDR_S) - _ctx->hlen, total - checked);
//			TRACE_DEBUG("head need_read=%d", need_read);
			NVRPC_rwinit(&ctx, _sockfd, (char*)&_ctx->hbuf + _ctx->hlen, need_read, RW_TIMEOUT);
			NVRPC_read(&ctx);
			if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
			{
				TRACE_INFO("NVRPC_read error, errno_cpy=%d", ctx.errno_cpy);
				return -1;
			}

			_ctx->last_socket_read = time(NULL);
			_ctx->hlen += need_read;
			checked += need_read;
//			TRACE_DEBUG("_ctx->hlen=%d, checked=%d", _ctx->hlen, checked);
			continue;
		}

		ASSERT(_ctx->hlen == sizeof(RTSP_ITLEAVED_HDR_S), "_ctx->hlen=%d", _ctx->hlen);

		int data_len = ntohl(_ctx->hbuf.payloadLen) - sizeof(RTP_HDR_S);
		unsigned int timestamp = ntohl(_ctx->hbuf.rtpHead.ts);
//		TRACE_DEBUG("data_len=%d, timestamp=%d, _ctx->last_timestamp=%d", data_len, timestamp, _ctx->last_timestamp);


		if(_ctx->last_timestamp == 0)
		{
			_ctx->last_timestamp = timestamp;
		}

		if(_ctx->last_timestamp != timestamp)
		{
//			TRACE_DEBUG("finish frame, _ctx->flen=%d", _ctx->flen);
			_ctx->last_timestamp = timestamp;

			_ctx->time_stamp = timestamp;
			int nalu_type = _ctx->fbuf[4] & 0x1f;
			if(nalu_type == 1)
			{
				_ctx->is_key = 0;
			}
			else
			{
				_ctx->is_key = 1;
			}
			return 1;
		}


//		TRACE_DEBUG("_ctx->ftmplen=%d", _ctx->ftmplen);
		if(_ctx->ftmplen < data_len)
		{
			need_read = MIN(data_len - _ctx->ftmplen, total - checked);
//			TRACE_DEBUG("data need_read=%d", need_read);
			NVRPC_rwinit(&ctx, _sockfd, _ctx->fbuf + _ctx->flen, need_read, RW_TIMEOUT);
			NVRPC_read(&ctx);
			if(ctx.result != SOCKET_RW_RESULT_SUCCESS)
			{
				TRACE_INFO("NVRPC_read error, errno_cpy=%d", ctx.errno_cpy);
				return -1;
			}

			_ctx->last_socket_read = time(NULL);
			_ctx->flen += need_read;
			_ctx->ftmplen += need_read;
			checked += need_read;
//			TRACE_DEBUG("_ctx->ftmplen=%d, checked=%d", _ctx->ftmplen, checked);
		}

		if(_ctx->ftmplen == data_len)
		{
			_ctx->hlen = 0;
			_ctx->ftmplen = 0;
		}

		_ctx->last_timestamp = timestamp;
	}
//	TRACE_DEBUG("checked=%d, total=%d", checked, total);
	return 0;
}

#ifdef _IPC_CHECK_MODE
static int http_url_encode(const char* in_str, ssize_t const in_size, char* out_str, ssize_t const out_size)
{
	int i;
	int j = 0; /* for out_str index */
	char ch;

	if ((in_str == NULL) || (out_str == NULL) || (in_size <= 0) || (out_size <= 0)) {
		return 0;
	}

	for (i=0; (i<in_size) && (j<out_size); i++) {
		ch = in_str[i];
		if ((ch >= 'A') && (ch <= 'Z')) {
			out_str[j++] = ch;
		} else if ((ch >= 'a') && (ch <= 'z')) {
			out_str[j++] = ch;
		} else if ((ch >= '0') && (ch <= '9')) {
			out_str[j++] = ch;
		} else if(ch == ' '){
			out_str[j++] = '+';
		} else {
			if (j + 3 < out_size) {
				sprintf(out_str+j, "%%%02X", (unsigned char)ch);
				j += 3;
			} else {
				return 0;
			}
		}
	}

	out_str[j] = '\0';
	return j;
}


int _jaipc_send_and_receive_cmd(const NVR_ARG* arg, char* _cgi, char* _buf, int _buf_len, int has_receive)
{
	int sock = ConnectIpCam(arg->ip, arg->port);
	if(sock == -1)
	{
		TRACE_ERROR("ConnectIpCam error");
		return -1;
	}

	int s32Ret;
	int len = 0;
	struct timeval struTimeout;
	fd_set struReadSet;

	char sAuth[128] = {0};
	char sTemp[128] = {0};
	sprintf(sTemp, "%s:%s", arg->username, arg->password);
	base64Encode(sTemp, sAuth);

	memset(_buf, 0, _buf_len);
	#if 0
	sprintf(_buf, "GET %s HTTP/1.1\r\n", _cgi);
	strcpy(_buf+strlen(_buf), "Accept: */*\r\n");
	strcpy(_buf+strlen(_buf), "Accept-Language: zh-cn\r\n");
	strcpy(_buf+strlen(_buf), "Accept-Encoding: gzip, deflate\r\n");
	strcpy(_buf+strlen(_buf), "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\r\n");
	sprintf(_buf+strlen(_buf), "Host: %s\r\n", arg->ip);
	strcpy(_buf+strlen(_buf), "Connection: Keep-Alive\r\n");
	sprintf(_buf+strlen(_buf), "Authorization: Basic %s\r\n", sAuth);
	strcpy(_buf+strlen(_buf), "\r\n");
	#else
	sprintf(_buf, "GET %s HTTP/1.1\r\n", _cgi);
	sprintf(_buf+strlen(_buf), "Host: %s\r\n", arg->ip);
	strcpy(_buf+strlen(_buf), "Connection: Keep-Alive\r\n");
	strcpy(_buf+strlen(_buf), "\r\n");
	#endif
	len = strlen(_buf);

	//TRACE_DEBUG("_buf=%s", _buf);

	s32Ret = SendData(sock, (unsigned char *)_buf, len);
	if(0 != s32Ret)
	{
		printf("Send Error!\n");
		return -2;
	}

	if(has_receive == 0)
	{
		//printf("Send OK!\n");
		return 0;
	}

	memset(_buf, 0, _buf_len);
	len = 0;

	struTimeout.tv_sec = TIMEOUT/1000;
	struTimeout.tv_usec = TIMEOUT%1000;
	FD_ZERO(&struReadSet);
	FD_SET((unsigned int)sock, &struReadSet);

	s32Ret = select(sock + 1, &struReadSet, NULL, NULL, &struTimeout);
	if(s32Ret <= 0 )
	{
		printf("select Error!\n");
		return -3;
	}

	len = recv(sock, _buf, _buf_len, 0);
	if (len <= 0)
	{
		printf("Recv Error!\n");
		return -4;
	}	

	close(sock);
	sock = -1;

	//TRACE_DEBUG("_buf=%s", _buf);
	
	return len;
}

int _jaipc_restore_default_one(const NVR_ARG* arg)
{
	char sCgi[256];
	char sTmp[256];
	char sBuf[BUF_LEN];
	int rlt;

	memset(sCgi, 0 , sizeof(sCgi));
	sprintf(sCgi, "<juan ver=\"1.0\" seq=\"0\"><setup type=\"write\" user=\"%s\" password=\"\"><system operation=\"default factory\"></system></setup></juan>",
		arg->username); 

	//TRACE_DEBUG("sCgi=%s",sCgi);
	memset(sTmp, 0 , sizeof(sTmp));
	http_url_encode(sCgi, strlen(sCgi) , sTmp, 256);
	//TRACE_DEBUG("sTmp=%s",sTmp);
	memset(sCgi, 0 , sizeof(sCgi));
	sprintf(sCgi, "/cgi-bin/gw2.cgi?xml=%s", sTmp);
	//TRACE_DEBUG("sCgi=%s",sCgi);

	rlt = _jaipc_send_and_receive_cmd(arg, sCgi, sBuf, BUF_LEN, 0);
	if(rlt < 0)
		return -1;
	else
		return 0;
}

extern NVR_PROTOCOL_COLLECTION g_prc;
unsigned long JAIPC_restore_default_all(void)
{
	int i, ret;
	unsigned long restore_result = 0;
	DVRCommand_t cmd;

	for(i = 0; i < MAX_CAM_CH; i++)
	{
		if( g_prc.args[i].using_flag )
		{
			ret = _jaipc_restore_default_one(&(g_prc.args[i]));
			cmd.type = CMD_NOTICE;
			cmd.flag = DVR_CMD_FLAG;
			cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
			cmd.notice_info.chk_chn= i;
			cmd.notice_info.chk_type= IPC_CHECK_RESTORE;		
			if( ret == 0)
			{
				restore_result |= (1 << i);
				g_prc.args[i].stop_flag = 1;
				TRACE_INFO("IPC(%d) restore default success", i);
				cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
				sprintf(cmd.notice_info.chk_desc,"IPC%d恢复出厂设置成功.\0", (i+1));
				
			}
			else
			{
				TRACE_INFO("IPC(channel %d) restore default fail", i);
				cmd.notice_info.chk_state = CHECKRESULT_FAIL;
				sprintf(cmd.notice_info.chk_desc,"IPC%d恢复出厂设置失败!\0", (i+1));
			}
			SEND_DVRCMD_ACK(cmd, socket_index_gui);
		}
	}

	return restore_result;
}

int _jaipc_get_info(const NVR_ARG* arg, char *_model_no, char *_device_sno, char *_hw_ver, char *_sw_ver)
{
	char sCgi[256];
	char sTmp[256];
	char sBuf[BUF_LEN];
	int rlt;

	memset(sCgi, 0 , sizeof(sCgi));
	sprintf(sCgi, "<juan ver=\"1.0\" seq=\"0\"><conf type=\"read\" user=\"%s\" password=\"\"><info device_model=\"\" device_sn=\"\" hardware_version=\"\" software_version=\"\"></info></conf></juan>",
		arg->username); 
	//TRACE_DEBUG("sCgi=%s",sCgi);
	memset(sTmp, 0 , sizeof(sTmp));
	http_url_encode(sCgi, strlen(sCgi) , sTmp, 256);
	//TRACE_DEBUG("sTmp=%s",sTmp);
	memset(sCgi, 0 , sizeof(sCgi));
	sprintf(sCgi, "/cgi-bin/gw2.cgi?xml=%s", sTmp);
	//TRACE_DEBUG("sCgi=%s",sCgi);

	rlt = _jaipc_send_and_receive_cmd(arg, sCgi, sBuf, BUF_LEN, 1);
	if(rlt <= 0)
		return -1;

	char *model_no = NULL;
	char *device_sno = NULL;
	char *hw_ver = NULL;
	char *sw_ver = NULL;

	model_no = strstr(sBuf, "device_model=");
	device_sno = strstr(sBuf, "device_sn=");
	hw_ver = strstr(sBuf, "hardware_version=");
	sw_ver = strstr(sBuf, "software_version=");

	if(model_no)
	{
		model_no += strlen("device_model=\"");
		*strstr(model_no, "\"") = 0;
		strcpy(_model_no, model_no);
		
	}
	if(device_sno)
	{
		device_sno += strlen("device_sn=\"");
		*strstr(device_sno, "\"") = 0;
		strcpy(_device_sno, device_sno);
	}
	if(hw_ver)
	{
		hw_ver += strlen("hardware_version=\"");
		*strstr(hw_ver, "\"") = 0;
		strcpy(_hw_ver, hw_ver);
	}
	if(sw_ver)
	{
		sw_ver += strlen("software_version=\"");
		*strstr(sw_ver, "\"") = 0;
		strcpy(_sw_ver, sw_ver);
	}
	
	return 0;
}

#define IPC_MODEL_MAX_NUM	3
#define IPC_SNO_VALID		0
#define IPC_SNO_INVALID	1
#define IPC_SNO_NOFOUND	2
typedef struct _IPC_MODEL_SNO{
	char model_no[16];
	char sno_header[8];
}IPC_MODEL_SNO;

IPC_MODEL_SNO jaipc_model_sno[IPC_MODEL_MAX_NUM]={
	{"N18A\0","C1\0"},
	{"N18C\0","C2\0"},
	{"N16C\0","C3\0"},
};
int _jaipc_sno_check(const char *model_no, const char *_sno)
{
	int rlt = IPC_SNO_NOFOUND;
	int i;

	for(i = 0; i < IPC_MODEL_MAX_NUM; i++)
	{
		if(strncmp(jaipc_model_sno[i].model_no, model_no, strlen(jaipc_model_sno[i].model_no)) == 0)
		{
			if(strncmp(jaipc_model_sno[i].sno_header, _sno, strlen(jaipc_model_sno[i].sno_header)) == 0)
			{
				rlt = IPC_SNO_VALID;
			}
			else
				rlt = IPC_SNO_INVALID;
			break;
		}
	}

	return rlt;
}


int _jaipc_auto_check(const NVR_ARG* arg)
{
	
	DVRCommand_t cmd;
	char sTmp[256];
	int chk_rlt = 0;

	TRACE_INFO("IPC(%d) connect OK",arg->chn);
	cmd.type = CMD_NOTICE;
	cmd.flag = DVR_CMD_FLAG;
	cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
	cmd.notice_info.chk_chn= arg->chn;
	cmd.notice_info.chk_type= IPC_CHECK_CONNECT;
	cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
	sprintf(cmd.notice_info.chk_desc,"IPC%d连接成功.\0", (arg->chn+1));
	SEND_DVRCMD_ACK(cmd, socket_index_gui);
	
	{
		extern Sysenv* g_pstSysEnv;
		char sCgi[256] = {0};
		char sTitle[32] = {0};
		g_pstSysEnv->GetTitle(g_pstSysEnv, arg->chn, sTitle);
		sprintf(sCgi, "/cgi-bin/hi3510/param.cgi?cmd=setoverlayattr&-region=1&-show=1&-name=%s", sTitle);
		TRACE_INFO("IPC(%d) set title...", arg->chn);
		int ret = _send_cmd(arg->ip, arg->port, arg->username, arg->password, sCgi);
		if(ret != 0)
		{
			TRACE_INFO("IPC(%d) set title fail",arg->chn);		
			cmd.type = CMD_NOTICE;
			cmd.flag = DVR_CMD_FLAG;
			cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
			cmd.notice_info.chk_chn = arg->chn;
			cmd.notice_info.chk_type = IPC_CHECK_TIME;
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			sprintf(cmd.notice_info.chk_desc, "IPC%d设置通道名称(%s)失败!\0", (arg->chn+1), sTitle);
			SEND_DVRCMD_ACK(cmd, socket_index_gui);			
			CLEAR();
			chk_rlt = 2;
			goto ipc_auto_check_completed;
		}
	}

	{
		time_t now_t = time(NULL);
		struct tm now_tm = *(localtime(&now_t));
		char sCgi[256] = {0};
		sprintf(sCgi, "/cgi-bin/hi3510/param.cgi?cmd=setservertime&-time=%02d.%02d.%02d.%02d.%02d.%02d",
				now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
		memset(sTmp, 0, sizeof(sTmp));
		sprintf(sTmp, "%04d-%02d-%02d %02d:%02d:%02d",
				now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);	
		TRACE_INFO("IPC(%d) time sync...",arg->chn);
		int ret = _send_cmd(arg->ip, arg->port, arg->username, arg->password, sCgi);
		if(ret != 0)
		{
			TRACE_INFO("IPC(%d) time sync fail",arg->chn);		
			cmd.type = CMD_NOTICE;
			cmd.flag = DVR_CMD_FLAG;
			cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
			cmd.notice_info.chk_chn = arg->chn;
			cmd.notice_info.chk_type = IPC_CHECK_TIME;
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			sprintf(cmd.notice_info.chk_desc, "IPC%d时间(%s)同步失败!\0", (arg->chn+1), sTmp);
			SEND_DVRCMD_ACK(cmd, socket_index_gui);
			CLEAR();
			chk_rlt = 2;
			goto ipc_auto_check_completed;
		}
	}
		
	cmd.type = CMD_NOTICE;
	cmd.flag = DVR_CMD_FLAG;
	cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
	cmd.notice_info.chk_chn = arg->chn;
	cmd.notice_info.chk_type = IPC_CHECK_TIME;
	cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
	sprintf(cmd.notice_info.chk_desc, "IPC%d时间(%s)同步成功.\0", (arg->chn+1), sTmp);
	SEND_DVRCMD_ACK(cmd, socket_index_gui);

	TRACE_INFO("IPC(%d) get version...",arg->chn);	
	char model_no[64] = {0};
	char device_sno[64] = {0};
	char hw_ver[64] = {0};
	char sw_ver[64] = {0};
	int get_info_result;
	get_info_result = _jaipc_get_info(arg, model_no, device_sno, hw_ver, sw_ver);
	if((get_info_result < 0) || (model_no[0] == 0) || 
		(hw_ver[0] == 0) || (sw_ver[0] == 0))
	{
		chk_rlt = 2;
		cmd.type = CMD_NOTICE;
		cmd.flag = DVR_CMD_FLAG;
		cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
		cmd.notice_info.chk_chn = arg->chn;
		cmd.notice_info.chk_type = IPC_CHECK_VERSION;
		cmd.notice_info.chk_state = CHECKRESULT_FAIL;
		sprintf(cmd.notice_info.chk_desc, "IPC%d型号及版本获取失败!\0", (arg->chn+1));
		SEND_DVRCMD_ACK(cmd, socket_index_gui);
	}
	else
	{
		cmd.type = CMD_NOTICE;
		cmd.flag = DVR_CMD_FLAG;
		cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
		cmd.notice_info.chk_chn = arg->chn;
		cmd.notice_info.chk_type = IPC_CHECK_VERSION;
		cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
		sprintf(cmd.notice_info.chk_desc, "IPC%d型号%s,软件%s,硬件%s\0", (arg->chn+1), model_no, sw_ver, hw_ver);
		SEND_DVRCMD_ACK(cmd, socket_index_gui);
	}

	TRACE_INFO("IPC(%d) SNO check...",arg->chn);		
	if(device_sno[0] == 0)
	{
		chk_rlt = 2;
		cmd.type = CMD_NOTICE;
		cmd.flag = DVR_CMD_FLAG;
		cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
		cmd.notice_info.chk_chn = arg->chn;
		cmd.notice_info.chk_type = IPC_CHECK_SNO;
		cmd.notice_info.chk_state = CHECKRESULT_FAIL;
		sprintf(cmd.notice_info.chk_desc, "IPC%d序列号获取失败!\0", (arg->chn+1));
		SEND_DVRCMD_ACK(cmd, socket_index_gui); 
		goto ipc_auto_check_completed;
	}

	cmd.type = CMD_NOTICE;
	cmd.flag = DVR_CMD_FLAG;
	cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
	cmd.notice_info.chk_chn = arg->chn;
	cmd.notice_info.chk_type = IPC_CHECK_SNO;
	int sno_check_result = _jaipc_sno_check(model_no, device_sno);
	if(sno_check_result)
	{
		chk_rlt = 2;
		cmd.notice_info.chk_state = CHECKRESULT_FAIL;
	}
	else
		cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
	char sno_check_result_desc[64];
	memset(sno_check_result_desc, 0 , sizeof(sno_check_result_desc));
	switch(sno_check_result)
	{
		case IPC_SNO_VALID:
			sprintf(sno_check_result_desc,"符合要求.\0");
			cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
			break;
		case IPC_SNO_INVALID:
			chk_rlt = 2;
			sprintf(sno_check_result_desc,"不符合要求!\0");
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			break;			
		case IPC_SNO_NOFOUND:
			chk_rlt = 2;
			sprintf(sno_check_result_desc,"没有定义!\0");
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			break;
		default:
			chk_rlt = 2;
			sprintf(sno_check_result_desc,"无法判断!\0");
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			break;
	}
	sprintf(cmd.notice_info.chk_desc,"IPC%d序列号%s,%s\0", (arg->chn+1), device_sno, sno_check_result_desc);
	SEND_DVRCMD_ACK(cmd, socket_index_gui);
	
ipc_auto_check_completed:
	TRACE_INFO("IPC(%d) auto-check completed.",arg->chn);		
	cmd.type = CMD_NOTICE;
	cmd.flag = DVR_CMD_FLAG;
	cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
	cmd.notice_info.chk_chn = arg->chn;
	cmd.notice_info.chk_type = IPC_CHECK_AUTO;
	if(chk_rlt)
	{
		cmd.notice_info.chk_state = CHECKRESULT_FAIL;
		sprintf(cmd.notice_info.chk_desc,"IPC%d自动测试失败!\0", (arg->chn+1));
		SEND_DVRCMD_ACK(cmd, socket_index_gui);
	}
	else
	{
		cmd.notice_info.chk_state = CHECKRESULT_SUCCESS;
		sprintf(cmd.notice_info.chk_desc,"IPC%d自动测试成功.\0", (arg->chn+1));
		SEND_DVRCMD_ACK(cmd, socket_index_gui);
	}

	return chk_rlt;
}
#endif

int* NVRPC_hg_run(NVR_ARG* arg)
{
	TRACE_INFO("HG(%d,%s) thread start, pid=%d, ip=%s, port=%d, pwd=%s",
			arg->chn, arg->ip, getpid(), arg->ip, arg->port, arg->password);

	arg->private_data = (HG_PRIVATE_DATA*)malloc(sizeof(HG_PRIVATE_DATA));
	memset(arg->private_data, 0, sizeof(HG_PRIVATE_DATA));

	_HG_GET_FRAME_CTX* main_ctx = (_HG_GET_FRAME_CTX*)malloc(sizeof(_HG_GET_FRAME_CTX));
	_HG_GET_FRAME_CTX* sub_ctx = (_HG_GET_FRAME_CTX*)malloc(sizeof(_HG_GET_FRAME_CTX));

	SOCKET_CONN_CTX conn_ctx;

connect_begin:
	TEST_EXIT();
	NVRPC_conninit(&conn_ctx, arg->ip, arg->port, 3);
	NVRPC_connect(&conn_ctx);
	if(conn_ctx.result != SOCKET_CONN_RESULT_SUCCESS)
	{
		TRACE_DEBUG("hg(%d,%s:%d) main connect failed sock=%d errno=%d", arg->chn, conn_ctx.ip, conn_ctx.port, conn_ctx.socketfd, conn_ctx.errno_cpy);
		sleep(CONNECT_FIALED_SLEEP);
		goto connect_begin;
	}
	else
	{
		PRIVATE_DATA()->main_sock = conn_ctx.socketfd;
		PRIVATE_DATA()->main_connected = 1;
	}

	NVRPC_conninit(&conn_ctx, arg->ip, arg->port, 1);
	NVRPC_connect(&conn_ctx);
	if(conn_ctx.result != SOCKET_CONN_RESULT_SUCCESS)
	{
		TRACE_DEBUG("hg(%d,%s:%d) sub connect failed sock=%d errno=%d", arg->chn, conn_ctx.ip, conn_ctx.port, conn_ctx.socketfd, conn_ctx.errno_cpy);
		sleep(CONNECT_FIALED_SLEEP);
		CLEAR();
		goto connect_begin;
	}
	else
	{
		PRIVATE_DATA()->sub_sock = conn_ctx.socketfd;
		PRIVATE_DATA()->sub_connected = 1;
	}


	
	int res_main = 0;
	int res_sub = 0;
	unsigned int base_ts_main = 0;
	unsigned int base_ts_sub = 0;
	unsigned long long base_pts = time(NULL); //ms
	base_pts *= 1000;
	int main_frame_count = 0;
	int sub_frame_count = 0;
	HG_AV_PARAMS main_av_params;
	HG_AV_PARAMS sub_av_params;
//	TRACE_DEBUG("base_pts=%llu", base_pts);
	int frame_count = 0;

	res_main = _login(arg->ip, arg->port, arg->username, arg->password, PRIVATE_DATA()->main_sock, 0, &main_av_params);
	res_sub = _login(arg->ip, arg->port, arg->username, arg->password, PRIVATE_DATA()->sub_sock, 1, &sub_av_params);
	if(res_main != 0 || res_sub != 0)
	{
		#ifdef _IPC_CHECK_MODE
			DVRCommand_t cmd;
			printf("IPC(channel %d) connect Fail\n",arg->chn);
			cmd.type = CMD_NOTICE;
			cmd.flag = DVR_CMD_FLAG;
			cmd.notice_info.enSubType = NOTICE_CHECKINFO_SHOW;
			cmd.notice_info.chk_chn= arg->chn;
			cmd.notice_info.chk_type= IPC_CHECK_CONNECT;
			cmd.notice_info.chk_state = CHECKRESULT_FAIL;
			sprintf(cmd.notice_info.chk_desc,"IPC%d连接失败!\0", (arg->chn+1));
			SEND_DVRCMD_ACK(cmd, socket_index_gui);
		#endif
		TRACE_INFO("_login error, main=%d, sub=%d", res_main, res_sub);
		CLEAR();
		sleep(CONNECT_FIALED_SLEEP);
		goto connect_begin;
	}

	PRIVATE_DATA()->main_last_recv = time(NULL);
	PRIVATE_DATA()->sub_last_recv = PRIVATE_DATA()->main_last_recv;

	#ifdef _IPC_CHECK_MODE
		int ipc_rlt = _jaipc_auto_check(arg);
		if(ipc_rlt == 1)
			goto connect_begin;
		else
			goto manual_ipc_check;
	#endif
	{
		extern Sysenv* g_pstSysEnv;
		char sCgi[256] = {0};
		char sTitle[32] = {0};
		g_pstSysEnv->GetTitle(g_pstSysEnv, arg->chn, sTitle);
		sprintf(sCgi, "/cgi-bin/hi3510/param.cgi?cmd=setoverlayattr&-region=1&-show=1&-name=%s", sTitle);
//		TRACE_INFO("chn%d sync title, cgi=%s", arg->chn, sCgi);
		int ret = _send_cmd(arg->ip, arg->port, arg->username, arg->password, sCgi);
		if(ret != 0)
		{
			CLEAR();
			goto connect_begin;
		}
	}

	{
		time_t now_t = time(NULL);
		struct tm now_tm = *(localtime(&now_t));
		char sCgi[256] = {0};
		sprintf(sCgi, "/cgi-bin/hi3510/param.cgi?cmd=setservertime&-time=%02d.%02d.%02d.%02d.%02d.%02d",
				now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
//		TRACE_INFO("chn%d sync time, cgi=%s", arg->chn, sCgi);
		int ret = _send_cmd(arg->ip, arg->port, arg->username, arg->password, sCgi);
		if(ret != 0)
		{
			CLEAR();
			goto connect_begin;
		}
	}

#ifdef _IPC_CHECK_MODE 
manual_ipc_check:
#endif

	{
		extern Sysenv* g_pstSysEnv;
		int nHue, nBrightness, nContrast, nSaturation;
		nHue = g_pstSysEnv->GetHue(g_pstSysEnv, arg->chn);
		nBrightness = g_pstSysEnv->GetLuminance(g_pstSysEnv, arg->chn);
		nContrast = g_pstSysEnv->GetContrast(g_pstSysEnv, arg->chn);
		nSaturation = g_pstSysEnv->GetSaturation(g_pstSysEnv, arg->chn);
//		TRACE_INFO("chn%d sync color, nHue=%d, nBrightness=%d, nContrast=%d, nSaturation=%d", arg->chn, nHue, nBrightness, nContrast, nSaturation);
		VIDEO_SetColor(arg->chn, nHue, nBrightness, nContrast, nSaturation);
	}


//	{
//		extern Sysenv* g_pstSysEnv;
//		ENC_BR_VALUE brv = g_pstSysEnv->GetSubBitrateValue(g_pstSysEnv, arg->chn, 0);
//		int fr_i = 0;
//		ENC_FRAMERATE fr = g_pstSysEnv->GetEncSubFramerate(g_pstSysEnv, arg->chn, 0);
//		char sCgi[256] = {0};
//		sprintf(sCgi, "/cgi-bin/hi3510/param.cgi?cmd=setvencattr&-chn=11&-bps=%d&-fps=%d&-brmode=1&-imagegrade=1&-gop=%d",
//				1024, 25, 50);
//		int ret = _send_cmd(arg->ip, arg->port, arg->username, arg->password, sCgi);
//		if(ret != 0)
//		{
//			CLEAR();
//			goto connect_begin;
//		}
//	}

	time_t now_t = 0;
	AVFrameMark stFrameMark = {0};
//	_HG_GET_FRAME_CTX* main_ctx = (_HG_GET_FRAME_CTX*)malloc(sizeof(_HG_GET_FRAME_CTX));
	memset(main_ctx, 0, sizeof(_HG_GET_FRAME_CTX));
//	_HG_GET_FRAME_CTX* sub_ctx = (_HG_GET_FRAME_CTX*)malloc(sizeof(_HG_GET_FRAME_CTX));
	memset(sub_ctx, 0, sizeof(_HG_GET_FRAME_CTX));
	fd_set fd;
	struct timeval timeout;
	NVR_ARG_LIVE last_live = arg->live;
	int discard_delay = 0;
	unsigned long long delay_last = 0;
	int discard_delay_sub = 0;
	unsigned long long delay_last_sub = 0;
	while(arg->running == 1)
	{
		now_t = time(NULL);
		FD_ZERO(&fd);
		FD_SET(PRIVATE_DATA()->main_sock, &fd);
		FD_SET(PRIVATE_DATA()->sub_sock, &fd);
		timeout.tv_sec = 0;
		timeout.tv_usec = 10 * 1000;

		int max = MAX(PRIVATE_DATA()->main_sock, PRIVATE_DATA()->sub_sock);
		int ret = select(max + 1, &fd, NULL, NULL, &timeout);
		int read_frame;

		if(ret >= 0)
		{
			if(now_t - PRIVATE_DATA()->main_last_recv > 5
					|| now_t - PRIVATE_DATA()->sub_last_recv > 5)
			{
				TRACE_INFO("now_t=%d, main_last_recv=%d, sub_last_recv=%d",
						now_t, PRIVATE_DATA()->main_last_recv, PRIVATE_DATA()->sub_last_recv);
				CLEAR();
				goto connect_begin;
			}

			if(FD_ISSET(PRIVATE_DATA()->main_sock, &fd))
			{
				do
				{
					read_frame = _read_frame(PRIVATE_DATA()->main_sock, main_ctx);
//					TRACE_INFO("main read_frame=%d", read_frame);
					PRIVATE_DATA()->main_last_recv = now_t;
					if(read_frame == 1)
					{
						if(base_ts_main == 0)
						{
							base_ts_main = main_ctx->time_stamp;
						}

						struct timeval tv;
						gettimeofday(&tv, NULL);


						SDK_ENC_BUF_ATTR_t attr;
						attr.magic = SDK_ENC_BUF_DATA_MAGIC;
						attr.type = SDK_ENC_BUF_DATA_H264;
						attr.timestamp_us = (unsigned long long)tv.tv_sec * (unsigned long long)1000000 + (unsigned long long)tv.tv_usec;
						attr.time_us = ((unsigned long long)tv.tv_sec << 32) + (unsigned long long)tv.tv_usec;
						attr.data_sz = main_ctx->flen;
						attr.h264.keyframe = 1 == main_ctx->is_key ? TRUE : FALSE;
						attr.h264.ref_counter = 0;
						attr.h264.fps = 25;
						attr.h264.width = main_av_params.v.width;
						attr.h264.height = main_av_params.v.height;
//						TRACE_DEBUG("MEDIABUF_in_request ok, size=%d, key=%d", attr.data_sz, attr.h264.keyframe);
						int mediabuf_id = arg->chn * 2;
						if(0 == MEDIABUF_in_request(mediabuf_id, attr.data_sz + sizeof(attr), attr.h264.keyframe)){
							MEDIABUF_in_attach(mediabuf_id, &attr, sizeof(attr));
							MEDIABUF_in_attach(mediabuf_id, main_ctx->fbuf, attr.data_sz);
							MEDIABUF_in_committ(mediabuf_id);
//							TRACE_DEBUG("MEDIABUF_in_committ ok, size=%d, key=%d", attr.data_sz, attr.h264.keyframe);
						}


						memset(&stFrameMark, 0, sizeof(AVFrameMark));
						stFrameMark.nMagic = AVENC_FRAME_MAGIC;
						stFrameMark.nChn = arg->chn;
						stFrameMark.enType = 1 == main_ctx->is_key ? AVENC_IDR : AVENC_PSLICE;
						stFrameMark.nSize = main_ctx->flen;
						stFrameMark.u64TSP = (unsigned long long)tv.tv_sec * (unsigned long long)1000000 + (unsigned long long)tv.tv_usec;
						stFrameMark.pAddr = main_ctx->fbuf;
						stFrameMark.nGenTime = now_t;
						stFrameMark.nGenIndex = main_frame_count++;

						if(discard_delay == 0)
						{
//							TRACE_DEBUG("u64TSP=%llu", stFrameMark.u64TSP);
							if(delay_last == 0)
							{
								delay_last = stFrameMark.u64TSP;
							}
							if(stFrameMark.u64TSP - delay_last > 30*1000)
							{
//								TRACE_INFO("discard_delay ok u64TSP=%llu", stFrameMark.u64TSP);
								discard_delay = 1;
							}
							delay_last = stFrameMark.u64TSP;
							goto end_frame_main;
						}

						if(arg->live == NVR_ARG_LIVE_MAIN)
						{
							if(last_live != arg->live && stFrameMark.enType == AVENC_IDR)
							{
								DECODE_Reset(arg->chn);
								last_live = arg->live;
							}
							if(last_live == arg->live)
							{
				//				TRACE_DEBUG("DECODE_Request main ok, stFrameMark.nSize=%d, stFrameMark.enType=%d", stFrameMark.nSize, stFrameMark.enType);
								res_main = DECODE_Request(stFrameMark.nChn, stFrameMark.enType, stFrameMark.pAddr, stFrameMark.nSize, stFrameMark.u64TSP, FALSE);
								if(res_main != 0)
								{
									TRACE_INFO("DECODE_Request main failed");
								}
							}
						}
end_frame_main:
						main_ctx->flen = 0;
					}
					else if(read_frame == -1)
					{
						TRACE_DEBUG("read_frame main error");
						CLEAR();
						goto connect_begin;
					}
					else
					{

					}

				}while(read_frame == 1);
			}

			if(FD_ISSET(PRIVATE_DATA()->sub_sock, &fd))
			{
				do
				{
					read_frame = _read_frame(PRIVATE_DATA()->sub_sock, sub_ctx);
					PRIVATE_DATA()->sub_last_recv = now_t;
//					TRACE_INFO("sub read_frame=%d", read_frame);
					if(read_frame == 1)
					{
						if(base_ts_sub == 0)
						{
							base_ts_sub = sub_ctx->time_stamp;
						}

						struct timeval tv;
						gettimeofday(&tv, NULL);

						SDK_ENC_BUF_ATTR_t attr;
						attr.magic = SDK_ENC_BUF_DATA_MAGIC;
						attr.type = SDK_ENC_BUF_DATA_H264;
						attr.timestamp_us = (unsigned long long)tv.tv_sec * (unsigned long long)1000000 + (unsigned long long)tv.tv_usec;
						attr.time_us = ((unsigned long long)tv.tv_sec << 32) + (unsigned long long)tv.tv_usec;
						attr.data_sz = main_ctx->flen;
						attr.h264.keyframe = 1 == main_ctx->is_key ? TRUE : FALSE;
						attr.h264.ref_counter = 0;
						attr.h264.fps = 25;
						attr.h264.width = sub_av_params.v.width;
						attr.h264.height = sub_av_params.v.height;
//						TRACE_DEBUG("MEDIABUF_in_request ok, size=%d, key=%d", attr.data_sz, attr.h264.keyframe);
						int mediabuf_id = arg->chn * 2 + 1;
						if(0 == MEDIABUF_in_request(mediabuf_id, attr.data_sz + sizeof(attr), attr.h264.keyframe)){
							MEDIABUF_in_attach(mediabuf_id, &attr, sizeof(attr));
							MEDIABUF_in_attach(mediabuf_id, main_ctx->fbuf, attr.data_sz);
							MEDIABUF_in_committ(mediabuf_id);
//							TRACE_DEBUG("MEDIABUF_in_committ ok, size=%d, key=%d", attr.data_sz, attr.h264.keyframe);
						}


						memset(&stFrameMark, 0, sizeof(AVFrameMark));
						stFrameMark.nMagic = AVENC_FRAME_MAGIC;
						stFrameMark.nChn = arg->chn;
						stFrameMark.enType = 1 == sub_ctx->is_key ? AVENC_IDR : AVENC_PSLICE;
						stFrameMark.nSize = sub_ctx->flen;
						stFrameMark.u64TSP = (unsigned long long)tv.tv_sec * (unsigned long long)1000000 + (unsigned long long)tv.tv_usec;
						stFrameMark.pAddr = sub_ctx->fbuf;
						stFrameMark.nGenTime = now_t;
						stFrameMark.nGenIndex = sub_frame_count++;

						if(discard_delay_sub == 0)
						{
//							TRACE_DEBUG("u64TSP=%llu", stFrameMark.u64TSP);
							if(delay_last_sub == 0)
							{
								delay_last_sub = stFrameMark.u64TSP;
							}
							if(stFrameMark.u64TSP - delay_last_sub > 30*1000)
							{
//								TRACE_INFO("discard_delay ok u64TSP=%llu", stFrameMark.u64TSP);
								discard_delay_sub = 1;
							}
							delay_last_sub = stFrameMark.u64TSP;
							goto end_frame_sub;
						}

				//		if(arg->live == NVR_ARG_LIVE_SUB || arg->live == NVR_ARG_LIVE_MAIN)
						if(arg->live == NVR_ARG_LIVE_SUB)
						{
							if(last_live != arg->live && stFrameMark.enType == AVENC_IDR)
							{
								DECODE_Reset(arg->chn);
								last_live = arg->live;
							}
							if(last_live == arg->live)
							{
				//				TRACE_DEBUG("DECODE_Request sub ok, stFrameMark.nSize=%d, stFrameMark.enType=%d", stFrameMark.nSize, stFrameMark.enType);
								res_sub = DECODE_Request(stFrameMark.nChn, stFrameMark.enType, stFrameMark.pAddr, stFrameMark.nSize, stFrameMark.u64TSP, FALSE);
								if(res_sub != 0)
								{
									TRACE_INFO("DECODE_Request sub failed");
								}
							}
						}
end_frame_sub:
						sub_ctx->flen = 0;
					}
					else if(read_frame == -1)
					{
						TRACE_DEBUG("read_frame sub error");
						CLEAR();
						goto connect_begin;
					}
					else
					{

					}

				}while(read_frame == 1);
			}
		}
		else
		{
			TRACE_DEBUG("select error, errno=%d", errno);
			CLEAR();
			goto connect_begin;
		}
	}

clear_all:
	AVENC_rec_stop(arg->chn);
	CLEAR();
	free(arg->private_data);
	arg->private_data = NULL;
	free(main_ctx);
	free(sub_ctx);
	video_send_logo(arg->chn);
	DECODE_Reset(arg->chn);
	TRACE_INFO("HG(%d,%s) thread end", arg->chn, arg->ip);
	return NULL;
}


int Search_hg(FILE *fd_IPCInfo)
{
	//todo:need refactoring, see nvrpc_function.c:NVRPC_send_mcas
	struct ip_mreq mreq;
	int count = 0;
	unsigned char recvbuf[BUFSIZE], sendbuf[BUFSIZE];
	char client_id_str[MAX_CLIENT_ID_SIZE+1];

	struct sockaddr_in  local, remote, from;
	int sock;
	int  len = sizeof(struct sockaddr_in), ret, optval;
	unsigned long	 dwInterface, dwMulticastGroup;
	short iPort;

	bzero(&mreq, sizeof(struct ip_mreq));
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	dwInterface = INADDR_ANY;
	dwMulticastGroup = inet_addr(MCASTADDR);
	iPort = MCASTPORT;

	local.sin_family = AF_INET;
	local.sin_port   = htons(iPort);
	local.sin_addr.s_addr = INADDR_ANY;

	inet_pton(AF_INET, MCASTADDR, &mreq.imr_multiaddr.s_addr);
	/* 设置发送组播消息的源主机的地址信息 */
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	optval = 0;
	//setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,(char *)&optval, sizeof(optval));
	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	bind(sock, (struct sockaddr *)&local, sizeof(local));

	remote.sin_family      = AF_INET;
	remote.sin_port        = htons(iPort);
	remote.sin_addr.s_addr = dwMulticastGroup;


	printf("设置组播搜索消息\n");
	memset(client_id_str, 0, sizeof(client_id_str));
	//sprintf(client_id_str, "nvmOPxEnYfQRAeLFdsMrpBbnMDbEPiMC");
	generate_client_id(client_id_str);
	memset(sendbuf, 0, sizeof(sendbuf));
	snprintf(sendbuf, BUFSIZE, "SEARCH * HDS/1.0\r\n"
			"CSeq:1\r\n"
			"Client-ID:%s\r\n"
			"Accept-Type:text/HDP\r\n"
			"Content-Length:0\r\n"
			"X-Search-Type:NVR\r\n"
			"\r\n\r\n"
			,client_id_str);
	printf("%s\n---len = %d\n", sendbuf, strlen(sendbuf));

	ret = sendto(sock, (char *)sendbuf, strlen(sendbuf), 0,(struct sockaddr *)&remote, sizeof(remote));
	//	printf("-------sendto_ret = %d--------\r\n", ret);


	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;
	//	unsigned char loop = 0;
	while(1)
		//for (i = 0; i < 100; i++)
	{
		fd_set r;
		struct timeval t;

		t.tv_sec = 0;
		t.tv_usec = 200;


		FD_ZERO(&r);
		FD_SET(sock, &r);

		ret = select(sock + 1, &r, NULL, NULL, &t);
		if (ret > 0) {
			memset(recvbuf, 0, sizeof(recvbuf));
			ret = recvfrom(sock, recvbuf, BUFSIZE, 0, (struct sockaddr *)&from, &len);
			recvbuf[ret] = 0;
			//printf("receive\r\n");
		}
		if (strstr(recvbuf, "HDS/1.0 200 OK") && strstr(recvbuf,client_id_str)) {			

			if(Write_IPCInfo(recvbuf, fd_IPCInfo, IPCAM_HG) == 0)
			{
				count++;
			}
			memset(recvbuf, 0, sizeof(recvbuf));
		}
		curr_time = time(NULL);
		if(curr_time - begin_time > IPC_SEARCH_TIME){
			break;
		}
	}
	close(sock);
	printf("count =%d\r\n", count);
	return count;
}


int _modify_dev_hg(char* _dev_id, char* _ip, char* _mac, char* _netmask,
		char* _gateway, int _port, char* _usr, char* _pwd, char* _modify_type)
{
	//todo:need refactoring, see nvrpc_function.c:NVRPC_send_mcas
	TRACE_DEBUG("%s, _dev_id=%s, _ip=%s, _mac=%s, _port=%d, _usr=%s, _pwd=%s",
			__FUNCTION__, _dev_id, _ip, _mac, _port, _usr, _pwd);

	struct ip_mreq mreq;
	int count = 0;
	unsigned char recvbuf[BUFSIZE], sendbuf[BUFSIZE];

	struct sockaddr_in  local, remote, from;
	int sock;
	int  len = sizeof(struct sockaddr_in), ret, optval;
	unsigned long	 dwInterface, dwMulticastGroup;
	short iPort;

	bzero(&mreq, sizeof(struct ip_mreq));
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	dwInterface = INADDR_ANY;
	dwMulticastGroup = inet_addr(MCASTADDR);
	iPort = MCASTPORT;

	local.sin_family = AF_INET;
	local.sin_port   = htons(iPort);
	local.sin_addr.s_addr = INADDR_ANY;

	inet_pton(AF_INET, MCASTADDR, &mreq.imr_multiaddr.s_addr);
	/* 设置发送组播消息的源主机的地址信息 */
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	optval = 0;
	//setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP,(char *)&optval, sizeof(optval));
	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	bind(sock, (struct sockaddr *)&local, sizeof(local));

	remote.sin_family      = AF_INET;
	remote.sin_port        = htons(iPort);
	remote.sin_addr.s_addr = dwMulticastGroup;

	int nCSeq = 0;
	char content[256] = {0};
	char client_id_str[MAX_CLIENT_ID_SIZE+1];
	memset(client_id_str, 0, sizeof(client_id_str));
	//sprintf(client_id_str, "nvmOPxEnYfQRAeLFdsMrpBbnMDbEPiMC");
	generate_client_id(client_id_str);

	if(strlen(_ip) > 0 || strlen(_mac) > 0 || strlen(_netmask) || strlen(_gateway))
	{
		strcpy(content, "netconf set");
		if(strlen(_ip) > 0)
		{
			sprintf(content + strlen(content), " -ipaddr %s", _ip);
		}
		if(strlen(_mac) > 0)
		{
			sprintf(content + strlen(content), " -hwaddr %s", _mac);
		}
		if(strlen(_netmask) > 0)
		{
			sprintf(content + strlen(content), " -netmask %s", _netmask);
		}
		if(strlen(_gateway) > 0)
		{
			sprintf(content + strlen(content), " -gateway %s", _gateway);
		}

		memset(sendbuf, 0, sizeof(sendbuf));
		snprintf(sendbuf, BUFSIZE, "CMD * HDS/1.0\r\n"
				"CSeq:%d\r\n"
				"Client-ID:%s\r\n"
				"Accept-Type:text/HDP\r\n"
				"Authorization:Basic %s:%s\r\n"
				"Device-ID:%s\r\n"
				"Content-Length:%d\r\n"
				"X-Modify-Type:%s\r\n"
				"\r\n"
				"%s\r\n", nCSeq, client_id_str, _usr, _pwd, _dev_id, strlen(content), _modify_type, content);
		//TRACE_DEBUG("len=%d, sendbuf=%s", strlen(sendbuf), sendbuf);
		ret = sendto(sock, (char *)sendbuf, strlen(sendbuf), 0,(struct sockaddr *)&remote, sizeof(remote));
		nCSeq++;
		//sleep(5);
	}

	if(_port > 0)
	{
		if(_port > 0)
		{
			sprintf(content, "httpport set -httpport %d", _port);
		}
		memset(sendbuf, 0, sizeof(sendbuf));
		snprintf(sendbuf, BUFSIZE, "CMD * HDS/1.0\r\n"
				"CSeq:%d\r\n"
				"Client-ID:%s\r\n"
				"Accept-Type:text/HDP\r\n"
				"Authorization:Basic %s:%s\r\n"
				"Device-ID:%s\r\n"
				"Content-Length:%d\r\n"
				"\r\n"
				"%s\r\n", nCSeq, client_id_str, _usr, _pwd, _dev_id, strlen(content), content);
		//TRACE_DEBUG("len=%d, sendbuf=%s", strlen(sendbuf), sendbuf);
		ret = sendto(sock, (char *)sendbuf, strlen(sendbuf), 0,(struct sockaddr *)&remote, sizeof(remote));
		nCSeq++;
	}

	time_t begin_time = time(NULL);
	time_t curr_time = begin_time;
	//	unsigned char loop = 0;
	while(1)
		//for (i = 0; i < 100; i++)
	{
		fd_set r;
		struct timeval t;

		t.tv_sec = 0;
		t.tv_usec = 200;


		FD_ZERO(&r);
		FD_SET(sock, &r);

		ret = select(sock + 1, &r, NULL, NULL, &t);
//		TRACE_DEBUG("ret=%d", ret);
		if (ret > 0) {
			memset(recvbuf, 0, sizeof(recvbuf));
			ret = recvfrom(sock, recvbuf, BUFSIZE, 0, (struct sockaddr *)&from, &len);
			int errno_cpy = errno;
			if(ret < 0)
			{
				TRACE_INFO("ret=%d, errno_cpy=%d, receive=\"%s\"\r\n", ret, errno_cpy, recvbuf);
			}
		}
		if (strstr(recvbuf, "MCTP/1.0 200 OK") &&
				strstr(recvbuf,client_id_str)) {

			count++;
			memset(recvbuf, 0, sizeof(recvbuf));
		}
		curr_time = time(NULL);
		if(curr_time - begin_time > IPC_SEARCH_TIME){
			break;
		}
	}
	close(sock);
	//printf("count =%d\r\n", count);
	return count;
}

#endif
