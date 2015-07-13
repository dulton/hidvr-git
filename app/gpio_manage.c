/*
 * gpio_manage.c
 *
 *  Created on: 2010-12-1
 *      Author: root
 */

#include "std.h"
#include <pthread.h>

#include "gpio_manage.h"
#include <sys/ioctl.h>
#include "gpio.h"

GpioManage* g_gpio_manage_ptr = NULL;



int pthread_gpio_manage_init()
{
	g_gpio_manage_ptr = PGpioManageStruct();
	g_gpio_manage_ptr->init_proc(g_gpio_manage_ptr);

	return 0;
}

void pthread_gpio_manage_deinti()
{
	g_gpio_manage_ptr->m_gpio_proc_running = 0;
	pthread_join(g_gpio_manage_ptr->m_gpio_proc_tid, NULL);

	GPIO_SetAlarm(GPIO_PIN_LOW);
	GPIO_SetBuzzer(GPIO_PIN_LOW);

	PGpioManageDestruct(g_gpio_manage_ptr);
	g_gpio_manage_ptr = NULL;
}

static void* gpio_proc(void* arg)
{
	GpioManage* thiz = (GpioManage*)g_gpio_manage_ptr;
	while(thiz->m_gpio_proc_running == 1)
	{
		if(thiz->alerm_sec > 0)
		{
			GPIO_SetAlarm(GPIO_PIN_HIGH);
			thiz->alerm_sec--;
		}
		else
		{
			GPIO_SetAlarm(GPIO_PIN_LOW);
		}

		if(thiz->buzzer_sec > 0)
		{
			GPIO_SetBuzzer(GPIO_PIN_HIGH);
			thiz->buzzer_sec--;
		}
		else
		{
			GPIO_SetBuzzer(GPIO_PIN_LOW);
		}
		sleep(1);
		if(thiz->alerm_sec > 0 || thiz->buzzer_sec > 0)
		{
			TRACE_DEBUG("thiz->buzzer_sec=%d, thiz->alerm_sec=%d", thiz->buzzer_sec, thiz->alerm_sec);
		}
	}
	return NULL;
}

static void init_proc(GpioManage* thiz)
{
	thiz->m_gpio_proc_running = 1;
	pthread_create(&thiz->m_gpio_proc_tid, NULL, gpio_proc, thiz);
}

static void set_alerm(GpioManage* thiz, int _sec)
{
	thiz->alerm_sec = _sec;
}

static void set_buzzer(GpioManage* thiz, int _sec)
{
	thiz->buzzer_sec = _sec;
}


GpioManage* PGpioManageStruct()
{
	GpioManage* thiz = (GpioManage*)malloc(sizeof(GpioManage));

	thiz->m_gpio_proc_tid = 0;
	thiz->m_gpio_proc_running = 0;

	thiz->alerm_sec = 0;
	thiz->buzzer_sec = 0;

	thiz->init_proc = init_proc;
	thiz->set_alerm = set_alerm;
	thiz->set_buzzer = set_buzzer;

	return thiz;
}

int PGpioManageDestruct(GpioManage* thiz)
{
	if(!thiz)
	{
		return -1;
	}

	free(thiz);
	thiz = NULL;

	return 0;
}

