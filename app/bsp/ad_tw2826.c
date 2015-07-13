
#define TW2826X_I2C_ADDR  0x58

#define TW2868A_I2C_ADDR  0x50
#define TW2868B_I2C_ADDR  0x52
#define TW2868C_I2C_ADDR  0x54
#define TW2868D_I2C_ADDR  0x56

const unsigned char TW2868_IC[1][2] = {
    {TW2868A_I2C_ADDR, TW2868B_I2C_ADDR},
//    {TW2868C_I2C_ADDR, TW2868D_I2C_ADDR}
};

#define tw2826_byte_read  gpio_i2c_read
#define tw2826_byte_write gpio_i2c_write

static unsigned char tw2826_check(int chip)
{
    unsigned char tmpAddr = TW2826_DEV[chip];
    
    if(TW2826X_I2C_ADDR == tmpAddr) {
        return gpio_i2c_probe(tmpAddr);
    }

    return 0;
}

int tw2868_byte_read(
    int Grp,
    unsigned char addr,
    unsigned char page,
    unsigned char reg)
{
    gpio_i2c_writeX(Grp, addr, 0x40, page);
    return gpio_i2c_readX(Grp, addr, reg);
}

void tw2868_byte_write(
    int Grp,
    unsigned char addr,
    unsigned char page,
    unsigned char reg,
    unsigned char data)
{
    gpio_i2c_writeX(Grp, addr, 0x40, page);
    gpio_i2c_writeX(Grp, addr, reg,  data);
}

static void tw2868_video_SetMode(int Grp, unsigned char addr, const char *std)
{
    int i,j;

    udelay(50);

    if (VIDEO_IS_PAL(std)) {
        for (i=0; i<2; i++) {
	        for (j=0; j<4; j++) {
	            tw2868_byte_write(Grp, addr, i, 0x0e + 0x10*j, 0x01);/* PAL (B,D,G,H,I) */
	            //tw2868_byte_write(addr, 0x0e + 0x10*i, 0x07);/* Auto detection */

	            tw2868_byte_write(Grp, addr, i, 0x07 + 0x10*j, 0x12);/* Cropping Register, High */
	            tw2868_byte_write(Grp, addr, i, 0x08 + 0x10*j, 0x17);/* Vertical Delay Register, Low  */
	            tw2868_byte_write(Grp, addr, i, 0x09 + 0x10*j, 0x20);/* Vertical Active Register, Low */
	            tw2868_byte_write(Grp, addr, i, 0x0a + 0x10*j, 0x0c);/* Horizontal Delay Register, Low */
	            tw2868_byte_write(Grp, addr, i, 0x0b + 0x10*j, 0xd0);/* Horizontal Active Register, Low */                        
	        }
      	}

        tw2868_byte_write(Grp, addr, 0x00, 0x41, 0xd4);/* PAL-B -->  HZ50:1 FSCSEL:1 PHALT:1 PED:0 */
        printk("tw2868 0x%x set to PAL mode ok! \n", addr);
    }
    else {
        for (i=0; i<2; i++) {
	        for (j=0; j<4; j++) {
	            tw2868_byte_write(Grp, addr, i, 0x0e + 0x10*j, 0x00);/* NTSC(M) */
	            //tw2868_byte_write(Grp, addr, 0x0e + 0x10*i, 0x07);/* Auto detection */

	            tw2868_byte_write(Grp, addr, i, 0x07 + 0x10*j, 0x02);/* Cropping Register, High */
	            tw2868_byte_write(Grp, addr, i, 0x08 + 0x10*j, 0x15);/* Vertical Delay Register, Low  */
	            tw2868_byte_write(Grp, addr, i, 0x09 + 0x10*j, 0xf0);/* Vertical Active Register, Low */
	            tw2868_byte_write(Grp, addr, i, 0x0a + 0x10*j, 0x0c);/* Horizontal Delay Register, Low */
	            tw2868_byte_write(Grp, addr, i, 0x0b + 0x10*j, 0xd0);/* Horizontal Active Register, Low */                                    
	        }
      	}
        
        tw2868_byte_write(Grp, addr, 0x00, 0x41, 0x40);/* NTSC-M -->  HZ50:0 FSCSEL:0 PHALT:0 PED:0 */
        printk("tw2868 0x%x set to NTSC mode ok! \n", addr);
    }

    udelay(50);
}

