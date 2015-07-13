#define CX25824_REV_ID 0x0824
#define CX25828_REV_ID 0x0828
#define CX25838_REV_ID 0x0838
#define CX26824_REV_ID 0x6824
#define CX26828_REV_ID 0x6828
#define CX26844_REV_ID 0x6844
#define CX26848_REV_ID 0x6848
#define CX2NONE_REV_ID 0xFFFF

enum {
    CX258X8_SYSM1 = 0,  //Cx25828_11Z + Cx25838_12Z
    CX258X8_SYSM2,      //Cx25828_11Z + Cx25828_11Z
    CX268X8_SYSM1,      //Cx268x8_11Z x 1
};

typedef struct {
    const unsigned char ADDR[2];
    const unsigned int  SYID[2];
    const unsigned char ACMD[2];  //0:Audio Master Cas_In, 1:Audio Slave Cas_Out
    const unsigned char VCMD[2];  //0:VIN1~8 1:VIN9~16
    const unsigned char CLKM[2];  //0:48MHz Clock Source, 1:24MHz Clok Source
    const unsigned char SYSM;
          unsigned int  VRES[2][8];  //Video Standards Extra for Cx268XX
} __CX258X8_CON__;

__CX258X8_CON__ CX258X8_PLAT[] = {
    { {0x88, 0x8C}, {CX25828_REV_ID, CX25838_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX258X8_SYSM1},
    { {0x8E, 0x8C}, {CX25828_REV_ID, CX25838_REV_ID}, {0, 1}, {0, 1}, {0, 0}, CX258X8_SYSM1},
    { {0x8A, 0x8C}, {CX25828_REV_ID, CX25828_REV_ID}, {1, 0}, {1, 0}, {0, 0}, CX258X8_SYSM2},
    { {0x8A, 0x8E}, {CX25828_REV_ID, CX25828_REV_ID}, {1, 0}, {1, 0}, {1, 1}, CX258X8_SYSM2},
    { {0x88, 0x8E}, {CX26828_REV_ID, CX26828_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
    { {0x88, 0x00}, {CX25824_REV_ID, CX2NONE_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX258X8_SYSM2},
    { {0x88, 0x00}, {CX26824_REV_ID, CX2NONE_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
    { {0x88, 0x00}, {CX26828_REV_ID, CX2NONE_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
    { {0x88, 0x8E}, {CX26848_REV_ID, CX26848_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
    { {0x88, 0x00}, {CX26844_REV_ID, CX2NONE_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
    { {0x88, 0x00}, {CX26848_REV_ID, CX2NONE_REV_ID}, {0, 1}, {0, 1}, {1, 1}, CX268X8_SYSM1},
};

static unsigned long port_sel_tbl[2][4] = {
#if   defined(GPIO_PLAT_TYPE4) || defined(GPIO_PLAT_TYPE6)
	{0x0, 0x1, 0x1, 0x0}, {0x0, 0x1, 0x1, 0x0},
#else
	{0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, //Left Blank for Other Platforms;
#endif
};

static unsigned long vsrc_sel_tbl[2][4][4] = {
#if   defined(GPIO_PLAT_TYPE4)
	{{0x0, 0x0, 0x0, 0x0}, {0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x0, 0x0, 0x0}},
	{{0x0, 0x0, 0x0, 0x0}, {0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x0, 0x0, 0x0}},
#elif defined(GPIO_PLAT_TYPE6)
#if   (MAX_CAM_CH == 16)
	{{0x0, 0x0, 0x0, 0x0}, {0x2, 0x3, 0x0, 0x1}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x0, 0x0, 0x0}},
	{{0x0, 0x0, 0x0, 0x0}, {0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x0, 0x0, 0x0}},
#else
	{{0x0, 0x0, 0x0, 0x0}, {0x2, 0x3, 0x0, 0x1}, {0x5, 0x4, 0x7, 0x6}, {0x0, 0x0, 0x0, 0x0}},
	{{0x0, 0x0, 0x0, 0x0}, {0x2, 0x3, 0x0, 0x1}, {0x5, 0x4, 0x7, 0x6}, {0x0, 0x0, 0x0, 0x0}},
#endif
#else
	{{0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}},
	{{0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}, {0x0, 0x1, 0x2, 0x3}, {0x4, 0x5, 0x6, 0x7}},
#endif
};

static unsigned long vsrc_sel_buf[16];

void vsrc_sel_build(void)
{
    int ii, jj, ll = 0;
    for(ii = 0; ii < ARRAY_SIZE(port_sel_tbl); ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(port_sel_tbl[ii]); jj ++) {
            if(port_sel_tbl[ii][jj]) {
                int kk = 0;
                while(kk < ARRAY_SIZE(vsrc_sel_tbl[ii][jj])) {
                    vsrc_sel_buf[ll] = vsrc_sel_tbl[ii][jj][kk];
                    ll ++;
                    kk ++;
                }
            }
        }
    }
}

static int g_cx258x8_plat = 0;
static int g_cx258x8_sysm = 0;

int PAL = 0;
int NTSC = 1;

static void cx258x8_video_format(int chip, const char* std, const char* siz);

int cx258x8_i2c_mread(
	unsigned char dev,
	unsigned short address,
	unsigned char *buf,
	int len)
{
    unsigned char addrbuf[] = {
        (unsigned char)((address >> 8) & 0xFF),
        (unsigned char)(address & 0xFF),
    };

    gpio_i2c_rd_arr(0, dev, addrbuf, sizeof(addrbuf), buf, len);

    return 0;
}

int cx258x8_i2c_mwrite(
	unsigned char dev,
	unsigned short address,
	unsigned char *buf,
	int len)
{
    unsigned char addrbuf[] = {
        (unsigned char)((address >> 8) & 0xFF),
        (unsigned char)(address & 0xFF),
    };

    gpio_i2c_wr_arr(0, dev, addrbuf, sizeof(addrbuf), buf, len);

    return 0;
}
#if 0
void f2_WriteBitField(unsigned char i2c_adr, int reg_adr,int bitl,int bith,unsigned long value)
{
	unsigned long tmpData = 0;
	unsigned long tmpMask = 0;
	unsigned char bufData[4] = {0, 0, 0, 0};
	int ii, jj;
	
	int AccNum = ((bith/8) - (bitl/8) + 1);
	int AccBgn = (bitl/8);

	if((bitl > bith) || (bith >31) || (bitl <0)) return;
	tmpMask   = -1UL;
	tmpMask <<= (31-bith); tmpMask >>= (31-bith);
	tmpMask >>= (bitl);

	for(ii = AccBgn, jj = 0; ii < AccNum; ii ++, jj ++) {
		cx258x8_i2c_mread(i2c_adr, reg_adr+ii, &bufData[jj], 1);
	}
	tmpData  |= bufData[3]; tmpData <<= 8;
	tmpData  |= bufData[2]; tmpData <<= 8;
	tmpData  |= bufData[1]; tmpData <<= 8;
	tmpData  |= bufData[0];

	tmpData  &= ~tmpMask;
	tmpData  |= (value & tmpMask);

	bufData[0] = (tmpData >> 0) & 0xFF;
	bufData[1] = (tmpData >> 8) & 0xFF;
	bufData[2] = (tmpData >>16) & 0xFF;
	bufData[3] = (tmpData >>24) & 0xFF;

	for(ii = AccBgn, jj = 0; ii < AccNum; ii ++, jj ++) {
		cx258x8_i2c_mwrite(i2c_adr, reg_adr+ii, &bufData[jj], 1);
	}
}

unsigned long f2_ReadBitField(unsigned char i2c_adr, int reg_adr, int bitl,int bith)
{
	unsigned long tmpData = 0;
	unsigned long tmpMask = 0;
	unsigned char bufData[4] = {0, 0, 0, 0};
	int ii, jj;
	
	int AccNum = ((bith/8) - (bitl/8) + 1);
	int AccBgn = (bitl/8);

	if((bitl > bith) || (bith >31) || (bitl <0)) return;
	tmpMask   = -1UL;
	tmpMask <<= (31-bith); tmpMask >>= (31-bith);
	tmpMask >>= (bitl);

	for(ii = AccBgn, jj = 0; ii < AccNum; ii ++, jj ++) {
		cx258x8_i2c_mread(i2c_adr, reg_adr+ii, &bufData[jj], 1);
	}
	tmpData  |= bufData[3]; tmpData <<= 8;
	tmpData  |= bufData[2]; tmpData <<= 8;
	tmpData  |= bufData[1]; tmpData <<= 8;
	tmpData  |= bufData[0];

	tmpData  &= tmpMask;
	tmpData >>= bitl;

	return tmpData;
}
#else
void f2_WriteBitField(unsigned char i2c_adr, int reg_adr,int bitl,int bith,unsigned long value)
{
	unsigned char buf[4];
	int reg_num, idx, i;
	unsigned long tmp_val = 0, mask_val;
	if(bith < bitl || (bith - bitl) > 31){
		//printk("f_WriteBitField: error!\n");
		return;
	}
	reg_num = ((bith - bitl) / 8) + 1 + ((bith%8 + (8-(bitl%8))) < 8 ? 1 : 0);
	idx = bitl / 8;
	cx258x8_i2c_mread(i2c_adr, reg_adr+idx, buf+idx, reg_num);
	udelay(500);
	
	/*printk("rd:");
	for(i = 0; i < reg_num; i++)
		printk("buf[%d]=0x%x ", i+idx, buf[i+idx]);
	printk("\n");*/

	for(i = 0; i < reg_num; i++){
		tmp_val |= (buf[idx+i] << ((idx+i)*8));
	}

	mask_val = 0;
	for(i = 0; i < (bith - bitl + 1); i++){
		mask_val <<= 1;
		mask_val |= 1;
	}

	mask_val <<= bitl;
	tmp_val = (~mask_val & tmp_val) | ((value << bitl) & mask_val);
	buf[0] = tmp_val & 0xff;
	buf[1] = (tmp_val>>8) & 0xff;
	buf[2] = (tmp_val>>16) & 0xff;
	buf[3] = (tmp_val>>24) & 0xff;
	
	/*printk("rd:");
	for(i = 0; i < reg_num; i++)
		printk("buf[%d]=0x%x ", i+idx, buf[i+idx]);
	printk("\n");*/
	cx258x8_i2c_mwrite(i2c_adr, reg_adr+idx, buf+idx, reg_num);
	return ;
}

unsigned long f2_ReadBitField(unsigned char i2c_adr, int reg_adr, int bitl,int bith)
{
	unsigned char buf[4];
	int reg_num, idx, i;
	unsigned long tmp_val = 0, mask_val;

	if(bith < bitl || (bith - bitl) > 31) {
		return 0;
	}
	reg_num = ((bith - bitl) / 8) + 1 + ((bith%8 + (8-(bitl%8))) < 8 ? 1 : 0);
	idx = bitl / 8;

    for(i = idx; i < reg_num; i ++) {
    	cx258x8_i2c_mread(i2c_adr, reg_adr+i, &buf[i], 1);
    }

	for(i = 0; i < reg_num; i++) {
		tmp_val |= (buf[idx+i] << ((idx+i)*8));
	}

	mask_val = 0;
	for(i = 0; i < (bith - bitl + 1); i++) {
		mask_val <<= 1;
		mask_val |= 1;
	}

	mask_val <<= bitl;
	tmp_val = (mask_val & tmp_val) >> bitl;
	return tmp_val;
}
#endif

#define RegMaskWrite32 f2_WriteBitField
#define RegMaskRead32  f2_ReadBitField

void cx_delay(int t)
{
	udelay(t*1000);
}

typedef struct{
	void (*sleep)(int t);
}scripting_t;
scripting_t scripting = {cx_delay};

#include "Perseus_Function_Definition.js"
#include "Andromeda_Function_Definition.js"
#include "Alcaeus_Function_Definition.js"

void cx258x8_audio_config(void)
{
}

#define CX258X8_CHN_PER_CHIP (8)

int  cx258x8_PreViewSource(int Source)
{
    if(g_cx258x8_sysm == CX258X8_SYSM1) {
        int ii,addr,Chip,Chnl;
        if(Source >= CX258X8_CHN_PER_CHIP*2 || Source < 0) {
            return 0;
        }

        Chip = (Source/CX258X8_CHN_PER_CHIP);
        Chnl = (Source%CX258X8_CHN_PER_CHIP)*2+1;

        for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); ii ++) {
            addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[ii];

            switch(CX258X8_PLAT[g_cx258x8_plat].SYID[ii]) {
            case CX25828_REV_ID:
                Cx25828_Omux_bypass_out(addr,
                    0x3, Chnl);
            break;
            case CX25838_REV_ID:
                Cx25838_Port_Switch_Source(addr,
                    0x00, Chnl);
            break;
            }
        }
    }

    return !0;
}

static unsigned char cx258x8_check(int chip)
{
    int  ii, jj;
    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT); ii ++) {
        __CX258X8_CON__ * tmpCON = &CX258X8_PLAT[ii];
        int  ic_count = 0;

        for(jj = 0; jj < ARRAY_SIZE(tmpCON->ADDR); jj ++) {
            int  addr = tmpCON->ADDR[jj];
            int  syid = tmpCON->SYID[jj];
            
            if((syid == CX2NONE_REV_ID)
            || (gpio_i2c_probe(addr)
                && ((syid == CX25838_REV_ID)  //Cx25838 is Different.
                    || (syid == f2_ReadBitField(addr, 0x200, 0, 15))))) {
                printk("Got cx258x8 plat: 0x%02X(Cx%04X)\n", addr, syid);
                ic_count ++;
            }
        }

        if(ic_count == ARRAY_SIZE(tmpCON->ADDR)) {
            g_cx258x8_plat = ii;
            g_cx258x8_sysm = CX258X8_PLAT[ii].SYSM;
            vsrc_sel_build();
            return !0;
        }
    }

    return 0;
}

static int  cx258x8_get_vsup(video_sup_t *vsup)
{
    int vsiz;

    if(!vsup) return -1;

    vsiz  = (1 << VIDEO_SUP_FLD1);
    vsiz |= (1 << VIDEO_SUP_FLEF);

    if(CX268X8_SYSM1 == g_cx258x8_sysm) {
        vsiz |= (1 << VIDEO_SUP_FLEF);
    }
    else {
        vsiz &= ~(1 << VIDEO_SUP_FLEF);
    }

    vsup->vsiz = vsiz;

    return 0;
}

enum {
    CX258X8_COLOR_HUE = 0,
    CX258X8_COLOR_BRI,
    CX258X8_COLOR_CON,
    CX258X8_COLOR_SAT,
    CX258X8_COLOR_LST
};

static void cx258x8_set_color(int Chn, int Color, unsigned char Gain)
{
    const unsigned char COLOR_MAP[CX258X8_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
        VIDEO_COLOR_ADJTBL0, //HUE
        VIDEO_COLOR_ADJTBL0, //BRI
        VIDEO_COLOR_ADJTBL1, //CON
        VIDEO_COLOR_ADJTBL1, //SAT
    };
    __CX258X8_CON__ * tmpCON = &CX258X8_PLAT[g_cx258x8_plat];
    int addr, nchn;
    
    if((Color >= CX258X8_COLOR_LST)
    || (Gain >= ARRAY_SIZE(COLOR_MAP[0]))
    || (Chn >= CX258X8_CHN_PER_CHIP*2)) {
        return;
    }

    addr = tmpCON->ADDR[tmpCON->VCMD[Chn/CX258X8_CHN_PER_CHIP]];
    nchn = Chn%CX258X8_CHN_PER_CHIP;
	
#if   defined(GPIO_PLAT_TYPE6)
    {
        if(Chn < ARRAY_SIZE(vsrc_sel_buf)) {
            nchn = vsrc_sel_buf[Chn];
        }
    }
#endif

    switch(Color) {
    case CX258X8_COLOR_HUE:
        Cx25838_Hue_Adjust(addr, nchn,
            COLOR_MAP[Color][Gain]);
        break;
    case CX258X8_COLOR_BRI:
        Cx25838_Bright_Adjust(addr, nchn,
            COLOR_MAP[Color][Gain]);
        break;
    case CX258X8_COLOR_CON:
        Cx25838_Contrast_Adjust(addr, nchn,
            COLOR_MAP[Color][Gain]);
        break;
    case CX258X8_COLOR_SAT:
        Cx25838_Sat_Adjust(addr, nchn,
            COLOR_MAP[Color][Gain],
            COLOR_MAP[Color][Gain]);
        break;
    }
}

static int cx258x8_set_standard(int chip, const char* std, const char* siz)
{
    cx258x8_video_format(chip, std, siz);
	return 0;
}

static unsigned int cx258x8_get_loss(int chip)
{
	const int Regs[] = {
        VDECH_GEN_STAT, VDECG_GEN_STAT, VDECF_GEN_STAT, VDECE_GEN_STAT,
        VDECD_GEN_STAT, VDECC_GEN_STAT, VDECB_GEN_STAT, VDECA_GEN_STAT,
	};
	unsigned int ret_bit = 0;
	unsigned int tmpStat = 0;

    __CX258X8_CON__ * tmpCON = &CX258X8_PLAT[g_cx258x8_plat];

    for(chip = ARRAY_SIZE(tmpCON->VCMD); chip > 0; chip --) {
        int ii, jj;
        for(ii = 0; ii < ARRAY_SIZE(tmpCON->ADDR); ii ++) {
            if((chip == tmpCON->VCMD[ii]+1) && (tmpCON->SYID[ii] != CX2NONE_REV_ID)) {
                jj = ((tmpCON->SYID[ii] == CX25824_REV_ID) || (tmpCON->SYID[ii] == CX26824_REV_ID))
                    ?ARRAY_SIZE(Regs)/2
                    :0;
                for( ; jj < ARRAY_SIZE(Regs); jj ++) {
                    tmpStat   = f2_ReadBitField(tmpCON->ADDR[ii], Regs[jj], 0, 15);
                    tmpStat  &= 0x00000400;
                    ret_bit <<= 1;
                    if(!tmpStat) {
                        ret_bit ++;
                    }
                    msleep(10);
                }
            }
        }
    }

#if   defined(GPIO_PLAT_TYPE6)
    {
        int rebit[32], ii, shift;

        memset(rebit, 0, sizeof(rebit));
        for(ii = 0; ii < ARRAY_SIZE(vsrc_sel_buf); ii ++) {
            shift  = (ii < ARRAY_SIZE(vsrc_sel_buf)/2) ?0 :ARRAY_SIZE(vsrc_sel_buf)/2;
            rebit[ii] = (ret_bit & (1 << (vsrc_sel_buf[ii] + shift))) ?1 :0;
        }
        ret_bit = 0;
        for(ii = 0; ii < ARRAY_SIZE(rebit); ii ++) {
            ret_bit |= (rebit[ii] << ii);
        }
    }
#endif

	return ret_bit;
}

static int  cx258x8_clock_init(void)
{
    int ii, addr = 0, syid = 0, clkm = 0, ic_typ;
    unsigned char ic_clk = 0;
    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); ii ++) {
        addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[ii];
        syid = CX258X8_PLAT[g_cx258x8_plat].SYID[ii];
        clkm = CX258X8_PLAT[g_cx258x8_plat].CLKM[ii];

        if(syid == CX2NONE_REV_ID) continue;
		
        if(syid == CX26828_REV_ID || syid ==CX26824_REV_ID) {
            ic_typ = 0;  
        }
        else {
            ic_typ = 0;  //Force Using Cx26828 Driver For Cx26848
        }

        switch(syid) {
        case CX25824_REV_ID:
        case CX25828_REV_ID:
            Andromeda_Colibri_Init(addr, clkm);
            break;
        case CX25838_REV_ID:
            Cx25838_Pll_Init(addr, clkm);
            break;
        case CX26824_REV_ID:
        case CX26828_REV_ID:
        case CX26844_REV_ID:
        case CX26848_REV_ID:
			if(clkm == 0) {
            	ic_clk = COLIBRI_FCLK_720MHZ_CRYSTAL_48MHZ;
			}
			if(clkm == 1) {
            	ic_clk = COLIBRI_FCLK_720MHZ_CRYSTAL_24MHZ;
			}
			if(clkm == 2) {
            	ic_clk = COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz;
			}

            ALCS_Reset(addr);
            //Not reset audio
            writeDword(addr, ALCS_CHIP_RST, 0xF4FFE6);
            ALCS_initColibri(addr,
                            ic_typ,
                            ic_clk);
            break;
        default:
            break;
        }

        if(syid != f2_ReadBitField(addr, 0x200, 0, 15)) {
            goto CLOCK_INIT_FAILED;
        }
    }

    return !0;

CLOCK_INIT_FAILED:
    printk("CX258X8 : clock initialization failed!!!!!\n");

    return 0;
}

static int  cx258x8_regdbg(audio_reg_xdbg_t * reg_dbg)
{
    int addr = 0;

    if(!reg_dbg
    && (g_cx258x8_plat >= ARRAY_SIZE(CX258X8_PLAT) || g_cx258x8_plat < 0)
    && reg_dbg->chip >= ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR)) {
        return -1;
    }

    addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[reg_dbg->chip];

    if(!reg_dbg->rw_flg) {  //I2C Device Write Operation
        RegMaskWrite32(addr, reg_dbg->addr, 0, 31, reg_dbg->value);
    }
    else {
        reg_dbg->value = RegMaskRead32(addr, reg_dbg->addr, 0, 31);
    }
    
    return 0;
}

static void cx258x8_SysM1_init(const char* std)
{
    int i;
    
    int addr0 = CX258X8_PLAT[g_cx258x8_plat].ADDR[0];
    int addr1 = CX258X8_PLAT[g_cx258x8_plat].ADDR[1];

    Cx25838_Decoder_Init(addr1, VIDEO_IS_PAL(std) ?PAL :NTSC);
    Cx25838_DDR_Init(addr1);

    if(VIDEO_IS_PAL(std)) {
        //AndromedaDecoder_PAL_Init(addr0);
        Andromeda_PAL_Init(addr0);
    }
    else {
        //AndromedaDecoder_NTSC_Init(addr0);
        Andromeda_NTSC_Init(addr0);
    }

    for (i = 0; i < 8; i ++) {
        RegMaskWrite32(addr0, //scaling to CIF
            CX828_VDECA_HSCALE_CTRL +(0x200*i), 0, 23, 0x100000);
        RegMaskWrite32(addr0, //change the blue field
            CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 16, 23, (0x60+i));
        RegMaskWrite32(addr0, //Enable the blue field
            CX828_VDECA_OUT_CTRL1 +(0x200*i), 7, 7, 0x1);
        RegMaskWrite32(addr0, //Enable the blue field
            CX828_VDECA_OUT_CTRL_NS +(0x200*i), 7, 7, 0x1);
        //RegMaskWrite32(addr0, //Disable AGC
        //    CX828_VDECA_MAN_VGA_CTRL+(0x200*i), 0, 31, 0x00002018);
        //RegMaskWrite32(addr0, //Disable AGC
        //    CX828_VDECA_MAN_AGC_CTRL+(0x200*i), 0, 31, 0x01000416);
        //RegMaskWrite32(addr0, //Fix 12dB Amp.
        //    CX828_VDECA_DFE_CTRL1 + (0x200*i), 28, 28, 0);
    }

    Cx25828_video_pack_en(addr0, 0x4, 0x0, 0x0, 0x3, 0x1);//vpe 4,chA
    Cx25828_video_pack_en(addr0, 0x5, 0x2, 0x1, 0x3, 0x1);//vpe 5,chB
    Cx25828_video_pack_en(addr0, 0x6, 0x4, 0x2, 0x3, 0x1);//vpe 6,chC
    Cx25828_video_pack_en(addr0, 0x7, 0x6, 0x3, 0x3, 0x1);//vpe 7,chD

    Cx25828_video_pack_en(addr0, 0x8, 0x8, 0x0, 0x3, 0x1);//vpe 8,chE
    Cx25828_video_pack_en(addr0, 0x9, 0xa, 0x1, 0x3, 0x1);//vpe 9,chF
    Cx25828_video_pack_en(addr0, 0xa, 0xc, 0x2, 0x3, 0x1);//vpe a,chG
    Cx25828_video_pack_en(addr0, 0xb, 0xe, 0x3, 0x3, 0x1);//vpe b,chH

    Cx25828_Omux_Enable(addr0, 0x1, 0x4, 0x1,0x0,0x0,0x0); //PB
    Cx25828_Omux_Enable(addr0, 0x2, 0x4, 0x1,0x0,0x0,0x0); //PC
    Cx25828_Omux_remux_out(addr0, 0x1, 0x1, 0x0); //port B
    Cx25828_Omux_remux_out(addr0, 0x2, 0x1, 0x0); //port C

    RegMaskWrite32(addr0, CX828_VP_OMUX_A, 0, 0, 0x0); //disable remux function on PA
    Cx25828_Demux_Enable(addr0, 0x0, 0x0, 0x0,0x0,0x0,0x0,0x1,0x2,0x3);//demux PA
    Cx25828_bigframe_port_en(addr0, 0x1,0x1,0x2);

    //1D1 bypass to Port D
    Cx25828_Omux_Enable(addr0, 0x3, 0x1, 0x1,0x0,0x0,0x0); //Port D, Bypass mode
    Cx25828_Omux_bypass_out(addr0, 0x3, 0x1);
    
    for(i = 0; i < 8; i ++) { //Fix Top Red-Fixel Lines
        RegMaskWrite32(addr0, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x0);
        RegMaskWrite32(addr1, VDECA_HORIZ_TIM_CTRL +(0x200*i), 0, 31, 0x632D007D);
    }
    for(i = 0; i < 8; i ++) { 
        /* Apply to revision A1 only */
        unsigned long value     = 0;
        unsigned long bgdel_cnt = 0;
        { //rev_id A1 only
        value = RegMaskRead32(addr0, CX828_VDECA_HORIZ_TIM_CTRL + (0x200*i), 0, 31);
        value = (value & 0xFF000000) >> 24;
        value += 0x14;

        bgdel_cnt = RegMaskRead32(addr0, CX828_VDECA_BLUE_FLD_CTRL + (0x200*i), 0, 31);
        bgdel_cnt &= 0x00FFFFFF;
        RegMaskWrite32(addr0, CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 0, 31, bgdel_cnt | (value << 24) );
        }
    }

    //D1 to Hi3515 for preview
    Perseus_GPIO_SetValue(addr1, 0x0, 0x0); //GPIO 0 = 0;
    Perseus_GPIO_SetValue(addr1, 0x1, 0x0); //GPIO 1 = 0;
    Perseus_GPIO_SetValue(addr1, 0x2, 0x1); //GPIO 2 = 1;
    Cx25838_Port_Enable(addr1, 0x00, 0x01, 0x01, 0x01);
    Cx25838_16CIF_Mode_BigFrame(addr1,
        VIDEO_IS_PAL(std) ?PAL :NTSC, 0x3, 0x1, 0x2, 1);
    RegMaskWrite32(addr1, VP_TF0_SRC9_CFG,  0, 31, 0x180A0001);  //FIXUP 16 CIF FRAME:
    RegMaskWrite32(addr1, VP_TF0_SRC10_CFG, 0, 31, 0x190B0011);
    RegMaskWrite32(addr1, VP_TF0_SRC11_CFG, 0, 31, 0x1a080011);
    RegMaskWrite32(addr1, VP_TF0_SRC12_CFG, 0, 31, 0x1b090011);
    
    for(i = 0; i < 8; i ++) {
        //RegMaskWrite32(addr0, CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x4e004);
        RegMaskWrite32(addr0, CX828_VDECA_OUT_CTRL1 +(0x200*i), 0, 31, 0x1005a2);
        RegMaskWrite32(addr0, CX828_VDECA_DFE_CTRL1 +(0x200*i), 0, 31, 0xebf0000a);
        //Shorten video lock
        RegMaskWrite32(addr0, CX828_VDECA_PLL_CTRL_FAST +(0x200*i),  0, 31, 0x801f1000);
        //Support 360 Pixel With CIF Fix
        RegMaskWrite32(addr1, VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x1);
        RegMaskWrite32(addr0, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x1);
    }

    RegMaskWrite32(addr0, 0x218, 0, 15, 0xff04);
    
    RegMaskWrite32(addr1, VP_DVP_B_DEMUX, 0, 31, 0x80321031);
    RegMaskWrite32(addr1, VP_DVP_C_DEMUX, 0, 31, 0x80765431);

    RegMaskWrite32(addr1, VP_FS_INIT_CMD, 0, 31, 0x00000000);
    RegMaskWrite32(addr1, VP_TF0_CFG,     0, 31, 0x580c06f0);
    RegMaskWrite32(addr1, VP_FS_INIT_CMD, 0, 31, 0x80000000);
    RegMaskWrite32(addr1, VP_TF0_CFG,     0, 31, 0x580c06f1);
}

static void cx258x8_SysM2_init(const char* std)
{
    int ii, addr;
    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); ii ++) {
        int i;
        addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[ii];

        if(VIDEO_IS_PAL(std)) {
            Andromeda_PAL_Init(addr);
        }
        else {
            Andromeda_NTSC_Init(addr);
        }

        for (i = 0; i < 8; i ++) {
            RegMaskWrite32(addr, //scaling to CIF
                CX828_VDECA_HSCALE_CTRL +(0x200*i), 0, 23, 0x100000);
            RegMaskWrite32(addr, //change the blue field
                CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 16, 23, (0x60+i));
            RegMaskWrite32(addr, //Enable the blue field
                CX828_VDECA_OUT_CTRL1 +(0x200*i), 7, 7, 0x1);
            RegMaskWrite32(addr, //Enable the blue field
                CX828_VDECA_OUT_CTRL_NS +(0x200*i), 7, 7, 0x1);
            //RegMaskWrite32(addr, //Disable AGC
            //    CX828_VDECA_MAN_VGA_CTRL+(0x200*i), 0, 31, 0x00002018);
            //RegMaskWrite32(addr, //Disable AGC
            //    CX828_VDECA_MAN_AGC_CTRL+(0x200*i), 0, 31, 0x01000416);
            //RegMaskWrite32(addr, //Fix 12dB Amp.
            //    CX828_VDECA_DFE_CTRL1 + (0x200*i), 28, 28, 0);
        }
#if 1
        Cx25828_video_pack_en(addr, 0x8, 0x1, 0x0, 0x3, 0x0);//vpe 8,chE
        Cx25828_video_pack_en(addr, 0x9, 0x3, 0x1, 0x3, 0x0);//vpe 9,chF
        Cx25828_video_pack_en(addr, 0xa, 0x5, 0x2, 0x3, 0x0);//vpe a,chG
        Cx25828_video_pack_en(addr, 0xb, 0x7, 0x3, 0x3, 0x0);//vpe b,chH

        Cx25828_video_pack_en(addr, 0xc, 0x9, 0x4, 0x3, 0x0);//vpe c,chA
        Cx25828_video_pack_en(addr, 0xd, 0xb, 0x5, 0x3, 0x0);//vpe d,chB
        Cx25828_video_pack_en(addr, 0xe, 0xd, 0x6, 0x3, 0x0);//vpe e,chC
        Cx25828_video_pack_en(addr, 0xf, 0xf, 0x7, 0x3, 0x0);//vpe f,chD

        Cx25828_Omux_Enable(addr, 0x2, 0x4, 0x1,0x0,0x0,0x0); //PC
        Cx25828_Omux_Enable(addr, 0x3, 0x4, 0x1,0x0,0x0,0x0); //PD
        Cx25828_Omux_remux_out(addr, 0x2, 0x0, 0x0); //port C
        Cx25828_Omux_remux_out(addr, 0x3, 0x0, 0x0); //port D
        RegMaskWrite32(addr, CX828_VP_OMUX_C, 17, 17, 0x0);
        RegMaskWrite32(addr, CX828_VP_OMUX_D, 17, 17, 0x0);
#endif
#if 0
        //1D1 bypass to Port D
        Cx25828_Omux_Enable(addr, 0x3, 0x1, 0x1,0x0,0x0,0x0); //Port D, Bypass mode
        Cx25828_Omux_bypass_out(addr, 0x3, 0x1);
        //1D1 bypass to Port C
        Cx25828_Omux_Enable(addr, 0x2, 0x1, 0x1,0x0,0x0,0x0); //Port D, Bypass mode
        Cx25828_Omux_bypass_out(addr, 0x2, 0x1);
#endif
#if 1
        for(i = 0; i < 8; i ++) {
            /* Apply to revision A1 only */
            unsigned long value     = 0;
            unsigned long bgdel_cnt = 0;
            { //rev_id A1 only
            value = RegMaskRead32(addr, CX828_VDECA_HORIZ_TIM_CTRL + (0x200*i), 0, 31);
            value = (value & 0xFF000000) >> 24;
            value += 0x14;

            bgdel_cnt = RegMaskRead32(addr, CX828_VDECA_BLUE_FLD_CTRL + (0x200*i), 0, 31);
            bgdel_cnt &= 0x00FFFFFF;
            RegMaskWrite32(addr, CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 0, 31, bgdel_cnt | (value << 24) );
            }
        }

        for(i = 0; i < 8; i ++) {
            //Fix Top Red-Fixel Lines
            RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x0);

            //RegMaskWrite32(addr, CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x4e004);
            RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1 +(0x200*i), 0, 31, 0x1005a2);
            RegMaskWrite32(addr, CX828_VDECA_DFE_CTRL1 +(0x200*i), 0, 31, 0xebf0000a);
            //Shorten video lock
            RegMaskWrite32(addr, CX828_VDECA_PLL_CTRL_FAST +(0x200*i),  0, 31, 0x801f1000);
            //Support 360 Pixel With CIF Fix
            //RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x1);
        }

        RegMaskWrite32(addr, 0x218, 0, 15, 0xff04);
#endif
    }
}

