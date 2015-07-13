#include "gpio_drv_keypad_matrix_setting.c"

#define keypad_gpio_row_cnt() ARRAY_SIZE(s_keypad_row)
#define keypad_gpio_col_cnt() ARRAY_SIZE(s_keypad_col)

static inline unsigned int MK_KEY(int row, int column)
{
	return (unsigned short)((((row)<<4)|(column)) + 1);
}
static unsigned int keyapd_gpio_scan(void);
void ClrKeySts(void)
{
	keypad.sts = KEY_NULL;
	keypad.key = keyapd_gpio_scan();
}

void SetKeyMap(struct input_dev* dev)
{
    int ii, jj;
    dev->keycodemax = keypad_gpio_row_cnt()
                    * keypad_gpio_col_cnt();
    for(ii = 0; ii < keypad_gpio_row_cnt(); ++ ii){
        for(jj = 0; jj < keypad_gpio_col_cnt(); ++ jj){
            set_bit(MK_KEY(ii, jj), dev->keybit);
        }
    }
    clear_bit(0, dev->keybit);
}

void keypad_config(void)
{
	int ii = 0;

    keypad_io_fix(gpio_Platform_VerProbe());

	// row output
	for(ii = 0; ii < keypad_gpio_row_cnt(); ++ ii){
		int const grp = s_keypad_row[ii].grp;
		int const pin = s_keypad_row[ii].pin;
		// config as output
		gpio_pin_init(s_keypad_row[ii]);
		// disable interrupt
		reg_clear32(GPIO_IE(grp), (1<<pin));
	}

	// column input clear bit
	for(ii = 0; ii < keypad_gpio_col_cnt(); ++ ii){
		int const grp = s_keypad_col[ii].grp;
		int const pin = s_keypad_col[ii].pin;
		// config as input
		gpio_pin_init(s_keypad_col[ii]);
		// input interrupt config as level trigger mode
		reg_set32(GPIO_IS(grp), (1<<pin));
		// low level trigger mode
		reg_clear32(GPIO_IEV(grp), (1<<pin));
	}
}

void keypad_gpio_set_row(int row, int level)
{
	if(row < keypad_gpio_row_cnt()){
		gpio_set_pin(s_keypad_row[row].grp,
                     s_keypad_row[row].pin,
                     level);
	}
}

void keypad_gpio_pulldown_row(unsigned char row)
{
	int i = 0;
	if(0xff == row) {
		// pull down all
		for(i = 0; i < keypad_gpio_row_cnt(); ++i) {
			keypad_gpio_set_row(i, GPIO_PIN_LOW);
		}
	}else if(row < keypad_gpio_row_cnt()) {
		for(i = 0; i < keypad_gpio_row_cnt(); ++i) {
			keypad_gpio_set_row(i, (i == row) ? GPIO_PIN_LOW : GPIO_PIN_HIGH);
		}
	}
}

int keypad_gpio_get_col(unsigned char column)
{
	if(column < keypad_gpio_col_cnt()) {
		return gpio_get_pin(s_keypad_col[column].grp,
                            s_keypad_col[column].pin);
	}
	return 0;
}

unsigned char keypad_gpio_check_col(void)
{
	int ii = 0;
	for(ii = 0; ii < keypad_gpio_col_cnt(); ++ ii) {
		// looking for the low level pin
		if(!keypad_gpio_get_col(ii)) {
			return ii;
		}
	}
	return 0xff;
}

void keypad_enable_irq(void)
{
	int ii = 0;
	// pull down all the output pin
	// when the row and column connected
	// the column input will get an interrupt
	keypad_gpio_pulldown_row(0xff);
	///
	for(ii = 0; ii < keypad_gpio_col_cnt(); ++ ii) {
		int const grp = s_keypad_col[ii].grp;
		int const pin = s_keypad_col[ii].pin;
		// clear all interrupt
		reg_set32(GPIO_IC(grp), (1<<pin));
		// enable interrupt
		reg_set32(GPIO_IE(grp), (1<<pin));
	}
}

void keypad_disable_irq(void)
{
	int ii = 0;
	for(ii = 0; ii < keypad_gpio_col_cnt(); ++ ii) {
		int const grp = s_keypad_col[ii].grp;
		int const pin = s_keypad_col[ii].pin;
		// clear all interrupt
		reg_set32(GPIO_IC(grp), (1<<pin));
		// disable interrupt
		reg_clear32(GPIO_IE(grp), (1<<pin));
	}
}

static void keypad_report_event(void)
{
	int const row = (keypad.key & 0xf0) >> 4;
	int const col = (keypad.key & 0x0f);
	if(row < keypad_gpio_row_cnt() && col < keypad_gpio_col_cnt()){
		int const code  = MK_KEY(row, col);
		int const state = (KEY_PRESS == keypad.sts) ? 1 : 0;
		input_report_key(keypad.dev, code, state);
		input_sync(keypad.dev);

		if(state){
			gpio_keypad_buzzer();
		}
	}
}

static unsigned int keyapd_gpio_scan(void)
{
	static unsigned char upsidedown = 0;
	int i = 0;
	int ii = 0;

	int const row_cnt = keypad_gpio_row_cnt();
	int const col_cnt = keypad_gpio_col_cnt();

	if(upsidedown){
		upsidedown = 0;
		// right to left

		for(i = row_cnt - 1; i >= 0; --i){
			// scan row by row
			keypad_gpio_pulldown_row(i);
			for(ii = col_cnt - 1; ii >= 0; --ii){
				if(!keypad_gpio_get_col(ii)){
//					KEYPAD_TRACE("scan key @ row=%d col=%d", i, ii);
					return ((i << 4) | ii);
				}
			}
		}
	}else{
		upsidedown = 1;
		// left to right
		for(i = 0; i < row_cnt; ++i){
			// scan row by row
			keypad_gpio_pulldown_row(i);
			for(ii = 0; ii < col_cnt; ++ii){
				if(!keypad_gpio_get_col(ii)){
//					KEYPAD_TRACE("scan key @ row=%d col=%d", i, ii);
					return ((i << 4) | ii);
				}
			}
		}
	}
	return 0xff;
}

static void keypad_scan_timer(unsigned long data)
{
	unsigned int key_scan = 0;
	switch(keypad.sts)
	{
	case KEY_NULL:
		// from interrupt timer start
		// check twice to avoid key shakable
		if(keypad.key == keyapd_gpio_scan() && 0xff != keypad.key){
			// no shakable
			keypad.sts = KEY_PRESS;
			keypad_report_event();
			keypad_proc_timer_start(data);
		}else{
			// shakable
			keypad_enable_irq();
		}
		break;
	case KEY_PRESS:
		if(keypad.key == (key_scan = keyapd_gpio_scan())){
			// no shakable
			// KEYPAD_TRACE("key %02x pressing", keypad.key);
		}else{
			// shakable
			keypad.sts = KEY_RELEASE;
			keypad_report_event();
		}
		keypad_proc_timer_start(data);
		break;
	case KEY_RELEASE:
        keypad_gpio_pulldown_row(-1);
        if(0xff == keypad_gpio_check_col()) {
			// lease timer
			keypad.sts = KEY_NULL;
			//KEYPAD_TRACE("key all release");
			keypad_proc_timer_stop();
			keypad_enable_irq();
		}else{
			// KEYPAD_TRACE("key waiting to release");
			keypad_proc_timer_start(data);
		}
		break;
	default:
		break;
	}
}

DEFINE_IOCTL(keypad_ioctl, file, cmd, arg)
{
    return 0;
}
