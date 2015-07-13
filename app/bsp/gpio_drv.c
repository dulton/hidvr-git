#ifdef __KERNEL__

#include "drv_headers.h"

#include "hi3515_drv.h"
#include "gpio_drv.h"

enum {
    GPIO_BUZZR = 0,
    GPIO_ALARM,
    GPIO_RS485,
    GPIO_CDRST,
    GPIO_LEDPD,
    GPIO_SENSR,
};

typedef struct {
    gpio_conf_t gpio;
    int type;
} gpio_dev_t;

#if   defined(GPIO_PLAT_TYPE1)
#include "gpio_drv_ja7204.c"
#elif defined(GPIO_PLAT_TYPE2)
#include "gpio_drv_ja7216.c"
#elif defined(GPIO_PLAT_TYPE3)
#include "gpio_drv_ja2024.c"
#elif defined(GPIO_PLAT_TYPE4)
#include "gpio_drv_ja3116.c"
#elif defined(GPIO_PLAT_TYPE5)
#include "gpio_drv_ja9204.c"
#elif defined(GPIO_PLAT_TYPE6)
#include "gpio_drv_ja2108.c"
#elif defined(GPIO_PLAT_TYPE7)
#include "gpio_drv_ja1508.c"
#elif defined(GPIO_PLAT_TYPE12)
#include "gpio_drv_ja3108hd.c"
#else
#error "GPIO_PLAT Not Defined!!"
#endif

#include "gpio_drv_keypad.h"
#include "gpio_i2c_drv.h"

// states
static unsigned char s_buzzer_stat    = GPIO_PIN_LOW;
static unsigned char s_kp_buzzer_stat = GPIO_PIN_LOW;
static unsigned char s_didi_buzzer_stat = GPIO_PIN_LOW;

// buzzer mutex
static DEFINE_MUTEX(bzr_mutex);
#define BUZZER_LOCK()   mutex_lock(&bzr_mutex)
#define BUZZER_UNLOCK() mutex_unlock(&bzr_mutex)

EXPORT_SYMBOL(gpio_Platform_VerProbe);

void _gpio_init(void)
{
	int ii = 0;

	gpio_mode_conf();

    for(ii = 0; ii < ARRAY_SIZE(s_gpio_devs); ii ++) {
        gpio_pin_init(s_gpio_devs[ii].gpio);
    }

	printk("GPIO Init Succeed!\r\n");
}

int  gpio_set_common_dev(int type, int idx, int set)
{
    int ii, jj;
    for(ii = 0, jj = 0; ii < ARRAY_SIZE(s_gpio_devs); ii ++) {
        if((type == s_gpio_devs[ii].type)) {
            if(jj == idx) {
                gpio_set_pin(s_gpio_devs[ii].gpio.grp,
                    s_gpio_devs[ii].gpio.pin,
                    set);
                return 0;
            }
            jj ++;
        }
    }

    return -1;
}

int gpio_get_common_dev(int type, int idx)
{
    int ii, jj;
    for(ii = 0, jj = 0; ii < ARRAY_SIZE(s_gpio_devs); ii ++) {
        if((type == s_gpio_devs[ii].type)) {
            if(jj == idx) {
                return gpio_get_pin(s_gpio_devs[ii].gpio.grp,
                    s_gpio_devs[ii].gpio.pin);
            }
            jj ++;
        }
    }

    return -1;
}

static void _gpio_set_buzzer(int set)
{
	gpio_set_common_dev(GPIO_BUZZR, 0, set);
}

EXPORT_SYMBOL(gpio_set_buzzer);
void gpio_set_buzzer(int set)
{
	s_buzzer_stat = set ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
	// not disable until keypad buzzer off
	if(s_kp_buzzer_stat && !set){
		return;
	}
	BUZZER_LOCK();
	_gpio_set_buzzer(set);
	BUZZER_UNLOCK();
}

EXPORT_SYMBOL(gpio_rst_device);
void gpio_rst_device(void)
{
	gpio_set_common_dev(GPIO_CDRST, 0, GPIO_PIN_HIGH);
	mdelay(10);
	gpio_set_common_dev(GPIO_CDRST, 0, GPIO_PIN_LOW);
	mdelay(100);
	gpio_set_common_dev(GPIO_CDRST, 0, GPIO_PIN_HIGH);
	mdelay(20);
}
EXPORT_SYMBOL(gpio_AckoutFreqSel);

