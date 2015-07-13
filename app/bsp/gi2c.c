#include "drv_headers.h"

#include "gi2c.h"

#if   defined(SDK_PLATFORM_HI3515)
#define GRP                 (3)
#define SCL                 (1 << 2)    /* GPIO 3_2 */
#define SDA                 (1 << 3)    /* GPIO 3_3 */
#elif defined(SDK_PLATFORM_HI3520)
#define GRP                 (3)
#define SCL                 (1 << 5)    /* GPIO 3_5 */
#define SDA                 (1 << 4)    /* GPIO 3_4 */
#elif defined(SDK_PLATFORM_HI3521)
#if   defined(_HI3515A_EXT) || defined(_HI3520D_EXT) //FIXME:Fake
#define GRP                 (1)
#define SCL                 (1 << 4)    /* GPIO 1_4 */
#define SDA                 (1 << 4)    /* GPIO 1_4 */
#else
#define GRP                 (7)
#define SCL                 (1 << 0)    /* GPIO 7_0 */
#define SDA                 (1 << 1)    /* GPIO 7_1 */
#endif
#elif defined(SDK_PLATFORM_HI3531)
#define GRP                 (16)
#define SCL                 (1 << 5)    /* GPIO 16_5 */
#define SDA                 (1 << 6)    /* GPIO 16_6 */
#else
#error "Unknown Platform!!!"
#endif

#define REG_GPIO_BS         (0x20150000 + 0x10000 * (GRP))

#define GPIO_DIR            IO_ADDRESS(REG_GPIO_BS + 0x400)

#define GPIO_I2C_SCL_REG    IO_ADDRESS(REG_GPIO_BS + (SCL<<2))
#define GPIO_I2C_SDA_REG    IO_ADDRESS(REG_GPIO_BS + (SDA<<2))
#define GPIO_I2C_SCLSDA_REG IO_ADDRESS(REG_GPIO_BS + ((SCL<<2)|(SDA<<2)))

#define HW_REG(reg)         *((volatile unsigned int *)(reg))
#define DELAY(us)           udelay(us)

static void i2c_in(unsigned char pins)
{
	unsigned char regvalue;

	regvalue = HW_REG(GPIO_DIR);
	regvalue &= ~pins;
	HW_REG(GPIO_DIR) = regvalue;
}

static void i2c_out(unsigned char pins)
{
	unsigned char regvalue;

	regvalue = HW_REG(GPIO_DIR);
	regvalue |= pins;
	HW_REG(GPIO_DIR) = regvalue;
}

static void i2c_clr(unsigned char pins)
{
	i2c_out(pins);

	if(pins == SCL) {
		HW_REG(GPIO_I2C_SCL_REG) = 0;
		return;
	}
	else if(pins == SDA) {
		HW_REG(GPIO_I2C_SDA_REG) = 0;
		return;
	}
	else if(pins == (SDA|SCL)) {
		HW_REG(GPIO_I2C_SCLSDA_REG) = 0;
		return;
	}
	else {
		printk("Error input.\n");
		return;
	}
}

static void  i2c_set(unsigned char pins)
{
	i2c_out(pins);

	if(pins == SCL) {
		HW_REG(GPIO_I2C_SCL_REG) = SCL;
		return;
	}
	else if(pins == SDA) {
		HW_REG(GPIO_I2C_SDA_REG) = SDA;
		return;
	}
	else if(pins == (SDA|SCL)) {
		HW_REG(GPIO_I2C_SCLSDA_REG) = (SDA|SCL);
		return;
	}
	else {
		printk("Error input.\n");
		return;
	}
}

static unsigned char i2c_data_read(void)
{
	if((HW_REG(GPIO_I2C_SDA_REG) & SDA) != 0)
		return 1;
	else
		return 0;
}

static void i2c_init_clock(void)
{
	int i;

    i2c_set(SDA);
	DELAY(2);

	for(i = 0; i < 6; i ++) {
		i2c_clr(SCL);
		DELAY(2);

		i2c_set(SCL);
		DELAY(2);
	}
}

static void i2c_start_bit(void)
{
	i2c_set(SDA);
	DELAY(2);
	i2c_set(SCL);
	DELAY(2);

	i2c_clr(SDA);
	DELAY(2);
	i2c_clr(SCL);
	DELAY(2);
}

