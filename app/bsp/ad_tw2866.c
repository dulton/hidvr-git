
#define TW2866_CHN_PER_CHIP (4)

#define TW2866_BYTE_WRITE(chip,addr,data) gpio_i2c_write(TW2866_DEV[chip], addr, data)
#define TW2866_BYTE_READ(chip,addr) gpio_i2c_read(TW2866_DEV[chip], addr)

static unsigned char tw2866_check(int chip)
{
	unsigned char id = TW2866_BYTE_READ(chip, 0xff);
	id >>= 3;
	id &= 0x1f;
	return (0x19 == id);
}

static void tw2866_set_hue(int chip, int chn, unsigned char gain)
{
	const unsigned char HUE_MAP[] = VIDEO_COLOR_ADJTBL0;
	if(!(chn < TW2866_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	TW2866_BYTE_WRITE(chip, 0x06 + 0x10 * chn, HUE_MAP[gain]);
}

static void tw2866_set_contrast(int chip, int chn, unsigned char gain)
{
	const unsigned char CONTRAST_MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < TW2866_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	TW2866_BYTE_WRITE(chip, 0x02 + 0x10 * chn, CONTRAST_MAP[gain]);
}

static void tw2866_set_brightness(int chip, int chn, unsigned char gain)
{
	const unsigned char BRIGHTNESS_MAP[] = VIDEO_COLOR_ADJTBL0;

	if(!(chn < TW2866_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	TW2866_BYTE_WRITE(chip, 0x01 + 0x10 * chn, BRIGHTNESS_MAP[gain]);
}

static void tw2866_set_saturation(int chip, int chn, unsigned char gain)
{
	const unsigned char SATURATION_MAP[] = VIDEO_COLOR_ADJTBL1;

	if(!(chn < TW2866_CHN_PER_CHIP)){
		return;
	}
	if(gain > VIDEO_MAX_COLOR_GAIN){
		gain = VIDEO_MAX_COLOR_GAIN;
	}
	TW2866_BYTE_WRITE(chip, 0x04 + 0x10 * chn, SATURATION_MAP[gain]);
	TW2866_BYTE_WRITE(chip, 0x05 + 0x10 * chn, SATURATION_MAP[gain]);
}

static int tw2866_set_mode_4d1(int chip)
{
	/* Video Channel Output Control*/
	/* Four Channel ITU0R BT.656 Time-multiplexed format output*/
	TW2866_BYTE_WRITE(chip, 0xca, 0xaa);
	/* Output Enable Control and Clock Output Control */
	/* 108MHz clock output*/
	TW2866_BYTE_WRITE(chip, 0xfa, 0x4a);
	printk("tw2866(%x) set 4d1 mode\r\n", chip);
	return 0;
}

static int tw2866_set_mode_2d1(int chip)
{
	/* Video Channel Output Control*/
	/* Two Channel ITU0R BT.656 Time-multiplexed format output*/
	TW2866_BYTE_WRITE(chip, 0xca, 0x55);
	/* Output Enable Control and Clock Output Control */
	/* 54MHz clock output*/
	TW2866_BYTE_WRITE(chip, 0xfa, 0x45);/* CLKNO1 pin/CLKPO1 pin */
	TW2866_BYTE_WRITE(chip, 0x6a, 0x05);/* CLKNO2 pin/CLKPO2 pin */
	printk("tw2866(%x) set 2d1 mode\r\n", chip);
	return 0;
}

static int tw2866_set_standard(int chip, const char* std)
{
	int i = 0;
	udelay(50);
	if(!strncasecmp(std, "PAL", strlen("PAL"))){
		for (i = 0; i < TW2866_CHN_PER_CHIP; ++i){
	            TW2866_BYTE_WRITE(chip, 0x0e + 0x10*i, 0x01);/* PAL (B,D,G,H,I) */
	            TW2866_BYTE_WRITE(chip, 0x07 + 0x10*i, 0x12);/* Cropping Register, High */
	            TW2866_BYTE_WRITE(chip, 0x08 + 0x10*i, 0x17);/* Vertical Delay Register, Low  */
	            TW2866_BYTE_WRITE(chip, 0x09 + 0x10*i, 0x20);/* Vertical Active Register, Low */
	            TW2866_BYTE_WRITE(chip, 0x0a + 0x10*i, 0x0c);/* Horizontal Delay Register, Low */
	            TW2866_BYTE_WRITE(chip, 0x0b + 0x10*i, 0xd0);/* Horizontal Active Register, Low */
		}
		TW2866_BYTE_WRITE(chip, 0x41, 0xd4);/* PAL-B -->  HZ50:1 FSCSEL:1 PHALT:1 PED:0 */
		printk("tw2866(%x) set to PAL\r\n", chip);
	}else if(!strncasecmp(std, "NTSC", strlen("NTSC"))){
		for (i = 0; i < TW2866_CHN_PER_CHIP; ++i){
			TW2866_BYTE_WRITE(chip, 0x0e + 0x10*i, 0x00);/* NTSC(M) */
			TW2866_BYTE_WRITE(chip, 0x07 + 0x10*i, 0x02);/* Cropping Register, High */
			TW2866_BYTE_WRITE(chip, 0x08 + 0x10*i, 0x15);/* Vertical Delay Register, Low  */
			TW2866_BYTE_WRITE(chip, 0x09 + 0x10*i, 0xf0);/* Vertical Active Register, Low */
			TW2866_BYTE_WRITE(chip, 0x0a + 0x10*i, 0x0c);/* Horizontal Delay Register, Low */
			TW2866_BYTE_WRITE(chip, 0x0b + 0x10*i, 0xd0);/* Horizontal Active Register, Low */			
		}
		TW2866_BYTE_WRITE(chip, 0x41, 0x41);/* NTSC-M -->  HZ60:0 FSCSEL:0 PHALT:0 PED:0 */
		printk("tw2866(%x) set to NTSC\r\n", chip);
	}else if(!strncasecmp(std, "AUTO", strlen("AUTO"))){
		printk("tw2866(%x) not support AUTO mode\r\n", chip);
		return -1;
	}else{
		printk("tw2866(%x) not support this video norm: %s\n", chip, std);
		return -1;
	}
	udelay(50);
	return 0;
}

static unsigned int tw2866_get_loss(int chip)
{
	unsigned int reg = 0;
	unsigned int ret_bit = 0;
	int i = 0;	
	for(i = 0; i < TW2866_CHN_PER_CHIP; ++i){
		reg = TW2866_BYTE_READ(chip, 0x00 + i * 0x10);
		if(reg & (1 << 7)){
			ret_bit |= (1<<i);
		}else{
			ret_bit &= ~(1<<i);
		}
	}
	return ret_bit;
}
#if 0
static void tw2866_comm_init(int chip)
{
	unsigned char id = 0;

	id = TW2866_BYTE_READ(chip, 0xff);
	if (0xff == id || 0x00 == id)
	{
		printk(" tw2866(%x) id(%x) error\r\n", chip, id);
		return;
	}

	// Video Software Reset
	TW2866_BYTE_WRITE(chip, 0x80, 0x1f);
	//Clock PLL Control
	//2x XTI input frequency.54MHz clock input mode
	TW2866_BYTE_WRITE(chip, 0x60, 0x05);
	//108MHz Clock Select
	// Analog Clock PLL 108MHz output is used for all  system clock source
	TW2866_BYTE_WRITE(chip, 0x61, 0x03);
	// Enable Video Detection
	TW2866_BYTE_WRITE(chip, 0xfc, 0x0f);
}
#endif
static void tw2866_video_init(int chip, const char* std)
{
	int i;
	for (i = 0; i < TW2866_CHN_PER_CHIP; ++i){
		TW2866_BYTE_WRITE(chip, 0x01 + 0x10*i, 0x00);/* brightness. */
		TW2866_BYTE_WRITE(chip, 0x02 + 0x10*i, 0x64);/* luminance contrast gain */
		TW2866_BYTE_WRITE(chip, 0x03 + 0x10*i, 0x11);/* sharpness */
		TW2866_BYTE_WRITE(chip, 0x04 + 0x10*i, 0x80);/* Chroma (U) */
		TW2866_BYTE_WRITE(chip, 0x05 + 0x10*i, 0x80);/* Chroma (V) */
		TW2866_BYTE_WRITE(chip, 0x06 + 0x10*i, 0x00);/* hue */
	}

	tw2866_set_standard(chip, std);
	TW2866_BYTE_WRITE(chip, 0x43, 0x08);/* YDEL = 8h */ 

	/* CLKPO,CLKPN, 4*/
	/* Output Enable Control and Clock Output Control */
	TW2866_BYTE_WRITE(chip, 0xfa, 0x4a);/* CLKN O1 108M */ 
    
	/* CHID*/
	TW2866_BYTE_WRITE(chip, 0x9e, 0x52);    
    
	/* Video Channel Output Control */
	TW2866_BYTE_WRITE(chip, 0xca, 0xaa);/* Four Channel ITU0R BT.656 Time-multiplexed */

	/* Video AGC Control */
	TW2866_BYTE_WRITE(chip, 0xaa, 0x00);/* */

	/* Video Miscellaneous Function Control*/
	TW2866_BYTE_WRITE(chip, 0xf9, 0x51);

	/* Miscellaneous Control II */
	TW2866_BYTE_WRITE(chip, 0x96, 0xe6);/* Blue color, else set to 0xe0 */

	TW2866_BYTE_WRITE(chip, 0xaf, 0x22);
	TW2866_BYTE_WRITE(chip, 0xb0, 0x22);

	/* Enable video encoder  */
	TW2866_BYTE_WRITE(chip, 0x41, 0xd4);
	TW2866_BYTE_WRITE(chip, 0x43, 0x00);
	TW2866_BYTE_WRITE(chip, 0x4b, 0x00);

	/* For Hi3515 Demo board 2D1 mode */
	TW2866_BYTE_WRITE(chip, 0xcc, 0x0d);
	TW2866_BYTE_WRITE(chip, 0xcd, 0x08);
}

static int tw2866_set_audio_samplerate(int chip, audio_samplerate_t samplerate)
{
    int temp, val;
    
    switch (samplerate)
    {
        case SAMPLE_RATE_8000:
            val = 0x00;
            break;
        case SAMPLE_RATE_16000:
            val = 0x01;
            break;
        case SAMPLE_RATE_32000:
            val = 0x02;
            break;
        case SAMPLE_RATE_44100:
            val = 0x03;
            break;
        case SAMPLE_RATE_48000:
            val = 0x04;
            break;
        default:
            printk("not support this samplerate:%d \n", samplerate);
            return -1;
    }
    
    /* NOTES:320xfs mode: AIN5MD=1,AFS384=0. 44.1kHz/48kHz are not supported */
    
    /* ACKI control is automatically set up by AFMD register values [3] */
    temp = TW2866_BYTE_READ(chip, 0x70);
    temp &= ~0x08;
    temp |= 0x1<<3;
    TW2866_BYTE_WRITE(chip,  0x70, temp);

    /* AFAUTO control mode 0x70 [2:0] */
    temp = TW2866_BYTE_READ(chip, 0x70);
    temp &= ~0x07;
    temp |= val;
    TW2866_BYTE_WRITE(chip,  0x70, temp);

    if (SAMPLE_RATE_8000 == samplerate)
    {
        TW2866_BYTE_WRITE(chip, 0xE1, 0xc0);
        TW2866_BYTE_WRITE(chip, 0xE2, 0xaa);
        TW2866_BYTE_WRITE(chip, 0xE3, 0xaa);
    }
    else 
    {
        TW2866_BYTE_WRITE(chip, 0xE1, 0xe0);
        TW2866_BYTE_WRITE(chip, 0xE2, 0xbb);
        TW2866_BYTE_WRITE(chip, 0xE3, 0xbb);
    }
    
    //printk("tw2866 audio samplerate set to %d ok\n", val);
    return 0;
}

static void tw2866_set_audio_format(int chip, audio_format_t audio_format)
{
    unsigned char temp;

    /* Define the digital serial audio data format for record
        and mixing audio on the ACLKR, ASYNR, ADATR and ADATM pin.*/
    temp = TW2866_BYTE_READ(chip, 0xd2);
    temp &= 0xbf;/* [6]*/
    temp |= (0==audio_format.format) ? 0 : 0x40;/* 0:I2S format 1:DSP format */
    TW2866_BYTE_WRITE(chip, 0xd2, temp);

    /* Define the digital serial audio data format for
        playback audio on the ACLKP, ASYNP and ADATP pin.*/
    temp = TW2866_BYTE_READ(chip, 0xdb);
    temp &= 0xf7;/* [3]*/
    temp |= (0==audio_format.format) ? 0x0 : 0x8; /* 0:I2S format 1:DSP format */
    TW2866_BYTE_WRITE(chip, 0xdb, temp);
}

static void tw2866_audio_init(int chip)
{
    /* Analog Power Down Control */
    //TW2866_BYTE_WRITE(chip, 0xCE, 0x40 );

    /* 0xD0, 0xD1, 0x7F - Analog Audio Input Gain */
	TW2866_BYTE_WRITE(chip, 0x7f, 0x80);/* [7:4] AIGAIN5 [3:0] MIXRATIO5 */
    TW2866_BYTE_WRITE(chip, 0xD0, 0x88);/* [7:4] AIGAIN2 [3:0] AIGAIN1 */
    TW2866_BYTE_WRITE(chip, 0xD1, 0x88);/* [7:4] AIGAIN4 [3:0] AIGAIN3 */

    /* Number of Audio to be Recorded */ 
    TW2866_BYTE_WRITE(chip, 0xD2, 0x01);/* recode: I2S, 4 chn */

    /* 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA ¨C Sequence of Audio to be Recorded*/
    TW2866_BYTE_WRITE(chip, 0xD3, 0x10);/* default value: 1 0 */
    TW2866_BYTE_WRITE(chip, 0xD7, 0x32);/* default value: 9 8 */

    /* playback: I2S, master, 16bit, ACLKR pin is output */
    TW2866_BYTE_WRITE(chip, 0xDB, 0xE1);
    
    /* u-Law/A-Law Output and Mix Mute Control */
    TW2866_BYTE_WRITE(chip, 0xDC, 0x00);/* PCM output */

    /* Mix Output Selection */
    TW2866_BYTE_WRITE(chip, 0xE0, 0x10);/* Select playback audio of the first stage chips*/
    
    /* Audio Detection Threshold, 8K typical setting value */
    TW2866_BYTE_WRITE(chip, 0xE1, 0xc0);
    TW2866_BYTE_WRITE(chip, 0xE2, 0xaa);
    TW2866_BYTE_WRITE(chip, 0xE3, 0xaa);
    
    /* Audio Clock Increment, ACKI[21:0]: 09B583h for fs = 8kHz is default. */
    TW2866_BYTE_WRITE(chip, 0xF0, 0x83);/* ACKI[7:0] */
    TW2866_BYTE_WRITE(chip, 0xF1, 0xb5);/* ACKI[15:8] */
    TW2866_BYTE_WRITE(chip, 0xF2, 0x09);/* ACKI[21:16] */        

    /* [3] ACKI control is automatically set up by AFMD register values */
    /* [2:0] AFAUTO control mode. 0: 8kHz setting(default). */
	TW2866_BYTE_WRITE(chip, 0x70, 0x08);/* [2:0] 0:8k, 1:16k, 2:32k, 3:44.1k 4:48k */

    /* Audio Clock Control */
    TW2866_BYTE_WRITE(chip, 0xF8, 0xc4);/* bit2: Loop open in ACKG block */

    /* Enable state register updating and interrupt request of audio AIN5 detection for each input*/
	TW2866_BYTE_WRITE(chip, 0x73, 0x01);
    
    /* ADATM I2S Output Select (default value)*/
    TW2866_BYTE_WRITE(chip, 0x7B, 0x15);/* Select record audio of channel 51(AIN51)*/
    TW2866_BYTE_WRITE(chip, 0x7C, 0x15);/* Select record audio of channel 51(AIN51)*/
    /* MIX_MUTE_A5 ?????? */
    TW2866_BYTE_WRITE(chip, 0x7E, 0xc0);
    /* Audio Fs Mode Control */
    TW2866_BYTE_WRITE(chip, 0x89, 0x05);/* AIN5MD=1, AFS384=0 */
}


static void tw2866_audio_cascade(int chip)
{
    unsigned char temp;
    
    /* cascade */
    TW2866_BYTE_WRITE(chip, 0x82, 0x00);
   
    /* the number of audio for record on the ADATR pin */
    temp = TW2866_BYTE_READ(chip, 0xd2);
    temp &= 0xfc;/* [1:0]*/
    temp |= 0x3;/* 0x3 16chn */
    TW2866_BYTE_WRITE(chip, 0xd2, temp);
        
    /* SMD = b'01 */
    temp = TW2866_BYTE_READ(chip, 0xcf);
    temp &= 0x3f;/* [7:6]*/
    temp |= 0x80;/* 0x10 */ /* not 0x01*/
    TW2866_BYTE_WRITE(chip, 0xcf, temp);
        
    /* set PB_MASTER ???? */
    temp = TW2866_BYTE_READ(chip, 0xdb);
    temp &= 0xfc;
    temp |= 0x01;
    TW2866_BYTE_WRITE(chip, 0xdb, temp); 

    /* Sequence of Audio to be Recorded */
    TW2866_BYTE_WRITE(chip, 0xD3, 0x10);
    TW2866_BYTE_WRITE(chip, 0xD4, 0x32);
    TW2866_BYTE_WRITE(chip, 0xD5, 0x54);
    TW2866_BYTE_WRITE(chip, 0xD6, 0x76);
    TW2866_BYTE_WRITE(chip, 0xD7, 0x98);
    TW2866_BYTE_WRITE(chip, 0xD8, 0xBA);
    TW2866_BYTE_WRITE(chip, 0xD9, 0xDC);
    TW2866_BYTE_WRITE(chip, 0xDA, 0xFE); 

    /*the amplifier gain for analog audio output, default 0x80*/
    TW2866_BYTE_WRITE(chip, 0xDF, 0x80); 
        
    /* Mix Ratio Value */
    //TW2866_BYTE_WRITE(chip, 0xDD, 0x33); 
    //TW2866_BYTE_WRITE(chip, 0xDE, 0x33); 
    //TW2866_BYTE_WRITE(chip, 0xDF, 0x33); 
}
