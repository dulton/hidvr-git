/*
 * gpio_manage.h
 *
 *  Created on: 2010-12-1
 *      Author: root
 */

#ifndef GPIO_MANAGE_H_
#define GPIO_MANAGE_H_

#include "gpio.h"


extern int pthread_gpio_manage_init();
extern void pthread_gpio_manage_deinti();

typedef struct tagGpioManage GpioManage;
struct tagGpioManage
{
	pthread_t m_gpio_proc_tid;
	int m_gpio_proc_running;

	int alerm_sec;
	int buzzer_sec;

	void (*init_proc)(GpioManage* thiz);
	void (*set_alerm)(GpioManage* thiz, int _sec);
	void (*set_buzzer)(GpioManage* thiz, int _sec);

};

extern GpioManage* PGpioManageStruct();
extern int PGpioManageDestruct(GpioManage* thiz);
extern GpioManage* g_gpio_manage_ptr;

#endif //GPIO_MANAGE_H_

