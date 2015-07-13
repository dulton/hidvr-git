
#include "ad_nvp1108_def.h"

enum {
    NVP110X_SYSM_TYPE1 = 1,
    NVP110X_SYSM_TYPE2 = 2,
    NVP110X_SYSM_TYPE3 = 3,
};

unsigned char NVP110X_ADDR[MAX_IC_NUM] = {0x60, 0x62, 0x64, 0x66};
unsigned char NVP110X_SYID[MAX_IC_NUM] = {0x00, 0x00, 0x00, 0x00};
unsigned char NVP110X_ACMD[MAX_IC_NUM] = {0x00, 0x01, 0x01, 0x01};
unsigned char NVP110X_VCMD[MAX_IC_NUM] = {0x00, 0x01, 0x00, 0x01};
unsigned char NVP110X_ALIC[MAX_IC_NUM] = {0x00, 0x00, 0x00, 0x00};
unsigned char NVP110X_I2CP[MAX_IC_NUM] = {0, 0, 0, 0};
unsigned char NVP110X_SYSM[MAX_IC_NUM] = {0, 0, 0, 0};

enum {
	NVPCLK_108M = 0,
	NVPCLK_144M = 1,
};
static int g_nvp1918_clock = NVPCLK_108M;

#define NVP110X_MAX_IC_CHNLS 8
#define NVP1104_MAX_IC_CHNLS 4

#define NVP1108A_DEV_ID (0x75)
#define NVP1104A_DEV_ID (0x77)
#define NVP1108B_DEV_ID (0x78)

#define NVP1918B_DEV_ID (0x80)
#define NVP1914B_DEV_ID (0x81)

#define NVP1918B_960EXT (0x00)
#define NVP1918B_720EXT (0x40)

#define NVP1918C_DEV_ID (0x82)

#define NVP1108_CHN_PER_CHIP (8)
#define NVC1700_CHN_PER_CHIP (16)

#define NVP1108_DELAY1 0x66

#define NVP1108A_DELAY2 0x64
#define NVP1108B_DELAY2 0x68

#define NVP1108A_DELAY3 0x65
#define NVP1108B_DELAY3 0x70

////////////////////////////////////////////////////////////////////////////////////
// chip detect
////////////////////////////////////////////////////////////////////////////////////
static unsigned char nvp1108_check(int Grp, int chip)
{
    unsigned char Sys_ID = 0;

    // change to bank0
    gpio_i2c_writeX(Grp, NVP110X_ADDR[chip], 0xff, 0x10);
    Sys_ID = gpio_i2c_readX(Grp, NVP110X_ADDR[chip], 0x1b);

    if(  Sys_ID == NVP1108A_DEV_ID
      || Sys_ID == NVP1108B_DEV_ID
      || Sys_ID == NVP1104A_DEV_ID) {
        NVP110X_SYID[chip] = Sys_ID;
        NVP110X_SYSM[chip] = NVP110X_SYSM_TYPE1;
        printk("Grp = %d, Chip = %d, ID = 0x%02X, Addr = 0x%02X Found\n",
            Grp, chip, Sys_ID, NVP110X_ADDR[chip]);
        return Sys_ID;
    }

    gpio_i2c_writeX(Grp, NVP110X_ADDR[chip], 0xff, 0x01);
    Sys_ID = gpio_i2c_readX(Grp, NVP110X_ADDR[chip], 0xF4);
    if(  Sys_ID == NVP1914B_DEV_ID
      || Sys_ID == NVP1918B_DEV_ID) {
        NVP110X_SYID[chip] = Sys_ID;
        NVP110X_SYSM[chip] = NVP110X_SYSM_TYPE2;
        printk("Grp = %d, Chip = %d, ID = 0x%02X, Addr = 0x%02X Found\n",
            Grp, chip, Sys_ID, NVP110X_ADDR[chip]);
        return Sys_ID;
    }
    if(  Sys_ID == NVP1918C_DEV_ID ) {
        NVP110X_SYID[chip] = Sys_ID;
        NVP110X_SYSM[chip] = NVP110X_SYSM_TYPE3;
        printk("Grp = %d, Chip = %d, ID = 0x%02X, Addr = 0x%02X Found\n",
            Grp, chip, Sys_ID, NVP110X_ADDR[chip]);
        return Sys_ID;
    }

    return 0;
}

static unsigned char nvp1108_check_all(int Grp)
{
    int tmpRet = 0;
    int ii;

    for(ii = 0; ii < ARRAY_SIZE(NVP110X_ADDR); ii ++) {
        if(gpio_i2c_probe(NVP110X_ADDR[ii])) {
            if(nvp1108_check(Grp, ii)) {
                tmpRet ++;
            }
        }
    }

    return tmpRet;
}

static unsigned char nvc1700_check(int Grp)
{
    int RetPrb = gpio_i2c_probeX(Grp, NVC1700X_I2C_ADDR);

    return RetPrb;
}

static int  nvp1108_get_vsup(video_sup_t *vsup)
{
    int ii, vsiz;

    if(!vsup) return -1;

    vsiz  = (1 << VIDEO_SUP_FLD1);
    vsiz |= (1 << VIDEO_SUP_FLEF);
	
    for(ii = 0; ii < ARRAY_SIZE(NVP110X_ADDR); ii ++) {
        int  Grp  = 0;
        int  Addr = NVP110X_ADDR[ii];	
        int  Syid = NVP110X_SYID[ii];
		
        if(!gpio_i2c_probeX(Grp, Addr)) {
            continue;
        }

        if((Syid != NVP1918B_DEV_ID)
        && (Syid != NVP1914B_DEV_ID)
        && (Syid != NVP1918C_DEV_ID)) {
            vsiz &= ~(1 << VIDEO_SUP_FLEF);
            continue;
        }

        gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
        if(gpio_i2c_readX(Grp, Addr, 0xF5) == NVP1918B_720EXT) {
            vsiz &= ~(1 << VIDEO_SUP_FLEF);
        }
        else {
            vsiz |= (1 << VIDEO_SUP_FLEF);
        }
    }

    vsup->vsiz = vsiz;

    return 0;
}

static int NetChip_MapTBL[MAX_IC_NUM*NVP110X_MAX_IC_CHNLS][2] = {
#if (defined(GPIO_PLAT_TYPE6) || defined(GPIO_PLAT_TYPE7)) && (MAX_CAM_CH <= 16)
#if (MAX_CAM_CH == 8)
    {-1, 2}, {-1, 3}, {-1, 0}, {-1, 1},
    {-1, 6}, {-1, 7}, {-1, 4}, {-1, 5},
#else
    {-1, 2}, {-1, 3}, {-1, 0}, {-1, 1},
    {-1, 7}, {-1, 6}, {-1, 5}, {-1, 4},
#endif
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
#else
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7},
#endif
};

int nvp110x_MapInit(void)
{
    int ii, jj, kk;
    int tmpCnt;

    for(jj = 0; jj < ARRAY_SIZE(NetChip_MapTBL); jj ++) {
        NetChip_MapTBL[jj][0] = -1;
    }

    for(ii = 0, jj = 0; ii < ARRAY_SIZE(NVP110X_SYID); ii ++) {
        if(0x00 == NVP110X_SYID[ii]) {
            continue;
        }

        tmpCnt = (NVP110X_SYID[ii] == NVP1104A_DEV_ID) ? 4 : 8;

        for(kk = 0; kk < tmpCnt; kk ++) {
            NetChip_MapTBL[jj][0] = ii;
            jj ++;
        }
    }

    return jj;
}

int nvp110x_MapGet(int Chn, int *Chip, int *nChn)
{
    if(    !Chip
        || !nChn
        ||  (Chn >= ARRAY_SIZE(NetChip_MapTBL))
        || (-1 == NetChip_MapTBL[Chn][0])) {

        return -1;
    }

    *Chip = NetChip_MapTBL[Chn][0];
    *nChn = NetChip_MapTBL[Chn][1];

    return 0;
}

const unsigned char NVP_PAL_MODE_INIT[][2] = {
    {0xFF, 0x10},
    {0x84, 0x06}, {0xBB, 0x0f}, {0xE8, 0x00}, {0xE9, 0x0F},
    {0xEC, 0x84}, {0x82, 0xD8}, {0x98, 0xBD}, {0xC7, 0x00},
    {0xC3, 0x40}, {0xAC, 0x00}, {0xB0, 0x04}, {0xB1, 0x04},
    {0xB5, 0xCA}, {0xB9, 0x01}, {0xCA, 0x70}, {0xCC, 0x00},
    {0x73, 0x6C},
};
const unsigned char NVP_NTSC_MODE_INIT[][2] = {
    {0xFF, 0x10},
    {0xE9, 0x0F}, {0x73, 0x6C}, {0xCC, 0x00}, {0x84, 0x76},
    {0xEC, 0x7F}, {0xBA, 0x93}, {0xEA, 0x04}, {0xBB, 0x08},
    {0xE8, 0x00}, {0xCA, 0x71}, {0x82, 0xB8}, {0x98, 0x60},
    {0xAC, 0x02}, {0xB0, 0x00}, {0xB1, 0x00}, {0xB4, 0x1A},
    {0xB5, 0xDC}, {0xB9, 0x82}, {0xBE, 0x9D}, {0xC3, 0x2F},
    {0xC7, 0x04},
};

