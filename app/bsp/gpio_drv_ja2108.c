
static gpio_dev_t s_gpio_devs[] =
{
    {{ 7, 2, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_BUZZR},  //BUZ OUT
    {{ 6, 0, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_ALARM},  //ALM OUT
    {{ 9, 2, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_RS485},  //485 RTX
    {{10, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_CDRST},  //DEV RST
    {{ 8, 5, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_CDRST},  //DEV RST

    {{ 4, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //RUN LED
    {{ 5, 1, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //REC LED
    {{ 5, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},  //ALM LED

    {{14, 1,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN1
    {{14, 0,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN2
    {{ 6, 1,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN3
    {{14, 2,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},  //ALM_IN4
};

int gpio_AckoutFreqSel(unsigned long Freq)
{
    if(Freq == 27) {
        reg_write32(IO_CONFIG_REG(57), 1);  //GPIO12_7, VOU656_CLK, SDIO_CCLK_OUT
    }
    return 0;
}

int gpio_Platform_VerProbe(void)
{
    return GPIO_PROBE_VER0;
}

static void gpio_mode_conf(void)
{
    reg_write32(IO_CONFIG_REG(0), 0);  //VIU0_CLK, GPIO12_0
    reg_write32(IO_CONFIG_REG(1), 2);  //VIU0_VS, GPIO12_2, VIU0_CLKA
    reg_write32(IO_CONFIG_REG(2), 2);  //VIU0_HS, GPIO12_3, UART2_RXD
    reg_write32(IO_CONFIG_REG(3), 0);  //VIU0_DAT15, GPIO0_0
    reg_write32(IO_CONFIG_REG(4), 0);  //VIU0_DAT14, GPIO0_1
    reg_write32(IO_CONFIG_REG(5), 0);  //VIU0_DAT13, GPIO0_2
    reg_write32(IO_CONFIG_REG(6), 0);  //VIU0_DAT12, GPIO0_3
    reg_write32(IO_CONFIG_REG(7), 0);  //VIU0_DAT11, GPIO0_4
    reg_write32(IO_CONFIG_REG(8), 0);  //VIU0_DAT10, GPIO0_5
    reg_write32(IO_CONFIG_REG(9), 0);  //VIU0_DAT9, GPIO0_6
    reg_write32(IO_CONFIG_REG(10), 0);  //VIU0_DAT8, GPIO0_7
    reg_write32(IO_CONFIG_REG(11), 0);  //VIU0_DAT7, GPIO1_0
    reg_write32(IO_CONFIG_REG(12), 0);  //VIU0_DAT6, GPIO1_1
    reg_write32(IO_CONFIG_REG(13), 0);  //VIU0_DAT5, GPIO1_2
    reg_write32(IO_CONFIG_REG(14), 0);  //VIU0_DAT4, GPIO1_3
    reg_write32(IO_CONFIG_REG(15), 0);  //VIU0_DAT3, GPIO1_4
    reg_write32(IO_CONFIG_REG(16), 0);  //VIU0_DAT2, GPIO1_5
    reg_write32(IO_CONFIG_REG(17), 0);  //VIU0_DAT1, GPIO1_6
    reg_write32(IO_CONFIG_REG(18), 0);  //VIU0_DAT0, GPIO1_7
    reg_write32(IO_CONFIG_REG(19), 0);  //VIU1_CLK, GPIO12_1
    reg_write32(IO_CONFIG_REG(20), 2);  //VIU1_VS, GPIO12_4, VIU1_CLKA
    reg_write32(IO_CONFIG_REG(21), 2);  //VIU1_HS, GPIO12_5, UART2_TXD
    reg_write32(IO_CONFIG_REG(22), 0);  //VIU1_DAT15, GPIO2_0
    reg_write32(IO_CONFIG_REG(23), 0);  //VIU1_DAT14, GPIO2_1
    reg_write32(IO_CONFIG_REG(24), 0);  //VIU1_DAT13, GPIO2_2
    reg_write32(IO_CONFIG_REG(25), 0);  //VIU1_DAT12, GPIO2_3
    reg_write32(IO_CONFIG_REG(26), 0);  //VIU1_DAT11, GPIO2_4
    reg_write32(IO_CONFIG_REG(27), 0);  //VIU1_DAT10, GPIO2_5
    reg_write32(IO_CONFIG_REG(28), 0);  //VIU1_DAT9, GPIO2_6
    reg_write32(IO_CONFIG_REG(29), 0);  //VIU1_DAT8, GPIO2_7
    reg_write32(IO_CONFIG_REG(30), 0);  //VIU1_DAT7, GPIO3_0
    reg_write32(IO_CONFIG_REG(31), 0);  //VIU1_DAT6, GPIO3_1
    reg_write32(IO_CONFIG_REG(32), 0);  //VIU1_DAT5, GPIO3_2
    reg_write32(IO_CONFIG_REG(33), 0);  //VIU1_DAT4, GPIO3_3
    reg_write32(IO_CONFIG_REG(34), 0);  //VIU1_DAT3, GPIO3_4
    reg_write32(IO_CONFIG_REG(35), 0);  //VIU1_DAT2, GPIO3_5
    reg_write32(IO_CONFIG_REG(36), 0);  //VIU1_DAT1, GPIO3_6
    reg_write32(IO_CONFIG_REG(37), 0);  //VIU1_DAT0, GPIO3_7
    reg_write32(IO_CONFIG_REG(38), 0);  //GPIO12_6, VOU1120_CLK, LCD_CLK
    reg_write32(IO_CONFIG_REG(39), 0);  //GPIO4_0, VOU1120_DATA15, LCD_DATA_G0
    reg_write32(IO_CONFIG_REG(40), 0);  //GPIO4_1, VOU1120_DATA14, LCD_DATA_G1
    reg_write32(IO_CONFIG_REG(41), 0);  //GPIO4_2, VOU1120_DATA13, LCD_DATA_G2
    reg_write32(IO_CONFIG_REG(42), 0);  //GPIO4_3, VOU1120_DATA12, LCD_DATA_G3
    reg_write32(IO_CONFIG_REG(43), 0);  //GPIO4_4, VOU1120_DATA11, LCD_DATA_G4
    reg_write32(IO_CONFIG_REG(44), 0);  //GPIO4_5, VOU1120_DATA10, LCD_DATA_G5
    reg_write32(IO_CONFIG_REG(45), 0);  //GPIO4_6, VOU1120_DATA9, LCD_DATA_G6
    reg_write32(IO_CONFIG_REG(46), 0);  //GPIO4_7, VOU1120_DATA8, LCD_DATA_G7
    reg_write32(IO_CONFIG_REG(47), 0);  //GPIO5_0, VOU1120_DATA7, LCD_DATA_B0
    reg_write32(IO_CONFIG_REG(48), 0);  //GPIO5_1, VOU1120_DATA6, LCD_DATA_B1
    reg_write32(IO_CONFIG_REG(49), 0);  //GPIO5_2, VOU1120_DATA5, LCD_DATA_B2
    reg_write32(IO_CONFIG_REG(50), 0);  //GPIO5_3, VOU1120_DATA4, LCD_DATA_B3
    reg_write32(IO_CONFIG_REG(51), 0);  //GPIO5_4, VOU1120_DATA3, LCD_DATA_B4
    reg_write32(IO_CONFIG_REG(52), 0);  //GPIO5_5, VOU1120_DATA2, LCD_DATA_B5
    reg_write32(IO_CONFIG_REG(53), 0);  //GPIO5_6, VOU1120_DATA1, LCD_DATA_B6
    reg_write32(IO_CONFIG_REG(54), 0);  //GPIO5_7, VOU1120_DATA0, LCD_DATA_B7
    reg_write32(IO_CONFIG_REG(55), 2);  //GPIO13_0, VOU1120_HS, VGA_HS
    reg_write32(IO_CONFIG_REG(56), 2);  //GPIO13_1, VOU1120_VS, VGA_VS
    reg_write32(IO_CONFIG_REG(57), 0);  //GPIO12_7, VOU656_CLK, SDIO_CCLK_OUT
    reg_write32(IO_CONFIG_REG(58), 0);  //GPIO6_0, VOU656_DATA7, LCD_DATA_R0, SDIO_CARD_POWER_EN
    reg_write32(IO_CONFIG_REG(59), 0);  //GPIO6_1, VOU656_DATA6, LCD_DATA_R1, SDIO_CARD_DETECT
    reg_write32(IO_CONFIG_REG(60), 0);  //GPIO6_2, VOU656_DATA5, LCD_DATA_R2, SDIO_CARD_CWPR
    reg_write32(IO_CONFIG_REG(61), 0);  //GPIO6_3, VOU656_DATA4, LCD_DATA_R3, SDIO_CCMD
    reg_write32(IO_CONFIG_REG(62), 0);  //GPIO6_4, VOU656_DATA3, LCD_DATA_R4, SDIO_CDATA3
    reg_write32(IO_CONFIG_REG(63), 0);  //GPIO6_5, VOU656_DATA2, LCD_DATA_R5, SDIO_CDATA2
    reg_write32(IO_CONFIG_REG(64), 0);  //GPIO6_6, VOU656_DATA1, LCD_DATA_R6, SDIO_CDATA1
    reg_write32(IO_CONFIG_REG(65), 0);  //GPIO6_7, VOU656_DATA0, LCD_DATA_R7, SDIO_CDATA0
    reg_write32(IO_CONFIG_REG(66), 0);  //GPIO7_0, SIO0_RCLK
    reg_write32(IO_CONFIG_REG(67), 0);  //GPIO7_1, SIO0_RFS
    reg_write32(IO_CONFIG_REG(68), 0);  //GPIO7_2, SIO0_DIN
    reg_write32(IO_CONFIG_REG(69), 1);  //GPIO7_3, SIO1_RCLK
    reg_write32(IO_CONFIG_REG(70), 1);  //GPIO7_4, SIO1_RFS
    reg_write32(IO_CONFIG_REG(71), 1);  //GPIO7_5, SIO1_DIN
    reg_write32(IO_CONFIG_REG(72), 0);  //GPIO7_6, SIO2_XCLK
    reg_write32(IO_CONFIG_REG(73), 0);  //GPIO7_7, SIO2_XFS
    reg_write32(IO_CONFIG_REG(74), 1);  //GPIO8_0, SIO2_RCLK
    reg_write32(IO_CONFIG_REG(75), 1);  //GPIO8_1, SIO2_RFS
    reg_write32(IO_CONFIG_REG(76), 1);  //GPIO8_2, SIO2_DIN
    reg_write32(IO_CONFIG_REG(77), 0);  //GPIO8_3, SPI_SCLK
    reg_write32(IO_CONFIG_REG(78), 0);  //GPIO8_4, SPI_SDO
    reg_write32(IO_CONFIG_REG(79), 0);  //GPIO8_5, SPI_SDI
    reg_write32(IO_CONFIG_REG(80), 0);  //GPIO8_6, SPI_CSN0
    reg_write32(IO_CONFIG_REG(81), 0);  //SPI_CSN3, NF_BOOT_PIN4, CLK_TEST_OUT0, CLK_TEST_OUT1, CLK_TEST_OUT2, CLK_TEST_OUT3
    reg_write32(IO_CONFIG_REG(82), 0);  //GPIO13_2, I2C_SDA
    reg_write32(IO_CONFIG_REG(83), 0);  //GPIO13_3, I2C_SCL
    reg_write32(IO_CONFIG_REG(84), 0);  //GPIO9_0, UART1_RTSN
    reg_write32(IO_CONFIG_REG(85), 1);  //GPIO9_1, UART1_RXD
    reg_write32(IO_CONFIG_REG(86), 0);  //GPIO9_2, UART1_CTSN
    reg_write32(IO_CONFIG_REG(87), 1);  //GPIO9_3, UART1_TXD
    reg_write32(IO_CONFIG_REG(88), 1);  //PLL_TEST_OUT0, RGMII_TXD0, PLL_TEST_OUT1, PLL_TEST_OUT2, PLL_TEST_OUT3
//    reg_write32(IO_CONFIG_REG(89), 0);  //RGMII_TXCLK, GPIO8_7
//    reg_write32(IO_CONFIG_REG(90), 0);  //GPIO13_4, RGMII_CRS, RGMII_RXER
//    reg_write32(IO_CONFIG_REG(91), 0);  //GPIO13_5, RGMII_COL, RGMII_TXER
    reg_write32(IO_CONFIG_REG(92), 0);  //IR_IN, GPIO9_4
    reg_write32(IO_CONFIG_REG(93), 0);  //NF_DQ0, GPIO9_5
    reg_write32(IO_CONFIG_REG(94), 0);  //NF_DQ1, GPIO9_6
    reg_write32(IO_CONFIG_REG(95), 0);  //NF_DQ2, GPIO9_7
    reg_write32(IO_CONFIG_REG(96), 0);  //NF_DQ3, GPIO10_0
    reg_write32(IO_CONFIG_REG(97), 0);  //NF_DQ4, GPIO10_1
    reg_write32(IO_CONFIG_REG(98), 0);  //NF_DQ5, GPIO10_2
    reg_write32(IO_CONFIG_REG(99), 1);  //NF_DQ6, GPIO10_3
    reg_write32(IO_CONFIG_REG(100), 0);  //NF_DQ7, GPIO10_4
    reg_write32(IO_CONFIG_REG(101), 1);  //NF_RDY0, GPIO10_5
    reg_write32(IO_CONFIG_REG(102), 1);  //NF_RDY1, GPIO10_6
    reg_write32(IO_CONFIG_REG(103), 0);  //SFC_DIO, GPIO13_6
    reg_write32(IO_CONFIG_REG(104), 0);  //SFC_WP_IO2, GPIO10_7
    reg_write32(IO_CONFIG_REG(105), 0);  //SFC_DOI, GPIO13_7
    reg_write32(IO_CONFIG_REG(106), 0);  //SFC_HOLD_IO3, GPIO14_4
    reg_write32(IO_CONFIG_REG(107), 0);  //GPIO14_0, USB0_OVRCUR
    reg_write32(IO_CONFIG_REG(108), 0);  //GPIO14_1, USB0_PWREN
    reg_write32(IO_CONFIG_REG(109), 0);  //GPIO14_2, USB1_OVRCUR
    reg_write32(IO_CONFIG_REG(110), 0);  //GPIO14_3, USB1_PWREN
    reg_write32(IO_CONFIG_REG(111), 1);  //GPIO11_0, UART3_RXD
    reg_write32(IO_CONFIG_REG(112), 1);  //GPIO11_1, UART3_TXD
    reg_write32(IO_CONFIG_REG(113), 1);  //GPIO11_2, SATA_LED_N0
    reg_write32(IO_CONFIG_REG(114), 1);  //GPIO11_3, SATA_LED_N1
    reg_write32(IO_CONFIG_REG(115), 1);  //GPIO11_4, HDMI_HOTPLUG
    reg_write32(IO_CONFIG_REG(116), 1);  //GPIOII_5, HDMI_CEC
    reg_write32(IO_CONFIG_REG(117), 1);  //GPIOII_6, HDMI_SDA
    reg_write32(IO_CONFIG_REG(118), 1);  //GPIOII_7, HDMI_SCL
    //gpio_AckoutFreqSel(27);
}
