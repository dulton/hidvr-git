#ifdef __KERNEL__

#include "drv_headers.h"

#include "hi3515_drv.h"
#include "gpio_drv.h"

#include "gpio_drv_keypad.h"

typedef struct Keypad {
	struct input_dev* dev;
	unsigned int      key;
	KEY_STATUS        sts;
}Keypad;

static Keypad keypad;

// irq -> timer -> check shakable
static struct timer_list key_timer;
static void keypad_proc_timer_start(unsigned long time_after)
{
	mod_timer(&key_timer, jiffies + time_after);
}

static void keypad_proc_timer_stop(void)
{
	del_timer(&key_timer);
}

#define KEYPAD_TRACE printk

#if   defined(GPIO_KEYPAD_MATRIX)
#include "gpio_drv_keypad_matrix.c"
#elif defined(GPIO_KEYPAD_PANEL)
#include "gpio_drv_keypad_panel.c"
#else
#error "GPIO_PLAT Not Defined!!"
#endif

int keypad_get_irq_number(int** ret_irqnum)
{
	if(ret_irqnum){
		*ret_irqnum = (int*)s_irq_number;
	}
	return ARRAY_SIZE(s_irq_number);
}

static irqreturn_t keypad_press_irq(int irq, void *dev)
{
	keypad_disable_irq(); // disable interrupt first
    ClrKeySts();
	keypad_proc_timer_start(1);
	return IRQ_HANDLED;
}

void keypad_init_irq(void)
{
	int i = 0;
	int* irq_number = NULL;
	int irq_cnt = 0;
    int irq_ret = 0;

	irq_cnt = keypad_get_irq_number(&irq_number);

	for(i = 0; i < irq_cnt; ++i) {
        if(irq_number[i] < 0) continue;
		irq_ret = request_irq(irq_number[i], &keypad_press_irq,
                            IRQF_SHARED | IRQF_DISABLED, KEYPAD_DEV_NAME,
                            &keypad_press_irq);
        if(0 != irq_ret) {
			KEYPAD_TRACE("reqest irq(%d) failed\r\n", irq_number[i]);
		}
	}
}

void keypad_free_irq(void)
{
	int i = 0;
	int* irq_number = NULL;
	int irq_cnt = 0;

	keypad_disable_irq();
	irq_cnt = keypad_get_irq_number(&irq_number);

	for(i = 0; i < irq_cnt; ++i){
        if(irq_number[i] < 0) continue;
		free_irq(irq_number[i], &keypad_press_irq);
	}
}

static int s_keypad_init = 0;
int keypad_init(void)
{
	int ret = 0;

	keypad_disable_irq();
	keypad.dev = input_allocate_device();
	if(!keypad.dev){
		return -ENOMEM;
	}

	keypad.dev->evbit[0] = BIT_MASK(EV_KEY);
	keypad.dev->name = KEYPAD_DEV_NAME;
	keypad.dev->id.bustype = BUS_HOST;
	keypad.dev->id.vendor  = 0x0001;
	keypad.dev->id.product = 0x0001;
	keypad.dev->id.version = 0x0100;
	keypad.dev->keycode    = &keypad.key;
	keypad.dev->keycodesize = sizeof(unsigned short);

    SetKeyMap(keypad.dev);

	if(0 != (ret = input_register_device(keypad.dev))){
		pr_err("input_register_device error: %d\n", ret);
		goto keypad_init_exit1;
	}

	setup_timer(&key_timer,keypad_scan_timer, 3);

	keypad_config();
	keypad_init_irq();
	keypad_disable_irq(); // disable interrupt first

	ClrKeySts();

	keypad_proc_timer_start(1);

	KEYPAD_TRACE("Keypad for HI3515 @ %s %s\r\n", __TIME__, __DATE__);
	s_keypad_init = 1;
	return 0;

keypad_init_exit1:
	input_free_device(keypad.dev);
	KEYPAD_TRACE("Keypad init failed");
	return -1;
}

void keypad_exit(void)
{
	if(s_keypad_init){
		keypad_free_irq();
		keypad_proc_timer_stop();
		input_unregister_device(keypad.dev);
		input_free_device(keypad.dev);
		del_timer(&key_timer);
		s_keypad_init = 0;
	}
}

#ifdef KEYPAD_ISOLATE_DRIVER
module_init(keypad_init);
module_exit(keypad_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Law @ Guangzhou JUAN");
MODULE_DESCRIPTION("Keypad Driver for DVR based on HI3515");
#endif //KEYPAD_ISOLATE_DRIVER

#endif // __KERNEL__


