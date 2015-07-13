typedef enum {
    PANELC_ENB = 0,  //Panel KeyPad Enable Pin
    PANELC_INT,      //Panel KeyPad IRQ Pin
    PANELC_NUM
} PANELC_PIN;

#include "gpio_drv_keypad_panel_setting.c"
#include "gpio_i2c_drv.h"

#define PANEL_I2C_GRP   0
#define PANEL_I2C_ADDR  0x34
#define PANEL_I2C_SIZE  8

#define	ISP_CMD_UPDATE	(0x80 | (1 << 0))	//系统升级，这里使用两位，避免和IAP中的重复
#define	ISP_CMD_IAP		(0x80 | (1 << 1))	//切换到IAP
#define	ISP_CMD_DATA	(0x80 | (1 << 2))	//发送数据
#define ISP_KEY_READY   (0x14)              //可以进行下一个命令交互

#define	IAP_CMD_H8_EXTEND   (1 << 0)	//扩展命令
#define	IAP_CMD_H8_RESERVE  (1 << 1)	//保留位
#define	IAP_CMD_H8_RESET    (1 << 2)	//系统重启(软关机)
#define	IAP_CMD_H8_LED8     (1 << 3)	//控制CH8-16的灯
#define	IAP_CMD_H8_LED0     (1 << 4)	//控制CH0-7的灯
#define	IAP_CMD_H8_MORELED  (1 << 5)	//控制其它的灯
#define	IAP_CMD_H8_KEY      (1 << 6)	//设置连发键
#define	IAP_CMD_H8_DEV      (1 << 7)	//设置设备号

#define	IAP_CMD_L8_VERSION  (1 << 0)	//获得面板版本号，固定为3位数，比如1.21
#define	IAP_CMD_L8_START    (1 << 1)	//设置开机计划
#define	IAP_CMD_L8_SYSTIME  (1 << 2)	//设置系统时间
#define	IAP_CMD_L8_FAIL     (1 << 3)	//加密芯片验证失败通知（目前没有用）
#define	IAP_CMD_L8_CIPHER   (1 << 4)	//加密
#define	IAP_CMD_L8_STATUS   (1 << 5)	//获得系统状态(如ACC状态等)
#define	IAP_CMD_L8_HDD      (1 << 6)	//硬盘出错关机重启
#define	IAP_CMD_L8_UPDATE   (1 << 7)	//面板升级

void keypad_toggle_i2c(void)
{
    gpio_set_pin(PanelC_Pins[PANELC_ENB].grp,
        PanelC_Pins[PANELC_ENB].pin,
        GPIO_PIN_LOW);
}

void keypad_disable_i2c(void)
{
    gpio_set_pin(PanelC_Pins[PANELC_ENB].grp,
        PanelC_Pins[PANELC_ENB].pin,
        GPIO_PIN_HIGH);
}

void (*Panel_Func_Fix[2])(void) = {
    &keypad_toggle_i2c,
    &keypad_disable_i2c
};

void ClrKeySts(void)
{
	keypad.sts = KEY_PRESS;
	keypad.key = 0xFF;
}

void SetKeyMap(struct input_dev* dev)
{
    int ii;

    keypad.dev->keycodemax = 5*5;
    for(ii = 0; ii < keypad.dev->keycodemax; ++ ii) {
        set_bit(ii, keypad.dev->keybit);
    }
    clear_bit(0, dev->keybit);
}

void keypad_config(void)
{
    int i;
    for(i = 0; i < ARRAY_SIZE(PanelC_Pins); i ++) {
        // initialize GPIO
        gpio_pin_init(PanelC_Pins[i]);
        // disable interrupt
        reg_clear32(GPIO_IE(PanelC_Pins[i].grp), (1<<PanelC_Pins[i].pin));
    }

    reg_clear32(GPIO_IS (PanelC_Pins[PANELC_INT].grp),
        (1 << PanelC_Pins[PANELC_INT].pin));
    reg_clear32(GPIO_IBE(PanelC_Pins[PANELC_INT].grp),
        (1 << PanelC_Pins[PANELC_INT].pin));
    reg_clear32(GPIO_IEV(PanelC_Pins[PANELC_INT].grp),
        (1 << PanelC_Pins[PANELC_INT].pin));
}

