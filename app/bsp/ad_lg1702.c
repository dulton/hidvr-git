
//#define LG1702_XTAL_54M

#define LG1702_CHN_PER_CHIP (4)

#define LG1702_WRITE(chip,addr,data) lg1702_i2c_write(LG1702_DEV[chip], addr, data)
#define LG1702_READ(chip,addr) lg1702_i2c_read(LG1702_DEV[chip], addr)

int lg1702_i2c_read(unsigned char dev, unsigned short addr)
{
    unsigned char addrbuf[] = {
        (unsigned char)((addr >> 8) & 0xFF),
        (unsigned char)(addr & 0xFF),
    };
    unsigned char rdata;

    gpio_i2c_rd_arr(0, dev, addrbuf, sizeof(addrbuf), &rdata, sizeof(rdata));

	return rdata;
}

int lg1702_i2c_write(unsigned char dev, unsigned short addr, unsigned char data)
{
    unsigned char addrbuf[] = {
        (unsigned char)((addr >> 8) & 0xFF),
        (unsigned char)(addr & 0xFF),
    };

    gpio_i2c_wr_arr(0, dev, addrbuf, sizeof(addrbuf), &data, sizeof(data));

	return 0 ;
}

static unsigned char lg1702_check(int chip)
{
	unsigned char id = LG1702_READ(chip, 0x0000);
	return (id == 0x02);
}

