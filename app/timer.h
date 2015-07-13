/*
 * timer.h
 *
 *  Created on: 2010-12-25
 *      Author: root
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

typedef void* (*TIMER_CALLBACK)(void*);

typedef struct _TIMER TIMER;
struct _TIMER
{
	int id;
	int priority;
	int period;
	time_t last_time;
	pthread_t tid;
	TIMER_CALLBACK call_back;
	TIMER* next;
};

void init_timer();
void free_timer();
int add_timer(int _priority, int _period, TIMER_CALLBACK _call_back);
int remove_timer(int _id);

#endif /* TIMER_H_ */