static void i2c_stop_bit(void)
{
	i2c_out(SDA);
	i2c_clr(SCL);
    DELAY(2);
	i2c_clr(SDA);
    DELAY(2);

    i2c_set(SCL);
    DELAY(2);
	i2c_set(SDA);
	DELAY(2);

	i2c_clr(SCL);
	DELAY(2);
}

/*******************************************************************
                 字节数据传送函数               
函数原型: void i2c_send_byte(unsigned char c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
static void i2c_send_byte(unsigned char c)
{
    unsigned char i;
    local_irq_disable();

	i2c_out(SDA);
    for (i=0; i<8; i++) {
        if (c & 0x80)
            i2c_set(SDA);
        else
            i2c_clr(SDA);
		c <<= 1;
		DELAY(2);

		i2c_set(SCL);
		DELAY(2);

        i2c_clr(SCL);
        DELAY(2);
    }

	DELAY(2);
    local_irq_enable();
}

/*******************************************************************
                 字节数据传送函数               
函数原型: unsigned char i2c_receive_byte(void);
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     发完后请用应答函数。  
********************************************************************/
static unsigned char i2c_receive_byte(void)
{
    unsigned char retc = 0;
    unsigned char i;

    local_irq_disable();

	i2c_in(SDA);
    for (i=0; i<8; i++) {
        i2c_clr(SCL);
        DELAY(2);
        i2c_set(SCL);
        DELAY(2);

		retc <<= 1;
        if (i2c_data_read())
            retc += 1;
		DELAY(2);
    }

	i2c_clr(SCL);
    DELAY(2);

	local_irq_enable();

    return retc;
}

/********************************************************************
                     应答子函数
原型:  void i2c_send_ack(unsigned char a);
 
功能:主控器进行应答信号,(可以是应答或非应答信号)
********************************************************************/
static void i2c_send_ack(unsigned char a)
{
	i2c_out(SDA);

	if(a & 0x01)
		i2c_set(SDA);
	else
		i2c_clr(SDA);
	DELAY(2);

    i2c_set(SCL);
    DELAY(2);
    i2c_clr(SCL);
    DELAY(1);
}

static unsigned char i2c_wait_ack(void)
{
	unsigned char ack;

	i2c_in(SDA);
	DELAY(2);

    i2c_clr(SCL);
	DELAY(2);
    i2c_set(SCL);
    DELAY(2);

	if(i2c_data_read()) //If Signal is HIGH
		ack = NACK;     //then NACK,
	else
		ack = ACK;      //else ACK

	i2c_clr(SCL);
	DELAY(2);

	return ack;
}

static DEFINE_IOCTL(gpioi2c_ioctl, file, cmd, arg)
{
	int retc = 0;

	switch(cmd) {
    case GPIO_I2C_INIT:
		i2c_init_clock();
		break;
	case GPIO_I2C_START:
		i2c_start_bit();
		break;
	case GPIO_I2C_STOP:
		i2c_stop_bit();
		break;
	case GPIO_I2C_SEND_BYTE:
		i2c_send_byte(arg);
		break;
	case GPIO_I2C_RECV_BYTE:
		retc = i2c_receive_byte();
		break;
	case GPIO_I2C_SEND_ACK:
		i2c_send_ack(arg);
		break;
	case GPIO_I2C_WAIT_ACK:
		retc = i2c_wait_ack();
		break;

	default:
		return -1;
	}

    return retc;
}

int gpioi2c_open(struct inode * inode, struct file * file)
{
    i2c_set(SCL | SDA);
    return 0;
}
int gpioi2c_close(struct inode * inode, struct file * file)
{
    i2c_set(SCL | SDA);
    return 0;
}

static struct file_operations gpioi2c_fops = {
    .owner      = THIS_MODULE,
    .ioctl      = gpioi2c_ioctl,
    .open       = gpioi2c_open,
    .release    = gpioi2c_close
};

static struct miscdevice gpioi2c_dev = {
   .minor	= MISC_DYNAMIC_MINOR,
   .name	= "at88sc",
   .fops  = &gpioi2c_fops,
};

static int __init gpio_i2c_init(void)
{
    int ret;
    ret = misc_register(&gpioi2c_dev);
    if(0 != ret)
    	return -1;

    i2c_set(SCL | SDA);

    return 0;    
}

static void __exit gpio_i2c_exit(void)
{
    misc_deregister(&gpioi2c_dev);
}

module_init(gpio_i2c_init);
module_exit(gpio_i2c_exit);

MODULE_LICENSE("GPL");
