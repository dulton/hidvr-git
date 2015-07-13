
#include <pthread.h>

//#define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "gpio.h"

#define GPIO_DEV	"/dev/gpio"

#define DEBUG_GPIO
#ifdef DEBUG_GPIO
#define GPIO_TRACE(fmt...) \
	do{printf("\033[1;31mGPIO->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define GPIO_TRACE(fmt...)
#endif


// buzzer
static void gpio_SetBuzzer(struct Gpio* const thiz, unsigned char const bSet)
{
	GPIODRV_SetBuzzer(thiz->nFileD, bSet);
}

// alarm
static void gpio_SetAlarm(struct Gpio* const thiz, unsigned char const bSet)
{
	GPIODRV_SetAlarm(thiz->nFileD, bSet);
}

// sensor
static int gpio_GetSensor(struct Gpio* const thiz, int nSensor)
{
	return GPIODRV_GetSensor(thiz->nFileD, nSensor);
}

// led
static void gpio_SetLED1(struct Gpio* const thiz, unsigned char const bSet)
{
	GPIODRV_SetLED1(thiz->nFileD, bSet);
}

static void gpio_SetLED2(struct Gpio* const thiz, unsigned char const bSet)
{
	GPIODRV_SetLED2(thiz->nFileD, bSet);
}

static void gpio_EnableRS485(struct Gpio* const thiz)
{
	GPIODRV_SetRS485DE(thiz->nFileD, GPIO_PIN_HIGH);
}

static void gpio_DisableRS485(struct Gpio* const thiz)
{
	GPIODRV_SetRS485DE(thiz->nFileD, GPIO_PIN_LOW);
}

static void gpio_EnableKeypadBuzzer(struct Gpio* const thiz)
{
	GPIODRV_SetKeypadBuzzer(thiz->nFileD, TRUE);
}

static void gpio_DisableKeypadBuzzer(struct Gpio* const thiz)
{
	GPIODRV_SetKeypadBuzzer(thiz->nFileD, FALSE);
}

static void gpio_EnableIrdaBuzzer(struct Gpio* const thiz)
{
	GPIODRV_SetIrdaBuzzer(thiz->nFileD, TRUE);
}

static void gpio_DisableIrdaBuzzer(struct Gpio* const thiz)
{
	GPIODRV_SetIrdaBuzzer(thiz->nFileD, FALSE);
}

// blink
static void gpio_AliveBlink(struct Gpio* const thiz)
{
	static int nAliveStatus = FALSE;
	nAliveStatus = !nAliveStatus;
	GPIODRV_SetLED1(thiz->nFileD, nAliveStatus);
}

static void gpio_HddBlink(struct Gpio* const thiz)
{
	static int nHddStatus = FALSE;
	nHddStatus = !nHddStatus;
	GPIODRV_SetLED2(thiz->nFileD, nHddStatus);
}

static void gpio_SetIrdaId(struct Gpio* const thiz, unsigned char u8Id)
{
	KEYPADDRV_SetDevID(u8Id);
}

static void gpio_SetIrdaBuzzer(struct Gpio* const thiz, int SetOn)
{
	GPIODRV_SetIrdaBuzzer(thiz->nFileD, SetOn);
}

Gpio* PGPIO_Struct()
{
	Gpio* thiz = NULL;
	thiz = calloc(sizeof(Gpio), 1);
	thiz->nFileD = open("/dev/"GPIO_DEVICE_NAME, O_RDWR);
	// interface
	thiz->SetBuzzer = gpio_SetBuzzer;
	thiz->SetAlarm = gpio_SetAlarm;
	thiz->GetSensor = gpio_GetSensor;
	thiz->SetLED1 = gpio_SetLED1;
	thiz->SetLED2 = gpio_SetLED2;
	thiz->EnableRS485 = gpio_EnableRS485;
	thiz->DisableRS485 = gpio_DisableRS485;
	thiz->EnableKeypadBuzzer = gpio_EnableKeypadBuzzer;
	thiz->DisableKeypadBuzzer = gpio_DisableKeypadBuzzer;
	thiz->EnableIrdaBuzzer = gpio_EnableIrdaBuzzer;
	thiz->DisableIrdaBuzzer = gpio_DisableIrdaBuzzer;
	thiz->AliveBlink = gpio_AliveBlink;
	thiz->HddBlink = gpio_HddBlink;
	thiz->SetIrdaId = gpio_SetIrdaId;
	thiz->SetIrdaBuzzer = gpio_SetIrdaBuzzer;
	return thiz;
}

void PGPIO_Destruct(Gpio** pthiz)
{
	if((*pthiz)->nFileD){
		close((*pthiz)->nFileD);
		(*pthiz)->nFileD = -1;
	}
	free(*pthiz);
	*pthiz = NULL;
}

static Gpio s_stGpio;

void GPIO_SetBuzzer(unsigned char const bSet)
{
	return s_stGpio.SetBuzzer(&s_stGpio, bSet);
}

void GPIO_SetAlarm(unsigned char const bSet)
{
	return s_stGpio.SetAlarm(&s_stGpio, bSet);
}

int GPIO_GetSensor(int nSensor)
{
	return s_stGpio.GetSensor(&s_stGpio, nSensor);
}

void GPIO_SetLED1(unsigned char const bSet)
{
	return s_stGpio.SetLED1(&s_stGpio, bSet);
}

void GPIO_SetLED2(unsigned char const bSet)
{
	return s_stGpio.SetLED2(&s_stGpio, bSet);
}

void GPIO_EnableRS485(void)
{
	return s_stGpio.EnableRS485(&s_stGpio);
}

void GPIO_DisableRS485(void)
{
	s_stGpio.DisableRS485(&s_stGpio);
}

void GPIO_EnableKeypadBuzzer(void)
{
	return s_stGpio.EnableKeypadBuzzer(&s_stGpio);
}

void GPIO_DisableKeypadBuzzer(void)
{
	s_stGpio.DisableKeypadBuzzer(&s_stGpio);
}

void GPIO_AliveBlink()
{
	s_stGpio.AliveBlink(&s_stGpio);
}

void GPIO_HddBlink()
{
	s_stGpio.HddBlink(&s_stGpio);
}

void GPIO_SetIrdaId(unsigned char u8Id)
{
	s_stGpio.SetIrdaId(&s_stGpio, u8Id);
}

void GPIO_SetIrdaBuzzer(int SetOn)
{
	s_stGpio.SetIrdaBuzzer(&s_stGpio, SetOn);
}

void GPIO_SetPWROff(unsigned char u8Id)
{
	KEYPADDRV_SetPWROff(u8Id);
}

int GPIO_Init()
{
	Gpio* thiz = NULL;
	if((thiz = PGPIO_Struct())){
		memcpy(&s_stGpio, thiz, sizeof(Gpio));
		free(thiz);
		thiz = NULL;
	}
	GPIO_EnableRS485();
	KEYPADDRV_SetInit();
	GPIO_SetIrdaBuzzer(!0);
	return 0;
}

void GPIO_Destroy(void)
{
	GPIO_DisableRS485();
}

//#define TEST_GPIO
#ifdef TEST_GPIO
int main(int argc, char** argv)
{
	Gpio* gpio = PGPIO_Struct();
	if(gpio){
		gpio->SetBuzzer(gpio, 1);
		sleep(1);
		gpio->SetBuzzer(gpio, 0);
		PGPIO_Destruct(&gpio);
	}
	return 0;
}
#endif


