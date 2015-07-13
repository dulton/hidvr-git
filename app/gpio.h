

#ifndef __GPIO_H__
#define __GPIO_H__

#include "conf.h"
#include "bsp.h"

typedef struct Gpio
{
	int nFileD;
	// buzzer
	void (*SetBuzzer)(struct Gpio* const thiz, unsigned char const bSet);
	// alarm
	void (*SetAlarm)(struct Gpio* const thiz, unsigned char const bSet);
	// sensor
	int (*GetSensor)(struct Gpio* const thiz, int const nSensor);
	// rs485
	void (*EnableRS485)(struct Gpio* const thiz);
	void (*DisableRS485)(struct Gpio* const thiz);
	// les
	void (*SetLED1)(struct Gpio* const thiz, unsigned char const bSet);
	void (*SetLED2)(struct Gpio* const thiz, unsigned char const bSet);
	// keypad buzzer
	void (*EnableKeypadBuzzer)(struct Gpio* const thiz);
	void (*DisableKeypadBuzzer)(struct Gpio* const thiz);
	// irda buzzer
	void (*EnableIrdaBuzzer)(struct Gpio* const thiz);
	void (*DisableIrdaBuzzer)(struct Gpio* const thiz);
	// blink
	void (*AliveBlink)(struct Gpio* const thiz);
	void (*HddBlink)(struct Gpio* const thiz);
	// BEGIN: Added by Law, 2011/6/23   PN:irda id function added 
	// irda id
	void (*SetIrdaId)(struct Gpio* const thiz, unsigned char u8Id);
	void (*SetIrdaBuzzer)(struct Gpio* const thiz, int SetOn);
}Gpio;

extern Gpio* PGPIO_Struct();
extern void PGPIO_Destruct(Gpio** pthiz);

extern int GPIO_Init(void);
extern void GPIO_Destroy(void);
extern void GPIO_SetBuzzer(unsigned char const bSet);
extern void GPIO_SetAlarm(unsigned char const bSet);
extern int GPIO_GetSensor(int nSensor);

extern void GPIO_SetLED1(unsigned char const bSet);
extern void GPIO_SetLED2(unsigned char const bSet);

extern void GPIO_EnableRS485(void);
extern void GPIO_DisableRS485(void);
extern void GPIO_EnableKeypadBuzzer(void);
extern void GPIO_DisableKeypadBuzzer(void);

extern void GPIO_AliveBlink();
extern void GPIO_HddBlink();

// BEGIN: Added by Law, 2011/6/23   PN:irda id function added 
extern void GPIO_SetIrdaId(unsigned char u8Id);
extern void GPIO_SetPWROff(unsigned char u8Id);
// END:   Added by Law, 2011/6/23   PN:irda id function added 
// BEGIN: Added by Law, 2011/7/5   PN:admin default password is null 
extern unsigned char GPIO_GetIrdaId();
// END:   Added by Law, 2011/7/5   PN:admin default password is null 

#endif

