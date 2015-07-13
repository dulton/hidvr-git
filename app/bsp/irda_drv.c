#include "drv_headers.h"

#include "hi3515_drv.h"
#include "gpio_drv.h"
#include "irda_drv_map.h"

#define DEBUG_IRDA
#ifdef DEBUG_IRDA
#define IRDA_TRACE(fmt...) \
	do{printk("\033[1;31mIRDA->[%s]:%d ", __FUNCTION__, __LINE__);printk(fmt);printk("\033[m\r\n");}while(0)
#else
#define IRDA_TRACE(fmt...)
#endif

#define IRDA_DEV_NAME "irda"
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define IRDA_IRQ_NUM (9)
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531)
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT)
#define IRDA_IRQ_NUM (46)
#else
#define IRDA_IRQ_NUM (48)
#endif
#endif

#define IR_BASE (0x20070000)
#define IR_EN (IR_BASE+0x0000)
#define IR_CFG (IR_BASE+0x0004)
#define IR_LEADS (IR_BASE+0x0008)
#define IR_LEADE (IR_BASE+0x000c)
#define IR_SLEADE (IR_BASE+0x0010)
#define IR_B0 (IR_BASE+0x0014)
#define IR_B1 (IR_BASE+0x0018)
#define IR_BUSY (IR_BASE+0x001c)
#define IR_DATAH (IR_BASE+0x0020)
#define IR_DATAL (IR_BASE+0x0024)
#define IR_INT_MASK (IR_BASE+0x0028)
	#define INTM_RELEASE (1<<3)
	#define INTM_OVERFLOW (1<<2)
	#define INTM_FRAMERR (1<<1)
	#define INTM_RCV (1<<0)
#define IR_INT_STATUS (IR_BASE+0x002c)
	#define INTMS_RELEASE (1<<19)
	#define INTMS_OVERFLOW (1<<18)
	#define INTMS_FRAMERR (1<<17)
	#define INTMS_RCV (1<<16)
	#define INTRS_RELEASE (1<<3)
	#define INTRS_OVERFLOW (1<<2)
	#define INTRS_FRAMERR (1<<1)
	#define INTRS_RCV (1<<0)
#define IR_INT_CLR (IR_BASE+0x0030)
	#define INTC_RELEASE (1<<3)
	#define INTC_OVERFLOW (1<<2)
	#define INTC_FRAMERR (1<<1)
	#define INTC_RCV (1<<0)
#define IR_START (IR_BASE+0x0034)

#define SC_BASE (0x20050000)
#define SC_PEREN (SC_BASE+0x0024)
#define SC_PERDIS (SC_BASE+0x0028)
#define SC_PERCTRL8 (SC_BASE+0x0025)

typedef struct Irda
{
	struct input_dev* device;
	unsigned int key;
	struct timer_list buzzer;
}Irda;
static Irda irda;

static void irda_buzzer_timer(unsigned long data)
{
	gpio_irda_buzzer();
}

static void irda_buzzer(void)
{
	gpio_irda_buzzer();
	mod_timer(&irda.buzzer, jiffies + 3);
}

static int irda_config(void)
{
	struct hiir_dev_param
	{
		unsigned short int leads_min;
		unsigned short int leads_max;

		unsigned short int leade_min;
		unsigned short int leade_max;

		unsigned short int cnt0_b_min;
		unsigned short int cnt0_b_max;

		unsigned short int cnt1_b_min;
		unsigned short int cnt1_b_max;

		unsigned short int sleade_min;
		unsigned short int sleade_max;

		unsigned short int code_len;
		unsigned short int codetype;
		unsigned short int frequence;
	};
	unsigned value = 0;
	struct hiir_dev_param parm =
		{828, 972, 414, 486, 45, 67 , 135, 203, 180, 270, 32, 0, 24};

	reg_write32(IR_EN, 1);
	if(reg_read32(IR_BUSY)){
		IRDA_TRACE("irda busy!");
		return -1;
	}
	// NEC simple
	//reg_clear32(IR_CFG, (3<<24));

	value = (parm.codetype << 14);
	value |= (parm.code_len - 1) << 8;
	value |= (parm.frequence - 1);
	reg_write32(IR_CFG, value);

	value = parm.leads_min << 16;
	value |= parm.leads_max;
	reg_write32(IR_LEADS, value);

	value = parm.leade_min << 16;
	value |= parm.leade_max;
	reg_write32(IR_LEADE, value);

	value = parm.sleade_min << 16;
	value |= parm.sleade_max;
	reg_write32(IR_SLEADE, value);

	value = parm.cnt0_b_min << 16;
	value |= parm.cnt0_b_max;
	reg_write32(IR_B0, value);

	value = parm.cnt1_b_min << 16;
	value |= parm.cnt1_b_max;
	reg_write32(IR_B1, value);
	return 0;
}

static void irda_start(void)
{
	reg_write32(IR_START, 0);
}

static void irda_enable_irq(unsigned int flag)
{
	reg_clear32(IR_INT_MASK, flag);
}

static void irda_disable_irq(unsigned int flag)
{
	reg_set32(IR_INT_MASK, flag);
}

