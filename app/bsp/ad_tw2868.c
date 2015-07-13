
#define MAX_TW2868_IC_NUM 4

#define TW2868_DEV_ID     0xB0

enum {
    TW2868_SYSM1 = 0,  //(TW2868 x 2)
};

typedef struct {
    unsigned int  ADDR[MAX_TW2868_IC_NUM];
    unsigned int  SYID[MAX_TW2868_IC_NUM];
    unsigned int  I2CP[MAX_TW2868_IC_NUM];
    unsigned int  ACMD[MAX_TW2868_IC_NUM];  //0:Audio Master Cas_In, 1:Audio Slave Cas_Out
    unsigned int  VCMD[MAX_TW2868_IC_NUM];  //0:VIN1~8 1:VIN9~16 2:VIN17~24 3:VIN25~32
    unsigned int  SYSM;
} __TW2868_CON__;

const __TW2868_CON__ const TW2868_PLAT[] = {
    {{0x50, 0x52, 0x54, 0x56}, {TW2868_DEV_ID, TW2868_DEV_ID, 0, 0},{0,0,0,0},{0,1,1,1},{0,1,2,3}, TW2868_SYSM1},
};

static int  g_tw2868_plat = 0;
static int  g_tw2868_sysm = 0;

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

static unsigned int  tw2868_getID(int Grp, int Addr)
{
    unsigned int tmpID = 0;
#if 0
    tmpID  = ((tw2868_byte_read(Grp, Addr, 0, 0xFE) & 0xC0) >> 1);
    printk("cjiejigieg --- - 0x%08X\n", tw2868_byte_read(Grp, Addr, 0, 0xFE));
    tmpID += ((tw2868_byte_read(Grp, Addr, 0, 0xFF) & 0xF8) >> 3);
    printk("cjiejigieg --- - 0x%08X\n", tw2868_byte_read(Grp, Addr, 0, 0xFF));
#else
    tmpID += tw2868_byte_read(Grp, Addr, 0, 0xFF);
#endif
    return tmpID;
}

static unsigned char tw2868_check(int chip)
{
    int ii, jj;
    for(ii = 0; ii < ARRAY_SIZE(TW2868_PLAT); ii ++) {
        __TW2868_CON__ * pCON = &TW2868_PLAT[ii];
        for(jj = 0; jj < ARRAY_SIZE(pCON->ADDR); jj ++) {
            if(!pCON->SYID[jj]) {
                continue;
            }
            if(!gpio_i2c_probeX(pCON->I2CP[jj], pCON->ADDR[jj])) {
                return 0;
            }
            if(tw2868_getID(pCON->I2CP[jj], pCON->ADDR[jj]) != pCON->SYID[jj]) {
                return 0;
            }
        }
        g_tw2868_plat = ii;
        g_tw2868_sysm = pCON->SYSM;
        return !0;
    }

    return 0;
}

static void tw2868_video_SetMode(int Grp, unsigned char addr, const char *std)
{
    int i,j;

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
}

static int  tw2868_set_standard(int chip, const char* std)
{
    int Grp, Addr;
    __TW2868_CON__ * pCON = &TW2868_PLAT[g_tw2868_plat];
    for(chip = 0; chip < ARRAY_SIZE(pCON->ADDR); chip ++) {
        if(!pCON->SYID[chip]) {
            continue;
        }
        Grp  = pCON->I2CP[chip];
        Addr = pCON->ADDR[chip];

        tw2868_video_SetMode(Grp, Addr, std);
    }

    return 0;
}