static int CX268XX_STD[][8];

static void cx268x8_SysM1_init(const char* std, const char* siz)
{
    int ii, addr, clkm, syid;
    int plat = g_cx258x8_plat;

    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[plat].ADDR); ii ++) {
        int jj;
        for(jj = 0; jj < ARRAY_SIZE(CX258X8_PLAT[plat].VRES[ii]); jj ++) {
			CX258X8_PLAT[plat].VRES[ii][jj] =
				VIDEO_IS_960H(siz) ?RES_960H :RES_720H;
		}
    }

    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[plat].ADDR); ii ++) {

        int port;
        int ic_typ;

        addr = CX258X8_PLAT[plat].ADDR[ii];
        clkm = CX258X8_PLAT[plat].CLKM[ii];
        syid = CX258X8_PLAT[plat].SYID[ii];

        if(!addr) continue;

        if(syid == CX26828_REV_ID || syid == CX26824_REV_ID) {
            ic_typ = 0;
        }
        else {
            ic_typ = 0;  //Force Using Cx26828 Driver For Cx26848
        }

        ALCS_initDecoder(addr, ic_typ, std, CX258X8_PLAT[plat].VRES[ii]);

        for(port = 0; port < 4; port++) {
            if(!port_sel_tbl[ii][port]) {
                continue;
            }
            ALCS_setupByteInterleaveOutput(addr,
                    ic_typ,
                    4,
                    960,
                    OUTPUT_RATE_144_MHZ,
                    port,
                    (unsigned long *)vsrc_sel_tbl[ii],
                    1,
                    std,
                    CX258X8_PLAT[plat].VRES[ii]);
        }
    }
}

