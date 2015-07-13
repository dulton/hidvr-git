#if defined(GPIO_PLAT_TYPE1)
// irq 6 gpio0 interrupt
// irq 8 gpio2 gpio3 gpio4 gpio5 combo interrupt
static int s_irq_number[] = {6, 8};

static gpio_conf_t s_keypad_row[] = {
    {1, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {3, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {2, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {3, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {0, 4, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver)
{
    if(Ver == GPIO_PROBE_VER1) {
        s_keypad_row[0].grp = 1;
        s_keypad_row[0].pin = 5;
        s_keypad_col[4].grp = 3;
        s_keypad_col[4].pin = 4;
    }
}
#elif defined(GPIO_PLAT_TYPE2)
// irq 6 gpio0 interrupt
// irq 8 gpio2 gpio3 gpio4 gpio5 combo interrupt
static int s_irq_number[] = {6, 8};

static gpio_conf_t s_keypad_row[] = {
    {1, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {3, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {2, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {3, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {0, 4, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE3)
// irq 6 gpio0 interrupt
// irq 7 gpio0 interrupt
// irq 8 gpio2 gpio3 gpio4 gpio5 combo interrupt
static int s_irq_number[] = {6, 7, 8};

static gpio_conf_t s_keypad_row[] = {
    {0, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {1, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {3, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {3, 6, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {3, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {7, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {1, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
    {3, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {3, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE4)
// irq 112 gpio7/gpio8  interrupt source
// irq 113 gpio9/gpio10 interrupt source
static int s_irq_number[] = {112, 113};

static gpio_conf_t s_keypad_row[] = {
    {9, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {8, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {8, 6, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {8, 5, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {8, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {9, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {9, 4, GPIO_DIR_IN, GPIO_PIN_LOW},
    {9, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {9, 2, GPIO_DIR_IN, GPIO_PIN_LOW},
    {9, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE5)
// irq 8 gpio2 gpio3 gpio4 gpio5 combo interrupt
static int s_irq_number[] = {8};

static gpio_conf_t s_keypad_row[] = {
    {0, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {1, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {1, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {2, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 2, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {2, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE6)
// irq 109 gpio4  interrupt source
// irq 110 gpio5 interrupt source
static int s_irq_number[] = {109, 110};

static gpio_conf_t s_keypad_row[] = {
    {4, 1, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {4, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {4, 6, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {5, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {5, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {4, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
    {4, 4, GPIO_DIR_IN, GPIO_PIN_LOW},
    {5, 2, GPIO_DIR_IN, GPIO_PIN_LOW},
    {4, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {4, 7, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE7)
// irq 87 gpio8  interrupt source
static int s_irq_number[] = {87};

static gpio_conf_t s_keypad_row[] = {
    {2, 6, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 5, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {2, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {8, 2, GPIO_DIR_IN, GPIO_PIN_LOW},
    {8, 1, GPIO_DIR_IN, GPIO_PIN_LOW},
    {8, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
    {8, 3, GPIO_DIR_IN, GPIO_PIN_LOW},
    {8, 4, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#elif defined(GPIO_PLAT_TYPE12)
// irq 113 gpio9/10
// irq 114 gpio11/12
// irq 116 gpio15/16
// irq 117 gpio17/18
static int s_irq_number[] = {113, 114, 116, 117};

static gpio_conf_t s_keypad_row[] = {
    {10, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {12, 1, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {15, 6, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {15, 7, GPIO_DIR_OUT, GPIO_PIN_HIGH},
    {16, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH},
};

static gpio_conf_t s_keypad_col[] = {
    {17, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
    {12, 2, GPIO_DIR_IN, GPIO_PIN_LOW},
    {12, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
    {16, 0, GPIO_DIR_IN, GPIO_PIN_LOW},
    {15, 5, GPIO_DIR_IN, GPIO_PIN_LOW},
};

void keypad_io_fix(int Ver) {}
#endif