#define DIDI_NUM 10
static struct timer_list s_didi_buzzer_timer;
static void gpio_didi_buzzer_timer(unsigned long data)
{
	int didi_array[DIDI_NUM] = {
		3 * HZ / 10,
		15 * HZ / 100,
		3 * HZ / 10,
		15 * HZ / 100,
		3 * HZ / 10,
		15 * HZ / 100,
		1 * HZ / 10,
		10 * HZ / 100,
		1 * HZ / 10,
		3 * HZ
	};
	static int didi_pos = 0;
	int didi_mode = GPIO_PIN_LOW;
	if(s_didi_buzzer_stat)
	{
		if(didi_pos % 2)
			didi_mode = GPIO_PIN_LOW;
		else
			didi_mode = GPIO_PIN_HIGH;
		gpio_set_buzzer(didi_mode);
		mod_timer(&s_didi_buzzer_timer, jiffies + didi_array[didi_pos]);
		if( ++didi_pos >= DIDI_NUM )
			didi_pos = 0;
	}
	else
	{
		didi_pos = 0;
		didi_mode = GPIO_PIN_LOW;
		gpio_set_buzzer(didi_mode);
	}
	//printk("didi_pos=%d, didi_mode=%d\n", didi_pos, didi_mode);
}

//EXPORT_SYMBOL(gpio_set_didi_buzzer);
void gpio_set_didi_buzzer(int set)
{
	if( set )
	{
		if(s_didi_buzzer_stat)
			return;
		else
		{
			s_didi_buzzer_stat = GPIO_PIN_HIGH;
			mod_timer(&s_didi_buzzer_timer, jiffies + 10);
		}
	}
	else
	{
		if(!s_didi_buzzer_stat)
			return;
		else
		{
			s_didi_buzzer_stat = GPIO_PIN_LOW;
		}
	}
}

////////////////////////////////////////////////////////////////
// keypad buzzer
////////////////////////////////////////////////////////////////
static int keypad_buzzer = 0;
static int irda_buzzer   = 0;
module_param(keypad_buzzer, bool, S_IRUGO);
module_param(irda_buzzer,   bool, S_IRUGO);

static struct timer_list s_buzzer_timer;
static void gpio_keypad_buzzer_timer(unsigned long data)
{
	s_kp_buzzer_stat = GPIO_PIN_LOW;
	_gpio_set_buzzer(GPIO_PIN_LOW);
}

EXPORT_SYMBOL(gpio_keypad_buzzer);
void gpio_keypad_buzzer(void)
{
	// keypad buzzer use and buzzer is disable
	if(keypad_buzzer && !s_buzzer_stat){
		_gpio_set_buzzer(GPIO_PIN_HIGH);
		mod_timer(&s_buzzer_timer, jiffies + 16);
		s_kp_buzzer_stat = GPIO_PIN_HIGH;
	}
}

static void gpio_set_keypad_buzzer(int set)
{
	keypad_buzzer = set ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
}

