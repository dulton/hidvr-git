

#ifndef __GPIO_DRV_H__
#define __GPIO_DRV_H__

#include <linux/ioctl.h>

enum {
    GPIO_PROBE_VER0 = 0,
    GPIO_PROBE_VER1,
    GPIO_PROBE_VER2,
    GPIO_PROBE_NUMB,
};

#define GPIO_DEVICE_NAME "gpio"

#define GPIO_IOC_MAGIC ('G')
#define GPIO_SET_MOTHERBRD _IOW(GPIO_IOC_MAGIC, 1, int)
#define GPIO_SET_BUZZER _IOW(GPIO_IOC_MAGIC, 2, int)
#define GPIO_SET_ALARM _IOW(GPIO_IOC_MAGIC, 3, int)
#define GPIO_SET_RS485DE _IOW(GPIO_IOC_MAGIC, 4, int)
#define GPIO_GET_SENSOR _IOWR(GPIO_IOC_MAGIC, 5, int)
#define GPIO_RST_DEVICE _IO(GPIO_IOC_MAGIC, 6)
#define GPIO_SET_KEYPAD_BUZZER _IOW(GPIO_IOC_MAGIC, 7, int)
#define GPIO_SET_IRDA_BUZZER   _IOW(GPIO_IOC_MAGIC, 8, int)
#define GPIO_SET_LED1 _IOW(GPIO_IOC_MAGIC, 10, int)
#define GPIO_SET_LED2 _IOW(GPIO_IOC_MAGIC, 11, int)
#define GPIO_GET_PLAT_VER _IOR(GPIO_IOC_MAGIC, 12, unsigned int)

#ifdef __KERNEL__
// driver part

#include "../../conf.h"

extern void gpio_set_buzzer(int set);
extern void gpio_keypad_buzzer(void);
extern void gpio_irda_buzzer(void);
extern void gpio_set_alarm(int set);
extern void gpio_set_rs485de(int set);
extern int  gpio_get_sensor(unsigned int sensor);
extern void gpio_rst_device(void);

extern unsigned char gpio_edit_irda_id(unsigned char id);
extern unsigned char gpio_edit_irda_match(void);

extern int gpio_AckoutFreqSel(unsigned long Freq);
extern int gpio_Platform_VerProbe(void);

#else //__KERNEL__
// application part

extern void GPIODRV_SetMotherboard(int nNode, int enBoard);

extern void GPIODRV_SetBuzzer(int nNode, int nPin);
extern void GPIODRV_SetAlarm(int nNode, int nPin);

// sensor
extern int GPIODRV_GetSensor(int nNode, int nSensor);
extern void GPIODRV_SetRS485DE(int nNode, int nPin);

// keypad buzzer
extern void GPIODRV_SetKeypadBuzzer(int nNode, int nOnOff);
extern void GPIODRV_SetIrdaBuzzer(int nNode, int nOnOff);

// blink
extern void GPIODRV_SetLED1(int nNode, int nPin);
extern void GPIODRV_SetLED2(int nNode, int nPin);

extern int  KEYPADDRV_SetInit(void);
extern int  KEYPADDRV_SetDevID(unsigned char u8Id);
extern int  KEYPADDRV_SetPWROff(unsigned char BeOff);

extern int GPIODRV_GetVerFix(void);

#endif //__KERNEL__

#endif //__GPIO_DRV_H__

