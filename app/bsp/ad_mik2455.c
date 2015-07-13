
#include "ad_mik2455_def.h"

#define MIK2455_FAST_SWITCH (0)
#define MIK2455_CHN_PER_CHIP (4)

static unsigned char MIK2455_WRITE(int chip,
								unsigned char addr, unsigned char data) 
{
    gpio_i2c_write(MIK2455_DEV[chip],addr,data);
	udelay(75);
	return 0;
}

static unsigned char MIK2455_READ(int chip, unsigned char addr)
{	
	//master slave mode I2C need double read
    gpio_i2c_read(MIK2455_DEV[chip],addr);
	udelay(75);
    return gpio_i2c_read(MIK2455_DEV[chip],addr);
}

static inline void MIK2455_WRITE_TABLE(unsigned char chip,
	const unsigned char *tbl_ptr, unsigned char tbl_cnt)
{
	unsigned char i = 0;
	for(i = 0; i < tbl_cnt; i += 2){
		MIK2455_WRITE(chip, *(tbl_ptr + i), *(tbl_ptr + i + 1));
	}

}

static void mik2455_set_hue(int chip, int chn, unsigned char gain)
{
	const unsigned char MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < MIK2455_CHN_PER_CHIP)){
		return;
	}
	if(gain >= sizeof(MAP)/sizeof(MAP[0])){
		gain = sizeof(MAP)/sizeof(MAP[0])-1;
	}
	MIK2455_WRITE(chip, 0x40, (1 << chn));
	MIK2455_WRITE(chip, 0x2c, MAP[gain]);
}

static void mik2455_set_contrast(int chip, int chn, unsigned char gain)
{
	const unsigned char MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < MIK2455_CHN_PER_CHIP)){
		return;
	}
	if(gain >= sizeof(MAP)/sizeof(MAP[0])) {
		gain = sizeof(MAP)/sizeof(MAP[0])-1;
	}
	MIK2455_WRITE(chip, 0x40, (1 << chn));
	MIK2455_WRITE(chip, 0x2e, MAP[gain]);
}

static void mik2455_set_brightness(int chip, int chn, unsigned char gain)
{
	const unsigned char MAP[] = VIDEO_COLOR_ADJTBL0;

	if(!(chn < MIK2455_CHN_PER_CHIP)){
		return;
	}
	if(gain >= sizeof(MAP)/sizeof(MAP[0])) {
		gain = sizeof(MAP)/sizeof(MAP[0])-1;
	}
	MIK2455_WRITE(chip, 0x40, (1 << chn));
	MIK2455_WRITE(chip, 0x2f, MAP[gain]);
}

static void mik2455_set_saturation(int chip, int chn, unsigned char gain)
{
	const unsigned char MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < MIK2455_CHN_PER_CHIP)){
		return;
	}
	if(gain >= sizeof(MAP)/sizeof(MAP[0])) {
		gain = sizeof(MAP)/sizeof(MAP[0])-1;
	}
	MIK2455_WRITE(chip, 0x40, (1 << chn));
	MIK2455_WRITE(chip, 0x2d, MAP[gain]);
}

static int mik2455_set_standard(int chip, const char* std)
{
	if(!strncasecmp(std, "PAL", strlen("PAL"))){
		MIK2455_WRITE(chip, 0x40 ,0x0f);//select 4 Channel one time
		MIK2455_WRITE(chip, 0x10 ,0x40);//force 4 channel to PAL
	}
	else if(!strncasecmp(std, "NTSC", strlen("NTSC"))){
		MIK2455_WRITE(chip, 0x40 ,0x0f);//select 4 Channel one time
		MIK2455_WRITE(chip, 0x10 ,0x04);//force 4 channel to PAL
	}
	else {
		printk("MIK2455 not support this video norm : %d\n", chip);
		return -1;
	}

	return 0;
}

static unsigned int mik2455_get_loss(int chip)
{
	//return 0;
	int i = 0;
	unsigned int ret_bit = 0;
	for(i = 0; i < MIK2455_CHN_PER_CHIP; ++i){
		if(MIK2455_READ(chip, 0x51 + i * 0x05)){
			ret_bit &= ~(1<<i);
		}else{
			ret_bit |= (1<<i);
		}
	}
	return ret_bit;
}

