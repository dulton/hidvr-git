/*
 * queue.c
 *
 *  Created on: 2011-4-29
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#include "queue.h"


static void _lock(QUEUE* _queue)
{
	pthread_mutex_lock(&_queue->lock);
}

static void _unlock(QUEUE* _queue)
{
	pthread_mutex_unlock(&_queue->lock);
}

static void* _find_blank_item(QUEUE* _queue)
{
	QUEUE_ITEM_HEAD* item_head = (QUEUE_ITEM_HEAD*)_queue->buf;
	int current_len = 0;
	while(current_len < QUEUE_BUF_MAX)
	{
		if(item_head->used == QUEUE_UNUSED_FLAG)
		{
			return (void*)item_head;
		}
		current_len += sizeof(QUEUE_ITEM_HEAD) + _queue->item_size;
		item_head = (QUEUE_ITEM_HEAD*)(_queue->buf + current_len);
	}
	return NULL;
}

QUEUE* queue_init(QUEUE_COMPARE _compare, int _item_size)
{
	QUEUE* queue = malloc(sizeof(QUEUE));
	memset(queue, 0, sizeof(QUEUE));
	queue->item_size = _item_size;
	queue->item_count = 0;
	queue->compare = _compare;
	queue->head = NULL;
	queue->tail = NULL;
	pthread_mutex_init(&queue->lock, NULL);
	queue->buf = malloc(QUEUE_BUF_MAX);
	memset(queue->buf, 0, QUEUE_BUF_MAX);
	return queue;
}



int queue_detory(QUEUE* _queue)
{
	if(_queue != NULL)
	{
		if(_queue->buf != NULL)
		{
			free(_queue->buf);
		}
		pthread_mutex_destroy(&_queue->lock);
		free(_queue);
	}
	return 0;
}

int queue_print(QUEUE* _queue)
{
	return 0;
}


int queue_add(QUEUE* _queue, void* _data)
{
retry:
	_lock(_queue);
	QUEUE_ITEM_HEAD* tmp;
	if(_queue->item_count == 0)
	{
		_queue->head = _queue->buf;
		_queue->tail = _queue->buf;
		_queue->item_count ++;

		tmp = _queue->head;
		tmp->used = QUEUE_USED_FLAG;
		tmp->next = NULL;
		tmp->prev = NULL;
		memcpy(tmp+1, _data, _queue->item_size);
	}
	else
	{
		tmp = _find_blank_item(_queue);
		if(tmp != NULL)
		{
			tmp->used = QUEUE_USED_FLAG;
			tmp->next = NULL;
			tmp->prev = _queue->tail;
			memcpy(tmp+1, _data, _queue->item_size);

			((QUEUE_ITEM_HEAD*)_queue->tail)->next = tmp;

			_queue->tail = tmp;
			_queue->item_count ++;
		}
		else //queue full
		{
			_unlock(_queue);
			usleep(5000);
			goto retry;
		}
	}
	_unlock(_queue);
	return 0;
}

int queue_get(QUEUE* _queue, void* _data)
{
	int found = -1;

	if (_queue == NULL) return -1;
	_lock(_queue);

	QUEUE_ITEM_HEAD* tmp = _queue->head;
	while(tmp != NULL)
	{
		if(_queue->compare(tmp+1, _data) == 0)
		{
			memcpy(_data, tmp+1, _queue->item_size);

			if(tmp->prev == NULL)
			{
				_queue->head = tmp->next;
			}
			else
			{
				((QUEUE_ITEM_HEAD*)tmp->prev)->next = tmp->next;
			}

			if(tmp->next == NULL)
			{
				_queue->tail = tmp->prev;
			}
			else
			{
				((QUEUE_ITEM_HEAD*)tmp->next)->prev = tmp->prev;
			}

			_queue->item_count --;
			memset(tmp, 0, sizeof(QUEUE_ITEM_HEAD)+_queue->item_size);
			found = 1;
			break;
		}
		tmp = tmp->next;
	}
	_unlock(_queue);
	if(found == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int queue_clear(QUEUE* _queue)
{
	_lock(_queue);
	_queue->item_count = 0;
	_queue->head = NULL;
	_queue->tail = NULL;
	memset(_queue->buf, 0, QUEUE_BUF_MAX);
	_unlock(_queue);
	return 0;
}