static void tw2868_video_Init(int Grp, unsigned char addr, const char *std)
{
    int i,j;

	for (i=0; i<2; i++) {
	    for (j=0; j<4; j++) {
	        tw2868_byte_write(Grp, addr, i, 0x01 + 0x10*j, 0x00);/* brightness. */
	        tw2868_byte_write(Grp, addr, i, 0x02 + 0x10*j, 0x64);/* luminance contrast gain */
	        tw2868_byte_write(Grp, addr, i, 0x03 + 0x10*j, 0x11);/* sharpness */
	        tw2868_byte_write(Grp, addr, i, 0x04 + 0x10*j, 0x80);/* Chroma (U) */
	        tw2868_byte_write(Grp, addr, i, 0x05 + 0x10*j, 0x80);/* Chroma (V) */
	        tw2868_byte_write(Grp, addr, i, 0x06 + 0x10*j, 0x00);/* hue */
	    }
  	}

    tw2868_video_SetMode(Grp, addr, std);

    tw2868_byte_write(Grp, addr, 0x00, 0x43, 0x08);/* YDEL = 8h */ 

    /* CLKPO,CLKPN, 4*/
  	/* Output Enable Control and Clock Output Control */
    tw2868_byte_write(Grp, addr, 0x00, 0xfa, 0x4a);/* CLKN O1 108M */ 
    tw2868_byte_write(Grp, addr, 0x00, 0x60, 0x15); //Input 27MHz
    //tw2868_byte_write(addr, 0x00, 0x61, 0x13); //Input 27MHz
    /* CHID*/
    tw2868_byte_write(Grp, addr, 0x00, 0x9e, 0x52);    

    /* Video Channel Output Control */
    tw2868_byte_write(Grp, addr, 0x01, 0x60, 0xaa);/* Four Channel ITU0R BT.656 Time-multiplexed */
    tw2868_byte_write(Grp, addr, 0x01, 0x61, 0x10);
    tw2868_byte_write(Grp, addr, 0x01, 0x62, 0x32);
    tw2868_byte_write(Grp, addr, 0x01, 0x63, 0x54);
    tw2868_byte_write(Grp, addr, 0x01, 0x64, 0x76);

    /* Video AGC Control */
    tw2868_byte_write(Grp, addr, 0x00, 0xaa, 0x00);/* VIN1~4 */
    tw2868_byte_write(Grp, addr, 0x01, 0xaa, 0x00);/* VIN5~8 */

    /* Video Miscellaneous Function Control*/
    tw2868_byte_write(Grp, addr, 0x00, 0xf9, 0xd1);/* limit output ranges */

    /* Miscellaneous Control II */
    tw2868_byte_write(Grp, addr, 0x00, 0x96, 0xe6);/* Blue color, else set to 0xe0 */
    tw2868_byte_write(Grp, addr, 0x01, 0x96, 0xe6);

    tw2868_byte_write(Grp, addr, 0x00, 0xaf, 0x22);
    tw2868_byte_write(Grp, addr, 0x00, 0xb0, 0x22);
    tw2868_byte_write(Grp, addr, 0x01, 0xaf, 0x22);
    tw2868_byte_write(Grp, addr, 0x01, 0xb0, 0x22);

    /* Enable video encoder  */
    //tw2868_byte_write(addr, 0x00, 0x41, 0xd4);
    tw2868_byte_write(Grp, addr, 0x00, 0x43, 0x08);
    tw2868_byte_write(Grp, addr, 0x00, 0x4b, 0x00);
}

static void tw2826_video_SetMode(unsigned char addr, const char* std)
{
    if(VIDEO_IS_PAL(std)) {
        tw2826_byte_write(addr, 0x48, 0x40);
        tw2826_byte_write(addr, 0x49, 0x40);
        tw2826_byte_write(addr, 0x60, 0x01);
        tw2826_byte_write(addr, 0x61, 0x00);
        tw2826_byte_write(addr, 0x62, 0x1d);
        tw2826_byte_write(addr, 0x63, 0x01);
        tw2826_byte_write(addr, 0x5c, 0xc0);
        tw2826_byte_write(addr, 0x5d, 0x06);
        tw2826_byte_write(addr, 0x5e, 0x71);
        tw2826_byte_write(addr, 0x5f, 0x02);
        tw2826_byte_write(addr, 0x58, 0x00);
        tw2826_byte_write(addr, 0x59, 0x00);
        tw2826_byte_write(addr, 0x5a, 0x02);
        tw2826_byte_write(addr, 0x54, 0xb4);
        tw2826_byte_write(addr, 0x55, 0x20);
        tw2826_byte_write(addr, 0x56, 0x01);
        tw2826_byte_write(addr, 0x50, 0x9d);
        tw2826_byte_write(addr, 0x51, 0xa0);
        tw2826_byte_write(addr, 0x52, 0x0e);
        tw2826_byte_write(addr, 0x4c, 0x9c);
        tw2826_byte_write(addr, 0x4d, 0xa0);
        tw2826_byte_write(addr, 0x4e, 0x0e);
        tw2826_byte_write(addr, 0x64, 0x02);
        tw2826_byte_write(addr, 0x65, 0xc0);
        tw2826_byte_write(addr, 0x66, 0x18);
        tw2826_byte_write(addr, 0x67, 0x80);
        tw2826_byte_write(addr, 0x68, 0x38);
        tw2826_byte_write(addr, 0x69, 0x01);
        tw2826_byte_write(addr, 0x6a, 0x3a);
        tw2826_byte_write(addr, 0x6b, 0x81);
        tw2826_byte_write(addr, 0x6c, 0x51);
        tw2826_byte_write(addr, 0x6d, 0xc1);
        tw2826_byte_write(addr, 0x6e, 0x71);
        tw2826_byte_write(addr, 0x6f, 0x42);
    }
}