static void cx258x8_video_init(int chip, const char* std, const char* siz)
{
    cx258x8_clock_init();

    switch(g_cx258x8_sysm) {
    case CX258X8_SYSM1:
        printk("<><><><><><>CX258X8_SYSM1_TYPE><><><><><>\n");
        cx258x8_SysM1_init(std);
        break;
    case CX258X8_SYSM2:
        printk("<><><><><><>CX258X8_SYSM2_TYPE<><><><><><>\n");
        cx258x8_SysM2_init(std);
        break;
    case CX268X8_SYSM1:
        printk("<><><><><><>CX268X8_SYSM1_TYPE<><><><><><>\n");
        cx268x8_SysM1_init(std, siz);
        break;
    default:
        break;
    }
}

static void cx258x8_SysM1_format(const char* std)
{
    int addr0 = CX258X8_PLAT[g_cx258x8_plat].ADDR[0];
    int addr1 = CX258X8_PLAT[g_cx258x8_plat].ADDR[1];
    int i;

    Cx25838_Decoder_Format(addr1, VIDEO_IS_PAL(std) ?PAL :NTSC);

    if(VIDEO_IS_PAL(std)) {
        Andromeda_PAL_Format(addr0);
    }
    else {
        Andromeda_NTSC_Format(addr0);
    }
    for(i = 0; i < 8; i ++) {
        /* Apply to revision A1 only */
        unsigned long value     = 0;
        unsigned long bgdel_cnt = 0;
        { //rev_id A1 only
            value = RegMaskRead32(addr0, CX828_VDECA_HORIZ_TIM_CTRL + (0x200*i), 0, 31);
            value = (value & 0xFF000000) >> 24;
            value += 0x14;

            bgdel_cnt = RegMaskRead32(addr0, CX828_VDECA_BLUE_FLD_CTRL + (0x200*i), 0, 31);
            bgdel_cnt &= 0x00FFFFFF;
            RegMaskWrite32(addr0, CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 0, 31, bgdel_cnt | (value << 24) );
        }
    }
        
    Cx25838_16CIF_Mode_BigFrame(addr1,
        VIDEO_IS_PAL(std) ?PAL :NTSC, 0x3, 0x1, 0x2, 1);
    RegMaskWrite32(addr1, VP_TF0_SRC9_CFG,  0, 31, 0x180A0001);  //FIXUP 16 CIF FRAME:
    RegMaskWrite32(addr1, VP_TF0_SRC10_CFG, 0, 31, 0x190B0011);
    RegMaskWrite32(addr1, VP_TF0_SRC11_CFG, 0, 31, 0x1a080011);
    RegMaskWrite32(addr1, VP_TF0_SRC12_CFG, 0, 31, 0x1b090011);
    
    for(i = 0; i < 8; i ++) {
        //RegMaskWrite32(addr0, CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x4e004);
        RegMaskWrite32(addr0, CX828_VDECA_OUT_CTRL1 +(0x200*i), 0, 31, 0x1005a2);
        RegMaskWrite32(addr0, CX828_VDECA_DFE_CTRL1 +(0x200*i), 0, 31, 0xebf0000a);
        //Shorten video lock
        RegMaskWrite32(addr0, CX828_VDECA_PLL_CTRL_FAST +(0x200*i),  0, 31, 0x801f1000);
    }

    RegMaskWrite32(addr0, 0x218, 0, 15, 0xff04);
    
    RegMaskWrite32(addr1, VP_DVP_B_DEMUX, 0, 31, 0x80321031);
    RegMaskWrite32(addr1, VP_DVP_C_DEMUX, 0, 31, 0x80765431);

    RegMaskWrite32(addr1, VP_FS_INIT_CMD, 0, 31, 0x00000000);
    RegMaskWrite32(addr1, VP_TF0_CFG,     0, 31, 0x580c06f0);
    RegMaskWrite32(addr1, VP_FS_INIT_CMD, 0, 31, 0x80000000);
    RegMaskWrite32(addr1, VP_TF0_CFG,     0, 31, 0x580c06f1);
}