static void nvp1108_video_mode_init(int chip, int video_mode)
{
    unsigned char chip_addr = NVP110X_ADDR[chip];
    int           Grp       = NVP110X_I2CP[chip];
    int           ii;

    gpio_i2c_writeX(Grp, chip_addr, 0xFF, 0x10);
    gpio_i2c_writeX(Grp, chip_addr, 0x22, 0x0F);  // Video VDO Enable
    gpio_i2c_writeX(Grp, chip_addr, 0x71, 0x00);  // Video AFE Power On

    if(video_mode == NVP1108_PAL) {
        for(ii = 0; ii < ARRAY_SIZE(NVP_PAL_MODE_INIT); ii ++) {
            gpio_i2c_writeX(Grp, chip_addr, NVP_PAL_MODE_INIT[ii][0], NVP_PAL_MODE_INIT[ii][1]);
        }
    }
    else {
        for(ii = 0; ii < ARRAY_SIZE(NVP_NTSC_MODE_INIT); ii ++) {
            gpio_i2c_writeX(Grp, chip_addr, NVP_NTSC_MODE_INIT[ii][0], NVP_NTSC_MODE_INIT[ii][1]);
        }
    }

    gpio_i2c_writeX(Grp, chip_addr, 0xFF, 0x11);
    gpio_i2c_writeX(Grp, chip_addr, 0x11, 0x30);
    gpio_i2c_writeX(Grp, chip_addr, 0x12, 0x30);
    gpio_i2c_writeX(Grp, chip_addr, 0x17, 0x30);
    gpio_i2c_writeX(Grp, chip_addr, 0x18, 0x30);
    gpio_i2c_writeX(Grp, chip_addr, 0x19, 0x30);
    gpio_i2c_writeX(Grp, chip_addr, 0x1A, 0x30);

    gpio_i2c_writeX(Grp, chip_addr, 0xFF, 0x12);
    if(video_mode == NVP1108_PAL) {
        for(ii = 0; ii < 0xE0; ii ++) {
            gpio_i2c_writeX(Grp, chip_addr, ii, NVP1108_PAL_Buf[ii]);
        }
    }
    else {
        for(ii = 0; ii < 0xE0; ii ++) {
            gpio_i2c_writeX(Grp, chip_addr, ii, NVP1108_NT_Buf[ii]);
        }
    }

    switch(NVP110X_SYID[chip]) {
    case NVP1108A_DEV_ID:
    case NVP1108B_DEV_ID:
        if(NVP110X_VCMD[chip] != 0x00) {
            gpio_i2c_writeX(Grp, chip_addr, 0x54, 0x98);
            gpio_i2c_writeX(Grp, chip_addr, 0x55, 0xba);
            gpio_i2c_writeX(Grp, chip_addr, 0xd4, 0xdc);
            gpio_i2c_writeX(Grp, chip_addr, 0xd5, 0xfe);
        }
        break;
    case NVP1104A_DEV_ID:
        if(NVP110X_VCMD[chip] == 0x00) {
            gpio_i2c_writeX(Grp, chip_addr, 0x54, 0x54);
            gpio_i2c_writeX(Grp, chip_addr, 0x55, 0x76);
        }
        if(NVP110X_VCMD[chip] == 0x01) {
            gpio_i2c_writeX(Grp, chip_addr, 0x54, 0x98);
            gpio_i2c_writeX(Grp, chip_addr, 0x55, 0xba);
        }
        if(NVP110X_VCMD[chip] == 0x02) {
            gpio_i2c_writeX(Grp, chip_addr, 0x54, 0xdc);
            gpio_i2c_writeX(Grp, chip_addr, 0x55, 0xfe);
        }
        break;
    }

    if(video_mode == NVP1108_PAL) {
        gpio_i2c_writeX(Grp, chip_addr, 0x07, 0xaa);
        gpio_i2c_writeX(Grp, chip_addr, 0x87, 0xaa);
        gpio_i2c_writeX(Grp, chip_addr, 0x2C, 0xff);
        gpio_i2c_writeX(Grp, chip_addr, 0x2D, 0xff);
        gpio_i2c_writeX(Grp, chip_addr, 0xAC, 0xff);
        gpio_i2c_writeX(Grp, chip_addr, 0xAD, 0xff);
    }
    else {
        gpio_i2c_writeX(Grp, chip_addr, 0x07, 0x55);
        gpio_i2c_writeX(Grp, chip_addr, 0x87, 0x55);
        gpio_i2c_writeX(Grp, chip_addr, 0x2C, 0x66);
        gpio_i2c_writeX(Grp, chip_addr, 0x2D, 0x66);
        gpio_i2c_writeX(Grp, chip_addr, 0xAC, 0x66);
        gpio_i2c_writeX(Grp, chip_addr, 0xAD, 0x66);
    }

    gpio_i2c_writeX(Grp, chip_addr, 0x5C, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0x5D, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0x5E, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0x5F, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0xDC, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0xDD, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0xDE, 0xa8);
    gpio_i2c_writeX(Grp, chip_addr, 0xDF, 0xa8);

    gpio_i2c_writeX(Grp, chip_addr, 0xFF, 0x13);
    gpio_i2c_writeX(Grp, chip_addr, 0x00, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x02, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x04, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x06, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x08, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x0a, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x0c, 0x06);
    gpio_i2c_writeX(Grp, chip_addr, 0x0e, 0x06);
}

unsigned char const NVP1918_720NTSC[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x08, 0xA0}, {0x09, 0xA0}, {0x0A, 0xA0}, {0x0B, 0xA0},

    {0x88, 0xA0}, {0x89, 0xA0}, {0x8A, 0xA0}, {0x8B, 0xA0}, {0x18, 0x22}, {0x19, 0x22}, {0x98, 0x22}, {0x99, 0x22},
    {0x35, 0x82}, {0xB5, 0x82}, {0x40, 0x01}, {0x41, 0x01}, {0x42, 0x01}, {0x43, 0x01}, {0xC0, 0x01}, {0xC1, 0x01},
    {0xC2, 0x01}, {0xC3, 0x01}, {0x4C, 0x00}, {0x4D, 0x00}, {0x4E, 0x00}, {0x4F, 0x00}, {0xCC, 0x00}, {0xCD, 0x00},
    {0xCE, 0x00}, {0xCF, 0x00}, {0x50, 0x00}, {0x51, 0x00}, {0x52, 0x00}, {0x53, 0x00}, {0xD0, 0x00}, {0xD1, 0x00},
    {0xD2, 0x00}, {0xD3, 0x00}, {0x54, 0xF1}, {0xD4, 0xF1},
#if 1 //H_DELAY in 108M Clock Mode
    {0x58, 0x44}, {0x59, 0x44}, {0x5A, 0x44}, {0x5B, 0x44}, {0xD8, 0x44}, {0xD9, 0x44}, {0xDA, 0x44}, {0xDB, 0x44},
#else
    {0x58, 0x96}, {0x59, 0x96}, {0x5A, 0x96}, {0x5B, 0x96}, {0xD8, 0x96}, {0xD9, 0x96}, {0xDA, 0x96}, {0xDB, 0x96},
#endif
    {0x64, 0x08}, {0x65, 0x08}, {0x66, 0x08}, {0x67, 0x08}, {0xE4, 0x08}, {0xE5, 0x08}, {0xE6, 0x08}, {0xE7, 0x08},

    {0xFF, 0x01},//Bank1
    {0xA0, 0x40},
#if 1
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70},
    {0xCE, 0x70}, {0xCF, 0x70},
#else
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70},
    {0xCE, 0x70}, {0xCF, 0x70},
#endif

    {0xFF, 0x03}, //Bank3
    {0x20, 0x80}, {0x25, 0xDC}, {0x53, 0x00}, {0x68, 0xff}, {0x69, 0xff},
#if 1 //Working in 108M Clock Mode
    {0x62, 0x00}, {0x63, 0x00}, {0x81, 0x00}, {0x83, 0x00}, {0x85, 0x00}, {0x87, 0x00}, {0x89, 0x00}, {0x8B, 0x00},
    {0x8D, 0x00}, {0x8F, 0x00},
#else
    {0x62, 0x01}, {0x63, 0xFF}, {0x81, 0x40}, {0x83, 0x40}, {0x85, 0x40}, {0x87, 0x40}, {0x89, 0x40}, {0x8B, 0x40},
    {0x8D, 0x40}, {0x8F, 0x40},
#endif
    {0x90, 0x08}, {0x91, 0x08}, {0x92, 0x08}, {0x93, 0x08}, {0x94, 0x08}, {0x95, 0x08}, {0x96, 0x08}, {0x97, 0x08},
};

unsigned char const NVP1918_720PAL[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x08, 0xdd}, {0x09, 0xdd}, {0x0A, 0xdd}, {0x0B, 0xdd},

    {0x88, 0xdd}, {0x89, 0xdd}, {0x8A, 0xdd}, {0x8B, 0xdd}, {0x18, 0x33}, {0x19, 0x33}, {0x98, 0x33}, {0x99, 0x33},
    {0x35, 0x02}, {0xB5, 0x02}, {0x40, 0x00}, {0x41, 0x00}, {0x42, 0x00}, {0x43, 0x00}, {0xC0, 0x00}, {0xC1, 0x00},
    {0xC2, 0x00}, {0xC3, 0x00}, {0x4C, 0x04}, {0x4D, 0x04}, {0x4E, 0x04}, {0x4F, 0x04}, {0xCC, 0x04}, {0xCD, 0x04},
    {0xCE, 0x04}, {0xCF, 0x04}, {0x50, 0x04}, {0x51, 0x04}, {0x52, 0x04}, {0x53, 0x04}, {0xD0, 0x04}, {0xD1, 0x04},
    {0xD2, 0x04}, {0xD3, 0x04}, {0x54, 0x01}, {0xD4, 0x01},
#if 1 //H_DELAY in 108M Clock Mode
    {0x58, 0x50}, {0x59, 0x50}, {0x5A, 0x50}, {0x5B, 0x50}, {0xD8, 0x50}, {0xD9, 0x50}, {0xDA, 0x50}, {0xDB, 0x50},
#else
    {0x58, 0xc0}, {0x59, 0xc0}, {0x5A, 0xc0}, {0x5B, 0xc0}, {0xD8, 0xc0}, {0xD9, 0xc0}, {0xDA, 0xc0}, {0xDB, 0xc0},
#endif
    {0x64, 0x0D}, {0x65, 0x0D}, {0x66, 0x0D}, {0x67, 0x0D}, {0xE4, 0x0D}, {0xE5, 0x0D}, {0xE6, 0x0D}, {0xE7, 0x0D},

    {0xFF, 0x01}, //Bank1
    {0xA0, 0x4D},
#if 0
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70}, {0xCE, 0x70}, {0xCF, 0x70},
#else
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70}, {0xCE, 0x70}, {0xCF, 0x70},
#endif

    {0xFF, 0x03}, //Bank3
    {0x20, 0x80}, {0x25, 0xCC}, {0x53, 0x00}, {0x68, 0xff}, {0x69, 0xff},
#if 1 //Working in 108M Clock Mode
    {0x62, 0x00}, {0x63, 0x00}, {0x81, 0x00}, {0x83, 0x00}, {0x85, 0x00},{0x87, 0x00}, {0x89, 0x00}, {0x8B, 0x00},
    {0x8D, 0x00}, {0x8F, 0x00},
#else
    {0x62, 0x01}, {0x63, 0xFF}, {0x81, 0x40}, {0x83, 0x40}, {0x85, 0x40},{0x87, 0x40}, {0x89, 0x40}, {0x8B, 0x40},
    {0x8D, 0x40}, {0x8F, 0x40},
#endif
    {0x90, 0x01}, {0x91, 0x01}, {0x92, 0x01}, {0x93, 0x01}, {0x94, 0x01},{0x95, 0x01}, {0x96, 0x01}, {0x97, 0x01},
};

unsigned char const NVP1918_960NTSC[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x08, 0xA0}, {0x09, 0xA0}, {0x0A, 0xA0}, {0x0B, 0xA0},

    {0x88, 0xA0}, {0x89, 0xA0}, {0x8A, 0xA0}, {0x8B, 0xA0}, {0x18, 0x00}, {0x19, 0x00}, {0x98, 0x00}, {0x99, 0x00},
    {0x35, 0x82}, {0xB5, 0x82}, {0x40, 0x01}, {0x41, 0x01}, {0x42, 0x01}, {0x43, 0x01}, {0xC0, 0x01}, {0xC1, 0x01},
    {0xC2, 0x01}, {0xC3, 0x01}, {0x4C, 0x00}, {0x4D, 0x00}, {0x4E, 0x00}, {0x4F, 0x00}, {0xCC, 0x00}, {0xCD, 0x00},
    {0xCE, 0x00}, {0xCF, 0x00}, {0x50, 0x00}, {0x51, 0x00}, {0x52, 0x00}, {0x53, 0x00}, {0xD0, 0x00}, {0xD1, 0x00},
    {0xD2, 0x00}, {0xD3, 0x00}, {0x54, 0xF1}, {0xD4, 0xF1},
