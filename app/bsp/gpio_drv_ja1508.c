
static gpio_dev_t s_gpio_devs[] =
{
    {{ 1, 2, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_BUZZR},  //BUZ OUT
    {{ 5, 0, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_ALARM},  //ALM OUT
    {{ 5, 2, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_RS485},  //485 RTX
//    {{ 8, 8, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_CDRST},  //DEV RST

    {{ 2, 1, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //RUN LED
    {{ 2, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //REC LED
//    {{ 8, 8, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //ALM LED

    {{ 6, 3,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN1
    {{ 6, 2,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN2
    {{ 6, 1,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN3
    {{ 6, 0,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN4
};

#define PERI_CRG_BAS    (0x20030000)
#define PERI_CRG_REG(x) (PERI_CRG_BAS + ((x)*4))

int gpio_AckoutFreqSel(unsigned long Freq)
{
	unsigned long tmpRead = 0;
	unsigned long tmpClkV = -1;

    switch(Freq) {
	case 54:
		tmpClkV = 0x0;
		break;
	case 27:
		tmpClkV = 0x1;
		break;
	case 24:
		tmpClkV = 0x2;
		break;
	default:
		tmpClkV = -1;
		break;
	}

	if(-1 != tmpClkV) {
        tmpRead  = reg_read32(PERI_CRG_REG(11));
		tmpRead &= 0xFFF3FFFF;
		tmpRead |= (tmpClkV << 18);
		reg_write32(PERI_CRG_REG(11), tmpRead);
		reg_write32(IO_CONFIG_REG(0), 1);  //VI_ADC_CLK
		return 0;
    }

    return -1;
}

static int          gpio_Platform_rawbufflg = 0;
static unsigned int gpio_Platform_rawbufval = 0;

int gpio_Platform_VerProbe(void)
{
	int ii;

	if(!gpio_Platform_rawbufflg) {
		gpio_Platform_rawbufflg = !gpio_Platform_rawbufflg;

		gpio_conf_t ProbeIO[] = {
			{1, 3,  GPIO_DIR_IN,  GPIO_PIN_HIGH},
			{0, 7,  GPIO_DIR_IN,  GPIO_PIN_HIGH},
		};
		unsigned int tmpValue = 0;

		for(ii = 0; ii < ARRAY_SIZE(ProbeIO); ii ++) {
			gpio_pin_init(ProbeIO[ii]);
		}

		udelay(1000); //Delay to Wait Input Signal Level Stable;

		for(ii = 0; ii < ARRAY_SIZE(ProbeIO); ii ++) {
			tmpValue <<= 1;
			if(gpio_get_pin(ProbeIO[ii].grp, ProbeIO[ii].pin) == GPIO_PIN_HIGH) {
				tmpValue  += 1;
			}
		}
		gpio_Platform_rawbufval = tmpValue;
		printk("\nGPIO_PROBE_VALUE : 0x%08X\n", gpio_Platform_rawbufval);
	}

	switch(gpio_Platform_rawbufval) {
	case 0x01:
		return GPIO_PROBE_VER0;
	case 0x00:
		return GPIO_PROBE_VER1;
	case 0x03:
		return GPIO_PROBE_VER2;
	}

	return GPIO_PROBE_VER0;
}

void gpio_Platform_VerFix(int Ver)
{
	if(Ver == GPIO_PROBE_VER1) {
		reg_write32(IO_CONFIG_REG(1), 3);  //VIU0_MODE_LSB, GPIO_MODE, fephy_dbg_out[0~7], VIU0_MODE_MSB
		reg_write32(IO_CONFIG_REG(2), 3);  //VIU1_MODE_LSB, GPIO_MODE, fephy_dbg_out[8~15], VIU1_MODE_MSB
	}
	gpio_AckoutFreqSel(27);  //27MHz Output for All Support Platforms;
}

static void gpio_mode_conf(void)
{
	reg_write32(IO_CONFIG_REG(0), 0);  //GPIO8_5, VI_ADC_CLK, CLK_TEST_OUT0, CLK_TEST_OUT1, CLK_TEST_OUT2, CLK_TEST_OUT3, RTC_TEST_CLK
	reg_write32(IO_CONFIG_REG(1), 0);  //VIU0_MODE_LSB, GPIO_MODE, fephy_dbg_out[0~7], VIU0_MODE_MSB
	reg_write32(IO_CONFIG_REG(2), 0);  //VIU1_MODE_LSB, GPIO_MODE, fephy_dbg_out[8~15], VIU1_MODE_MSB
	reg_write32(IO_CONFIG_REG(3), 1);  //GPIO2_0, VGA_HS
	reg_write32(IO_CONFIG_REG(4), 1);  //GPIO1_5, VGA_VS
	reg_write32(IO_CONFIG_REG(5), 2);  //AIO_MCLK, GPIO7_0, BOOT_SEL
	reg_write32(IO_CONFIG_REG(6), 0);  //AIO_BLCK_TX, GPIO7_1
	reg_write32(IO_CONFIG_REG(7), 0);  //AIO_WS_TX, GPIO7_2
	reg_write32(IO_CONFIG_REG(8), 0);  //AIO_SD_TX, GPIO7_3, JTAG_SEL
	reg_write32(IO_CONFIG_REG(9), 0);  //AIO_BLCK_RX, GPIO7_4
	reg_write32(IO_CONFIG_REG(10), 0);  //AIO_WS_RX, GPIO7_5
	reg_write32(IO_CONFIG_REG(11), 0);  //AIO_SD_RX, GPIO7_6
	reg_write32(IO_CONFIG_REG(12), 0);  //GPIO8_0, SPI_SCLK
	reg_write32(IO_CONFIG_REG(13), 4);  //PLL_TEST_OUT0, SPI_SDO, PLL_TEST_OUT1, PLL_TEST_OUT2, GPIO8_1
	reg_write32(IO_CONFIG_REG(14), 0);  //GPIO8_2, SPI_SDI
	reg_write32(IO_CONFIG_REG(15), 0);  //GPIO8_3, SPI_CSN0
	reg_write32(IO_CONFIG_REG(16), 0);  //GPIO8_4, SPI_CSN1, PWM_SVB
	reg_write32(IO_CONFIG_REG(17), 0);  //GPIO1_6, I2C_SDA
	reg_write32(IO_CONFIG_REG(18), 0);  //GPIO1_7, I2C_SCL
	reg_write32(IO_CONFIG_REG(19), 0);  //GPIO5_0, UART1_RTSN
	reg_write32(IO_CONFIG_REG(20), 1);  //GPIO5_1, UART1_RXD
	reg_write32(IO_CONFIG_REG(21), 0);  //GPIO5_2, UART1_CTSN, PWM_SVB
	reg_write32(IO_CONFIG_REG(22), 1);  //GPIO5_3, UART1_TXD
	reg_write32(IO_CONFIG_REG(23), 1);  //GPIO5_4, UART2_RXD
	reg_write32(IO_CONFIG_REG(24), 1);  //GPIO5_5, UART2_TXD
	reg_write32(IO_CONFIG_REG(25), 1);  //GPIO7_7, IR_IN
	reg_write32(IO_CONFIG_REG(26), 0);  //GPIO6_0, USB0_OVRCUR
	reg_write32(IO_CONFIG_REG(27), 0);  //GPIO6_1, USB0_PWREN
	reg_write32(IO_CONFIG_REG(28), 0);  //GPIO6_2, USB1_OVRCUR
	reg_write32(IO_CONFIG_REG(29), 0);  //GPIO6_3, USB1_PWREN
	reg_write32(IO_CONFIG_REG(30), 1);  //GPIO6_4, HDMI_HOTPLUG
	reg_write32(IO_CONFIG_REG(31), 1);  //GPIO6_5, HDMI_CEC
	reg_write32(IO_CONFIG_REG(32), 1);  //GPIO6_6, HDMI_SDA
	reg_write32(IO_CONFIG_REG(33), 1);  //GPIO6_7, HDMI_SCL
	reg_write32(IO_CONFIG_REG(34), 0);  //GPIO2_1, SATA_LED_N0
	reg_write32(IO_CONFIG_REG(35), 0);  //GPIO2_2, SATA_LED_N1
	reg_write32(IO_CONFIG_REG(36), 1);  //GPIO5_6, ETH_LED1
	reg_write32(IO_CONFIG_REG(37), 1);  //GPIO5_7, ETH_LED0
	reg_write32(IO_CONFIG_REG(38), 0);  //GPIO0_0, RMII_CLK
	reg_write32(IO_CONFIG_REG(39), 0);  //GPIO0_1, RMII_TX_EN
	reg_write32(IO_CONFIG_REG(40), 0);  //GPIO0_2, RMII_TXD0
	reg_write32(IO_CONFIG_REG(41), 0);  //GPIO0_3, RMII_TXD1
	reg_write32(IO_CONFIG_REG(42), 0);  //GPIO0_4, RMII_CRS_DV
	reg_write32(IO_CONFIG_REG(43), 0);  //GPIO0_5, RMII_RXD0
	reg_write32(IO_CONFIG_REG(44), 0);  //GPIO0_6, RMII_RXD1
	reg_write32(IO_CONFIG_REG(45), 0);  //GPIO0_7, RMII_RX_ER
	reg_write32(IO_CONFIG_REG(46), 1);  //GPIO1_0, UART3_TXD
	reg_write32(IO_CONFIG_REG(47), 1);  //GPIO1_1, UART3_RXD
	reg_write32(IO_CONFIG_REG(48), 0);  //GPIO1_2, TEMPER_DQ
	reg_write32(IO_CONFIG_REG(49), 0);  //GPIO1_3, MDCK
	reg_write32(IO_CONFIG_REG(50), 0);  //GPIO1_4, MDIO
	gpio_Platform_VerFix(gpio_Platform_VerProbe());
}