static void cx258x8_SysM2_format(const char* std)
{
    int ii, addr;
    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); ii ++) {
        int i;
        addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[ii];

        if(VIDEO_IS_PAL(std)) {
            Andromeda_PAL_Format(addr);
        }
        else {
            Andromeda_NTSC_Format(addr);
        }

        for (i = 0; i < 8; i ++) {
            RegMaskWrite32(addr, //scaling to CIF
                CX828_VDECA_HSCALE_CTRL +(0x200*i), 0, 23, 0x100000);
            RegMaskWrite32(addr, //change the blue field
                CX828_VDECA_BLUE_FLD_CTRL +(0x200*i), 16, 23, (0x60+i));
            RegMaskWrite32(addr, //Enable the blue field
                CX828_VDECA_OUT_CTRL1 +(0x200*i), 7, 7, 0x1);
            RegMaskWrite32(addr, //Enable the blue field
                CX828_VDECA_OUT_CTRL_NS +(0x200*i), 7, 7, 0x1);
            //RegMaskWrite32(addr, //Disable AGC
            //    CX828_VDECA_MAN_VGA_CTRL+(0x200*i), 0, 31, 0x00002018);
            //RegMaskWrite32(addr, //Disable AGC
            //    CX828_VDECA_MAN_AGC_CTRL+(0x200*i), 0, 31, 0x01000416);
            //RegMaskWrite32(addr, //Fix 12dB Amp.
            //    CX828_VDECA_DFE_CTRL1 + (0x200*i), 28, 28, 0);
        }
#if 1
        for(i = 0; i < 8; i ++) {
            //Fix Top Red-Fixel Lines
            RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x0);

            //RegMaskWrite32(addr, CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x4e004);
            RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1 +(0x200*i), 0, 31, 0x1305b2);
            RegMaskWrite32(addr, CX828_VDECA_DFE_CTRL1 +(0x200*i), 0, 31, 0xebf0000a);
            //Shorten video lock
            RegMaskWrite32(addr, CX828_VDECA_PLL_CTRL_FAST +(0x200*i),  0, 31, 0x801f1000);
            //Support 360 Pixel With CIF Fix
            //RegMaskWrite32(addr, CX828_VDECA_OUT_CTRL1+(0x200*i), 4, 4, 0x1);
        }

        RegMaskWrite32(addr, 0x218, 0, 15, 0xff04);
