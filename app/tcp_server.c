/*
 * tcp_server.c
 *
 *  Created on: 2011-8-18
 *      Author: root
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/param.h>

#include "std.h"
#include "wrap.h"
#include "socket_buf.h"
#include "tcp_server.h"
#include "ldlist.h"

#undef TRACE_DEBUG
#define TRACE_DEBUG(fmt, args...)
#undef TRACE_INFO
#define TRACE_INFO(fmt, args...)

#include "cross_sock.h"
#include "dvr_command.h"

#define SERVER_MAX_READ_BUF (128)
static void _clientsocket_read(TcpServer* thiz, int _socket_index)
{
	ClientSocket* cs = &thiz->m_client_socket[_socket_index];
	unsigned char read_buf[SERVER_MAX_READ_BUF];
	int read_count = Read(cs->socket, read_buf, SERVER_MAX_READ_BUF);
	if(read_count <= 0)
	{
		Close(cs->socket);
		cs->socket = -1;
		if(thiz->on_disconnect != NULL)
		{
			thiz->on_disconnect(thiz, _socket_index);
		}
		if(read_count == 0)
		{
			TRACE_DEBUG("client disconnect.");
		}
		else
		{
			TRACE_DEBUG("error disconnect:%s", strerror(errno));
		}
	}
	else if(read_count > 0)
	{
		if(thiz->on_recv != NULL)
		{
			thiz->on_recv(thiz, _socket_index, read_buf, read_count);
		}
		read_buf[read_count] = 0;
		TRACE_DEBUG("recv %d, %s", read_count, read_buf);
	}
}

static void _clientsocket_write(TcpServer* thiz, int _socket_index)
{
	ClientSocket* cs = &thiz->m_client_socket[_socket_index];
	LDListItemHead* item = LDList_get_head(cs->send_list);
	if(item != NULL)
	{
		SocketBuf* sbuf;
		LDList_copy_data(item, &sbuf);
		int send_total = sbuf->end - sbuf->current;
		int send_count = Write(cs->socket, sbuf->current, send_total);
		if(send_count < 0)
		{
			Close(cs->socket);
			cs->socket = -1;
			if(thiz->on_disconnect != NULL)
			{
				thiz->on_disconnect(thiz, _socket_index);
			}
			TRACE_DEBUG("disconnect:%s", strerror(errno));
		}
		else if(send_count > 0)
		{
			if(send_count == send_total)
			{
				SocketBuf_destory(sbuf);
				sbuf = NULL;
				LDList_del(cs->send_list, item);
				item = NULL;
			}
			else if(send_count < send_total)
			{
				sbuf->current += send_count;
			}
		}
	}
}

static void _clean_up_client(TcpServer* thiz, int _socket_index)
{
	Close(thiz->m_client_socket[_socket_index].socket);
	thiz->m_client_socket[_socket_index].socket = -1;
	if(thiz->on_disconnect != NULL)
	{
		thiz->on_disconnect(thiz, _socket_index);
	}
	int _loop(LDListItemHead* _item, void* _attr_args)
	{
		SocketBuf* sbuf;
		LDList_copy_data(_item, &sbuf);
		if(sbuf != NULL)
		{
			SocketBuf_destory(sbuf);
			sbuf = NULL;
		}
		return 0;
	}
	LDList_loop(LDList_get_head(thiz->m_client_socket[_socket_index].send_list), _loop, NULL);
	LDList_del_all(thiz->m_client_socket[_socket_index].send_list);
}

static void* _tcpserver_accept_proc(void* arg)
{
	TRACE_INFO("%s start. tid=%d, pid=%d", __FUNCTION__, (unsigned int)pthread_self(), getpid());

	TcpServer* thiz = (TcpServer*)arg;
	int i;
	fd_set rfd;
	fd_set wfd;
	int ret = 0;
	struct timeval timeout;
	int max_fd = 0;
	while(thiz->m_accept_proc_run == 1)
	{
		ASSERT(thiz->m_listen_socket != -1, "thiz->m_listen_socket=%d", thiz->m_listen_socket);

		max_fd = 0;
		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		FD_SET(thiz->m_listen_socket, &rfd);
		max_fd = MAX(thiz->m_listen_socket, max_fd);
		for(i = 0; i < thiz->m_max_client; i++)
		{
			if(thiz->m_client_socket[i].socket != -1)
			{
				FD_SET(thiz->m_client_socket[i].socket, &rfd);
				max_fd = MAX(thiz->m_client_socket[i].socket, max_fd);
				if(LDList_get_count(thiz->m_client_socket[i].send_list) > 0)
				{
					FD_SET(thiz->m_client_socket[i].socket, &wfd);
					max_fd = MAX(thiz->m_client_socket[i].socket, max_fd);
				}
			}
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		ret = select(max_fd+1, &rfd, &wfd, NULL, &timeout);

		if(ret > 0)
		{
			//check connect
			if(FD_ISSET(thiz->m_listen_socket, &rfd))
			{
#if !defined(COMM_TCP_USE_UNIX_SOCK)			
				struct sockaddr_in tmp_addr;
				int sin_size = sizeof(struct sockaddr_in);
				for(i = 0; i < thiz->m_max_client; i++)
				{
					if(thiz->m_client_socket[i].socket == -1)
					{
						int tmpfd = Accept(thiz->m_listen_socket, (struct sockaddr *)&tmp_addr, (socklen_t*)&sin_size);
						if(thiz->on_connect != NULL)
						{
							thiz->on_connect(thiz, i, tmpfd);
						}
						thiz->m_client_socket[i].socket = tmpfd;
						thiz->m_client_socket[i].client_addr = tmp_addr;
						TRACE_DEBUG("Client %s Connected!, client socket = %d", inet_ntoa(tmp_addr.sin_addr), thiz->m_client_socket[i].socket);
						break;
					}
				}
				if(i == thiz->m_max_client)
				{
					int tmpfd = Accept(thiz->m_listen_socket, (struct sockaddr *)&tmp_addr, (socklen_t*)&sin_size);
					if(thiz->on_max_connect != NULL)
					{
						thiz->on_max_connect(thiz, tmpfd);
					}
					else
					{
						Close(tmpfd);
					}
					TRACE_INFO("Max client.");
				}
#else
				char peerPath[200];
				for(i = 0; i < thiz->m_max_client; i++)
				{
					memset(peerPath, 0, sizeof(peerPath));
					if(thiz->m_client_socket[i].socket == -1)
					{
						int tmpfd = SOCK_domain_accept(thiz->m_listen_socket, peerPath);
						if(thiz->on_connect != NULL)
						{
							thiz->on_connect(thiz, i, tmpfd);
						}
						thiz->m_client_socket[i].socket = tmpfd;
						//thiz->m_client_socket[i].client_addr = tmp_addr;
						TRACE_DEBUG("Client %s Connected!, client socket = %d", peerPath, thiz->m_client_socket[i].socket);
						break;
					}
				}
				if(i == thiz->m_max_client)
				{
					int tmpfd = SOCK_domain_accept(thiz->m_listen_socket, peerPath);
					if(thiz->on_max_connect != NULL)
					{
						thiz->on_max_connect(thiz, tmpfd);
					}
					else
					{
						Close(tmpfd);
					}
					TRACE_INFO("Max client.");
				}

#endif								
			}

			for(i = 0; i < thiz->m_max_client; i++)
			{
				//check read;
				if(thiz->m_client_socket[i].socket != -1 && FD_ISSET(thiz->m_client_socket[i].socket, &rfd))
				{
					_clientsocket_read(thiz, i);
				}
				//check write;
				if(thiz->m_client_socket[i].socket != -1 && FD_ISSET(thiz->m_client_socket[i].socket, &wfd))
				{
					_clientsocket_write(thiz, i);
				}
			}
		}
	}
	return NULL;
}

TcpServer* TcpServer_init(int _port, int _max_client,
		TcpServerOnConnect _on_connect,
		TcpServerOnDisConnect _on_disconnect,
		TcpServerOnMaxConnect _on_max_connect,
		TcpServerOnRecv _on_recv)
{
	int total_size = sizeof(TcpServer) + _max_client * sizeof(ClientSocket);
	TcpServer* thiz = (TcpServer*)MALLOC(total_size);
	memset(thiz, 0, total_size);

	thiz->m_port = _port;
	thiz->m_listen_socket = -1;
	thiz->m_max_client = _max_client;
	thiz->m_client_socket = (ClientSocket*)(thiz+1);
	int i;
	for(i = 0; i < thiz->m_max_client; i++)
	{
		thiz->m_client_socket[i].socket = -1;
		thiz->m_client_socket[i].send_list = LDList_init(sizeof(SocketBuf*));
	}

	thiz->on_connect = _on_connect;
	thiz->on_disconnect = _on_disconnect;
	thiz->on_max_connect = _on_max_connect;
	thiz->on_recv = _on_recv;

#if !defined(COMM_TCP_USE_UNIX_SOCK)
	thiz->m_listen_socket = Socket (AF_INET, SOCK_STREAM, 0);
	ASSERT(thiz->m_listen_socket >= 0, "socket error: %s", strerror(errno));


	int ret;
	int yes = 1;
	ret = setsockopt(thiz->m_listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	ASSERT(ret == 0, "setsockopt error: %s", strerror(errno));


	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(thiz->m_port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	Bind(thiz->m_listen_socket, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));

	Listen(thiz->m_listen_socket, 10);
#else
	thiz->m_listen_socket = SOCK_domain_listen(COMM_UNIX_SOCK_SERVER);
	ASSERT(thiz->m_listen_socket >= 0, "socket error: %s", strerror(errno));
#endif
	return thiz;
}

void TcpServer_proc(TcpServer* thiz)
{
	ASSERT(thiz != NULL, "thiz is NULL");
	thiz->m_accept_proc_run = 1;
	pthread_create(&thiz->m_accept_proc_id, NULL, _tcpserver_accept_proc, thiz);
}

void TcpServer_destory(TcpServer* thiz)
{
	ASSERT(thiz != NULL, "thiz is NULL");
	if(thiz->m_accept_proc_run == 1)
	{
		thiz->m_accept_proc_run = 0;
		pthread_join(thiz->m_accept_proc_id, NULL);
		thiz->m_accept_proc_id = 0;
	}

	if(thiz->m_listen_socket != -1)
	{
		Close(thiz->m_listen_socket);
		thiz->m_listen_socket = -1;
	}

	int i;
	for(i = 0; i < thiz->m_max_client; i++)
	{
		if(thiz->m_client_socket[i].socket != -1)
		{
			_clean_up_client(thiz, i);
		}
		LDList_destory(thiz->m_client_socket[i].send_list);
	}
	free(thiz);
}

int TcpServer_is_connected(TcpServer* thiz, int _socket_index)
{
	ASSERT(thiz != NULL, "thiz is NULL");
	ASSERT(_socket_index < thiz->m_max_client, "_socket_index = %d", _socket_index);
	if(thiz->m_client_socket[_socket_index].socket == -1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void TcpServer_disconnect(TcpServer* thiz, int _socket_index)
{
	ASSERT(thiz != NULL, "thiz is NULL");
	ASSERT(_socket_index < thiz->m_max_client, "_socket_index = %d", _socket_index);
	if(thiz->m_client_socket[_socket_index].socket != -1)
	{
		_clean_up_client(thiz, _socket_index);
	}
}

void TcpServer_send(TcpServer* thiz, int _socket_index, void* _buf, int _len)
{
	ASSERT(thiz != NULL, "thiz is NULL");
	ASSERT(_buf != NULL, "_buf is NULL");
	ASSERT(_socket_index < thiz->m_max_client, "_socket_index = %d", _socket_index);

	SocketBuf* sbuf = SocketBuf_init(_buf, _len);
	ASSERT(sbuf != NULL, "sbuf is null. _buf=%s, _len=%d", (char*)_buf, _len);
	LDList_add(thiz->m_client_socket[_socket_index].send_list, &sbuf);
}

