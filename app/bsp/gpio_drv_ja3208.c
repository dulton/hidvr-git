
static gpio_dev_t s_gpio_devs[] =
{
};

int gpio_AckoutFreqSel(unsigned long Freq)
{
    return 0;
}

int gpio_Platform_VerProbe(void)
{
    return GPIO_PROBE_VER0;
}

static void gpio_mode_conf(void)
{
    reg_write32(IO_CONFIG_REG(0), 1); //VIU0_VS, VIU0_CLKA
    reg_write32(IO_CONFIG_REG(1), 1); //VIU0_HS, GPIO0_0
    reg_write32(IO_CONFIG_REG(2), 0); //VIU0_DAT7, GPIO0_1
    reg_write32(IO_CONFIG_REG(3), 0); //VIU0_DAT6, GPIO0_2
    reg_write32(IO_CONFIG_REG(4), 0); //VIU0_DAT5, GPIO0_3
    reg_write32(IO_CONFIG_REG(5), 0); //VIU0_DAT4, GPIO0_4
    reg_write32(IO_CONFIG_REG(6), 0); //VIU0_DAT3, GPIO0_5
    reg_write32(IO_CONFIG_REG(7), 0); //VIU0_DAT2, GPIO0_6
    reg_write32(IO_CONFIG_REG(8), 0); //VIU0_DAT1, GPIO0_7
    reg_write32(IO_CONFIG_REG(9), 0); //VIU0_DAT0, GPIO1_0
    reg_write32(IO_CONFIG_REG(10), 1); //VIU1_VS, VIU1_CLKA
    reg_write32(IO_CONFIG_REG(11), 1); //VIU1_HS, GPIO1_1
    reg_write32(IO_CONFIG_REG(12), 0); //VIU1_DAT7, GPIO1_2
    reg_write32(IO_CONFIG_REG(13), 0); //VIU1_DAT6, GPIO1_3
    reg_write32(IO_CONFIG_REG(14), 0); //VIU1_DAT5, GPIO1_4
    reg_write32(IO_CONFIG_REG(15), 0); //VIU1_DAT4, GPIO1_5
    reg_write32(IO_CONFIG_REG(16), 0); //VIU1_DAT3, GPIO1_6
    reg_write32(IO_CONFIG_REG(17), 0); //VIU1_DAT2, GPIO1_7
    reg_write32(IO_CONFIG_REG(18), 0); //VIU1_DAT1, GPIO2_0
    reg_write32(IO_CONFIG_REG(19), 0); //VIU1_DAT0, GPIO2_1
    reg_write32(IO_CONFIG_REG(20), 1); //VIU2_VS, VIU2_CLKA
    reg_write32(IO_CONFIG_REG(21), 1); //VIU2_HS, GPIO2_2
    reg_write32(IO_CONFIG_REG(22), 0); //VIU2_DAT7, GPIO2_3
    reg_write32(IO_CONFIG_REG(23), 0); //VIU2_DAT6, GPIO2_4
    reg_write32(IO_CONFIG_REG(24), 0); //VIU2_DAT5, GPIO2_5
    reg_write32(IO_CONFIG_REG(25), 0); //VIU2_DAT4, GPIO2_6
    reg_write32(IO_CONFIG_REG(26), 0); //VIU2_DAT3, GPIO2_7
    reg_write32(IO_CONFIG_REG(27), 0); //VIU2_DAT2, GPIO3_0
    reg_write32(IO_CONFIG_REG(28), 0); //VIU2_DAT1, GPIO3_1
    reg_write32(IO_CONFIG_REG(29), 0); //VIU2_DAT0, GPIO3_2
    reg_write32(IO_CONFIG_REG(30), 1); //VIU3_VS, VIU3_CLKA
    reg_write32(IO_CONFIG_REG(31), 1); //VIU3_HS, GPIO3_3
    reg_write32(IO_CONFIG_REG(32), 0); //VIU3_DAT7, GPIO3_4
    reg_write32(IO_CONFIG_REG(33), 0); //VIU3_DAT6, GPIO3_5
    reg_write32(IO_CONFIG_REG(34), 0); //VIU3_DAT5, GPIO3_6
    reg_write32(IO_CONFIG_REG(35), 0); //VIU3_DAT4, GPIO3_7
    reg_write32(IO_CONFIG_REG(36), 0); //VIU3_DAT3, GPIO4_0
    reg_write32(IO_CONFIG_REG(37), 0); //VIU3_DAT2, GPIO4_1
    reg_write32(IO_CONFIG_REG(38), 0); //VIU3_DAT1, GPIO4_2
    reg_write32(IO_CONFIG_REG(39), 0); //VIU3_DAT0, GPIO4_3
    reg_write32(IO_CONFIG_REG(40), 2); //VIU4_CLK, SFC_CLK, GPIO4_4
    reg_write32(IO_CONFIG_REG(41), 2); //VIU4_DAT15, SFC_DIO, GPIO4_5
    reg_write32(IO_CONFIG_REG(42), 2); //VIU4_DAT14, SFC_DOI, GPIO4_6
    reg_write32(IO_CONFIG_REG(43), 2); //VIU4_DAT13, SFC_WP_IO2, GPIO4_7
    reg_write32(IO_CONFIG_REG(44), 2); //VIU4_DAT12, SFC_HOLD_IO3, GPIO5_0
    reg_write32(IO_CONFIG_REG(45), 2); //VIU4_DAT11, SFC_CS0N, GPIO5_1
    reg_write32(IO_CONFIG_REG(46), 2); //VIU4_DAT10, SFC_CS1N, GPIO5_2
    reg_write32(IO_CONFIG_REG(47), 2); //VIU4_DAT9, SIO4_XCLK, GPIO5_3
    reg_write32(IO_CONFIG_REG(48), 2); //VIU4_DAT8, SIO4_XFS, GPIO5_4, VOU0_DV
    reg_write32(IO_CONFIG_REG(49), 2); //VIU4_DAT7, SIO4_RCLK, GPIO5_5
    reg_write32(IO_CONFIG_REG(50), 2); //VIU4_DAT6, SIO4_RFS, GPIO5_6
    reg_write32(IO_CONFIG_REG(51), 2); //VIU4_DAT5, SIO4_DOUT, GPIO5_7
    reg_write32(IO_CONFIG_REG(52), 2); //VIU4_DAT4, SIO4_DIN, GPIO6_0
    reg_write32(IO_CONFIG_REG(53), 1); //VIU4_DAT3, GPIO6_1
    reg_write32(IO_CONFIG_REG(54), 1); //VIU4_DAT2, GPIO6_2
    reg_write32(IO_CONFIG_REG(55), 2); //VIU4_DAT1, PCIE_RSETN, GPIO6_3
    reg_write32(IO_CONFIG_REG(56), 2); //VIU4_DAT0, PCIE_WAKE, GPIO6_4
    reg_write32(IO_CONFIG_REG(57), 1); //MDCK, VOU0_CLK
    reg_write32(IO_CONFIG_REG(58), 0); //UART_RXD, VOU0_HS
    reg_write32(IO_CONFIG_REG(59), 0); //UART_TXD, VOU0_VS
    reg_write32(IO_CONFIG_REG(60), 1); //MDIO, VOU0_DATA15
    reg_write32(IO_CONFIG_REG(61), 1); //MII_TXCK, VOU0_DATA14
    reg_write32(IO_CONFIG_REG(62), 1); //MII_TXEN, VOU0_DATA13
    reg_write32(IO_CONFIG_REG(63), 1); //MII_TXD3, VOU0_DATA12
    reg_write32(IO_CONFIG_REG(64), 1); //MII_TXD2, VOU0_DATA11
    reg_write32(IO_CONFIG_REG(65), 1); //MII_TXD1, VOU0_DATA10
    reg_write32(IO_CONFIG_REG(66), 1); //MII_TXD0, VOU0_DATA9, CLK_TEST_OUT0, CLK_TEST_OUT1, CLK_TEST_OUT2
    reg_write32(IO_CONFIG_REG(67), 1); //MII_RXDV, VOU0_DATA8
    reg_write32(IO_CONFIG_REG(68), 1); //MII_RXD3, VOU0_DATA7
    reg_write32(IO_CONFIG_REG(69), 1); //MII_RXD2, VOU0_DATA6
    reg_write32(IO_CONFIG_REG(70), 1); //MII_RXD1, VOU0_DATA5
    reg_write32(IO_CONFIG_REG(71), 1); //MII_RXD0, VOU0_DATA4
    reg_write32(IO_CONFIG_REG(72), 1); //MII_RXCK, VOU0_DATA3
    reg_write32(IO_CONFIG_REG(73), 1); //MII_CRS, VOU0_DATA2
    reg_write32(IO_CONFIG_REG(74), 1); //MII_COL, VOU0_DATA1
    reg_write32(IO_CONFIG_REG(75), 1); //PLL_TEST_OUT0, VOU0_DATA0, PLL_TEST_OUT1, PLL_TEST_OUT2
    reg_write32(IO_CONFIG_REG(76), 0); //GPIO6_5, SIO1_RCLK
    reg_write32(IO_CONFIG_REG(77), 0); //GPIO6_6, SIO1_RFS
    reg_write32(IO_CONFIG_REG(78), 0); //GPIO6_7, SIO1_DIN
    reg_write32(IO_CONFIG_REG(79), 0); //GPIO7_0, SIO2_RCLK
    reg_write32(IO_CONFIG_REG(80), 0); //GPIO7_1, SIO2_RFS
    reg_write32(IO_CONFIG_REG(81), 0); //GPIO7_2, SIO2_DIN
    reg_write32(IO_CONFIG_REG(82), 0); //GPIO7_3, SIO3_RCLK
    reg_write32(IO_CONFIG_REG(83), 0); //GPIO7_4, SIO3_RFS
    reg_write32(IO_CONFIG_REG(84), 0); //GPIO7_5, SIO3_DIN
    reg_write32(IO_CONFIG_REG(85), 0); //GPIO7_6, I2C_SDA
    reg_write32(IO_CONFIG_REG(86), 0); //GPIO7_7, I2C_SCL
    reg_write32(IO_CONFIG_REG(87), 0); //GPIO8_0, SPI_SCLK
    reg_write32(IO_CONFIG_REG(88), 0); //GPIO8_1, SPI_SDO
    reg_write32(IO_CONFIG_REG(89), 0); //GPIO8_2, SPI_SDI
    reg_write32(IO_CONFIG_REG(90), 0); //GPIO8_3, SPI_CSN0
    reg_write32(IO_CONFIG_REG(91), 0); //GPIO8_4, SPI_CSN1
    reg_write32(IO_CONFIG_REG(92), 0); //GPIO8_5, SPI_CSN2
    reg_write32(IO_CONFIG_REG(93), 0); //GPIO8_6, SPI_CSN3
    reg_write32(IO_CONFIG_REG(94), 0); //GPIO8_7, SPI_CSN4
    reg_write32(IO_CONFIG_REG(95), 0); //GPIO9_0, SPI_CSN5
    reg_write32(IO_CONFIG_REG(96), 1); //SPI_CSN6, JTAG_SEL, GPIO9_1
    reg_write32(IO_CONFIG_REG(97), 1); //SPI_CSN7, BOOT_SEL, GPIO9_2
}