static void lg1702_set_hue(int chip, int chn, unsigned char gain)
{
	const unsigned char HUE_MAP[] = VIDEO_COLOR_ADJTBL0;

	if(!(chn < LG1702_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	LG1702_WRITE(chip, (((1<<chn)<<12) |  0x00b), HUE_MAP[gain]);
}

static void lg1702_set_contrast(int chip, int chn, unsigned char gain)
{
	const unsigned char CONTRAST_MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < LG1702_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	LG1702_WRITE(chip, (((1<<chn)<<12) |  0x008), CONTRAST_MAP[gain]);
}

static void lg1702_set_brightness(int chip, int chn, unsigned char gain)
{
	const unsigned char BRIGHTNESS_MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < LG1702_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	LG1702_WRITE(chip, (((1<<chn)<<12) |  0x009), BRIGHTNESS_MAP[gain]);
}

static void lg1702_set_saturation(int chip, int chn, unsigned char gain)
{
	const unsigned char SATURATION_MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < LG1702_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	LG1702_WRITE(chip, (((1<<chn)<<12) |  0x00a), SATURATION_MAP[gain]);
}

static int lg1702_set_standard(int chip, const char* std)
{
	LG1702_WRITE(chip,0x0040,VIDEO_IS_NTSC(std) ? 0xE8:0xE9);
	LG1702_WRITE(chip,0x0042,VIDEO_IS_NTSC(std) ? 0x38:0x2A);
	LG1702_WRITE(chip,0x0043,VIDEO_IS_NTSC(std) ? 0x1F:0x1F);
	LG1702_WRITE(chip,0x0045,VIDEO_IS_NTSC(std) ? 0x21:0x2A);
	LG1702_WRITE(chip,0x0046,VIDEO_IS_NTSC(std) ? 0xF0:0x09);
	LG1702_WRITE(chip,0x0047,VIDEO_IS_NTSC(std) ? 0x7C:0x8A);
	LG1702_WRITE(chip,0x0048,VIDEO_IS_NTSC(std) ? 0x1F:0xCB);

	LG1702_WRITE(chip,0xF000,VIDEO_IS_NTSC(std) ? 0x00:0x32);
	LG1702_WRITE(chip,0xF001,VIDEO_IS_NTSC(std) ? 0x01:0x00);
	LG1702_WRITE(chip,0xF003,VIDEO_IS_NTSC(std) ? 0x00:0x02);
	LG1702_WRITE(chip,0xF004,VIDEO_IS_NTSC(std) ? 0xDD:0xDC);
	LG1702_WRITE(chip,0xF007,VIDEO_IS_NTSC(std) ? 0x61:0x64);
	LG1702_WRITE(chip,0xF00C,VIDEO_IS_NTSC(std) ? 0x8A:0x7C);

	LG1702_WRITE(chip,0xF018,VIDEO_IS_NTSC(std) ? 0x21:0x2A);
	LG1702_WRITE(chip,0xF019,VIDEO_IS_NTSC(std) ? 0xF0:0x09);
	LG1702_WRITE(chip,0xF01A,VIDEO_IS_NTSC(std) ? 0x7C:0x8A);
	LG1702_WRITE(chip,0xF01B,VIDEO_IS_NTSC(std) ? 0x1F:0xCB);
	LG1702_WRITE(chip,0xF02E,VIDEO_IS_NTSC(std) ? 0x7A:0x7E);
	LG1702_WRITE(chip,0xF030,VIDEO_IS_NTSC(std) ? 0x26:0x28);
	LG1702_WRITE(chip,0xF031,VIDEO_IS_NTSC(std) ? 0x60:0xC0);
	LG1702_WRITE(chip,0xF092,VIDEO_IS_NTSC(std) ? 0x00:0x00);
	LG1702_WRITE(chip,0xF0B0,VIDEO_IS_NTSC(std) ? 0x3A:0x38);
	LG1702_WRITE(chip,0xF0B1,VIDEO_IS_NTSC(std) ? 0xF3:0xD3);
	LG1702_WRITE(chip,0xF0B2,VIDEO_IS_NTSC(std) ? 0x58:0x58);
	LG1702_WRITE(chip,0xF0B5,VIDEO_IS_NTSC(std) ? 0x0C:0x08);

	LG1702_WRITE(chip,0xF2B4,VIDEO_IS_NTSC(std) ? 0x0E:0x0F);
	LG1702_WRITE(chip,0xF2B5,VIDEO_IS_NTSC(std) ? 0xE9:0xC0);
    
	return 0;
}

static unsigned int lg1702_get_loss(int chip)
{
	unsigned int ret_bit = 0;
	ret_bit = LG1702_READ(chip, 0x0062);
	ret_bit &= 0xf;
	return ret_bit;
}

static void lg1702_video_init(int chip, const char* std)
{
	LG1702_WRITE(chip,0x0011,0x30); // pll-m = 54MHz pll-n = 54MHz
	LG1702_WRITE(chip,0x0001,0xff); // pll power down, audio dac power down, audio adc power down, video adc channel power down
	LG1702_WRITE(chip,0x0002,0x07); // external REXT, crominance video dac normal, luminance video dac normal
	LG1702_WRITE(chip,0x0004,0xFE);
	LG1702_WRITE(chip,0x0005,0x0E);
/**********************************************/
//	LG1702_WRITE(chip,0x0008,0x09);
//	LG1702_WRITE(chip,0x0009,0x04);
/**********************************************/
//	LG1702_WRITE(chip,0x000A,0x0D);
//	LG1702_WRITE(chip,0x000B,0x2E);
//	LG1702_WRITE(chip,0x000C,0x04);
//	LG1702_WRITE(chip,0x000D,0xDA);
//	LG1702_WRITE(chip,0x000E,0xC1);
//	LG1702_WRITE(chip,0x000F,0xB9);
//	LG1702_WRITE(chip,0x0010,0x28);
/**********************************************/
//	LG1702_WRITE(chip,0x0016,0x00);
//	LG1702_WRITE(chip,0x0017,0x00);
//	LG1702_WRITE(chip,0x0018,0x00);
//	LG1702_WRITE(chip,0x0019,0x33);
//	LG1702_WRITE(chip,0x0029,0xC4);
/**********************************************/
	LG1702_WRITE(chip,0x0032,0x0C);
	LG1702_WRITE(chip,0x0033,0x00);
	LG1702_WRITE(chip,0x0034,0x0C);
	LG1702_WRITE(chip,0x0036,0x00);
	LG1702_WRITE(chip,0x0037,0x0D);
	LG1702_WRITE(chip,0x0038,0x00);
	LG1702_WRITE(chip,0x0039,0x00);
	LG1702_WRITE(chip,0x003A,0x00);
	LG1702_WRITE(chip,0x003B,0x04);
	LG1702_WRITE(chip,0x003C,0x00);
	LG1702_WRITE(chip,0x003D,0x00);
	LG1702_WRITE(chip,0x0040,VIDEO_IS_NTSC(std) ? 0xE8:0xE9);
	LG1702_WRITE(chip,0x0042,VIDEO_IS_NTSC(std) ? 0x38:0x2A);
	LG1702_WRITE(chip,0x0043,VIDEO_IS_NTSC(std) ? 0x1F:0x1F);
	LG1702_WRITE(chip,0x0045,VIDEO_IS_NTSC(std) ? 0x21:0x2A);
	LG1702_WRITE(chip,0x0046,VIDEO_IS_NTSC(std) ? 0xF0:0x09);
	LG1702_WRITE(chip,0x0047,VIDEO_IS_NTSC(std) ? 0x7C:0x8A);
	LG1702_WRITE(chip,0x0048,VIDEO_IS_NTSC(std) ? 0x1F:0xCB);
	LG1702_WRITE(chip,0x0049,0xE3);
	LG1702_WRITE(chip,0x004A,0x00);
	LG1702_WRITE(chip,0x0053,0x08);
	LG1702_WRITE(chip,0x0060,0xAA);
/**********************************************/
//	LG1702_WRITE(chip,0x0070,0x20);
//	LG1702_WRITE(chip,0x0075,0x9E);
//	LG1702_WRITE(chip,0x0076,0x00);
//	LG1702_WRITE(chip,0x007A,0x9A);
//	LG1702_WRITE(chip,0x007B,0x06);
//	LG1702_WRITE(chip,0x007C,0x0C);
/**********************************************/

	LG1702_WRITE(chip,0xF002,0x47);
	LG1702_WRITE(chip,0xF0A0,0x39);
	LG1702_WRITE(chip,0xF080,0x23);
	LG1702_WRITE(chip,0xF000,VIDEO_IS_NTSC(std) ? 0x00:0x32);
	LG1702_WRITE(chip,0xF001,VIDEO_IS_NTSC(std) ? 0x01:0x00);
	LG1702_WRITE(chip,0xF003,VIDEO_IS_NTSC(std) ? 0x00:0x02);
	LG1702_WRITE(chip,0xF004,VIDEO_IS_NTSC(std) ? 0xDD:0xDC);
	LG1702_WRITE(chip,0xF007,VIDEO_IS_NTSC(std) ? 0x61:0x64);
	LG1702_WRITE(chip,0xF00C,VIDEO_IS_NTSC(std) ? 0x8A:0x7C);
	LG1702_WRITE(chip,0xF013,0xD8);
	LG1702_WRITE(chip,0xF014,0x72);
	LG1702_WRITE(chip,0xF018,VIDEO_IS_NTSC(std) ? 0x21:0x2A);
	LG1702_WRITE(chip,0xF019,VIDEO_IS_NTSC(std) ? 0xF0:0x09);
	LG1702_WRITE(chip,0xF01A,VIDEO_IS_NTSC(std) ? 0x7C:0x8A);
	LG1702_WRITE(chip,0xF01B,VIDEO_IS_NTSC(std) ? 0x1F:0xCB);
	LG1702_WRITE(chip,0xF02E,VIDEO_IS_NTSC(std) ? 0x7A:0x7E);
	LG1702_WRITE(chip,0xF030,VIDEO_IS_NTSC(std) ? 0x26:0x28);
	LG1702_WRITE(chip,0xF031,VIDEO_IS_NTSC(std) ? 0x60:0xC0);
	LG1702_WRITE(chip,0xF092,VIDEO_IS_NTSC(std) ? 0x00:0x00);
	LG1702_WRITE(chip,0xF0B0,VIDEO_IS_NTSC(std) ? 0x3A:0x38);
	LG1702_WRITE(chip,0xF0B1,VIDEO_IS_NTSC(std) ? 0xF3:0xD3);
	LG1702_WRITE(chip,0xF0B2,VIDEO_IS_NTSC(std) ? 0x58:0x58);
	LG1702_WRITE(chip,0xF0B5,VIDEO_IS_NTSC(std) ? 0x0C:0x08);
	LG1702_WRITE(chip,0xF0C7,0x04);
	LG1702_WRITE(chip,0xF1A7,0xBD);
	LG1702_WRITE(chip,0xF2B3,0x00);
	LG1702_WRITE(chip,0xF2B4,VIDEO_IS_NTSC(std) ? 0x0E:0x0F);
	LG1702_WRITE(chip,0xF2B5,VIDEO_IS_NTSC(std) ? 0xE9:0xC0);
	LG1702_WRITE(chip,0xF2BC,0x02);

	LG1702_WRITE(chip,0xF03F,0x01);
	LG1702_WRITE(chip,0xF03F,0x00);
	LG1702_WRITE(chip,0x0006,0x1F);
	LG1702_WRITE(chip,0x0006,0x00);
	LG1702_WRITE(chip,0x0006,0x00); // soft reset
}

#define LG1702_SR_BGN_REG 0x000A
#define LG1702_SR_END_REG 0x0010
static unsigned char lg1702_sr_table[SAMPLE_RATE_BUTT][LG1702_SR_END_REG-LG1702_SR_BGN_REG+1] = {
	{ 0x0D, 0x2E, 0x04, 0xDA, 0xC1, 0xB9, 0x28 },
	{ 0x0D, 0x2E, 0x09, 0xB5, 0x83, 0x73, 0x0D },
	{ 0x0D, 0x2E, 0x13, 0x6B, 0x06, 0xE7, 0x00 },
	{ 0x07, 0x52, 0x1A, 0xC2, 0xB2, 0x50, 0x00 },
	{ 0x04, 0x64, 0x1D, 0x20, 0x8A, 0x5A, 0x04 }
};

static int lg1702_set_audio_samplerate(int chip, audio_samplerate_t samplerate)
{
	int i;

	if(samplerate >= SAMPLE_RATE_BUTT) {
		return -1;
	}

	//step.1: stop audio;
	//step.2: change sample rate configuration.
	for(i = 0; i < LG1702_SR_END_REG-LG1702_SR_BGN_REG+1; i ++) {
		LG1702_WRITE(chip, i+LG1702_SR_BGN_REG, lg1702_sr_table[samplerate][i]);
	}

	//step.3: sync audio device configuration.
 
    return 0;
}

static void lg1702_set_audio_format(int chip, audio_format_t audio_format)
{
    unsigned char temp;

    /* Define the digital serial audio data format for record
        and mixing audio on the ACLKR, ASYNR, ADATR and ADATM pin.*/
    temp  = LG1702_READ(chip, 0x0075);
    temp &= ~0x40;/* [6]*/
    temp |= (0==audio_format.format) ? 0 : 0x40;/* 0:I2S format 1:DSP format */
    LG1702_WRITE(chip, 0x0075, temp);

    /* Define the digital serial audio data format for
        playback audio on the ACLKP, ASYNP and ADATP pin.*/
    temp  = TW2866_BYTE_READ(chip, 0x007a);
    temp &= ~0x40;/* [5]*/
    temp |= (0==audio_format.format) ? 0x0 : 0x40; /* 0:I2S format 1:DSP format */
    LG1702_WRITE(chip, 0x007a, temp);
}

static void lg1702_audio_init(int chip)
{
	int temp = 0;

    /* Analog Power Down Control */
	temp  = LG1702_READ(chip,0x0001);
	temp |= 0xE0; //Enable PLL & ADC & DAC
    LG1702_WRITE(chip,0x0001,temp);

	temp  = LG1702_READ(chip,0x0005);
	if(!chip) {  //Chip0 Would Be Master Mode.
		temp &= 0xCF; //Enable ACLKR & ACLKP Output.
	}
	else {
		temp |= ~0xCF; //Disable ACLKR & ACLKP Output.
	}
	LG1702_WRITE(chip,0x0005,temp);

	/* Audio Clock Inv. Config */
	if(!chip) {  //Chip0 Would Be Master Mode.
		LG1702_WRITE(chip,0x0008,0x09); //0x00001001  //Master,Inv.
		LG1702_WRITE(chip,0x0009,0x03); //0x00000011  //Master,Inv.
	}
	else {
		LG1702_WRITE(chip,0x0008,0x0A); //0x00001010  //Slave,NoInv.
		LG1702_WRITE(chip,0x0009,0x05); //0x00000101  //Slave,NoInv.
	}

    /* Analog Audio Input Gain */
	/* Channel 1-5 PGA gain Control */
//	LG1702_WRITE(chip, 0x0016, 0x00);
//	LG1702_WRITE(chip, 0x0017, 0x00);
//	LG1702_WRITE(chip, 0x0018, 0x00);

	/* Audio ADC Channel 1-5 Digital Gain */
//	LG1702_WRITE(chip, 0x001A, 0x80);
//	LG1702_WRITE(chip, 0x001B, 0x80);
//	LG1702_WRITE(chip, 0x001C, 0x80);
//	LG1702_WRITE(chip, 0x001D, 0x80);
//	LG1702_WRITE(chip, 0x001E, 0x80);

	/* Audio ADC ref. level */
    LG1702_WRITE(chip, 0x0019, 0x33);

	/* ADC Working Mode */
    LG1702_WRITE(chip, 0x0029, 0xc4);

	/* Audio Interface Configuration */
	if(!chip) {  //Chip0 Would Be Master Mode.
		LG1702_WRITE(chip, 0x0070, 0xA1); //CH5:4,16Bit PCM
		LG1702_WRITE(chip, 0x0071, 0x64); //CH4:3,CH3:2
		LG1702_WRITE(chip, 0x0072, 0x20); //CH2:1,CH1:0
	}
	else {
		LG1702_WRITE(chip, 0x0070, 0x91); //CH5:4,16Bit PCM
		LG1702_WRITE(chip, 0x0071, 0x75); //CH4:3,CH3:2
		LG1702_WRITE(chip, 0x0072, 0x31); //CH2:1,CH1:0
	}

	/* ASYNC_REC atxclk period = a_end_of_tx_cnt+1 */
//	LG1702_WRITE(chip, 0x0073, 0); //atxclk High byte
//	LG1702_WRITE(chip, 0x0074, 16*2-1); //atxclk Low byte
	/* ASYNC_PB atrclk period = a_end_of_rx_cnt+1 */
//	LG1702_WRITE(chip, 0x0078, 0); //arxclk High byte
//	LG1702_WRITE(chip, 0x0079, 16*1-1); //arxclk Low byte

	/* Audio Output Mode */
//	LG1702_WRITE(chip, 0x0076, 0x01); //16Bit PCM

	/* Audio PlackBack & Loop Mode */
	LG1702_WRITE(chip, 0x0077, 0x00); //0x00000000
	LG1702_WRITE(chip, 0x007B, 0x06); //PlayBack Audio

	/* Audio Filter Working Mode */
	LG1702_WRITE(chip, 0x007C, 0xc0); //Filters

	/* Audio Ouput */
//	LG1702_WRITE(chip, 0x007D, 0x00); //Mixer Bits
//	LG1702_WRITE(chip, 0x007E, 0x00); //0x00100000
//	LG1702_WRITE(chip, 0x007F, 0x00); //Enable Auto Mute Channel
//	LG1702_WRITE(chip, 0x0081, 0x00); //Period for Sample Clock & Mute Detection
//	LG1702_WRITE(chip, 0x0082, 0xc0); //Output Gain

    /* Enable I2S Format, 16Bit, MSB First, Master, Delay */
	if(!chip) {  //Chip0 Would Be Master Mode.
		LG1702_WRITE(chip, 0x0075, 0x9e); //0x10011110
		LG1702_WRITE(chip, 0x007A, 0x9e); //0x10011110
	}
	else {
		LG1702_WRITE(chip, 0x0075, 0x98); //0x10011000
		LG1702_WRITE(chip, 0x007A, 0x9e); //0x10011000
	}

    lg1702_set_audio_samplerate(chip, SAMPLE_RATE_8000);

#if 1
	/* RESET AUDIO DEVICE */
	temp  = LG1702_READ(chip,0x0006);
	temp |= 0x07;
	temp  = LG1702_WRITE(chip,0x0006, temp);

	/* DISABLE SOFT RESET */
	LG1702_WRITE(chip,0x0006,0x00);
	LG1702_WRITE(chip,0x0006,0x00);
#endif
}

static void lg1702_audio_set_liveloop(int chip, int channel)
{
	if(chip < 0 || chip > 4){
		return;
	}
	if(channel >= 0 && channel < 4){
        LG1702_WRITE(chip, 0x007B, channel); //PlayBack Audio
	}
}

static void lg1702_audio_set_playback(int chip, int channel)
{
	if(chip < 0 || chip > 4){
		return;
	}
    LG1702_WRITE(chip, 0x007B, 0x06); //PlayBack Audio
}