EXPORT_SYMBOL(gpio_irda_buzzer);
void gpio_irda_buzzer(void)
{
	if(irda_buzzer){ //Follow the Old Logic, Irda Buzzer Depends on Keypad's.
		gpio_keypad_buzzer();
	}
}
static void gpio_set_irda_buzzer(int set)
{
	irda_buzzer = set ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
static DEFINE_IOCTL(gpio_ioctl, file, cmd, arg)
{
	unsigned int __user *argp = (unsigned int __user *)arg;
	int set = 0;
	int get = 0;
	int ret = 0;
	
	switch(cmd) {
	case GPIO_SET_BUZZER:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_didi_buzzer(set);
		break;

	case GPIO_SET_ALARM:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_common_dev(GPIO_ALARM, 0, set);
		break;

	case GPIO_SET_RS485DE:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_common_dev(GPIO_RS485, 0, set);
		break;

	case GPIO_GET_SENSOR:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
        get = gpio_get_common_dev(GPIO_SENSR, set);
		if((ret = copy_to_user(argp, &get, sizeof(get)))){
			break;
		};
		break;

	case GPIO_RST_DEVICE:
		gpio_rst_device();
		break;

	case GPIO_SET_KEYPAD_BUZZER:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_keypad_buzzer(set);
		break;

	case GPIO_SET_IRDA_BUZZER:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_irda_buzzer(set);
		break;

	case GPIO_SET_LED1:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_common_dev(GPIO_LEDPD, 0, set);
		break;

	case GPIO_SET_LED2:
		if((ret = copy_from_user(&set, argp, sizeof(set)))){
			break;
		}
		gpio_set_common_dev(GPIO_LEDPD, 1, set);
		break;

	case GPIO_GET_PLAT_VER:
		return gpio_Platform_VerProbe();

	default:
		ret = -1;
		break;
	}
	return ret;
}

static struct file_operations gpio_fops = {
	owner	:	THIS_MODULE,
	ioctl	:	gpio_ioctl,
};

static struct miscdevice gpio_dev = {
	MISC_DYNAMIC_MINOR,
	GPIO_DEVICE_NAME,
	&gpio_fops,
};

extern DEFINE_IOCTL(keypad_ioctl, file, cmd, arg);

static struct file_operations keypad_fops = {
	.owner      = THIS_MODULE,
	.ioctl      = keypad_ioctl,
};

static struct miscdevice keypad_dev = {
	.minor  = MISC_DYNAMIC_MINOR,
	.name   = "keypad",
	.fops   = &keypad_fops,
};

static int __init gpio_init(void)
{
	int  ret=0;
	if((ret = misc_register(&gpio_dev))){
		printk(KERN_ERR"register gpio failed!\n");
		return 0;
	}
	if((ret = misc_register(&keypad_dev))){
		printk(KERN_ERR"register keypad failed!\n");
		return 0;
	}

    _gpio_init();
    keypad_init();
	setup_timer(&s_buzzer_timer, gpio_keypad_buzzer_timer, 0);
	setup_timer(&s_didi_buzzer_timer, gpio_didi_buzzer_timer, 0);
	gpio_rst_device();
	return 0;
}

static void __exit gpio_exit(void)
{
	del_timer(&s_buzzer_timer);
	del_timer(&s_didi_buzzer_timer);
	keypad_exit();
	gpio_set_buzzer(GPIO_PIN_LOW);
	misc_deregister(&keypad_dev);
	misc_deregister(&gpio_dev);
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_AUTHOR("Law @ Guangzhou JUAN");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("gpio control for HI3515");

#else
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// application part

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <errno.h>

#define GPIO_DEV	"/dev/"GPIO_DEVICE_NAME
#define KEYPAD_DEV	"/dev/"KEYPAD_DEV_NAME

// buzzer
void GPIODRV_SetBuzzer(int nNode, int nPin)
{
	if(0 != ioctl(nNode, GPIO_SET_BUZZER, &nPin)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

// alarm
void GPIODRV_SetAlarm(int nNode, int nPin)
{
	if(0 != ioctl(nNode, GPIO_SET_ALARM, &nPin)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

// sensor
int GPIODRV_GetSensor(int nNode, int nSensor)
{
	if(0 != ioctl(nNode, GPIO_GET_SENSOR, &nSensor)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
		return 0;
	}
	return nSensor;
}

void GPIODRV_SetRS485DE(int nNode, int nPin)
{
	if(0 != ioctl(nNode, GPIO_SET_RS485DE, &nPin)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

void GPIODRV_SetKeypadBuzzer(int nNode, int nOnOff)
{
	if(0 != ioctl(nNode, GPIO_SET_KEYPAD_BUZZER, &nOnOff )){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

void GPIODRV_SetIrdaBuzzer(int nNode, int nOnOff)
{
	if(0 != ioctl(nNode, GPIO_SET_IRDA_BUZZER, &nOnOff )){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}


// blink
void GPIODRV_SetLED1(int nNode, int nPin)
{
	if(0 != ioctl(nNode, GPIO_SET_LED1, &nPin)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

void GPIODRV_SetLED2(int nNode, int nPin)
{
	if(0 != ioctl(nNode, GPIO_SET_LED2, &nPin)){
		fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
	}
}

int  KEYPADDRV_SetInit(void)
{
	int  fddev = open(KEYPAD_DEV, O_RDWR);
	if(fddev > 0) {
		int ret = ioctl(fddev, KEYPAD_INIT, NULL);
		if(ret < 0) {
			fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
		}
		close(fddev);

		return ret;
	}

	return -1;
}

int  KEYPADDRV_SetDevID(unsigned char u8Id)
{
	int  fddev = open(KEYPAD_DEV, O_RDWR);
	if(fddev > 0) {
		int ret = ioctl(fddev, KEYPAD_DEVID, &u8Id);
		if(ret < 0) {
			fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
		}
		close(fddev);

		return ret;
	}

	return -1;
}

int  KEYPADDRV_SetPWROff(unsigned char BeOff)
{
	int  fddev = open(KEYPAD_DEV, O_RDWR);
	if(fddev > 0) {
		int ret = ioctl(fddev, KEYPAD_PWROFF, &BeOff);
		if(ret < 0) {
			fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
		}
		close(fddev);

		return ret;
	}

	return -1;
}

int GPIODRV_GetVerFix(void)
{
	int  fddev = open(GPIO_DEV, O_RDWR);
	if(fddev > 0) {
		int ret = ioctl(fddev, GPIO_GET_PLAT_VER, NULL);
		if(ret < 0) {
			fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
		}
		close(fddev);

		return ret;
	}

	return -1;
}

#endif // __KERNEL__

