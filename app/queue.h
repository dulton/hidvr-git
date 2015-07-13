/*
 * queue.h
 *
 *  Created on: 2011-4-29
 *      Author: root
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <assert.h>
#include <stdio.h>
#include <pthread.h>

#define QUEUE_USED_FLAG   (0xFFFFFFFF)
#define QUEUE_UNUSED_FLAG (0x00000000)
typedef unsigned int QUEUE_NEXT_TYPE;
#define QUEUE_BUF_MAX (1024*128)
typedef int(*QUEUE_COMPARE)(void* _data1, void* _data2);
typedef struct
{
	int item_size;
	int item_count;
	QUEUE_COMPARE compare;
	unsigned char* buf;
	void* head;
	void* tail;
	pthread_mutex_t lock;
}QUEUE;

typedef struct
{
	int used;
	void* next;
	void* prev;
}QUEUE_ITEM_HEAD;

extern QUEUE* queue_init(QUEUE_COMPARE _compare, int _item_size);
extern int queue_detory(QUEUE* _queue);
extern int queue_print(QUEUE* _queue);
extern int queue_add(QUEUE* _queue, void* _data);
extern int queue_get(QUEUE* _queue, void* _data);
extern int queue_clear(QUEUE* _queue);


#endif /* QUEUE_H_ */
