/*
 * socket_buf.h
 *
 *  Created on: 2011-8-13
 *      Author: root
 */

#ifndef SOCKET_BUF_H_
#define SOCKET_BUF_H_

typedef struct _SocketBuf
{
	void* data;
	void* current;
	void* end;
}SocketBuf;

extern SocketBuf* SocketBuf_init(void* _buf, int _len);
extern void SocketBuf_destory(SocketBuf* thiz);

#endif /* SOCKET_BUF_H_ */