#if 1 //H_DELAY in 144M Clock Mode
	{0x58, 0xAD}, {0x59, 0xAD}, {0x5A, 0xAD}, {0x5B, 0xAD}, {0xD8, 0xAD}, {0xD9, 0xAD}, {0xDA, 0xAD}, {0xDB, 0xAD},
#endif
	{0x64, 0x08}, {0x65, 0x08}, {0x66, 0x08}, {0x67, 0x08}, {0xE4, 0x08}, {0xE5, 0x08}, {0xE6, 0x08}, {0xE7, 0x08},

    {0xFF, 0x01}, //Bank1
    {0xA0, 0x40},

#if 1 //Working in 144M Clock Mode
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70}, {0xCE, 0x70}, {0xCF, 0x70},
#endif

    {0xFF, 0x03}, //Bank3
    {0x20, 0x80}, {0x62, 0x01}, //{0x68, 0xff}, {0x69, 0xff},

    {0x25, 0xDC}, {0x53, 0xFF}, {0x63, 0x00}, {0x80, 0x40}, {0x81, 0x08}, {0x83, 0x08}, {0x85, 0x08}, {0x87, 0x08},
    {0x89, 0x08}, {0x8B, 0x08}, {0x8D, 0x08}, {0x8F, 0x08}, {0x90, 0x08}, {0x91, 0x08}, {0x92, 0x08}, {0x93, 0x08},
    {0x94, 0x08}, {0x95, 0x08}, {0x96, 0x08}, {0x97, 0x08},
};

unsigned char const NVP1918_960PAL[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x08, 0xdd}, {0x09, 0xdd}, {0x0A, 0xdd}, {0x0B, 0xdd},

    {0x88, 0xdd}, {0x89, 0xdd}, {0x8A, 0xdd}, {0x8B, 0xdd}, {0x18, 0x00}, {0x19, 0x00}, {0x98, 0x00}, {0x99, 0x00},
    {0x35, 0x02}, {0xB5, 0x02}, {0x40, 0x00}, {0x41, 0x00}, {0x42, 0x00}, {0x43, 0x00}, {0xC0, 0x00}, {0xC1, 0x00},
    {0xC2, 0x00}, {0xC3, 0x00}, {0x4C, 0x04}, {0x4D, 0x04}, {0x4E, 0x04}, {0x4F, 0x04}, {0xCC, 0x04}, {0xCD, 0x04},
    {0xCE, 0x04}, {0xCF, 0x04}, {0x50, 0x04}, {0x51, 0x04}, {0x52, 0x04}, {0x53, 0x04}, {0xD0, 0x04}, {0xD1, 0x04},
    {0xD2, 0x04}, {0xD3, 0x04}, {0x54, 0x01}, {0xD4, 0x01},
#if 1 //H_DELAY in 144M Clock Mode
	{0x58, 0xC3}, {0x59, 0xC3}, {0x5A, 0xC3}, {0x5B, 0xC3},{0xD8, 0xC3}, {0xD9, 0xC3}, {0xDA, 0xC3}, {0xDB, 0xC3},
#endif
	{0x64, 0x0D}, {0x65, 0x0D}, {0x66, 0x0D}, {0x67, 0x0D}, {0xE4, 0x0D}, {0xE5, 0x0D}, {0xE6, 0x0D}, {0xE7, 0x0D},

    {0xFF, 0x01}, //Bank1
    {0xA0, 0x4D},

#if 1 //Working in 144M Clock Mode
    {0xC0, 0x10}, {0xC1, 0x32}, {0xC2, 0x54}, {0xC3, 0x76}, {0xC4, 0x10}, {0xC5, 0x32}, {0xC6, 0x54}, {0xC7, 0x76},
    {0xC8, 0x88}, {0xC9, 0xFF}, {0xCC, 0x70}, {0xCD, 0x70}, {0xCE, 0x70}, {0xCF, 0x70},
#endif

    {0xFF, 0x03}, //Bank3
    {0x20, 0x80}, {0x62, 0x01}, //{0x68, 0xff}, {0x69, 0xff},

    {0x25, 0xCC}, {0x53, 0xFF}, {0x63, 0x00}, {0x80, 0x40}, {0x81, 0x08}, {0x83, 0x08}, {0x85, 0x08}, {0x87, 0x08},
    {0x89, 0x08}, {0x8B, 0x08}, {0x8D, 0x08}, {0x8F, 0x08}, {0x90, 0x01}, {0x91, 0x01}, {0x92, 0x01}, {0x93, 0x01},
    {0x94, 0x01}, {0x95, 0x01}, {0x96, 0x01}, {0x97, 0x01},
};

unsigned char const NVP1918C_720NTSC[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x58, 0x58}, {0x59, 0x58}, {0x5A, 0x58}, {0x5B, 0x58}, //H_delay Setting
    {0xD8, 0x58}, {0xD9, 0x58}, {0xDA, 0x58}, {0xDB, 0x58},

    {0xFF, 0x01}, //Bank1
    {0xC0, 0x54}, {0xC1, 0x76}, {0xC2, 0x10}, {0xC3, 0x32}, //Channel Order
    {0xCC, 0x74}, {0xcd, 0x74},  //Video Output Setting

    {0xFF, 0x03}, //Bank3
    {0x53, 0x00},
    {0x62, 0x00}, {0x63, 0x00}, {0x81, 0x00}, {0x83, 0x00},
    {0x85, 0x00}, {0x87, 0x00}, {0x89, 0x00}, {0x8B, 0x00},
    {0x8D, 0x00}, {0x8F, 0x00},
};

unsigned char const NVP1918C_720PAL[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x58, 0x60}, {0x59, 0x60}, {0x5A, 0x60}, {0x5B, 0x60}, //H_delay Setting
    {0xD8, 0x60}, {0xD9, 0x60}, {0xDA, 0x60}, {0xDB, 0x60},

    {0xFF, 0x01}, //Bank1
    {0xC0, 0x54}, {0xC1, 0x76}, {0xC2, 0x10}, {0xC3, 0x32}, //Channel Order
    {0xCC, 0x74}, {0xcd, 0x74},  //Video Output Setting

    {0xFF, 0x03}, //Bank3
    {0x53, 0x00},
    {0x62, 0x00}, {0x63, 0x00}, {0x81, 0x00}, {0x83, 0x00},
    {0x85, 0x00}, {0x87, 0x00}, {0x89, 0x00}, {0x8B, 0x00},
    {0x8D, 0x00}, {0x8F, 0x00},
};

unsigned char const NVP1918C_960NTSC[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x58, 0x20}, {0x59, 0x20}, {0x5A, 0x20}, {0x5B, 0x20}, //H_delay Setting
    {0xD8, 0x20}, {0xD9, 0x20}, {0xDA, 0x20}, {0xDB, 0x20},

    {0xFF, 0x01}, //Bank1
    {0xC0, 0x54}, {0xC1, 0x76}, {0xC2, 0x10}, {0xC3, 0x32}, //Channel Order
    {0xCC, 0x74}, {0xcd, 0x74},  //Video Output Setting
};

unsigned char const NVP1918C_960PAL[][2] = {
    {0xFF, 0x00}, //Bank0
    {0x58, 0x40}, {0x59, 0x40}, {0x5A, 0x40}, {0x5B, 0x40}, //H_delay Setting
    {0xD8, 0x40}, {0xD9, 0x40}, {0xDA, 0x40}, {0xDB, 0x40},

    {0xFF, 0x01}, //Bank1
    {0xC0, 0x54}, {0xC1, 0x76}, {0xC2, 0x10}, {0xC3, 0x32}, //Channel Order
    {0xCC, 0x74}, {0xcd, 0x74},  //Video Output Setting
};

unsigned char const NVP1918C_NTSC_Color_Adjust[][2] = {
	{0xFF, 0x00}, //Bank0

	//Brightness
	{0x0c, 0x04}, {0x0d, 0x04}, {0x0e, 0x04}, {0x0f, 0x04},
	{0x8c, 0x04}, {0x8d, 0x04}, {0x8e, 0x04}, {0x8f, 0x04},

	{0x10, 0x6c}, {0x11, 0x6c}, {0x12, 0x6c}, {0x13, 0x6c},
	{0x90, 0x6c}, {0x91, 0x6c}, {0x92, 0x6c}, {0x93, 0x6c},

	//Saturation
	{0x3C, 0x80}, {0x3D, 0x80}, {0x3E, 0x80}, {0x3F, 0x80},
	{0xBC, 0x80}, {0xBD, 0x80}, {0xBE, 0x80}, {0xBF, 0x80},

	//sharpness
	{0x14, 0xAF}, {0x15, 0xAF}, {0x16, 0xAF}, {0x17, 0xAF},
	{0x94, 0xAF}, {0x95, 0xAF}, {0x96, 0xAF}, {0x97, 0xAF},

	{0x1C, 0xA4}, {0x1D, 0xA4}, {0x1E, 0xA4}, {0x1F, 0xA4},
	{0x9C, 0xA4}, {0x9D, 0xA4}, {0x9E, 0xA4}, {0x9F, 0xA4},

	{0x1A, 0x33}, {0x1B, 0x33}, {0x9A, 0x33}, {0x9B, 0x33},

	//hdelay
	{0x58, 0x48}, {0x59, 0x48}, {0x5A, 0x48}, {0x5B, 0x48},
	{0xD8, 0x48}, {0xD9, 0x48}, {0xDA, 0x48}, {0xDB, 0x48},

	{0xFF, 0x03}, //Bank3

	{0x1B, 0x14}, {0x1C, 0x50},

	{0x90, 0x0D}, {0x91, 0x0D}, {0x92, 0x0D}, {0x93, 0x0D},
	{0x94, 0x0D}, {0x95, 0x0D}, {0x96, 0x0D}, {0x97, 0x0D},

	{0xC1, 0x50}, {0xC7, 0x40}, {0x21, 0x24},
};

unsigned char const NVP1918C_PAL_Color_Adjust[][2] = {
	{0xFF, 0x00}, //Bank0

	//Brightness
	{0x0c, 0x08}, {0x0d, 0x08}, {0x0e, 0x08}, {0x0f, 0x08},
	{0x8c, 0x08}, {0x8d, 0x08}, {0x8e, 0x08}, {0x8f, 0x08},

	{0x10, 0x68}, {0x11, 0x68}, {0x12, 0x68}, {0x13, 0x68},
	{0x90, 0x68}, {0x91, 0x68}, {0x92, 0x68}, {0x93, 0x68},

	//Saturation
	{0x3C, 0x80}, {0x3D, 0x80}, {0x3E, 0x80}, {0x3F, 0x80},
	{0xBC, 0x80}, {0xBD, 0x80}, {0xBE, 0x80}, {0xBF, 0x80},

	//V_Gain
	{0x48, 0x20}, {0x49, 0x20}, {0x4A, 0x20}, {0x4B, 0x20},
	{0xC8, 0x20}, {0xC9, 0x20}, {0xCA, 0x20}, {0xCB, 0x20},

	//Hue
	{0x40, 0x01}, {0x41, 0x01}, {0x42, 0x01}, {0x43, 0x01},
	{0xC0, 0x01}, {0xC1, 0x01}, {0xC2, 0x01}, {0xC3, 0x01},

	//Sharpness
	{0x14, 0xAF}, {0x15, 0xAF}, {0x16, 0xAF}, {0x17, 0xAF},
	{0x94, 0xAF}, {0x95, 0xAF}, {0x96, 0xAF}, {0x97, 0xAF},

	{0x1C, 0xA4}, {0x1D, 0xA4}, {0x1E, 0xA4}, {0x1F, 0xA4},
	{0x9C, 0xA4}, {0x9D, 0xA4}, {0x9E, 0xA4}, {0x9F, 0xA4},

	{0x1A, 0x00}, {0x1B, 0x00}, {0x9A, 0x00}, {0x9B, 0x00},

	//hdelay
	{0x58, 0x5C}, {0x59, 0x5C}, {0x5A, 0x5C}, {0x5B, 0x5C},
	{0xD8, 0x5C}, {0xD9, 0x5C}, {0xDA, 0x5C}, {0xDB, 0x5C},

	{0xFF, 0x03}, //Bank3

	{0x1B, 0x14}, {0x1C, 0x50},

	{0x90, 0x0D}, {0x91, 0x0D}, {0x92, 0x0D}, {0x93, 0x0D},
	{0x94, 0x0D}, {0x95, 0x0D}, {0x96, 0x0D}, {0x97, 0x0D},

	{0xC1, 0x50}, {0xC7, 0x38}, {0x21, 0x24},
};

