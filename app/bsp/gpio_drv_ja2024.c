
static gpio_dev_t s_gpio_devs[] =
{
    {{2, 3, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_BUZZR},
    {{2, 6, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_ALARM},
    {{0, 9, GPIO_DIR_OUT,  GPIO_PIN_LOW},  GPIO_RS485},  //NC
    {{0, 5, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_CDRST},
    {{7, 1, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},
    {{7, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},  GPIO_LEDPD},
    {{3, 0,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},
    {{2, 5,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},
    {{2, 4,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},
    {{2, 1,  GPIO_DIR_IN,  GPIO_PIN_LOW},  GPIO_SENSR},
};

int gpio_AckoutFreqSel(unsigned long Freq)
{
    unsigned long tmpRead = 0;

    //Set SIO0Clk 24MHz for ACKOUT Output
    reg_write32(IO_CONFIG_REG(41), 1);
    tmpRead  = reg_read32(0x20050064);
    tmpRead &= 0xFF000000;
    //According to SC_PERCTRL13 Register Details
    tmpRead += (1UL << 27)*Freq/500;
    reg_write32(0x20050064, tmpRead);

    return 0;
}

int gpio_Platform_VerProbe(void)
{
    return GPIO_PROBE_VER0;
}

static void gpio_mode_conf(void)
{
    reg_write32(IO_CONFIG_REG( 0), 1);
    reg_write32(IO_CONFIG_REG( 1), 1);
    reg_write32(IO_CONFIG_REG( 2), 0);
    reg_write32(IO_CONFIG_REG( 3), 0);
    reg_write32(IO_CONFIG_REG( 4), 0);
    reg_write32(IO_CONFIG_REG( 5), 0);
    reg_write32(IO_CONFIG_REG( 6), 0);
    reg_write32(IO_CONFIG_REG( 7), 0);
    reg_write32(IO_CONFIG_REG( 8), 0);
    reg_write32(IO_CONFIG_REG( 9), 0);
    reg_write32(IO_CONFIG_REG(10), 0);
    reg_write32(IO_CONFIG_REG(11), 0);
    reg_write32(IO_CONFIG_REG(12), 0);
    reg_write32(IO_CONFIG_REG(13), 0);
    reg_write32(IO_CONFIG_REG(14), 0);
    reg_write32(IO_CONFIG_REG(15), 0);
    reg_write32(IO_CONFIG_REG(16), 0);
    reg_write32(IO_CONFIG_REG(17), 0);
    reg_write32(IO_CONFIG_REG(18), 2);
    reg_write32(IO_CONFIG_REG(19), 2);
    reg_write32(IO_CONFIG_REG(20), 0);
    reg_write32(IO_CONFIG_REG(21), 0);
    reg_write32(IO_CONFIG_REG(22), 0);
    reg_write32(IO_CONFIG_REG(23), 0);
    reg_write32(IO_CONFIG_REG(24), 0);
    reg_write32(IO_CONFIG_REG(25), 0);
    reg_write32(IO_CONFIG_REG(26), 0);
    reg_write32(IO_CONFIG_REG(27), 0);
    reg_write32(IO_CONFIG_REG(28), 0);
    reg_write32(IO_CONFIG_REG(29), 0);
    reg_write32(IO_CONFIG_REG(30), 0);
    reg_write32(IO_CONFIG_REG(31), 0);
    reg_write32(IO_CONFIG_REG(32), 0);
    reg_write32(IO_CONFIG_REG(33), 0);
    reg_write32(IO_CONFIG_REG(34), 0);
    reg_write32(IO_CONFIG_REG(35), 0);
    reg_write32(IO_CONFIG_REG(36), 0);
    reg_write32(IO_CONFIG_REG(37), 0);
    reg_write32(IO_CONFIG_REG(38), 0);
    reg_write32(IO_CONFIG_REG(39), 0);
    reg_write32(IO_CONFIG_REG(40), 0);
    reg_write32(IO_CONFIG_REG(41), 0);
    reg_write32(IO_CONFIG_REG(42), 0);
    reg_write32(IO_CONFIG_REG(43), 0);
    reg_write32(IO_CONFIG_REG(44), 0);
    reg_write32(IO_CONFIG_REG(45), 1);
    reg_write32(IO_CONFIG_REG(46), 1);
    reg_write32(IO_CONFIG_REG(47), 1);
    reg_write32(IO_CONFIG_REG(48), 1);
    reg_write32(IO_CONFIG_REG(49), 1);
    reg_write32(IO_CONFIG_REG(50), 1);
    reg_write32(IO_CONFIG_REG(51), 1);
    reg_write32(IO_CONFIG_REG(52), 1);
    reg_write32(IO_CONFIG_REG(53), 1);
    reg_write32(IO_CONFIG_REG(54), 1);
    reg_write32(IO_CONFIG_REG(55), 1);
    reg_write32(IO_CONFIG_REG(56), 1);
    reg_write32(IO_CONFIG_REG(57), 1);
    reg_write32(IO_CONFIG_REG(58), 1);
    reg_write32(IO_CONFIG_REG(59), 1);
    reg_write32(IO_CONFIG_REG(60), 1);
    reg_write32(IO_CONFIG_REG(61), 1);
    reg_write32(IO_CONFIG_REG(62), 0);
    reg_write32(IO_CONFIG_REG(63), 1);
    reg_write32(IO_CONFIG_REG(64), 1);
    reg_write32(IO_CONFIG_REG(65), 0);
    reg_write32(IO_CONFIG_REG(66), 0);
    reg_write32(IO_CONFIG_REG(67), 0);
    reg_write32(IO_CONFIG_REG(68), 0);
    reg_write32(IO_CONFIG_REG(69), 0);
    reg_write32(IO_CONFIG_REG(70), 0);
    reg_write32(IO_CONFIG_REG(71), 0);
    reg_write32(IO_CONFIG_REG(72), 0);
    reg_write32(IO_CONFIG_REG(73), 1);
    reg_write32(IO_CONFIG_REG(74), 1);
    reg_write32(IO_CONFIG_REG(75), 0);
    reg_write32(IO_CONFIG_REG(76), 0);
    reg_write32(IO_CONFIG_REG(77), 0);
    reg_write32(IO_CONFIG_REG(78), 0);
    reg_write32(IO_CONFIG_REG(79), 0);
    reg_write32(IO_CONFIG_REG(80), 0);
    reg_write32(IO_CONFIG_REG(81), 0);
    reg_write32(IO_CONFIG_REG(82), 0);

    gpio_AckoutFreqSel(24);
}
