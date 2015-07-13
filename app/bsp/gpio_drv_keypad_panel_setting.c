#if defined(GPIO_PLAT_TYPE3)
// irq 8 gpio2 gpio3 gpio4 gpio5 combo interrupt
static const int s_irq_number[1] = {8};

static const gpio_conf_t PanelC_Pins[PANELC_NUM] =
{
    [PANELC_ENB] = {2, 2, GPIO_DIR_OUT, GPIO_PIN_HIGH },
    [PANELC_INT] = {2, 7, GPIO_DIR_IN,  GPIO_PIN_HIGH },
};
#elif defined(GPIO_PLAT_TYPE4)
// irq 112 gpio8 interrupt source
static const int s_irq_number[1] = {112};

static const gpio_conf_t PanelC_Pins[PANELC_NUM] =
{
    [PANELC_ENB] = {8, 0, GPIO_DIR_OUT, GPIO_PIN_HIGH },
    [PANELC_INT] = {8, 1, GPIO_DIR_IN,  GPIO_PIN_HIGH },
};
#elif defined(GPIO_PLAT_TYPE6)
// irq 114 gpio10 interrupt source
static const int s_irq_number[1] = {114};

static const gpio_conf_t PanelC_Pins[PANELC_NUM] =
{
    [PANELC_ENB] = {10, 3, GPIO_DIR_OUT, GPIO_PIN_HIGH },
    [PANELC_INT] = {10, 2, GPIO_DIR_IN,  GPIO_PIN_HIGH },
};
#elif defined(GPIO_PLAT_TYPE12)
// irq 117 gpio17/18 interrupt source
static const int s_irq_number[1] = {117};

static const gpio_conf_t PanelC_Pins[PANELC_NUM] =
{
    [PANELC_ENB] = {17, 4, GPIO_DIR_OUT, GPIO_PIN_HIGH },
    [PANELC_INT] = {17, 6, GPIO_DIR_IN,  GPIO_PIN_HIGH },
};
#endif