static void tw2826_video_init(int chip, const char* std)
{
    int i,j;
    int addr = TW2826X_I2C_ADDR;
    tw2826_byte_read (addr, 0x25);
    tw2826_byte_write(addr, 0x20, 0x80);	//Reset

    udelay(2000);

    tw2826_video_SetMode(addr, std);

//    tw2826_byte_write(addr, 0x23, 0x60);	//For Socket Board 0x23=0x6c
    tw2826_byte_write(addr, 0x23, 0x6c);	//For Socket Board 0x23=0x6c
    tw2826_byte_write(addr, 0x21, 0x00);

    tw2826_byte_write(addr, 0x30, 0x14); //VO0, 108MHz, 180 deg delay
    tw2826_byte_write(addr, 0x31, 0x04); //VO1, 108MHz, 180 deg delay

    //tw2826_byte_write(addr, 0x32, 0x04); //VO2, 27MHz, 180 deg delay
    //tw2826_byte_write(addr, 0x33, 0x04); //VO3, 27MHz, 180 deg delay

    tw2826_byte_write(addr, 0x34, 0xa0);  //VO0 Src0 Is ReSampled From VI0_CH0~4
    tw2826_byte_write(addr, 0x35, 0xa1);  //VO0 Src1 Is ReSampled From VI1_CH0~4
    tw2826_byte_write(addr, 0x36, 0xa2);  //VO0 Src2 Is ReSampled From VI2_CH0~4
    tw2826_byte_write(addr, 0x37, 0xa3);  //VO0 Src3 Is ReSampled From VI3_CH0~4

    tw2826_byte_write(addr, 0x38, 0xB0);  //VO1 Src0 Is ReSampled From VI0_CH0~4
    tw2826_byte_write(addr, 0x39, 0x30);  //VO1 Src1 Is ReSampled From VI1_CH0~4
    tw2826_byte_write(addr, 0x3a, 0x30);  //VO1 Src2 Is ReSampled From VI2_CH0~4
    tw2826_byte_write(addr, 0x3b, 0x30);  //VO1 Src3 Is ReSampled From VI3_CH0~4

    tw2826_byte_write(addr, 0x3c, 0xb0);

    tw2826_byte_write(addr, 0x20, 0x01);

    udelay(2000);

    for(i = 0; i < ARRAY_SIZE(TW2868_IC); i ++) {
        for(j = 0; j < ARRAY_SIZE(TW2868_IC[0]); j ++) {
            tw2868_video_Init(0, TW2868_IC[i][j], std);
            tw2868_video_Init(1, TW2868_IC[i][j], std);
            printk("tw2868 0xFA-------> 0x%02X\n", tw2868_byte_read(1, TW2868_IC[i][j], 0x00, 0xfa));
        }
    }

}

static int  tw2826_PreViewSource(int Source)
{
    if(Source >= 16 || Source < 0) {
        return 0;
    }

    tw2826_byte_write(TW2826X_I2C_ADDR, 0x38, 0xB0 + Source);  //VO1 Src0 Is ReSampled From VI0_CH0~4
    tw2826_byte_write(TW2826X_I2C_ADDR, 0x39, 0x30 + Source);  //VO1 Src1 Is ReSampled From VI1_CH0~4
    tw2826_byte_write(TW2826X_I2C_ADDR, 0x3a, 0x30 + Source);  //VO1 Src2 Is ReSampled From VI2_CH0~4
    tw2826_byte_write(TW2826X_I2C_ADDR, 0x3b, 0x30 + Source);  //VO1 Src3 Is ReSampled From VI3_CH0~4
    return !0;
}

