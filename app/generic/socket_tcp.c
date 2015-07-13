
#include "socket_tcp.h"
#include <assert.h>

static int tcp_close(struct SOCKET_TCP* const tcp)
{
	return close(tcp->sock);
}

static int tcp_connect(struct SOCKET_TCP* const tcp, const char* serv_addr, in_port_t serv_port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serv_port);
	addr.sin_addr.s_addr = inet_addr(serv_addr);
	return connect(tcp->sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
}

static int tcp_listen(struct SOCKET_TCP* const tcp, in_port_t port, int backlog)
{
	int ret = 0;
	struct sockaddr_in addr;

	// bind
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	ret = bind(tcp->sock, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0){
		return ret;
	}

	// listen
	ret = listen(tcp->sock, backlog);
	if(ret < 0){
		return ret;
	}
	return 0;
}

static int tcp_set_send_timeout(struct SOCKET_TCP* const tcp, time_t timeout_s, suseconds_t timeout_us)
{
	struct timeval tv = { .tv_sec = timeout_s, .tv_usec = timeout_us, };
	return setsockopt(tcp->sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

static int tcp_set_recv_timeout(struct SOCKET_TCP* const tcp, time_t timeout_s, suseconds_t timeout_us)
{
	struct timeval tv = { .tv_sec = timeout_s, .tv_usec = timeout_us, };
	return setsockopt(tcp->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

static int tcp_send(struct SOCKET_TCP* const tcp, const void *msg, size_t len, int flags)
{
	return send(tcp->sock, msg, len, flags);
}

static ssize_t tcp_recv(struct SOCKET_TCP* const tcp, void *buf, size_t len, int flags)
{
	return recv(tcp->sock, buf, len, flags);
}

static int tcp_send_timeout(struct SOCKET_TCP* const tcp, const void *msg, size_t len, time_t timeout_s)
{
	int ret = 0;
	size_t send_len = 0;
	int send_errno = 0;
	time_t const base_time = time(NULL);

	while(send_len < len){
		if(time(NULL) - base_time > timeout_s){
			// send timeout
			// only send a part of buffer
			break;
		}

		ret = send(tcp->sock, msg + send_len, len - send_len, MSG_DONTWAIT);
		send_errno = errno;
		if(ret < 0){
			if(EAGAIN == send_errno || EINTR == send_errno){
				// need to retry
				continue;
			}
			return ret;
		}
		send_len += ret;
	}
	return send_len;
}


static int tcp_getsockname(struct SOCKET_TCP* const tcp, struct sockaddr *name, socklen_t *namelen)
{
	return getsockname(tcp->sock, name, namelen);
}

static int tcp_getpeername(struct SOCKET_TCP* const tcp, struct sockaddr *name, socklen_t *namelen)
{
	return getpeername(tcp->sock, name, namelen);
}

static SOCKET_TCP_t* tcp_create(int sock)
{
	int ret = 0;
	int reuse_on = 1;
	SOCKET_TCP_t* tcp = calloc(sizeof(SOCKET_TCP_t), 1);
	assert(tcp);
	
	// create a system socket
	tcp->sock = sock;
	// reuse address
	ret = setsockopt(tcp->sock, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(reuse_on));
	
	// install the interfaces
	tcp->close = tcp_close;
	tcp->connect = tcp_connect;
	tcp->listen = tcp_listen;
	tcp->set_send_timeout = tcp_set_send_timeout;
	tcp->set_recv_timeout = tcp_set_recv_timeout;
	tcp->send = tcp_send;
	tcp->send_timeout = tcp_send_timeout;
	tcp->recv = tcp_recv;
	tcp->getsockname = tcp_getsockname;
	tcp->getpeername = tcp_getpeername;
	// success
	return tcp;
}

SOCKET_TCP_t* socket_tcp_create()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	return tcp_create(sock);
}

SOCKET_TCP_t* socket_tcp_dup(int sock)
{
	return tcp_create(sock);
}

void socket_tcp_release(SOCKET_TCP_t* tcp)
{
	if(tcp){
		// need to close the socket automatically?
		free(tcp);
		tcp = NULL;
	}
}