static int mik2455_set_mode_4d1(unsigned chip)
{
	// Video Channel Output Control
	// Four Channel ITU0R BT.656 Time-multiplexed format output
	MIK2455_WRITE(chip, 0x40, 0x0f);    

	// Output Enable Control and Clock Output Control
	// 108MHz clock output
	MIK2455_WRITE(chip, 0x45, 0x02);
	MIK2455_WRITE(chip, 0x44, 0x03);
	if (MIK2455_FAST_SWITCH){
		MIK2455_WRITE(chip, 0x47, 0x00);
	}else{
		MIK2455_WRITE(chip, 0x47, 0x04);
	}
	//printk("mik2455 0x%x set to 4d1 mode ok\n", chip_addr);
	return 0;
}

static int mik2455_set_mode_2d1(unsigned chip_addr)
{
	// Video Channel Output Control
	MIK2455_WRITE(chip_addr, 0x40, 0x0f);

	// Output Enable Control and Clock Output Control
	// 54MHz clock output
	MIK2455_WRITE(chip_addr, 0x45, 0x01);
	MIK2455_WRITE(chip_addr, 0x44, 0x03);
	if (MIK2455_FAST_SWITCH){
		MIK2455_WRITE(chip_addr, 0x47, 0x00);
	}else{
		MIK2455_WRITE(chip_addr, 0x47, 0x04);
	}
	return 0;
}

static int mik2455_set_audio_samplerate(int chip, audio_samplerate_t samplerate)
{
    int temp, val;

    // for mik2455
    switch (samplerate)
    {
        case SAMPLE_RATE_8000:
            val = 0xc;
            break;
        case SAMPLE_RATE_16000:
            val = 0x8;
            break;
/*        case SAMPLE_RATE_24000:
            val = 0x4;
            break;*/
        case SAMPLE_RATE_48000:
            val = 0x0;
            break;
        default:
            printk("not support this samplerate:%d \n", samplerate);
            return -1;
    }

	temp = MIK2455_READ(chip, 0xd4);
	temp &= 0xf3;
	temp |= val;
	MIK2455_WRITE(chip, 0xd4, temp);
	temp = MIK2455_READ(chip, 0xd8);
	temp &= 0xf3;
	temp |= val;
	MIK2455_WRITE(chip, 0xd8, temp);

    printk("mik2455 audio samplerate set to %d ok\n", val);
    return 0;
}

static int mik2455_set_audio_format(int chip, audio_format_t audio_format)
{
    unsigned char temp;
    
    /* Define the digital serial audio data format for record
        and mixing audio on the ACLKR, ASYNR, ADATR and ADATM pin.*/
    temp = MIK2455_READ(chip, 0xd4);
    temp &= 0xfd;/* [1]*/
    temp |= (0==audio_format.format) ? 0 : 0x2;/* 0:I2S format 1:DSP format */
    MIK2455_WRITE(chip, 0xd4, temp);

    /* Define the digital serial audio data format for
        playback audio on the ACLKP, ASYNP and ADATP pin.*/
    temp = MIK2455_READ(chip, 0xd8);
    temp &= 0xfd;/* [1]*/
    temp |= (0==audio_format.format) ? 0x0 : 0x2; /* 0:I2S format 1:DSP format */
    MIK2455_WRITE(chip, 0xd8, temp);

    return 0;
}

static int mik2455_audio_init(int chip)
{
	return 0;
}

static int mik2455_video_init(int chip, char* std)
{

	return 0;

}

static unsigned char mik2455_check(int chip)
{
	if(0x45 == MIK2455_READ(chip, 0xb6)) {
		// add for MIK2455
		MIK2455_WRITE(chip, 0x41, 0x03);

		// for i2c slave device
		MIK2455_WRITE(chip, 0xa9, 0xC0); //Device address ID is 0xc0
		MIK2455_WRITE(chip, 0xab, 0x40); //Not fast I2C speed

		MIK2455_WRITE(chip, 0x40, 0x0f);
		if(0x0f != MIK2455_READ(chip, 0x40)) {
			return 0;
		}

		MIK2455_WRITE_TABLE(chip,tbl_mik2455_common,sizeof(tbl_mik2455_common)/sizeof(unsigned char));
		mik2455_set_mode_4d1(chip);

		return 1;
	}

	return 0;
}