void nvp1118_mode_verfix(int chip, int video_mode, int video_vsiz)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];
    int  j;

	if(NVP1914B_DEV_ID == NVP110X_SYID[chip]) {
		int ChnCnt = 4;
		if(g_nvp1918_clock == NVPCLK_108M) {  //108MHz Working Clock;
			// bank0
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x00);
			for(j = 0; j < ChnCnt; j ++) {
				gpio_i2c_writeX(Grp, Addr, (0x30+j), 0x0C); //Y_delay setting
				gpio_i2c_writeX(Grp, Addr, (0x58+j),
					(NVP1108_PAL == video_mode) ?0x4B :0x2F); //h_delay VALUE NEEDS TO BE ODD!!! 12-14
			}

			// bank3
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x03);
			for(j = 0; j < ChnCnt; j ++) {
				gpio_i2c_writeX(Grp, Addr, (0x54+j), 0x00);
			}

			gpio_i2c_writeX(Grp, Addr, 0x61, 0x00);
			gpio_i2c_writeX(Grp, Addr, 0x62, 0x00);
			gpio_i2c_writeX(Grp, Addr, 0x64, 0x00);
			gpio_i2c_writeX(Grp, Addr, 0x65, 0x00);
			for(j = 0; j < ChnCnt; j ++) {
				gpio_i2c_writeX(Grp, Addr, (0x81+j*2), 0x08);
			}
		}
		else {  //144MHz Working Clock;
			// bank0
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x00);
			for(j = 0; j < ChnCnt; j ++) {
				if(NVP1108_PAL == video_mode) {
					gpio_i2c_writeX(Grp, Addr, (0x30+j),
						(NVP1108_720EXT == video_vsiz) ?0x11 :0x13);  //Y_delay setting
					gpio_i2c_writeX(Grp, Addr, (0x58+j),
						(NVP1108_720EXT == video_vsiz) ?0x7F :0x07);  //h_delay VALUE NEEDS TO BE ODD!!! 12-14
				}
				else {
					gpio_i2c_writeX(Grp, Addr, (0x30+j),
						(NVP1108_720EXT == video_vsiz) ?0x12 :0x14);
					gpio_i2c_writeX(Grp, Addr, (0x58+j),
						(NVP1108_720EXT == video_vsiz) ?0x6D :0xE7);
				}
			}

			// bank3
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x03);
			for(j = 0; j < ChnCnt; j ++) {
				gpio_i2c_writeX(Grp, Addr, (0x54+j),
					(NVP1108_720EXT == video_vsiz) ?0x8B :0xBB);
			}

			gpio_i2c_writeX(Grp, Addr, 0x61, 0x53);
			gpio_i2c_writeX(Grp, Addr, 0x62, 0x89);
			gpio_i2c_writeX(Grp, Addr, 0x64, 0x22);
			gpio_i2c_writeX(Grp, Addr, 0x65, 0x22);
			for(j = 0; j < ChnCnt; j ++) {
				if(NVP1108_720EXT == video_vsiz) {
					gpio_i2c_writeX(Grp, Addr, (0x81+j*2), 0x48);
				}
				else {
					gpio_i2c_writeX(Grp, Addr, (0x81+j*2),
						(NVP1108_PAL == video_mode) ?0x28 :0x08);
				}
			}
		}
	}
	else if(NVP1918B_DEV_ID == NVP110X_SYID[chip]) {
		int ChnCnt = 8;
		if(g_nvp1918_clock == NVPCLK_108M) {  //108MHz Working Clock;
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x03);
			gpio_i2c_writeX(Grp, Addr, 0x64, 0x00);
			gpio_i2c_writeX(Grp, Addr, 0x65, 0x00);
		}
		else { //144MHz Working Clock;
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x00);
			for(j = 0; j < ChnCnt; j ++) {
				if(NVP1108_PAL == video_mode) {
					gpio_i2c_writeX(Grp, Addr, (0x30+j%4+j/4*0x80),
						(NVP1108_720EXT == video_vsiz) ?0x11 :0x11);
					gpio_i2c_writeX(Grp, Addr, (0x58+j%4+j/4*0x80),
						(NVP1108_720EXT == video_vsiz) ?0xA7 :0xB8);
				}
				else {
					gpio_i2c_writeX(Grp, Addr, (0x30+j%4+j/4*0x80),
						(NVP1108_720EXT == video_vsiz) ?0x12 :0x0A);
					gpio_i2c_writeX(Grp, Addr, (0x58+j%4+j/4*0x80),
						(NVP1108_720EXT == video_vsiz) ?0x96 :0x9C);
				}
			}
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x03);
			for(j = 0; j < ChnCnt; j ++) {
				gpio_i2c_writeX(Grp, Addr, (0x81+j*2),
					(NVP1108_720EXT == video_vsiz) ?0x48 :0x08);
			}
		}
	}
}

static void nvp1118_system_init(int chip)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];

    //nvp1118_system_init
    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
    if(g_nvp1918_clock == NVPCLK_108M) {
        printk("NVP1918 Clock 108M Mode\n");
        gpio_i2c_writeX(Grp, Addr, 0x4A, 0x22);
        gpio_i2c_writeX(Grp, Addr, 0x4B, 0x20);
    }
    else {
        printk("NVP1918 Clock 144M Mode\n");
        gpio_i2c_writeX(Grp, Addr, 0x4A, 0x03);
        gpio_i2c_writeX(Grp, Addr, 0x4B, 0x10);
    }
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x61);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
    msleep(100);
	
    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x40);
    gpio_i2c_writeX(Grp, Addr, 0x49, 0x02);
    msleep(10);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
    gpio_i2c_writeX(Grp, Addr, 0x49, 0x00);
    msleep(10);

    if(g_nvp1918_clock == NVPCLK_108M) {
        gpio_i2c_writeX(Grp, Addr, 0x5E, 0x80);
        gpio_i2c_writeX(Grp, Addr, 0x38, 0x10);
        gpio_i2c_writeX(Grp, Addr, 0x38, 0x00);
    }
    else {
        gpio_i2c_writeX(Grp, Addr, 0x5E, 0x00);
        gpio_i2c_writeX(Grp, Addr, 0x38, 0x10);
        gpio_i2c_writeX(Grp, Addr, 0x38, 0x00);
    }

    printk("~~~~~~~~~~~~~~~~S/W reset~~~~~~~~~~~~~~~~\n");
    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x40);
    msleep(10);
    gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
    msleep(50);

    printk("~~~~~~~~~~~~~~~~Read Register~~~~~~~~~~~~~~~\n");
    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0xFF, gpio_i2c_readX(Grp, Addr, 0xFF));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x44, gpio_i2c_readX(Grp, Addr, 0x44));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x47, gpio_i2c_readX(Grp, Addr, 0x47));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x4a, gpio_i2c_readX(Grp, Addr, 0x4A));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x4b, gpio_i2c_readX(Grp, Addr, 0x4B));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x48, gpio_i2c_readX(Grp, Addr, 0x48));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x49, gpio_i2c_readX(Grp, Addr, 0x49));
    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x03);
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x68, gpio_i2c_readX(Grp, Addr, 0x68));
    printk("Read ID : %x reg[%x] = %x\n", Addr, 0x69, gpio_i2c_readX(Grp, Addr, 0x69));
    printk("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

static void nvp1918c_system_init(int chip)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];

	//Software Reset
	gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
	gpio_i2c_writeX(Grp, Addr, 0x48, 0x40);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x02);
	gpio_i2c_writeX(Grp, Addr, 0x48, 0x61);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x00);
	msleep(100);

	gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x00);
	msleep(100);
}

static void nvp1118_video_mode_init(int chip, int video_mode, int video_vsiz)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];
    int  ii, jj;

    g_nvp1918_clock = (NVP1108_720EXT != video_vsiz) ?NVPCLK_144M :NVPCLK_108M;

    //nvp1118_format_init
    if(NVP1108_PAL == video_mode) {
        for(ii = 0; ii < ARRAY_SIZE(NVP1918_FPGA_PAL_Buf); ii ++) {
            gpio_i2c_writeX(Grp, Addr, 0xFF, ii);
            for(jj = 0; jj < ARRAY_SIZE(NVP1918_FPGA_PAL_Buf[ii]); jj ++) {
                gpio_i2c_writeX(Grp, Addr, jj, NVP1918_FPGA_PAL_Buf[ii][jj]);
            }
        }
    }
    else {
        for(ii = 0; ii < ARRAY_SIZE(NVP1918_FPGA_NT_Buf); ii ++) {
            gpio_i2c_writeX(Grp, Addr, 0xFF, ii);
            for(jj = 0; jj < ARRAY_SIZE(NVP1918_FPGA_NT_Buf[ii]); jj ++) {
                gpio_i2c_writeX(Grp, Addr, jj, NVP1918_FPGA_NT_Buf[ii][jj]);
            }
        }
    }

    //nvp1118_format
    if(NVP1108_PAL == video_mode) {
		if(NVP1108_720EXT == video_vsiz) {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918_720PAL); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918_720PAL[ii][0], NVP1918_720PAL[ii][1]);
			}
		}
		else {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918_960PAL); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918_960PAL[ii][0], NVP1918_960PAL[ii][1]);
			}
		}
    }
    else {
		if(NVP1108_720EXT == video_vsiz) {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918_720NTSC); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918_720NTSC[ii][0], NVP1918_720NTSC[ii][1]);
			}
		}
		else {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918_960NTSC); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918_960NTSC[ii][0], NVP1918_960NTSC[ii][1]);
			}
		}
    }

    switch(NVP110X_SYID[chip]) {
    case NVP1918B_DEV_ID:
        if(NVP110X_VCMD[chip] != 0x00) {
            gpio_i2c_writeX(Grp, Addr, 0xFF, 0x00);
            gpio_i2c_writeX(Grp, Addr, 0x55, 0x98);
            gpio_i2c_writeX(Grp, Addr, 0x56, 0xba);
            gpio_i2c_writeX(Grp, Addr, 0xD5, 0xdc);
            gpio_i2c_writeX(Grp, Addr, 0xD6, 0xfe);
        }
        break;
    }

    nvp1118_mode_verfix(chip, video_mode, video_vsiz);
}

