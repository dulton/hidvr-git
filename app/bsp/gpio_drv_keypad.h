#ifndef __GPIO_DRV_KEYPAD_H__
#define __GPIO_DRV_KEYPAD_H__

#include "gpio_drv.h"

//#define KEYPAD_ISOLATE_DRIVER

typedef enum {
	KEYPAD_INIT = 0,
	KEYPAD_PWROFF,
	KEYPAD_RESET,
	KEYPAD_DEVID,
} KEY_IOCTL;

#define KEYPAD_DEV_NAME "keypad"

typedef enum
{
	KEY_NULL = -1,
	KEY_RELEASE,
	KEY_PRESS,
}KEY_STATUS;

#ifndef KEYPAD_ISOLATE_DRIVER
extern int keypad_init(void);
extern void keypad_exit(void);
#endif

#endif //__GPIO_DRV_KEYPAD_H__