#endif
    }
}

static void cx258x8_video_format(int chip, const char* std, const char* siz)
{
    switch(g_cx258x8_sysm) {
    case CX258X8_SYSM1:
        printk("<><><><><><>CX258X8_SYSM1_FORMAT<><><><><><>\n");
        cx258x8_SysM1_format(std);
        break;
    case CX258X8_SYSM2:
        printk("<><><><><><>CX258X8_SYSM2_FORMAT<><><><><><>\n");
        cx258x8_SysM2_format(std);
        break;
    case CX268X8_SYSM1:
        printk("<><><><><><>CX268X8_SYSM1_TYPE<><><><><><>\n");
        cx268x8_SysM1_init(std, siz);
        break;
    default:
        break;
    }
}

static int cx258x8_set_audio_samplerate(int chip, audio_samplerate_t samplerate)
{
    int sr_table[] = {0x3, 0x2, 0x1, 0x0};
    int index = 0;
    int ii;

    switch(samplerate) {
	case SAMPLE_RATE_8000:
        index = 0;
        break;
	case SAMPLE_RATE_16000:
        index = 1;
        break;
	case SAMPLE_RATE_32000:
        index = 2;
        break;
	case SAMPLE_RATE_44100:
        return -1;
        break;
	case SAMPLE_RATE_48000:
        index = 3;
        break;
	case SAMPLE_RATE_BUTT:
    default:
        return -1;
    }

    for(ii = 0; ii < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); ii ++) {
        switch(CX258X8_PLAT[g_cx258x8_plat].SYID[ii]) {
        case CX25824_REV_ID:
        case CX25828_REV_ID:
        case CX26824_REV_ID:
        case CX26828_REV_ID:
        case CX26844_REV_ID:
        case CX26848_REV_ID:
            RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[ii],
                CX828_AADC_DIG_CTRL1, 20, 22, sr_table[index]);
            break;
        case CX25838_REV_ID:
            Cx25838_Audio_Sample_Rate(CX258X8_PLAT[g_cx258x8_plat].ADDR[ii],
                sr_table[index]);
            break;
        }
    }

    return 0;
}

