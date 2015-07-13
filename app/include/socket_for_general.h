#ifndef __socket_for_smtp_ddns__
#define __socket_for_smtp_ddns__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int get_ipn( char *host );
int CreateSocket( int Send_system_buf, int Recv_system_buf, struct timeval Stimeout, struct timeval Rtimeout, struct linger *sLinger );
int ConnectSock( int sockfd, unsigned long ipn, int port );
int senddata( int sockfd, char *msg, int Max_send_trytimes, int msglen );
int recvdata( int sockfd, char **data, int Max_recv_trytimes, int bufsize );

#ifdef __cplusplus
};
#endif

#endif