#define NVP1918C_UNIT_NUMB (0x04)
#define NVP1918C_CHNL_NUMB (0x08) 
#define NVP1918C_UNIT_SHFT (0x80)
#define NVP1918C_UNIT_LOOP(X) \
    for(X = 0; X < NVP1918C_CHNL_NUMB; X ++)
#define NVP1918C_CHNL_SEEK(BaseReg, Chnl) \
    ((BaseReg) + (((Chnl) < NVP1918C_UNIT_NUMB) \
            ? (Chnl) \
            : (NVP1918C_UNIT_SHFT + ((Chnl)%NVP1918C_UNIT_NUMB))))

static void nvp1918c_video_mode_init(int chip, int video_mode, int video_vsiz)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];
    int  ii, jj;

    g_nvp1918_clock = (NVP1108_720EXT != video_vsiz) ?NVPCLK_144M :NVPCLK_108M;

    //nvp1118_format_init
    if(NVP1108_PAL == video_mode) {
        for(ii = 0; ii < ARRAY_SIZE(NVP1918C_PAL_Buf_960H); ii ++) {
            gpio_i2c_writeX(Grp, Addr, 0xFF, ii);
            for(jj = 0; jj < ARRAY_SIZE(NVP1918C_PAL_Buf_960H[ii]); jj ++) {
                gpio_i2c_writeX(Grp, Addr, jj, NVP1918C_PAL_Buf_960H[ii][jj]);
            }
        }
    }
    else {
        for(ii = 0; ii < ARRAY_SIZE(NVP1918C_NTSC_Buf_960H); ii ++) {
            gpio_i2c_writeX(Grp, Addr, 0xFF, ii);
            for(jj = 0; jj < ARRAY_SIZE(NVP1918C_NTSC_Buf_960H[ii]); jj ++) {
                gpio_i2c_writeX(Grp, Addr, jj, NVP1918C_NTSC_Buf_960H[ii][jj]);
            }
        }
    }

    if(NVP1108_PAL == video_mode) {
		if(NVP1108_720EXT == video_vsiz) {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918C_720PAL); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918C_720PAL[ii][0], NVP1918C_720PAL[ii][1]);
			}
		}
		else {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918C_960PAL); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918C_960PAL[ii][0], NVP1918C_960PAL[ii][1]);
			}
		}
    }
    else {
		if(NVP1108_720EXT == video_vsiz) {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918C_720NTSC); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918C_720NTSC[ii][0], NVP1918C_720NTSC[ii][1]);
			}
		}
		else {
			for(ii = 0; ii < ARRAY_SIZE(NVP1918C_960NTSC); ii ++) {
				gpio_i2c_writeX(Grp, Addr, NVP1918C_960NTSC[ii][0], NVP1918C_960NTSC[ii][1]);
			}
		}
    }

    if(NVP1108_PAL == video_mode) {
		for(ii = 0; ii < ARRAY_SIZE(NVP1918C_PAL_Color_Adjust); ii ++) {
		    gpio_i2c_writeX(Grp, Addr, NVP1918C_PAL_Color_Adjust[ii][0], NVP1918C_PAL_Color_Adjust[ii][1]);
		}

		if(NVP1108_720EXT == video_vsiz) {
            //720H
            gpio_i2c_writeX(Grp, Addr, 0xFF, 0x04);
            gpio_i2c_writeX(Grp, Addr, 0x56, 0x00);
		}
		else {
			//960H
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x04);
			gpio_i2c_writeX(Grp, Addr, 0x56, 0xFF);
			gpio_i2c_writeX(Grp, Addr, 0x78, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x79, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7A, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7B, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7C, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7D, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7E, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7F, 0x1C);
		}
    }
    else {
		for(ii = 0; ii < ARRAY_SIZE(NVP1918C_NTSC_Color_Adjust); ii ++) {
		    gpio_i2c_writeX(Grp, Addr, NVP1918C_NTSC_Color_Adjust[ii][0], NVP1918C_NTSC_Color_Adjust[ii][1]);
		}

		if(NVP1108_720EXT == video_vsiz) {
            //720H
            gpio_i2c_writeX(Grp, Addr, 0xFF, 0x04);
            gpio_i2c_writeX(Grp, Addr, 0x56, 0x00);
		}
		else {
			//960H
			gpio_i2c_writeX(Grp, Addr, 0xFF, 0x04);
			gpio_i2c_writeX(Grp, Addr, 0x56, 0xFF);
			gpio_i2c_writeX(Grp, Addr, 0x78, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x79, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7A, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7B, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7C, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7D, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7E, 0x1C);
			gpio_i2c_writeX(Grp, Addr, 0x7F, 0x1C);
		}
    }

	//System_Init_GrainMedia
	gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
	gpio_i2c_writeX(Grp, Addr, 0x48, 0x40);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x02);

	if(NVP1108_720EXT != video_vsiz) {
		gpio_i2c_writeX(Grp, Addr, 0x4A, 0x03);
		gpio_i2c_writeX(Grp, Addr, 0x4B, 0x10);
	}
	else {
		gpio_i2c_writeX(Grp, Addr, 0x4A, 0x22);
		gpio_i2c_writeX(Grp, Addr, 0x4B, 0x20);
	}

	gpio_i2c_writeX(Grp, Addr, 0x48, 0x61);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x00);
	msleep(100);

	gpio_i2c_writeX(Grp, Addr, 0x48, 0x60);
	gpio_i2c_writeX(Grp, Addr, 0x49, 0x00);
	if(NVP1108_720EXT == video_vsiz) {
		gpio_i2c_writeX(Grp, Addr, 0x5e, 0x80);
	}
	else {
		gpio_i2c_writeX(Grp, Addr, 0x5e, 0x00);
	}
	gpio_i2c_writeX(Grp, Addr, 0x38, 0x18);
	gpio_i2c_writeX(Grp, Addr, 0x38, 0x08);
	msleep(100);
}

enum {
    NVC1700_OUTMODE_2D1_8CIF = 0,
    NVC1700_OUTMODE_1D1_16CIF,
    NVC1700_OUTMODE_4D1_16CIF,
    NVC1700_OUTMODE_BUTT
};

enum {
    NVC1700_VM_NTSC = 0,
    NVC1700_VM_PAL,
    NVC1700_VM_BUTT
};

static int nvc1700_init(int Grp, unsigned char vformat, unsigned char outmode)
{
    int i = 0;
    unsigned char NVC1700_INIT_TABLE[][2] = {
        {0x00, 0x0e}, {0x01, 0x0a}, {0x02, 0x0e}, {0x03, 0x0a},
        {0x04, 0x1e}, {0x05, 0x1a}, {0x06, 0x1e}, {0x07, 0x1a},
        {0x08, 0x2e}, {0x09, 0x2a}, {0x0a, 0x2e}, {0x0b, 0x2a},
        {0x0c, 0x3e}, {0x0d, 0x3a}, {0x0e, 0x3e}, {0x0f, 0x3a}
    };

    unsigned char NVC1700_DAT_SEQ_TBL[] = {
#if defined(GPIO_PLAT_TYPE6) && (MAX_CAM_CH > 16)
        0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE
#else
        0x98, 0xBA, 0xDC, 0xFE, 0x10, 0x32, 0x54, 0x76
#endif
    };

    if(vformat >= NVC1700_VM_BUTT || outmode >= NVC1700_OUTMODE_BUTT) {
        printk("param error vformat[%d] outmode[%d]\n", vformat, outmode);
        return -1;
    }

    for(i = 0; i < ARRAY_SIZE(NVC1700_INIT_TABLE); i ++) {
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, NVC1700_INIT_TABLE[i][0], NVC1700_INIT_TABLE[i][1]);
    }

    if(outmode == NVC1700_OUTMODE_2D1_8CIF) {
        unsigned char NVC1700_2D1_8CIF[][2] = {
            {0x1a, 0x00}, {0x1b, 0x80}, {0x80, 0x00}, {0x18, 0x91},
            {0xa0, 0x45}, {0xa1, 0x00}, {0xa4, 0x40}, {0xa5, 0x70},
            {0xa6, 0x10}, {0xa8, 0xe0}, {0xa9, 0xe0}, {0xb3, 0x00},
            {0xb7, 0xf0}, {0xbb, 0xf0}, {0xbf, 0xf0}, {0xad, 0x00}
        };

        //Frame Interleave
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x10, 0xa9);
        if(vformat == NVC1700_VM_PAL) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xd1); //PAL
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x00);
        }
        else {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xd0); //ntsc
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x00);
        }

        for(i = 0; i < ARRAY_SIZE(NVC1700_2D1_8CIF); i ++) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, NVC1700_2D1_8CIF[i][0], NVC1700_2D1_8CIF[i][1]);
        }

        printk("NVC1700 OUTPUT MODE 2D1 8CIF\n");
    }
    else if(outmode == NVC1700_OUTMODE_1D1_16CIF) {
        unsigned char NVC1700_1D1_16CIF[][2] = {
            {0x1a, 0x00}, {0x1b, 0x80}, {0x80, 0x00}, {0x18, 0x92},
            {0xa0, 0x40}, {0xa4, 0x70}, {0xa5, 0x70}, {0xa6, 0x10},
            {0xa8, 0xe0}, {0xa9, 0xe0}, {0xb3, 0x00}, {0xb7, 0xf0},
            {0xbb, 0xf0}, {0xbf, 0xf0}, {0xad, 0x00}//, {0xad, 0x0F}
            //,{0x1c, 0x80}, {0x1d, 0x80},{0x1e, 0x80}, {0x1f, 0x80}
        };

        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x10, 0xa8);
        if(vformat == NVC1700_VM_PAL) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xe1); //PAL
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x03); //test 03.03
        }
        else {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xe0); //ntsc
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x02); //03.02
        }

        for(i = 0; i < ARRAY_SIZE(NVC1700_1D1_16CIF); i ++) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, NVC1700_1D1_16CIF[i][0], NVC1700_1D1_16CIF[i][1]);
        }

        printk("NVC1700 OUTPUT MODE 1D1 16CIF\n");
    }
    else {  //NVC1700_OUTMODE_1CH108MHZ
        unsigned char NVC1700_1CH_108MHZ[][2] = {
            {0x1a, 0x00}, {0x1b, 0x80}, {0x80, 0x00}, {0xa0, 0x30},
            {0xa1, 0x00}, {0xa4, 0x70}, {0xa5, 0x14}, {0xa6, 0x10},
            {0xa8, 0xe0}, {0xa9, 0xe0}, {0xad, 0x0f}
        };

        //Byte Interleave
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x10, 0xaa);
        if(vformat == NVC1700_VM_PAL) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xc1); //PAL
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x00);
        }
        else {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x11, 0xc0); //ntsc
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xac, 0x00);
        }

        for(i = 0; i < ARRAY_SIZE(NVC1700_1CH_108MHZ); i ++) {
            gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, NVC1700_1CH_108MHZ[i][0], NVC1700_1CH_108MHZ[i][1]);
        }

        printk("NVC1700 OUTPUT MODE 4D1 16CIF\n");
    }

    for(i = 0; i < ARRAY_SIZE(NVC1700_DAT_SEQ_TBL); i ++) {
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x20+i, NVC1700_DAT_SEQ_TBL[i]);
    }

        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x14, 0x02);
    msleep(50);
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x1a, 0x08);
    msleep(50);
        gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0x1a, 0x00);
    msleep(50);