static unsigned int tw2826_get_loss(int chip)
{
	unsigned int  ret_bit = 0;
    unsigned int  PR_Addrs[8][2] = {
        {0x00, 0x00}, {0x00, 0x10}, {0x00, 0x20}, {0x00, 0x30},
        {0x01, 0x00}, {0x01, 0x10}, {0x01, 0x20}, {0x01, 0x30}
    };

    int i, j, k;
    for(i = 0; i < ARRAY_SIZE(TW2868_IC); i ++) {
        for(j = 0; j < ARRAY_SIZE(TW2868_IC[0]); j ++) {
            for(k = 0; k < ARRAY_SIZE(PR_Addrs); k ++) {
                ret_bit <<= 1;
                if(0x80 & tw2868_byte_read(0, TW2868_IC[i][j], PR_Addrs[k][0], PR_Addrs[k][1])) {
                    ret_bit += 1;
                }
            }
        }
    }

    for(i = 0; i < ARRAY_SIZE(TW2868_IC); i ++) {
        for(j = 0; j < ARRAY_SIZE(TW2868_IC[0]); j ++) {
            for(k = 0; k < ARRAY_SIZE(PR_Addrs); k ++) {
                ret_bit <<= 1;
                if(0x80 & tw2868_byte_read(1, TW2868_IC[i][j], PR_Addrs[k][0], PR_Addrs[k][1])) {
                    ret_bit += 1;
                }
            }
        }
    }

	return ret_bit;	
}

static void tw2826_audio_init(int chip)
{
    unsigned char chip_addr = TW2868_IC[0][0];
    /* Analog Power Down Control */
    //tw2868_byte_write(chip_addr, 0xCE, 0x40 );

    /* 0xD0, 0xD1, 0x7F - Analog Audio Input Gain */
    tw2868_byte_write(0, chip_addr, 0x00, 0x7f, 0x80);/* [7:4] AIGAIN5 [3:0] MIXRATIO5 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD0, 0x33);/* [7:4] AIGAIN2 [3:0] AIGAIN1 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD1, 0x33);/* [7:4] AIGAIN4 [3:0] AIGAIN3 */
    tw2868_byte_write(0, chip_addr, 0x01, 0x7f, 0x80);
    tw2868_byte_write(0, chip_addr, 0x01, 0xD0, 0x33);
    tw2868_byte_write(0, chip_addr, 0x01, 0xD1, 0x33);

    /* Number of Audio to be Recorded */ 
    tw2868_byte_write(0, chip_addr, 0x00, 0xD2, 0x0a);/* recode: I2S, 16 chn */

    /* 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA ¨C Sequence of Audio to be Recorded*/
    tw2868_byte_write(0, chip_addr, 0x00, 0xD3, 0x10);/* default value: 1 0 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD4, 0x10);/* default value: 1 0 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD5, 0x10);/* default value: 1 0 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD6, 0x10);/* default value: 1 0 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD7, 0x10);/* default value: 1 0 */
    tw2868_byte_write(0, chip_addr, 0x00, 0xD8, 0x10);/* default value: 1 0 */

    /* playback: I2S, master, 16bit, ACLKR pin is output */
    tw2868_byte_write(0, chip_addr, 0x00, 0xDB, 0xE1);

    /* u-Law/A-Law Output and Mix Mute Control */
    tw2868_byte_write(0, chip_addr, 0x00, 0xDC, 0x00);/* PCM output */

    /* Mix Output Selection */
    tw2868_byte_write(0, chip_addr, 0x00, 0xE0, 0x10);/* Select playback audio of the first stage chips*/

    /* Audio Detection Threshold, 8K typical setting value */
    tw2868_byte_write(0, chip_addr, 0x00, 0xE1, 0xc0);
    tw2868_byte_write(0, chip_addr, 0x00, 0xE2, 0xaa);
    tw2868_byte_write(0, chip_addr, 0x00, 0xE3, 0xaa);
    tw2868_byte_write(0, chip_addr, 0x01, 0xE2, 0xaa);
    tw2868_byte_write(0, chip_addr, 0x01, 0xE3, 0xaa);

    /* Audio Clock Increment, ACKI[21:0]: 09B583h for fs = 8kHz is default. */
    //tw2868_byte_write(0, chip_addr, 0x00, 0xF0, 0x83);/* ACKI[7:0] */
    //tw2868_byte_write(0, chip_addr, 0x00, 0xF1, 0xb5);/* ACKI[15:8] */
    //tw2868_byte_write(0, chip_addr, 0x00, 0xF2, 0x09);/* ACKI[21:16] */        

    /* [3] ACKI control is automatically set up by AFMD register values */
    /* [2:0] AFAUTO control mode. 0: 8kHz setting(default). */
	tw2868_byte_write(0, chip_addr, 0x00, 0x70, 0x08);/* [2:0] 0:8k, 1:16k, 2:32k, 3:44.1k 4:48k */

    /* Audio Clock Control */
    tw2868_byte_write(0, chip_addr, 0x00, 0xF8, 0xc4);/* bit2: Loop open in ACKG block */

    /* Enable state register updating and interrupt request of audio AIN5 detection for each input*/
	//tw2868_byte_write(0, chip_addr, 0x00, 0x73, 0x01);

    /* ADATM I2S Output Select (default value)*/
    tw2868_byte_write(0, chip_addr, 0x00, 0x7B, 0x15);/* Select record audio of channel 51(AIN51)*/
    tw2868_byte_write(0, chip_addr, 0x00, 0x7C, 0x15);/* Select record audio of channel 51(AIN51)*/

    /* MIX_MUTE_A5 ?????? */
    tw2868_byte_write(0, chip_addr, 0x00, 0x7E, 0xc0);

    /* Audio Fs Mode Control */
    //tw2868_byte_write(0, chip_addr, 0x00, 0x89, 0x05);/* AIN5MD=1, AFS384=0 */
     tw2868_byte_write(0, chip_addr, 0x00, 0x89, 0x00);/* AIN5MD=1, AFS384=0 */
}

