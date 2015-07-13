/*
 * socket_buf.c
 *
 *  Created on: 2011-8-13
 *      Author: root
 */

#include <string.h>

#include "mempool.h"
#include "socket_buf.h"

SocketBuf* SocketBuf_init(void* _buf, int _len)
{
	SocketBuf* thiz = (SocketBuf*)MALLOC(sizeof(SocketBuf) + _len);

	thiz->data = (void*)(thiz + 1);
	thiz->current = thiz->data;
	thiz->end = thiz->data + _len;
	memcpy(thiz->data, _buf, _len);
	return thiz;
}

void SocketBuf_destory(SocketBuf* thiz)
{
	free(thiz);
}