#if 1
    for(i = 0; i <= 0xFF; i ++) {
        if(!(i%0x10)) printk("0x%02X :", i);
        printk(" 0x%02X,", gpio_i2c_readX(Grp, NVC1700X_I2C_ADDR, i));
        if(!((i+1)%0x10)) printk("\n");
    }
#endif
    return 0;
}

enum {
    NVP110X_COLOR_HUE = 0,
    NVP110X_COLOR_BRI,
    NVP110X_COLOR_CON,
    NVP110X_COLOR_SAT,
    NVP110X_COLOR_LST
};

static void nvp110x_set_color(int Chn, int Color, unsigned char Gain)
{
    const unsigned char COLOR_MAP[NVP110X_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
        VIDEO_COLOR_ADJTBL0, //HUE
        VIDEO_COLOR_ADJTBL0, //BRI
        VIDEO_COLOR_ADJTBL1, //CON
        VIDEO_COLOR_ADJTBL1, //SAT
    };

    int Chip, nChn;
    if((Color >= NVP110X_COLOR_LST)
    || (Gain >= ARRAY_SIZE(COLOR_MAP[0]))
    || (nvp110x_MapGet(Chn, &Chip, &nChn) < 0)) {
        return;
    }

    if(NVP110X_SYSM[Chip] == NVP110X_SYSM_TYPE1) {
        const unsigned char CHNL_MAP[NVP110X_COLOR_LST][NVP110X_MAX_IC_CHNLS] = {
            {0x18, 0x19, 0x1a, 0x1b, 0x98, 0x99, 0x9a, 0x9b}, //HUE
            {0x0c, 0x0d, 0x0e, 0x0f, 0x8c, 0x8d, 0x8e, 0x8f}, //BRI
            {0x10, 0x11, 0x12, 0x13, 0x90, 0x91, 0x92, 0x93}, //CON
            {0x14, 0x15, 0x16, 0x17, 0x94, 0x95, 0x96, 0x97}, //SAT
        };

        gpio_i2c_writeX(NVP110X_I2CP[Chip], NVP110X_ADDR[Chip], 0xff, 0x12);
        gpio_i2c_writeX(NVP110X_I2CP[Chip], NVP110X_ADDR[Chip],
            CHNL_MAP[Color][nChn], COLOR_MAP[Color][Gain]);
    }
    else {
        const unsigned char CHNL_MAP[NVP110X_COLOR_LST][NVP110X_MAX_IC_CHNLS] = {
            {0x40, 0x41, 0x42, 0x43, 0xC0, 0xC1, 0xC2, 0xC3}, //HUE
            {0x0C, 0x0D, 0x0E, 0x0F, 0x8C, 0x8D, 0x8E, 0x8F}, //BRI
            {0x10, 0x11, 0x12, 0x13, 0x90, 0x91, 0x92, 0x93}, //CON
            {0x3C, 0x3D, 0x3E, 0x3F, 0xBC, 0xBD, 0xBE, 0xBF}, //SAT
        };

        gpio_i2c_writeX(NVP110X_I2CP[Chip], NVP110X_ADDR[Chip], 0xff, 0x00);
        gpio_i2c_writeX(NVP110X_I2CP[Chip], NVP110X_ADDR[Chip],
            CHNL_MAP[Color][nChn], COLOR_MAP[Color][Gain]);
    }
}

static unsigned int nvp110x_get_loss(int chip)
{
    unsigned int ret_bit = 0;
    unsigned char tmpRet;
    int  ChipSize;

    int  ii = ARRAY_SIZE(NVP110X_SYID);
    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        do {
            -- ii;
            if(NVP110X_SYID[ii]) {
                ChipSize = ((NVP110X_SYID[ii] == NVP1104A_DEV_ID) ? 4 : 8);
                ret_bit <<= ChipSize;
                gpio_i2c_writeX(NVP110X_I2CP[ii], NVP110X_ADDR[ii], 0xff, 0x10);
                tmpRet    = gpio_i2c_readX(NVP110X_I2CP[ii], NVP110X_ADDR[ii], 0x03);
                tmpRet   &= ((ChipSize == 4) ? 0x0F : 0xFF);
                ret_bit  += tmpRet;
            }
        } while(ii);
    }
    else {
        do {
            -- ii;
            if(NVP110X_SYID[ii]) {
                int jj;
                ChipSize = ((NVP110X_SYID[ii] == NVP1914B_DEV_ID) ? 4 : 8);
                gpio_i2c_writeX(NVP110X_I2CP[ii], NVP110X_ADDR[ii], 0xFF, 0x01);
                tmpRet   = gpio_i2c_readX(NVP110X_I2CP[ii], NVP110X_ADDR[ii], 0xD8);
                tmpRet  &= ((ChipSize == 4) ? 0x0F : 0xFF);
                jj = ChipSize-1;
                do {
                    ret_bit <<= 1;
                    ret_bit  += (tmpRet & (1 << NetChip_MapTBL[jj+ii*8][1])) ?1 :0;
                } while(jj --);
            }
        } while(ii);
    }

    return ret_bit;
}

static int nvp1108_set_standard(int Grps, const char* std, const char* siz)
{
    int ii, jj;
    // Init NVP1108Xs
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            if(!gpio_i2c_probeX(ii, NVP110X_ADDR[jj]) || !nvp1108_check(ii, jj)) {
                continue;
            }

            printk("Now Setting up nvp1108_video_mode_init Grp = %d, Chip = %d\n", ii, jj);
            NVP110X_I2CP[jj] = ii;

            if(NVP110X_SYID[jj] != 0x00 && ii == NVP110X_I2CP[jj]) {
                unsigned char chip_addr = NVP110X_ADDR[jj];
                if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE1) {
                    if(NVP110X_SYID[jj] == NVP1108B_DEV_ID) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x10);
                        gpio_i2c_writeX(ii, chip_addr, 0x24, 0x01);
                    }

                    nvp1108_video_mode_init(jj, VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC);
                    printk("ccccccccccc- >>>>>>>%s\n", std);
                    if(Grps == 1) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x04, 0x10);    // Port 3 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x05, 0x32);
                        gpio_i2c_writeX(ii, chip_addr, 0x06, 0x54);    // Port 4 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x07, 0x76);
                        gpio_i2c_writeX(ii, chip_addr, 0x08, 0x00);
                        gpio_i2c_writeX(ii, chip_addr, 0x09, 0x88);
                        gpio_i2c_writeX(ii, chip_addr, 0x16, NVP1108_DELAY1);  //Video Clock Delay
                        gpio_i2c_writeX(ii, chip_addr, 0x19, NVP1108_DELAY1);  //Video Clock Delay
                    }
                    if(Grps == 2) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x02, 0x10);  // Port 2 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x03, 0x32);
                        gpio_i2c_writeX(ii, chip_addr, 0x04, 0x54);  // Port 3 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x05, 0x76);
                        gpio_i2c_writeX(ii, chip_addr, 0x08, 0x80);
                        gpio_i2c_writeX(ii, chip_addr, 0x09, 0x08);
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x13,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY3
                                : NVP1108A_DELAY3);  //Video Clock Delay
                        gpio_i2c_writeX(ii, chip_addr, 0x16,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY3
                                : NVP1108A_DELAY3);  //Video Clock Delay
                    }
				}
                else if((NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) || (NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3)) {
                    if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) {
                        nvp1118_video_mode_init(jj,
						    VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC,
						    VIDEO_IS_720H(siz)?NVP1108_720EXT:NVP1108_960EXT);
                    }
                    if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3) {
                        nvp1918c_video_mode_init(jj,
                            VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC,
                            VIDEO_IS_720H(siz)?NVP1108_720EXT:NVP1108_960EXT);
                    }
                    printk("ccccccccccc- >>>>>>>%s - %s\n", std, siz);
                    if(1) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x00);
                        if(NVP110X_VCMD[jj] == 0x00) {
#if (defined(GPIO_PLAT_TYPE6) && (MAX_CAM_CH == 8)) || defined(GPIO_PLAT_TYPE7)
                            gpio_i2c_writeX(ii, chip_addr, 0x55, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0x56, 0x10);
                            gpio_i2c_writeX(ii, chip_addr, 0xD5, 0x76);
                            gpio_i2c_writeX(ii, chip_addr, 0xD6, 0x54);
#elif defined(GPIO_PLAT_TYPE4)
                            gpio_i2c_writeX(ii, chip_addr, 0x55, 0x10);
                            gpio_i2c_writeX(ii, chip_addr, 0x56, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0xD5, 0x54);
                            gpio_i2c_writeX(ii, chip_addr, 0xD6, 0x76);
#else
                            gpio_i2c_writeX(ii, chip_addr, 0x55, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0x56, 0x10);
                            gpio_i2c_writeX(ii, chip_addr, 0xD5, 0x67);
                            gpio_i2c_writeX(ii, chip_addr, 0xD6, 0x45);
#endif
                        }
                        else {
                            gpio_i2c_writeX(ii, chip_addr, 0x55, 0x10);
                            gpio_i2c_writeX(ii, chip_addr, 0x56, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0xD5, 0x54);
                            gpio_i2c_writeX(ii, chip_addr, 0xD6, 0x76);
                        }
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x01);
                        if(NVP110X_VCMD[jj] == 0x00) {
#if defined(GPIO_PLAT_TYPE7)
                            gpio_i2c_writeX(ii, chip_addr, 0xC0, 0x10);  // Port 1 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC1, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0xC2, 0x54);  // Port 2 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC3, 0x76);
#else
                            gpio_i2c_writeX(ii, chip_addr, 0xC0, 0x54);  // Port 1 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC1, 0x76);
                            gpio_i2c_writeX(ii, chip_addr, 0xC2, 0x10);  // Port 2 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC3, 0x32);
#endif
                        }
                        else {
#if defined(GPIO_PLAT_TYPE4)
                            gpio_i2c_writeX(ii, chip_addr, 0xC0, 0x54);  // Port 1 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC1, 0x76);
                            gpio_i2c_writeX(ii, chip_addr, 0xC2, 0x10);  // Port 2 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC3, 0x32);
#else
                            gpio_i2c_writeX(ii, chip_addr, 0xC0, 0x10);  // Port 1 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC1, 0x32);
                            gpio_i2c_writeX(ii, chip_addr, 0xC2, 0x54);  // Port 2 Seq.
                            gpio_i2c_writeX(ii, chip_addr, 0xC3, 0x76);
#endif
                        }
                        gpio_i2c_writeX(ii, chip_addr, 0xC8, 0x88);
                        gpio_i2c_writeX(ii, chip_addr, 0xCE, 0x00);
#if defined(GPIO_PLAT_TYPE4)
                        gpio_i2c_writeX(ii, chip_addr, 0xCA, 0x03);