static void tw2868_video_init(int chip, const char *std)
{
    int i,j;
    int Grp, Addr;
    __TW2868_CON__ * pCON = &TW2868_PLAT[g_tw2868_plat];
    for(chip = 0; chip < ARRAY_SIZE(pCON->ADDR); chip ++) {
        if(!pCON->SYID[chip]) {
            continue;
        }
        Grp  = pCON->I2CP[chip];
        Addr = pCON->ADDR[chip];
        for (i=0; i<2; i++) {
            for (j=0; j<4; j++) {
                tw2868_byte_write(Grp, Addr, i, 0x01 + 0x10*j, 0x00);/* brightness. */
                tw2868_byte_write(Grp, Addr, i, 0x02 + 0x10*j, 0x64);/* luminance contrast gain */
                tw2868_byte_write(Grp, Addr, i, 0x03 + 0x10*j, 0x11);/* sharpness */
                tw2868_byte_write(Grp, Addr, i, 0x04 + 0x10*j, 0x80);/* Chroma (U) */
                tw2868_byte_write(Grp, Addr, i, 0x05 + 0x10*j, 0x80);/* Chroma (V) */
                tw2868_byte_write(Grp, Addr, i, 0x06 + 0x10*j, 0x00);/* hue */
            }
        }

        tw2868_video_SetMode(Grp, Addr, std);

        tw2868_byte_write(Grp, Addr, 0x00, 0x43, 0x08);/* YDEL = 8h */ 

        /* CLKPO,CLKPN, 4*/
        /* Output Enable Control and Clock Output Control */
        tw2868_byte_write(Grp, Addr, 0x00, 0xfa, 0x4a);/* CLKN O1 108M */
        tw2868_byte_write(Grp, Addr, 0x00, 0xfb, 0x80);/* CLKN O1 108M */
        tw2868_byte_write(Grp, Addr, 0x00, 0xfc, 0xff);// Enable Video and Audio Detection
        tw2868_byte_write(Grp, Addr, 0x00, 0x60, 0x15); //Input 27MHz
        tw2868_byte_write(Grp, Addr, 0x00, 0x61, 0x13); //Input 27MHz
        /* CHID*/
        tw2868_byte_write(Grp, Addr, 0x00, 0x9c, 0xa0);//for video output
        tw2868_byte_write(Grp, Addr, 0x00, 0x9e, 0x52);

        /* Video Channel Output Control */
        tw2868_byte_write(Grp, Addr, 0x01, 0x60, 0xaa);/* Four Channel ITU0R BT.656 Time-multiplexed */
        tw2868_byte_write(Grp, Addr, 0x01, 0x63, 0x10);
        tw2868_byte_write(Grp, Addr, 0x01, 0x64, 0x32);
        tw2868_byte_write(Grp, Addr, 0x01, 0x65, 0x54);
        tw2868_byte_write(Grp, Addr, 0x01, 0x66, 0x76);

        /* Video AGC Control */
        tw2868_byte_write(Grp, Addr, 0x00, 0xaa, 0x00);/* VIN1~4 */
        tw2868_byte_write(Grp, Addr, 0x01, 0xaa, 0x00);/* VIN5~8 */

        /* Video Miscellaneous Function Control*/
        tw2868_byte_write(Grp, Addr, 0x00, 0xf9, 0xd1);/* limit output ranges */

        /* Miscellaneous Control II */
        tw2868_byte_write(Grp, Addr, 0x00, 0x96, 0xe6);/* Blue color, else set to 0xe0 */
        tw2868_byte_write(Grp, Addr, 0x01, 0x96, 0xe6);

        tw2868_byte_write(Grp, Addr, 0x00, 0xaf, 0x22);
        tw2868_byte_write(Grp, Addr, 0x00, 0xb0, 0x22);
        tw2868_byte_write(Grp, Addr, 0x01, 0xaf, 0x22);
        tw2868_byte_write(Grp, Addr, 0x01, 0xb0, 0x22);

        /* Enable video encoder  */
        //tw2868_byte_write(Grp, Addr, 0x00, 0x41, 0xd4);
        //tw2868_byte_write(Grp, Addr, 0x00, 0x43, 0x08);
        //tw2868_byte_write(Grp, Addr, 0x00, 0x4b, 0x00);
    }
}

static int  tw2868_PreViewSource(int Source)
{
    return !0;
}

static unsigned int tw2868_get_loss(int chip)
{
	return 0;
}

static void tw2868_set_color(int Chn, int Color, unsigned char Gain)
{
    const unsigned char COLOR_MAP[CX258X8_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
        VIDEO_COLOR_ADJTBL0, //HUE
        VIDEO_COLOR_ADJTBL0, //BRI
        VIDEO_COLOR_ADJTBL1, //CON
        VIDEO_COLOR_ADJTBL1, //SAT
    };
    const unsigned char CHNL_MAP[CX258X8_COLOR_LST][8] = {
        {0x06, 0x16, 0x26, 0x36, 0x06, 0x16, 0x26, 0x36},
        {0x01, 0x11, 0x21, 0x31, 0x01, 0x11, 0x21, 0x31},
        {0x02, 0x12, 0x22, 0x32, 0x02, 0x12, 0x22, 0x32},
        {0x04, 0x14, 0x24, 0x34, 0x04, 0x14, 0x24, 0x34},
    };

    __TW2868_CON__ * tmpCON = &TW2868_PLAT[g_tw2868_plat];
    int nIdx, nChn;

    if((Color >= CX258X8_COLOR_LST)
    || (Gain >= ARRAY_SIZE(COLOR_MAP[0]))
    || (Chn >= 16)) {
        return;
    }

    nIdx  = Chn/8;
    nChn  = Chn%8;

    switch(Color) {
    case CX258X8_COLOR_HUE:
    case CX258X8_COLOR_BRI:
    case CX258X8_COLOR_CON:
        tw2868_byte_write(tmpCON->I2CP[nIdx], tmpCON->ADDR[nIdx], nChn/4,
            CHNL_MAP[Color][nChn],   COLOR_MAP[Color][Gain]);
        break;
    case CX258X8_COLOR_SAT:
        tw2868_byte_write(tmpCON->I2CP[nIdx], tmpCON->ADDR[nIdx], nChn/4,
            CHNL_MAP[Color][nChn],   COLOR_MAP[Color][Gain]);
        tw2868_byte_write(tmpCON->I2CP[nIdx], tmpCON->ADDR[nIdx], nChn/4,
            CHNL_MAP[Color][nChn]+1, COLOR_MAP[Color][Gain]);
        break;
    }
}
