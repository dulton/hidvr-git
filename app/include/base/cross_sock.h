#ifndef __RTSP_SOCK_H__
#define __RTSP_SOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#define UDP_SOCK_BUF_SIZE	(512*1024)

#ifndef RET_OK
#define RET_OK	(0)
#endif
#ifndef RET_FAIL
#define RET_FAIL	(-1)
#endif

#if defined(_WIN32) || defined(_WIN64)
	//#include <winsock.h>
	#include <winsock2.h>
	#include <windows.h>
	#include <windef.h>
	#include <WS2TCPIP.H>
	#include <errno.h>

	typedef SOCKET	SOCK_t;
	typedef int SOCKLEN_t; 
	typedef SOCKADDR SOCKADDR_t;
	typedef SOCKADDR_IN SOCKADDR_IN_t;
	typedef SOCKADDR_IN SOCKADDR_UN_t;
	typedef char SOCKOPTARG_t;

	#define SOCK_close(s) \
		do{\
			closesocket(s);\
			WSACleanup();\
		}while(0)
	#define SOCK_IOCTL		ioctlsocket
	#define SOCK_ERR		WSAGetLastError()
	#define SOCK_EAGAIN		WSAEWOULDBLOCK
	#define SOCK_ETIMEOUT	WSAETIMEDOUT
	#define SOCK_EINTR		WSAEINTR
	#define SOCK_EINPROGRESS		WSAEINPROGRESS
#else

	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/un.h>
	#include <netinet/tcp.h>
#ifdef HAVE_IFADDRS_H	
	#include <ifaddrs.h>
#endif
	#include <fcntl.h>
	#include <errno.h>

	typedef int	SOCK_t;
	typedef socklen_t SOCKLEN_t; 
	typedef struct sockaddr SOCKADDR_t;
	typedef struct sockaddr_in SOCKADDR_IN_t;
	typedef struct sockaddr_un SOCKADDR_UN_t;
	typedef void SOCKOPTARG_t;

	#define SOCK_close(s)	close(s)
	#define SOCK_IOCTL		ioctl
	#define SOCK_ERR		errno

	#define SOCK_EAGAIN		EAGAIN
	#define SOCK_ETIMEOUT	ETIMEDOUT
	#define SOCK_EINTR		EINTR
	#define SOCK_EINPROGRESS		EINPROGRESS
#endif


#define DEFAULT_SOCK_TIMEOUT	(5000)

SOCK_t SOCK_tcp_listen(char *bindip, int listen_port, int backlog, int reuseAddr);
SOCK_t SOCK_tcp_connect(char *ip,int port,int connTimeoutMS); 
SOCK_t SOCK_accept(SOCK_t listenfd, char *addrip, int *port);

int SOCK_set_rwtimeout(SOCK_t sock,int rTimeoutMS, int wTimeoutMS);
int SOCK_set_tcp_nodelay(SOCK_t sock,int nodelay /* 1 for enable, 0: disable */);
int SOCK_set_buf_size(SOCK_t sock, unsigned int sndBufSize, unsigned int rcvBufSize); /*if 0 , use default*/
int SOCK_set_broadcast(SOCK_t sock,int broadcast/* 1 for enable, 0: disable */);
int SOCK_set_rwlowat(SOCK_t sock, int readLowat, int writeLowat);
int SOCK_set_unblockmode(SOCK_t sock, int unblockingMode/* 1 for enable, 0: disable */);

SOCK_t SOCK_udp_init(char *bindip, int port, int bReuseAddr,int rwTimeoutMS);
SOCK_t SOCK_broadcast_udp_init(char *bindip, int port, int bReuseAddr,int rwTimeoutMS);
SOCK_t SOCK_multicast_udp_init(char *group, int port, int bReuseAddr,int rwTimeoutMS, char *binda);

SOCK_t SOCK_domain_listen(const char *path);
SOCK_t SOCK_domain_connect(const char *localPath, const char *serverPath);
SOCK_t SOCK_domain_accept(SOCK_t listenfd, char *peer_path);

SOCK_t SOCK_raw_init(int protocal, int rwTimeoutMS);
int SOCK_raw_sendto(SOCK_t sock, char *bind_ether, char *target_mac, int frame_type, char *buf, int size);

int SOCK_send(SOCK_t sock,void *buf,unsigned int size, int flag);
int SOCK_recv(SOCK_t sock,void *buf,unsigned int bufSize, int flag);// recv one time, return size of data received
unsigned int SOCK_recv_n(SOCK_t sock,void *buf,unsigned int needToReadSize, int flag);// recv until reach the the size to be read or error occur
unsigned int SOCK_unblock_recv_n(SOCK_t sock,void *buf,unsigned int bufSize, int flag);// recv all available data until socket buffer empty

int SOCK_sendto(SOCK_t sock,char *ip,int port,void *buf,unsigned int size, int flags); 
int SOCK_recvfrom(SOCK_t sock,char *ip,int *port,void *buf,unsigned int size, int flags);
int SOCK_unblock_recvfrom(SOCK_t sock,char *ip,int *port,void *buf,unsigned int size, int flags);

int SOCK_gethostbyname(char *name,char *ip);
int SOCK_getallhostip(void (*f_Add)(char *ip)) ;
int SOCK_getpeername(SOCK_t sock,char *peer);
int SOCK_getsockname(SOCK_t sock,char *ip);
int SOCK_getsockport(SOCK_t sock,unsigned short * const port);
//int SOCK_gethostname(char *ip);
int SOCK_isreservedip(char *szIp);
int SOCK_get_ether_ip(char *eth, char * const ip, char * const netmask, char *const mac);
int SOCK_get_gateway(char * const gateway);
int SOCK_get_ip_only(char *eth, unsigned char * const ip);
int SOCK_check_nic(char *nic_name);



#ifdef __cplusplus
}
#endif

#endif