#else
                        gpio_i2c_writeX(ii, chip_addr, 0xCA, 0x07);
#endif
                    }
                }
            }
        }
    }

    // Fix For NVP1114/18/1914/18 to Switch PLL Clocks;
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            if(!gpio_i2c_probeX(ii, NVP110X_ADDR[jj])) {
                continue;
            }

            if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) {
				nvp1118_system_init(jj);
            }

            if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3) {
				nvp1918c_system_init(jj);
            }
        }
    }

    return 0;
}

int  nvc1700_PreViewSource(int Source)
{
    int Grp = 0;
    if(Source < 0) {
        return 0;
    }

    if(Source >= 16) {
        Grp = 1;
        Source %= 16;
    }

    gpio_i2c_writeX(Grp, NVC1700X_I2C_ADDR, 0xa1, Source);

    return !0;
}

static int nvc1700_set_standard(int Grps, const char* std)
{
    int ii, jj;
    // Init NVC1700s
    for(ii = 0; ii < Grps; ii ++) {
        if(gpio_i2c_probeX(ii, NVC1700X_I2C_ADDR)) {
            unsigned char NVC1700_1D1_16CIF[][2] = {
                {0x14, 0x42}, {0xa5, 0x70}, {0xa8, 0x40}, {0xa9, 0x40},
            };

            for(jj = 0; jj < ARRAY_SIZE(NVC1700_1D1_16CIF); jj ++) {
                gpio_i2c_writeX(ii, NVC1700X_I2C_ADDR, NVC1700_1D1_16CIF[jj][0], NVC1700_1D1_16CIF[jj][1]);
            }
        }
    }
    msleep(50);

    // Init NVP1108Xs
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            if(!gpio_i2c_probeX(ii, NVP110X_ADDR[jj]) || !nvp1108_check(ii, jj)) {
                continue;
            }

            printk("Now Setting up nvp1108_video_mode_init Grp = %d, Chip = %d\n", ii, jj);
            NVP110X_I2CP[jj] = ii;

            if(NVP110X_SYID[jj] != 0x00 && ii == NVP110X_I2CP[jj]) {
                unsigned char chip_addr = NVP110X_ADDR[jj];
                if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE1) {
                    if(NVP110X_SYID[jj] == NVP1108B_DEV_ID) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x10);
                        gpio_i2c_writeX(ii, chip_addr, 0x23, 0x03);
                    }

                    nvp1108_video_mode_init(jj, VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC);
                    printk("ccccccccccc- >>>>>>>%s\n", std);
                    if(Grps == 1) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x04, 0x10);  // Port 3 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x05, 0x32);
                        gpio_i2c_writeX(ii, chip_addr, 0x06, 0x54);  // Port 4 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x07, 0x76);
                        gpio_i2c_writeX(ii, chip_addr, 0x08, 0x00);
                        gpio_i2c_writeX(ii, chip_addr, 0x09, 0x88);
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x16,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY2
                                : NVP1108A_DELAY2);  //Video Clock Delay
                        gpio_i2c_writeX(ii, chip_addr, 0x19,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY2
                                : NVP1108A_DELAY2);  //Video Clock Delay
                    }
                    if(Grps == 2) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x02, 0x10);  // Port 2 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x03, 0x32);
                        gpio_i2c_writeX(ii, chip_addr, 0x04, 0x54);  // Port 3 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0x05, 0x76);
                        gpio_i2c_writeX(ii, chip_addr, 0x08, 0x80);
                        gpio_i2c_writeX(ii, chip_addr, 0x09, 0x08);
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x11);
                        gpio_i2c_writeX(ii, chip_addr, 0x13,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY3
                                : NVP1108A_DELAY3);  //Video Clock Delay
                        gpio_i2c_writeX(ii, chip_addr, 0x16,
                            (NVP110X_SYID[jj] == NVP1108B_DEV_ID)
                                ? NVP1108B_DELAY3
                                : NVP1108A_DELAY3);  //Video Clock Delay
                    }
                }
                else if((NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) || (NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3)) {
                    if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) {
                        nvp1118_video_mode_init(jj,
						    VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC,
						    NVP1108_720EXT);
                    }
                    if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3) {
                        nvp1918c_video_mode_init(jj,
                            VIDEO_IS_PAL(std)?NVP1108_PAL:NVP1108_NTSC,
                            NVP1108_720EXT);
                    }
                    printk("ccccccccccc- >>>>>>>%s\n", std);
                    if(1) {
                        gpio_i2c_writeX(ii, chip_addr, 0xFF, 0x01);
                        gpio_i2c_writeX(ii, chip_addr, 0xC0, 0x10);  // Port 1 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0xC1, 0x32);
                        gpio_i2c_writeX(ii, chip_addr, 0xC2, 0x54);  // Port 2 Seq.
                        gpio_i2c_writeX(ii, chip_addr, 0xC3, 0x76);
                        gpio_i2c_writeX(ii, chip_addr, 0xC8, 0x88);
                        gpio_i2c_writeX(ii, chip_addr, 0xCE, 0x00);
                        gpio_i2c_writeX(ii, chip_addr, 0xCA, 0x07);
                    }
                }
            }
        }
    }

    msleep(50);
    for(ii = 0; ii < Grps; ii ++) {
        if(gpio_i2c_probeX(ii, NVC1700X_I2C_ADDR)) {
            nvc1700_init(ii, VIDEO_IS_PAL(std)
                    ? NVC1700_VM_PAL
                    : NVC1700_VM_NTSC,
                    (NVP110X_SYSM[0] == NVP110X_SYSM_TYPE1)
					? NVC1700_OUTMODE_1D1_16CIF
					: NVC1700_OUTMODE_4D1_16CIF);
        }
    }
    msleep(25);

    // Fix For NVP1114/18/1914/18 to Switch PLL Clocks;
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            if(!gpio_i2c_probeX(ii, NVP110X_ADDR[jj])) {
                continue;
            }

            if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE2) {
				nvp1118_system_init(jj);
            }

            if(NVP110X_SYSM[jj] == NVP110X_SYSM_TYPE3) {
				nvp1918c_system_init(jj);
            }
        }
    }

    return 0;
}

static void  nvp1108_video_init(int Grps, const char* std, const char* siz)
{
    int ii, jj;
    // Find Single NVP1108Xs
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            printk("Now Setting Single NVP1108X Without NVC1700 Grp = %d, Chip = %d\n", ii, jj);
            NVP110X_ALIC[jj] = 0x01;
        }
    }

    nvp1108_set_standard(Grps, std, siz);
    nvp110x_MapInit();
}

static void  nvc1700_video_init(int Grps, const char* std)
{
    int ii, jj;
    // Find Single NVP1108Xs
    for(ii = 0; ii < Grps; ii ++) {
        for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
            printk("Now Setting Single NVP1108X Without NVC1700 Grp = %d, Chip = %d\n", ii, jj);
            NVP110X_ALIC[jj] = 0x01;
        }
    }

    nvc1700_set_standard(Grps, std);
    nvp110x_MapInit();
}

////////////////////////////////////////////////////////////////////////////////////
// audio part
////////////////////////////////////////////////////////////////////////////////////

void nvp1918c_set_audio_format(int chip, int ChnlNum, int SampleRate, int Bits)
{
    int           Grp  = NVP110X_I2CP[chip];
    unsigned char Addr = NVP110X_ADDR[chip];

    gpio_i2c_writeX(Grp, Addr, 0xFF, 0x01);
	if( (Addr == 0x60) || (Addr == 0x64)) {
		gpio_i2c_writeX(Grp, Addr, 0x06, 0x1A);
		gpio_i2c_writeX(Grp, Addr, 0x07, 0x80|(SampleRate<<3)|(Bits<<2)); //Master
		if(8 == ChnlNum) {
			gpio_i2c_writeX(Grp, Addr, 0x06, 0x1B);
			gpio_i2c_writeX(Grp, Addr, 0x08, 0x02);
			gpio_i2c_writeX(Grp, Addr, 0x0F, 0x54); //set I2S right sequence
			gpio_i2c_writeX(Grp, Addr, 0x10, 0x76);
		}
		else if(4 == ChnlNum)
		{
			gpio_i2c_writeX(Grp, Addr, 0x06, 0x1B);
			gpio_i2c_writeX(Grp, Addr, 0x08, 0x01);
			gpio_i2c_writeX(Grp, Addr, 0x0F, 0x32); //set I2S right sequence
		}
		gpio_i2c_writeX(Grp, Addr, 0x13, 0x80|(SampleRate<<3)|(Bits<<2)); //Master
		gpio_i2c_writeX(Grp, Addr, 0x23, 0x18); // Audio mix out
	}
	else
	{
		gpio_i2c_writeX(Grp, Addr, 0x06, 0x19);
		gpio_i2c_writeX(Grp, Addr, 0x07, 0x00|(SampleRate<<3)|(Bits<<2)); //Rec I2C 16K 16bit : Slave
		gpio_i2c_writeX(Grp, Addr, 0x13, 0x00|(SampleRate<<3)|(Bits<<2)); //PB I2C 8k 16bit : Slave
	}

	gpio_i2c_writeX(Grp, Addr, 0x01, 0x08); //ch1 Audio input gain init
	gpio_i2c_writeX(Grp, Addr, 0x02, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x03, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x04, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x05, 0x08); //mic gain
	gpio_i2c_writeX(Grp, Addr, 0x40, 0x08); //ch5
	gpio_i2c_writeX(Grp, Addr, 0x41, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x42, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x43, 0x08);
	gpio_i2c_writeX(Grp, Addr, 0x22, 0x08); //aogain

	gpio_i2c_writeX(Grp, Addr, 0x24, 0x14); //set mic_1's data to i2s_sp left channel
	gpio_i2c_writeX(Grp, Addr, 0x25, 0x15); //set mic_2's data to i2s_sp left channel
}

static void nvp110x_set_audio_format(int chip, audio_format_t audio_format)
{
    unsigned char Dev = NVP110X_ADDR[chip];
    int           Grp = NVP110X_I2CP[chip];

    unsigned char tmpReg;

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x10);
        /* Define the digital serial audio data format for record
            and mixing audio on the ACLKR, ASYNR, ADATR and ADATM pin.*/
        tmpReg  = gpio_i2c_readX(Grp, Dev, 0x36);
        tmpReg &= 0xfE;/* [1]*/
        tmpReg |= (0==audio_format.format) ? 0 : 0x1;/* 0:I2S format 1:DSP format */
        gpio_i2c_writeX(Grp, Dev, 0x36, tmpReg);

        /* Define the digital serial audio data format for
            playback audio on the ACLKP, ASYNP and ADATP pin.*/
        tmpReg  = gpio_i2c_readX(Grp, Dev, 0x43);
        tmpReg &= 0xfE;/* [1]*/
        tmpReg |= (0==audio_format.format) ? 0x0 : 0x1; /* 0:I2S format 1:DSP format */
        gpio_i2c_writeX(Grp, Dev, 0x43, tmpReg);
    }
}