static int tw2826_set_audio_samplerate(int chip, int sample_rate)
{
    int temp, val;
    int i, j;

    switch (sample_rate) {
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
            return -1;
    }

    /* NOTES:320xfs mode: AIN5MD=1,AFS384=0. 44.1kHz/48kHz are not supported */
    i = 0, j = 0;
//    for(i = 0; i < ARRAY_SIZE(TW2868_IC); i ++) {
//        for(j = 0; j < ARRAY_SIZE(TW2868_IC[0]); j ++) {
            /* ACKI control is automatically set up by AFMD register values [3] */
            temp = tw2868_byte_read(0, TW2868_IC[i][j], 0x00, 0x70);
            temp &= ~0x08;
            temp |= 0x1<<3;
            tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0x70, temp);

            /* AFAUTO control mode 0x70 [2:0] */
            temp = tw2868_byte_read(0, TW2868_IC[i][j], 0x00, 0x70);
            temp &= ~0x07;
            temp |= val;
            tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0x70, temp);

            if (8000 == sample_rate) {
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE1, 0xc0);
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE2, 0xaa);
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE3, 0xaa);
            }
            else {
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE1, 0xe0);
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE2, 0xbb);
                tw2868_byte_write(0, TW2868_IC[i][j], 0x00, 0xE3, 0xbb);
            }
            printk("tw2826 audio samplerate set to %d ok\n", val);
//        }
//    }

    return 0;
}

static void tw2826_set_audio_format(int chip, audio_format_t audio_format)
{
    unsigned char i2c_addr, temp;
    
    int i = 0, j = 0;
//    for(i = 0; i < ARRAY_SIZE(TW2868_IC); i ++) {
//        for(j = 0; j < ARRAY_SIZE(TW2868_IC[0]); j ++) {
            i2c_addr = TW2868_IC[i][j];

            /* Define the digital serial audio data format for record
                and mixing audio on the ACLKR, ASYNR, ADATR and ADATM pin.*/
            temp = tw2868_byte_read(0, i2c_addr, 0x00, 0xd2);
            temp &= 0xbf;/* [6]*/
            temp |= (0==audio_format.format) ? 0 : 0x40;/* 0:I2S format 1:DSP format */
            tw2868_byte_write(0, i2c_addr, 0x00, 0xd2, temp);

            /* Define the digital serial audio data format for
                playback audio on the ACLKP, ASYNP and ADATP pin.*/
            temp = tw2868_byte_read(0, i2c_addr, 0x00, 0xdb);
            temp &= 0xf7;/* [3]*/
            temp |= (0==audio_format.format) ? 0x0 : 0x8; /* 0:I2S format 1:DSP format */
            tw2868_byte_write(0, i2c_addr, 0x00, 0xdb, temp);
//        }
//    }
}

static void tw2826_set_audio_volume(int chip, unsigned int volume)
{
    unsigned char i2c_addr, tmp;

	if(!(volume < 8)){
		volume = 8;
	}

    i2c_addr = TW2868_IC[0][0];
    tmp = tw2868_byte_read(0, i2c_addr, 0x00, 0xdf);	
    tmp &= 0x0f;
    tmp |= (volume << 4);
    tw2868_byte_write(0, i2c_addr, 0x00, 0xdf, tmp);
}
