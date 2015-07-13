/*
 * tcp_server.h
 *
 *  Created on: 2011-8-18
 *      Author: root
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_



#include <arpa/inet.h>
#include "ldlist.h"

typedef struct _ClientSocket
{
	int socket;
	struct sockaddr_in client_addr;
	LDList* send_list; //SocketBuf*
}ClientSocket;

struct _TcpServer;
typedef void (*TcpServerOnConnect)(struct _TcpServer* thiz, int _socket_index, int _new_fd);
typedef void (*TcpServerOnDisConnect)(struct _TcpServer* thiz, int _socket_index);
typedef void (*TcpServerOnMaxConnect)(struct _TcpServer* thiz, int _new_fd);
typedef void (*TcpServerOnRecv)(struct _TcpServer* thiz, int _socket_index, void* _buf, int _len);
typedef struct _TcpServer
{
	int m_port;
	int m_listen_socket;
	ClientSocket* m_client_socket;
	int m_max_client;

	pthread_t m_accept_proc_id;
	int m_accept_proc_run;

	//Accept和设置ClientSocket之间，无默认动作
	TcpServerOnConnect on_connect;
	//Close和清除CLientSocket之间，无默认动作
	TcpServerOnDisConnect on_disconnect;
	//Accept之后，默认动作为Close
	TcpServerOnMaxConnect on_max_connect;
	//Read之后，并且读到数量大于0
	TcpServerOnRecv on_recv;
}TcpServer;

extern TcpServer* TcpServer_init(int _port, int _max_client,
		TcpServerOnConnect _on_connect,
		TcpServerOnDisConnect _on_disconnect,
		TcpServerOnMaxConnect _on_max_connect,
		TcpServerOnRecv _on_recv);
extern void TcpServer_proc(TcpServer* thiz);
extern void TcpServer_destory(TcpServer* thiz);

extern int TcpServer_is_connected(TcpServer* thiz, int _socket_index);
extern void TcpServer_disconnect(TcpServer* thiz, int _socket_index);
extern void TcpServer_send(TcpServer* thiz, int _socket_index, void* _buf, int _len);


#endif /* TCP_SERVER_H_ */
