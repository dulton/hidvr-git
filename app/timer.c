/*
 * timer.c
 *
 *  Created on: 2010-12-25
 *      Author: root
 */

#include <pthread.h>

#include "std.h"
#include "timer.h"

int g_timer_count;
TIMER* g_timer_head;
pthread_attr_t g_tattr;

//int g_timer_running;
//pthread_t g_timer_tid;

//void* timer_proc(void* arg)
//{
//	time_t now;
//	TIMER* pcurr;
//	TIMER* pfound;
//	int tmp_priority;
//
//	pthread_attr_t tattr;
//	pthread_attr_init(&tattr);
//	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
//	while(g_timer_running == 1)
//	{
//		now = time(NULL);
//
//		pfound = NULL;
//		tmp_priority = -1;
//		pcurr = g_timer_head;
//		while(pcurr != NULL)
//		{
//			if((now - pcurr->last_time) >= pcurr->period  && pcurr->priority > tmp_priority)
//			{
//				pcurr->last_time = now;
//				pthread_create(&pcurr->tid, &tattr, pcurr->call_back, (void*)pcurr->id);
//			}
//			pcurr = pcurr->next;
//		}
//
////		printf("alive\n");
//		usleep(500000);
//	}
//	return NULL;
//}

void* timer_thread(void* arg)
{
	TIMER t = *(TIMER*)arg;
	sleep(t.period);

	while(1)
	{
		if(t.call_back != NULL)
		{
			t.call_back((void*)t.id);
		}
		pthread_testcancel();
		sleep(t.period);
	}
	return NULL;
}

void init_timer()
{
	g_timer_count = 0;
	g_timer_head = NULL;
	pthread_attr_init(&g_tattr);
	pthread_attr_setdetachstate(&g_tattr, PTHREAD_CREATE_DETACHED);

//	g_timer_running = 1;
//	pthread_create(&g_timer_tid, NULL, timer_proc, NULL);
}

void free_timer()
{
//	g_timer_running = 0;
//	pthread_join(g_timer_tid, NULL);

	TIMER* ptmp;
	while(g_timer_head != NULL)
	{
		ptmp = g_timer_head->next;
		free(g_timer_head);
		g_timer_head = ptmp;
	}
}

int add_timer(int _priority, int _period, TIMER_CALLBACK _call_back)
{
	int ret = -1;
	TIMER* pitem = (TIMER*)malloc(sizeof(TIMER));
	if(pitem != NULL)
	{
		pitem->priority = _priority;
		pitem->period = _period;
		pitem->call_back = _call_back;
		pitem->last_time = time(NULL);
		pitem->next = NULL;
		pitem->id = g_timer_count + 1;
		ret = pitem->id;
		pthread_create(&pitem->tid, &g_tattr, timer_thread, pitem);
		if(g_timer_count == 0)
		{
			g_timer_head = pitem;
		}
		else
		{
			TIMER* plast = g_timer_head;
			while(plast->next != NULL)
			{
				plast = plast->next;
			}
			plast->next = pitem;
		}
		g_timer_count++;
		return ret;
	}
	return ret;
}

int remove_timer(int _id)
{
	int is_found = 0;
	TIMER* pprev = NULL;
	TIMER* pfound = g_timer_head;
	while(pfound != NULL)
	{
		if(pfound->id == _id)
		{
			is_found = 1;

			pthread_cancel(pfound->tid);
			if(pprev == NULL)
			{
				g_timer_head = pfound->next;
				free(pfound);
			}
			else
			{
				pprev->next = pfound->next;
				free(pfound);
			}

			return 0;
		}
		pprev = pfound;
		pfound = pfound->next;
	}
	return -1;
}

void print_timer()
{
	TIMER* pcurr = g_timer_head;

	while(pcurr != NULL)
	{
		printf("id=%d, priority=%d, period=%d, call_back=%x\n",
				pcurr->id, pcurr->priority, pcurr->period, (unsigned int)pcurr->call_back);
		pcurr = pcurr->next;
	}
}