static int cx258x8_set_audio_volume(int chip, unsigned int volume)
{
    if(volume >= 16) {
        volume = 15;
    }

    volume <<= 2;
    if(volume >= 0x40) {
        volume = 0x3F;
    }

    for(chip = 0; chip < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); chip ++) {
        if(!CX258X8_PLAT[g_cx258x8_plat].ACMD[chip]) {
            RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[chip],
                CX828_AUD_GAIN2, 8, 13, volume);
            break;
        }
    }

    return 0;
}

static void cx258x8_set_audio_format(int chip, audio_format_t audio_format)
{
    //UnAvailable Option.
}

static void cx258x8_audio_init(int chip)
{
    int  i, addr, acmd;
    for(chip = 0; chip < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); chip ++) {
        addr = CX258X8_PLAT[g_cx258x8_plat].ADDR[chip];
        acmd = CX258X8_PLAT[g_cx258x8_plat].ACMD[chip];

        switch(CX258X8_PLAT[g_cx258x8_plat].SYID[chip]) {
        case CX25824_REV_ID:
        case CX25828_REV_ID:
        case CX26824_REV_ID:
        case CX26828_REV_ID:
        case CX26844_REV_ID:
        case CX26848_REV_ID: {
            ///************************PlayBack Configuration****************************///
            if(!acmd) {
                Cx25828_Audio_ADAC_Init(addr, 0x0, 0xa, 0x1, 0x10);
                Cx25828_Audio_PB_Cfg(addr, 0x0, 0x1, 0x0,
                    (g_cx258x8_sysm == CX268X8_SYSM1) ?0x0 :0x1, 0x1, 0x1);
            }
            //  Cx25828_Audio_MCLK_En(addr, 0x0);
            ///*********************Cascade Record Configuration*************************///
            Cx25828_Audio_AADC_Init(addr, 0x0);

            //I2S,16ch, 16BIT, MASTER, 1BIT SHIFT
            if(g_cx258x8_sysm == CX258X8_SYSM1) {
                Cx25828_Audio_Mix_Cfg(addr, 0x0, 0x3, 0x0, !acmd ?0x1 :0x0, 0x1, 0x1);
            }
            else {
                Cx25828_Audio_Rec_Cfg(addr, 0x0, 0x3, 0x0, !acmd ?0x1 :0x0, 0x1, 0x1);
            }

            //I2S,2ch, 16BIT, MASTER, 1BIT SHIFT
            for(i = 0; i < 8 && !acmd; i ++) {
                if(g_cx258x8_sysm == CX258X8_SYSM1) {
                    Cx25828_Audio_Mix_Mmap(addr, 0x0+i, 0x0+i); //Ch_x
                    Cx25828_Audio_Mix_Mmap(addr, 0x8+i, 0xb+i); //cas_x
                }
                else {
                    Cx25828_Audio_Rec_Rmap(addr, 0x0+i, 0x0+i); //Ch_x
                    Cx25828_Audio_Rec_Rmap(addr, 0x8+i, 0xd+i); //cas_x
                }
            }
            if(!acmd && (g_cx258x8_sysm == CX268X8_SYSM1)) { //FIXME: Fix 16Channels Audio Acasading;
                RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[chip],
                    0x2044, 0, 31, 0x20F734E6);
                RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[chip],
                    0x2048, 0, 31, 0x230A4E51);
            }
            Cx25828_Audio_Cas_Cfg(addr, acmd, 0x1, 0x0);
        }
        break;
        case CX25838_REV_ID: {
            Cx25838_Audio_Init(addr);
            Cx25838_Audio_Sample_Rate(addr, 0x3); //8KHz
            //put PerseusPlus in slave mode
            Cx25838_Audio_I2S_Mode(addr, 0x0, 0x0, !acmd ? 0x1 : 0x0);
            Cx25838_Audio_Cascade_Mode(addr, 0x2, 0x0, 0x1);
        }
        break;
        }
    }

    cx258x8_set_audio_samplerate(0, SAMPLE_RATE_8000);
}

static void cx258x8_audio_set_liveloop(int chip, int channel)
{
    if(channel >= 8) {
        channel -= 0x8;
        channel += (g_cx258x8_sysm == CX258X8_SYSM1) ?0xb :0xd;
    }

    for(chip = 0; chip < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); chip ++) {
        if(!CX258X8_PLAT[g_cx258x8_plat].ACMD[chip]) {
            RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[chip],
                CX828_AP_CFG2, 4, 8, channel);//choose the source to ADAC
            break;
        }
    }
}

static void cx258x8_audio_set_playback(int chip, int channel)
{
    for(chip = 0; chip < ARRAY_SIZE(CX258X8_PLAT[g_cx258x8_plat].ADDR); chip ++) {
        if(!CX258X8_PLAT[g_cx258x8_plat].ACMD[chip]) {
            RegMaskWrite32(CX258X8_PLAT[g_cx258x8_plat].ADDR[chip],
                CX828_AP_CFG2, 4, 8, 0xa); //choose the source to ADAC
            break;
        }
    }
}