static void nvp110x_audio_config(int chip)
{
    unsigned char Dev = NVP110X_ADDR[chip];
    int           Grp = NVP110X_I2CP[chip];

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x10); // to 1st. bank

        gpio_i2c_writeX(Grp, Dev, 0x31, 0x66);  // Audio input gain init
        gpio_i2c_writeX(Grp, Dev, 0x32, 0x66);
        gpio_i2c_writeX(Grp, Dev, 0x33, 0x66);
        gpio_i2c_writeX(Grp, Dev, 0x34, 0x66);
        gpio_i2c_writeX(Grp, Dev, 0x35, 0x66);

        if(NVP110X_ACMD[chip] == 0x00) { // master mode
            gpio_i2c_writeX(Grp, Dev, 0x36, 0x1a); // first stage
            gpio_i2c_writeX(Grp, Dev, 0x37, 0x80);
            gpio_i2c_writeX(Grp, Dev, 0x43, 0x80);
        }else { // slave mode
            gpio_i2c_writeX(Grp, Dev, 0x36, 0x19); // middle stage
            gpio_i2c_writeX(Grp, Dev, 0x37, 0x00);
            gpio_i2c_writeX(Grp, Dev, 0x43, 0x00);
        }
    }
    else {
        gpio_i2c_writeX(Grp, Dev, 0xFF, 0x01);

        if(NVP110X_ACMD[chip] == 0x00) {
            gpio_i2c_writeX(Grp, Dev, 0x07, 0x80); //8K/16bit Rec Master
#if   defined(GPIO_PLAT_TYPE7)
            gpio_i2c_writeX(Grp, Dev, 0x13, 0x80); //8K/16bit PB  Master
#else
            gpio_i2c_writeX(Grp, Dev, 0x13, 0x00); //8K/16bit PB  Slave
#endif
            gpio_i2c_writeX(Grp, Dev, 0x23, 0x00); //Mix Out Select PB Input
            gpio_i2c_writeX(Grp, Dev, 0xd5, 0x01);

            gpio_i2c_writeX(Grp, Dev, 0x06, 0x1a);
        }
        else {
            gpio_i2c_writeX(Grp, Dev, 0x07, 0x00); //8K/16bit Rec Slave
            gpio_i2c_writeX(Grp, Dev, 0x13, 0x00); //8K/16bit PB  Slave
            gpio_i2c_writeX(Grp, Dev, 0x23, 0x00); //Mix Out Select PB Input
            gpio_i2c_writeX(Grp, Dev, 0xd5, 0x01);

            gpio_i2c_writeX(Grp, Dev, 0x06, 0x59);
            gpio_i2c_writeX(Grp, Dev, 0x13, 0x00);
        }

        gpio_i2c_writeX(Grp, Dev, 0xFF, 0x01);
//        gpio_i2c_writeX(Grp, Dev, 0x38, 0xFF);
//        msleep(5);
//        gpio_i2c_writeX(Grp, Dev, 0x38, 0x00);
        if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE3) {
            gpio_i2c_writeX(Grp, Dev, 0x22, 0x08);
        }
        else {
            gpio_i2c_writeX(Grp, Dev, 0x22, 0x60);
        }
        gpio_i2c_writeX(Grp, Dev, 0x00, 0x02);
    }
}

static void nvp110x_audio_set_samplerate(int chip, int sample_rate)
{
    unsigned char tmpReg = 0;

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x10); // to 1st. bank
        tmpReg = gpio_i2c_readX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x37);
        if(sample_rate <= 8000){ // 8k
            tmpReg &= ~(1<<3);
        }
        else {// 16k
            tmpReg |= (1<<3);
        }
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x37, tmpReg);
    }
    else {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x01); // to 1st. bank
        tmpReg = gpio_i2c_readX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x07);
        if(sample_rate <= 8000){ // 8k
            tmpReg &= ~(1<<3);
        }
        else {// 16k
            tmpReg |= (1<<3);
        }
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x07, tmpReg);
    }
}

static void nvp110x_audio_set_bitwidth(int chip, int bitwidth)
{
    unsigned char tmpReg = 0;
    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x10); // to 1st. bank
        tmpReg = gpio_i2c_readX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x37);
        if(bitwidth <= 8){ // 8bits
            tmpReg |= (1<<2);
        }
        else { // 16bits
            tmpReg &= ~(1<<2);
        }
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x37, tmpReg);
    }
    else {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x01); // to 1st. bank
        tmpReg = gpio_i2c_readX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x07);
        if(bitwidth <= 8000){ // 8bits
            tmpReg &= ~(1<<2);
        }
        else {// 16bits
            tmpReg |= (1<<2);
        }
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x07, tmpReg);
    }
}

static void nvp110x_audio_set_volume(int chip, unsigned int volume)
{
    if(!(volume < 16)) {
        volume = 15;
    }

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x10);
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x52, (volume<<4));
    }
    else {
        gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0xff, 0x01);
        if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE3) {
            gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x22, (volume<<0));
        }
        else {
            gpio_i2c_writeX(NVP110X_I2CP[chip], NVP110X_ADDR[chip], 0x22, (volume<<4));
        }
    }
}

static void nvp110x_audio_set_channel_mode(int chip, int chn)
{
    unsigned char Dev = NVP110X_ADDR[chip];
    int           Grp = NVP110X_I2CP[chip];

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x10);

        if(chn <= 2){
            gpio_i2c_writeX(Grp, Dev, 0x3a, 0x10);
        }
        else if((chn <= 4)){
            gpio_i2c_writeX(Grp, Dev, 0x3a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x3b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x3c, 0x00);  //Clear Unused Input Channels
            gpio_i2c_writeX(Grp, Dev, 0x3d, 0x00);
        }
        else if((chn <= 8)){
            gpio_i2c_writeX(Grp, Dev, 0x3a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x3b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x3e, 0x54);
            gpio_i2c_writeX(Grp, Dev, 0x3f, 0x76);
        }else{
            gpio_i2c_writeX(Grp, Dev, 0x3a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x3b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x3e, 0x54);
            gpio_i2c_writeX(Grp, Dev, 0x3f, 0x76);

            gpio_i2c_writeX(Grp, Dev, 0x3e, 0x98);
            gpio_i2c_writeX(Grp, Dev, 0x3f, 0xba);
            gpio_i2c_writeX(Grp, Dev, 0x40, 0xdc);
            gpio_i2c_writeX(Grp, Dev, 0x41, 0xfe);
        }
    }
    else {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x01);

        if(chn <= 2){
            gpio_i2c_writeX(Grp, Dev, 0x0a, 0x10);
        }
        else if((chn <= 4)){
            gpio_i2c_writeX(Grp, Dev, 0x0a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x0b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x0c, 0x00);  //Clear Unused Input Channels
            gpio_i2c_writeX(Grp, Dev, 0x0d, 0x00);
        }
        else if((chn <= 8)){
            gpio_i2c_writeX(Grp, Dev, 0x0a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x0b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x0e, 0x54);
            gpio_i2c_writeX(Grp, Dev, 0x0f, 0x76);
        }else{
            gpio_i2c_writeX(Grp, Dev, 0x0a, 0x10);
            gpio_i2c_writeX(Grp, Dev, 0x0b, 0x32);
            gpio_i2c_writeX(Grp, Dev, 0x0c, 0x54);
            gpio_i2c_writeX(Grp, Dev, 0x0d, 0x76);

            gpio_i2c_writeX(Grp, Dev, 0x0e, 0x98);
            gpio_i2c_writeX(Grp, Dev, 0x0f, 0xba);
            gpio_i2c_writeX(Grp, Dev, 0x10, 0xdc);
            gpio_i2c_writeX(Grp, Dev, 0x11, 0xfe);
        }
    }
}

static void nvp110x_audio_set_playback(int chip, int channel)
{
    unsigned char Dev = NVP110X_ADDR[chip];
    int           Grp = NVP110X_I2CP[chip];

    unsigned char tmpReg = 0;

    if(chip < 0 || chip > 4) {
        return;
    }
    if(channel < 0 || channel >= 0x20){
        return;
    }
    if(channel >= 16) {
        channel = 0x1f;  //set value for no audio output.
    }

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x10); // bank 0 MIX_OUTSEL
        if(channel >= 0 && channel < 16) {
            gpio_i2c_writeX(Grp, Dev, 0x44, channel);
            channel = 0x10; //enable playback mixer output
        }
        tmpReg  = gpio_i2c_readX(Grp, Dev, 0x53);
        tmpReg &= ~0x1f;
        tmpReg |= channel; //mixer output select
        gpio_i2c_writeX(Grp, Dev, 0x53, tmpReg);
    }
    else {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x01);
        if(channel >= 0 && channel < 16) {
            gpio_i2c_writeX(Grp, Dev, 0x14, channel);
            channel = 0x10; //enable playback mixer output
        }
        gpio_i2c_writeX(Grp, Dev, 0x23, channel);
    }
}

static void nvp110x_audio_set_liveloop(int chip, int channel)
{
    unsigned char Dev = NVP110X_ADDR[chip];
    int           Grp = NVP110X_I2CP[chip];

    unsigned char tmpReg = 0;

    if(chip < 0 || chip > 4) {
        return;
    }
    if(channel < 0 || channel >= 0x20){
        return;
    }
    if(channel >= 16) {
        channel = 0x1f;  //set value for no audio output.
    }

    if(NVP110X_SYSM[chip] == NVP110X_SYSM_TYPE1) {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x10); // bank 0 MIX_OUTSEL
        tmpReg  = gpio_i2c_readX(Grp, Dev, 0x53);
        tmpReg &= ~0x1f;
        tmpReg |= channel; //mixer output select
        gpio_i2c_writeX(Grp, Dev, 0x53, tmpReg);
    }
    else {
        gpio_i2c_writeX(Grp, Dev, 0xff, 0x01);
        gpio_i2c_writeX(Grp, Dev, 0x23, channel);
    }
}

static void nvc1700_audio_set_liveloop(int chip, int channel)
{
    nvp110x_audio_set_liveloop(chip, channel);
}

static void nvc1700_audio_init(int chip);

static void nvp1108_audio_init(int chip)
{
	if(NVP110X_SYSM[0] == NVP110X_SYSM_TYPE1) {
		nvp110x_audio_config(chip);
		nvp110x_audio_set_samplerate(chip, 8000); // default 8k sample rate
		nvp110x_audio_set_bitwidth(chip, 16); // default 16bits data width
		if(NVP110X_SYID[chip] == NVP1104A_DEV_ID) {
			nvp110x_audio_set_channel_mode(chip, 4);
		}
		else{
			nvp110x_audio_set_channel_mode(chip, 8);
		}
		nvp110x_audio_set_playback(chip, 0);
		nvp110x_audio_set_liveloop(chip, 0);
	}
	else {
		nvc1700_audio_init(0);
	}
}

static void nvc1700_audio_init(int chip)
{
    int jj;
    for(jj = 0; jj < ARRAY_SIZE(NVP110X_ADDR); jj ++) {
        if(NVP110X_SYID[jj]) {
           nvp110x_audio_config(jj);
           nvp110x_audio_set_samplerate(jj, 8000); // default 8k sample rate
           nvp110x_audio_set_bitwidth(jj, 16); // default 16bits data width
           nvp110x_audio_set_channel_mode(jj, 16); // default 16 channel mode
        }
    }
    nvp110x_audio_set_playback(0, 0);
    nvp110x_audio_set_liveloop(0, 0);
}