static unsigned short irda_key_mapped(unsigned int key)
{
	int ii = 0;
	for(ii = 0; ii < ARRAY_SIZE(IRDA_CODE_RAW_TBL); ii ++) {//FIXME: Need Hash to Simplify
		if(IRDA_CODE_RAW_TBL[ii] == (key & IRDA_CODE_MASK)) {
			return (ii + IRDA_KEY_CODE_START);
		}
	}
	return 0;
}


static irqreturn_t irda_rcv_irq(int irq, void *dev_id)
{
	unsigned short key = 0;
	//irda_disable_irq();
	//IRDA_TRACE("interrupt!");
	if(reg_test32(IR_INT_STATUS, INTMS_FRAMERR)){
		IRDA_TRACE("frame error!");
	}else if(reg_test32(IR_INT_STATUS, INTMS_OVERFLOW)){
		IRDA_TRACE("frame overflow!");
	}else if(reg_test32(IR_INT_STATUS, INTMS_RELEASE)){
		if(irda.key != 0){
			key = irda_key_mapped(irda.key);
			//IRDA_TRACE("remote keypad %x release", irda.key);
			input_report_key(irda.device, key, 0);
			input_sync(irda.device);
			//IRDA_TRACE("remote report %x release", key);
			irda.key = 0;
		}
	}else if(reg_test32(IR_INT_STATUS, INTMS_RCV)){
		unsigned int ir_data = 0;
		unsigned int ir_data_h = 0;
		unsigned int ir_data_l = 0;
		ir_data_h = ir_data_l = reg_read32(IR_DATAL);
		ir_data = 	((ir_data_h & 0xffff) << 16) | ((ir_data_l & (0xffff<<16)) >> 16);
		if(irda.key != ir_data && irda.key == 0){
			irda.key = ir_data;
			//IRDA_TRACE("remote keypad %x press", irda.key);
			key = irda_key_mapped(irda.key);
		
			input_report_key(irda.device, key, 1);
			input_sync(irda.device);
			//IRDA_TRACE("remote report %x press", key);
			irda_buzzer();
		}
	}
	//IRDA_TRACE("IR_INT_STATUS = %08x", reg_read32(IR_INT_STATUS));
	reg_set32(IR_INT_CLR, INTC_RELEASE|INTC_OVERFLOW|INTC_FRAMERR|INTC_RCV);
	return IRQ_HANDLED;
}

static void irda_init_irq(void)
{
	if(0 != request_irq(IRDA_IRQ_NUM, irda_rcv_irq, IRQF_SHARED | IRQF_DISABLED, IRDA_DEV_NAME, &irda_rcv_irq)){
		IRDA_TRACE("reqest irq(%d) failed", IRDA_IRQ_NUM);
	}
	irda_disable_irq(INTM_RELEASE|INTM_FRAMERR|INTM_FRAMERR|INTM_RCV);
	return irda_enable_irq(INTM_RELEASE|INTM_RCV);
}

static void irda_free_irq(void)
{
	irda_disable_irq(INTM_RELEASE|INTM_RCV);
	free_irq(IRDA_IRQ_NUM, &irda_rcv_irq);
}

static void irda_enable_clock(void)
{
	reg_set32(SC_PEREN, (1<<11));
	reg_clear32(SC_PERDIS, (1<<11));
}

static void irda_disable_clock(void)
{
	reg_clear32(SC_PEREN, (1<<11));
	reg_set32(SC_PERDIS, (1<<11));
}

int irda_init(void)
{
#include "irda_drv_map.h"
	int i = 0, ret = 0;
	irda.device = input_allocate_device();
	if(!irda.device){
		return -ENOMEM;
	}
	irda.device->evbit[0] = BIT_MASK(EV_KEY);
	irda.device->name = IRDA_DEV_NAME;
	irda.device->id.bustype = BUS_HOST;
	irda.device->id.vendor =  0x0001;
	irda.device->id.product = 0x0001;
	irda.device->id.version = 0x0100;
	irda.device->keycode    = &irda.key;
	irda.device->keycodesize = sizeof(IRDA_CODE_RAW_TBL[0]);
	for(i = 0; i < ARRAY_SIZE(IRDA_CODE_RAW_TBL); ++i){
		set_bit(IRDA_KEY_CODE_START+i, irda.device->keybit);
	}
	clear_bit(0, irda.device->keybit);
	// register input device
	if(0 != (ret = input_register_device(irda.device))){
		pr_err("input_register_device error: %d\n", ret);
		goto irda_init_exit1;
	}
	//irda_reset_clock();
	irda_enable_clock();
	irda_config();
	irda_init_irq();
	irda_start();
	setup_timer(&irda.buzzer, irda_buzzer_timer, 0);
	printk("Irda for HI3515 @ %s %s\r\n", __TIME__, __DATE__);
	gpio_irda_buzzer();
/* END:   Added by Law, 2011/6/22   PN:irda id function added */
	return 0;
irda_init_exit1:
	input_free_device(irda.device);
	IRDA_TRACE("Irda init failed");
	return -1;
}

void irda_exit(void)
{
	irda_disable_clock();
	irda_free_irq();
	input_unregister_device(irda.device);
	input_free_device(irda.device);
	del_timer(&irda.buzzer);
}

module_init(irda_init);
module_exit(irda_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Law @ Guangzhou JUAN");
MODULE_DESCRIPTION("Irda Driver for DVR based on HI3515");