void keypad_enable_irq(void)
{
    //Reset GPIO Level Status
    gpio_pin_init(PanelC_Pins[PANELC_INT]);

    // enable interrupt
    reg_set32(GPIO_IC (PanelC_Pins[PANELC_INT].grp),
        (1<<PanelC_Pins[PANELC_INT].pin));
    reg_set32(GPIO_IE (PanelC_Pins[PANELC_INT].grp),
        (1<<PanelC_Pins[PANELC_INT].pin));
}

void keypad_disable_irq(void)
{
    // disable interrupt
    reg_set32  (GPIO_IC (PanelC_Pins[PANELC_INT].grp),
        (1<<PanelC_Pins[PANELC_INT].pin));
    reg_clear32(GPIO_IE (PanelC_Pins[PANELC_INT].grp),
        (1<<PanelC_Pins[PANELC_INT].pin));
}

int  keycode_panel_filter(int  keyraw)
{
   int ii;
   int outfilter[] = {0x0030,0x0031,0x0032,0x0033,0x0034, //   0,   1,   2,   3,   4,
                      0x0035,0x0036,0x0037,0x0038,0x0039, //   5,   6,   7,   8,   9,
                      0x0040,0x003E,0x003D,0x0047,0x003A, //QUAD, REC,PLAY, ESC,MENU,
                      0x0042,0x0043,0x0044,0x0045,0x0046, //  UP,DOWN,LEFT,RIGHT,ENTER,
                      0x003F,0x003B,0x003C,0x0051,       // PTZ,  F1,  F2,POWER
                      0x004E,0x004C,0x005B}; //PREV, NEXT, STOP
   for(ii = 0; ii < ARRAY_SIZE(outfilter); ii ++) {
       if(keyraw == outfilter[ii]) {
           return ++ii;
       }
   }

   return 0xFF;
}

static void keypad_scan_timer(unsigned long data)
{
    static int keyret = 0xFF;

	switch(keypad.sts) {
	case KEY_PRESS:
		keypad.sts = KEY_RELEASE;
		keypad.key = gpio_i2c_readX_KeyPad_Fix(
				PANEL_I2C_GRP,
				PANEL_I2C_ADDR,
				Panel_Func_Fix);
		keyret = keycode_panel_filter(keypad.key);
//		KEYPAD_TRACE("%s --> 0x%08X\n", __FUNCTION__, keyret);
		if(keyret != 0xFF) {
			input_report_key(keypad.dev, keyret, 1);
			input_sync(keypad.dev);

			gpio_keypad_buzzer();
		}
		keypad_proc_timer_start(1);
		break;
	case KEY_RELEASE:
	case KEY_NULL:
		keypad.sts = KEY_NULL;
		keypad_proc_timer_stop();
		if(keyret != 0xFF) {
			input_report_key(keypad.dev, keyret, 0);
			input_sync(keypad.dev);
		}
		keypad_enable_irq();
		break;
	}
}

DEFINE_IOCTL(keypad_ioctl, file, cmd, arg)
{
	unsigned char __user *argp = (unsigned char __user *)arg;
	int ret = 0;

	switch(cmd) {
	case KEYPAD_INIT: {
		char tmpCmd[] = {ISP_CMD_IAP, 0}; //Command To Call ISP_IAP_CALL
		ret = gpio_i2c_writeX_KeyPad_Fix(PANEL_I2C_GRP, PANEL_I2C_ADDR,
			tmpCmd, sizeof(tmpCmd),
			NULL, 0,
			Panel_Func_Fix);
	}
		break;

	case KEYPAD_DEVID: {
		char tmpCmd[] = {IAP_CMD_H8_DEV, *argp};
		if(!argp) {
			ret = -1;
			break;
		}
		ret = gpio_i2c_writeX_KeyPad_Fix(PANEL_I2C_GRP, PANEL_I2C_ADDR,
			tmpCmd, sizeof(tmpCmd),
			NULL, 0,
			Panel_Func_Fix);
	}
		break;

	case KEYPAD_PWROFF: {
        char tmpCmd[] = {IAP_CMD_H8_RESET, *argp};
		if(!argp) {
			ret = -1;
			break;
		}
		ret = gpio_i2c_writeX_KeyPad_Fix(PANEL_I2C_GRP, PANEL_I2C_ADDR,
			tmpCmd, sizeof(tmpCmd),
			NULL, 0,
			Panel_Func_Fix);
	}
		break;

	case KEYPAD_RESET: {
	}
		break;

	default:
		ret = -1;
	}

	return ret;
}
