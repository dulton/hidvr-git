#define RN6366_720H_FMT 0
#define RN6366_960H_FMT 1

#define RN6366_DEV_ID 0x56
#define RN6304_DEV_ID 0x54
#define RN6318_DEV_ID 0x74
#define RN6318A_DEV_ID 0x75

int  RN63XX_ADDR[] = { 0x58, 0x5C };
int  RN63XX_SYID[] = { 0x00, 0x00 };
int  RN63XX_NUMB[] = { 0x00, 0x00 };
int  RN63XX_ACMD[] = { 0x00, 0x01 };
int  RN63XX_VCMD[] = { 0x00, 0x01 };

#define RN63XX_MAX_IC_NUM  2
#define RN63XX_MAX_IC_CHN  8

static int RichNex_MapTBL[RN63XX_MAX_IC_NUM*RN63XX_MAX_IC_CHN][2] = {
#if   defined(GPIO_PLAT_TYPE1)
    { 0, 0}, { 0, 1}, { 0, 2}, { 0, 3}, //Chip0 0~3 -> 0x58[0~3]
    { 0, 4}, { 0, 5}, { 0, 6}, { 0, 7}, //Chip0 4~7 -> 0x5A[0~3]

    { 1, 0}, { 1, 1}, { 1, 2}, { 1, 3}, //Chip1 0~3 -> 0x5C[0~3]
    { 1, 4}, { 1, 5}, { 1, 6}, { 1, 7}, //Chip1 4~7 -> 0x5E[0~3]

#elif defined(GPIO_PLAT_TYPE7)
    { 0, 2}, { 0, 3}, { 0, 0}, { 0, 1}, //Chip0 0~3 -> 0x58[0~3]
    { 0, 6}, { 0, 7}, { 0, 4}, { 0, 5}, //Chip0 4~7 -> 0x5A[0~3]

    { 1, 2}, { 1, 3}, { 1, 0}, { 1, 1}, //Chip1 0~3 -> 0x58[0~3]
    { 1, 7}, { 1, 6}, { 1, 5}, { 1, 4}, //Chip1 4~7 -> 0x5A[0~3]

#else
    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3}, //Chip0 0~3 -> 0x58[0~3]
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7}, //Chip0 4~7 -> 0x5A[0~3]

    {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3}, //Chip1 0~3 -> 0x5C[0~3]
    {-1, 4}, {-1, 5}, {-1, 6}, {-1, 7}, //Chip1 4~7 -> 0x5E[0~3]

#endif
};

int RichNex_MapFix(void)
{
    return 0;
}

int RichNex_MapGet(int Chn, int *Chip, int *nChn)
{
    if((Chn >= ARRAY_SIZE(RichNex_MapTBL))
        || (-1 == RichNex_MapTBL[Chn][0])) {

        return -1;
    }

    if(Chip) {
        *Chip = RichNex_MapTBL[Chn][0];
    }

    if(nChn) {
        *nChn = RichNex_MapTBL[Chn][1];
    }

    return 0;
}

static int interface_format = RN6366_720H_FMT;

static unsigned char rn6366_check(int chip)
{
    int tmpRet = 0;
    int ii;

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        if(gpio_i2c_probe(RN63XX_ADDR[ii])) {
            unsigned char tmpID = gpio_i2c_read(RN63XX_ADDR[ii], 0xFD);
            if((tmpID == RN6366_DEV_ID) || (tmpID == RN6304_DEV_ID)) {
                RN63XX_SYID[ii] = tmpID;
                RN63XX_NUMB[ii] = 4/4;
                tmpRet ++;
            }
            if((tmpID == RN6318_DEV_ID) || (tmpID == RN6318A_DEV_ID) ) {
                RN63XX_SYID[ii] = tmpID;
                if(gpio_i2c_probe(RN63XX_ADDR[ii]+2)) {
                    RN63XX_NUMB[ii] = 8/4;
                }
                else {
                    RN63XX_NUMB[ii] = 4/4;
                }
                tmpRet ++;
            }
        }
    }

    RichNex_MapFix();

    return tmpRet;
}

static int  rn6366_get_vsup(video_sup_t *vsup)
{
    int ii, vsiz;

    if(!vsup) return -1;

    vsiz  = (1 << VIDEO_SUP_FLD1);
    vsiz |= (1 << VIDEO_SUP_FLEF);

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_SYID); ii ++) {
        if(!RN63XX_SYID[ii]) {
            continue;
        }

        if(RN63XX_SYID[ii]) { //All RichNex IC Support EFFIO Now.
            vsiz |= (1 << VIDEO_SUP_FLEF);
        }
        else {
            vsiz &= ~(1 << VIDEO_SUP_FLEF);
        }
    }

    vsup->vsiz = vsiz;

    return 0;
}

static unsigned int rn6366_get_loss(int chip)
{
    unsigned int  ret_bit = 0;
    int  ii;

    for (ii = ARRAY_SIZE(RichNex_MapTBL)-1; ii >= 0; ii --) {
        unsigned char tmpRet = 0x01; //Sign Video Loss

        int  Addr, Chip, nChn;

        if(!RichNex_MapGet(ii, &Chip, &nChn)) {
            if(!RN63XX_SYID[Chip]) {
                continue;
            }

            Addr = RN63XX_ADDR[Chip] + (nChn/4)*2; //RN63XX_ADDR[x] or RN63XX_ADDR[x]+2
            nChn = (nChn%4);  //range in 0~3

            gpio_i2c_write(Addr, 0xFF, nChn);
            tmpRet  = gpio_i2c_read(Addr, 0x00);
            tmpRet &= 0x10;
        }

        ret_bit <<= 1;
        ret_bit  += (tmpRet ?1 :0);
    }

    return ret_bit;
}

enum {
    RN63XX_COLOR_HUE = 0,
    RN63XX_COLOR_BRI,
    RN63XX_COLOR_CON,
    RN63XX_COLOR_SAT,
    RN63XX_COLOR_LST
};

static void rn6366_set_color(int Chn, int Color, unsigned char Gain)
{
    const unsigned char COLOR_MAP0[RN63XX_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
        VIDEO_COLOR_ADJTBL1, //HUE
        VIDEO_COLOR_ADJTBL0_1, //BRI
        VIDEO_COLOR_ADJTBL1, //CON
        VIDEO_COLOR_ADJTBL1, //SAT
    };
    const unsigned char COLOR_MAP1[RN63XX_COLOR_LST][VIDEO_MAX_COLOR_GAIN+1] = {
        VIDEO_COLOR_ADJTBL1, //HUE
        VIDEO_COLOR_ADJTBL0, //BRI
        VIDEO_COLOR_ADJTBL1, //CON
        VIDEO_COLOR_ADJTBL1, //SAT
    };
    const unsigned char COLOR_TBL[RN63XX_COLOR_LST] = {0x04, 0x01, 0x02, 0x03};
    int  Addr, Chip, Syid, nChn;

    if((Color >= RN63XX_COLOR_LST)
    || (Gain >= (VIDEO_MAX_COLOR_GAIN+1))
    || RichNex_MapGet(Chn, &Chip, &nChn)) {
        return;
    }

    Addr = RN63XX_ADDR[Chip] + (nChn/4)*2;
    Syid = RN63XX_SYID[Chip];
    nChn = nChn%4;

    gpio_i2c_write(Addr, 0xFF, nChn);
	if(RN6318A_DEV_ID == Syid) {
		gpio_i2c_write(Addr, COLOR_TBL[Color], COLOR_MAP1[Color][Gain]);
	}
	else {
		gpio_i2c_write(Addr, COLOR_TBL[Color], COLOR_MAP0[Color][Gain]);
	}
}

void rn6366_video_mode0(int addr, int sChn, int dChn, int ifmt, const char *std)
{
    gpio_i2c_write(addr, 0xFF, sChn);
    gpio_i2c_write(addr, 0x01, 0x00); //brightness (default: 00h)
    gpio_i2c_write(addr, 0x02, 0x65); //contrast   (default: 80h)
    gpio_i2c_write(addr, 0x03, 0x80); //saturation (default: 80h)
    gpio_i2c_write(addr, 0x04, 0x80); //hue        (default: 80h)
    gpio_i2c_write(addr, 0x05, 0x00); //sharpness  (default: 00h)

    gpio_i2c_write(addr, 0x13, 0x16); //set bandpass filtef for text pattern
    gpio_i2c_write(addr, 0x17, 0x0F); //disable digital clamping
    gpio_i2c_write(addr, 0x2A, 0x0F); //disable bessel filter

    gpio_i2c_write(addr, 0x36, 0x30); //set analog clamping current as normal current
    gpio_i2c_write(addr, 0x37, 0x02); //Faster Video Color Responsive Speed
    gpio_i2c_write(addr, 0x38, 0x87); //set blanking level range
    gpio_i2c_write(addr, 0x4E, 0x11);

    // decrease saw tooth line
    // set Reg1E[7:4] = 0 or 1 or 2 or 3. apply the different value in Reg1E[3:0] from 1 to F.
    // Observe the decoded result for appling a new value in Reg1E[7:0].
    // Choose the favors image and set the correponding value in  Reg1E[7:0].
    gpio_i2c_write(addr, 0x1E, 0x21); // Reg1E[3:0] : 1 -> more smooth but more foggy
                                      //              F -> less smooth but more clear

    // high freqency enhance
    gpio_i2c_write(addr, 0x05, 0x00); // Reg05[2:0] Luma high freqency enhance :
                                      //                      0 -> normal
                                      //                        7 -> more enhance high freqency Y
                                      // Reg05[6:4] : Color high freqency enhance
                                      //                        0 -> normal
                                      //                        7 -> more enhance high freqency C
    if(VIDEO_IS_NTSC(std)) { // NTSC
        gpio_i2c_write(addr, 0x07, 0x23);   // force in NTSC decoding mode

        // enable vertical cropping for NTSC:
        // V0Start  = 13h    V0End= 106h
        // V1Start  = 13h    V1End= 105h
        // Reg20[7:0] : A0h
        // Reg23[7:0] : V0Start [7:0] = 13h
        // Reg24[7:0] : V0End   [7:0] = 06h
        // Reg25[7:0] : V1Start [7:0] = 13h
        // Reg26[7:0] : V1End   [7:0] = 05h
        // Reg28[5:4] : enable vertical cropping

        gpio_i2c_write(addr, 0x20, 0xA4);
        gpio_i2c_write(addr, 0x23, 0x13);
        gpio_i2c_write(addr, 0x24, 0x06);
        gpio_i2c_write(addr, 0x25, 0x13);
        gpio_i2c_write(addr, 0x26, 0x05);
        gpio_i2c_write(addr, 0x28, 0x30);
    }
    else { // PAL
        gpio_i2c_write(addr, 0x07, 0x22);   // force in PAL decoding mode

        // enable vertical cropping for PAL:
        // V0Start  = 18h    V0End= 138h
        // V1Start  = 19h    V1End= 137h

        // Reg23[7:0] : V0Start [7:0] = 18h
        // Reg24[7:0] : V0End   [7:0] = 38h
        // Reg25[7:0] : V1Start [7:0] = 19h
        // Reg26[7:0] : V1End   [7:0] = 37h
        // Reg28[5:4] : enable vertical cropping

        // enable horizontal cropping for PAL: HStart  = 45h    HEnd= 1AEh
        // Reg20[1:0] : horizontal active pixels start point, HStart [9:8]
        // Reg20[3:2] : horizontal active pixels end point,  HEnd [9:8]
        // Reg21[7:0] : horizontal active pixels start point, HStart [7:0]
        // Reg22[7:2] : horizontal active pixels end point,  HEnd [7:0]
        // Reg28[6] : enable horizontal cropping

        gpio_i2c_write(addr, 0x23, 0x18);
        gpio_i2c_write(addr, 0x24, 0x3A);
        gpio_i2c_write(addr, 0x25, 0x19);
        gpio_i2c_write(addr, 0x26, 0x39);

        gpio_i2c_write(addr, 0x20, 0xA4);
        gpio_i2c_write(addr, 0x21, 0x45);
        gpio_i2c_write(addr, 0x22, 0xAE);
        gpio_i2c_write(addr, 0x28, 0x70);
    }

    // interface_format = 1 : 960H
    //                    2 : 720H
    if(ifmt == RN6366_960H_FMT) {   // 960H
        gpio_i2c_write(addr, 0x50, 0x01);   // output interface is BT-1302
    }
    else {                          // 720H
        gpio_i2c_write(addr, 0x50, 0x00);   // output interface is BT-656
    }

    gpio_i2c_write(addr, 0x1A, 0x03);  // blue field
    //gpio_i2c_write(addr, 0x1A, 0x83);  // snow field

    gpio_i2c_write (addr ,0x3A,0x20);   // Embedded channel ID  in SAV/EAV
    switch (dChn%4) {
        case 0:   gpio_i2c_write(addr, 0x3F, 0x20); break; // assign Channel ID as 0
        case 1:   gpio_i2c_write(addr, 0x3F, 0x21); break; // assign Channel ID as 1
        case 2:   gpio_i2c_write(addr, 0x3F, 0x22); break; // assign Channel ID as 2
        case 3:   gpio_i2c_write(addr, 0x3F, 0x23); break; // assign Channel ID as 3
    }
}

void rn6366_video_mode1(int addr, int sChn, int dChn, int ifmt, const char *std)
{
    gpio_i2c_write(addr, 0xFF, sChn);
    gpio_i2c_write(addr, 0x01, 0x08); //brightness (default: 00h)
    gpio_i2c_write(addr, 0x02, 0x65); //contrast   (default: 80h)
    gpio_i2c_write(addr, 0x03, 0x80); //saturation (default: 80h)
    gpio_i2c_write(addr, 0x04, 0x80); //hue        (default: 80h)

    if(VIDEO_IS_NTSC(std)) { // NTSC
        gpio_i2c_write(addr, 0x07, 0x23);   // force in NTSC decoding mode
    }
    else { // PAL
        gpio_i2c_write(addr, 0x07, 0x22);   // force in PAL decoding mode
    }

    // interface_format = 1 : 960H
    //                    2 : 720H
    if(ifmt == RN6366_960H_FMT) {   // 960H
        gpio_i2c_write(addr, 0x50, 0x01);   // output interface is BT-1302
        gpio_i2c_write(addr, 0x63, 0x09);   // composite anti-aliasing filter control (Reg63 default: 09h)
                                            // Reg63[3:1]: bandwidth selection, Reg63[0]: enable/disable
    }
    else {                          // 720H
        gpio_i2c_write(addr, 0x50, 0x00);   // output interface is BT-656
        gpio_i2c_write(addr, 0x63, 0x09);   // composite anti-aliasing filter control (Reg63 default: 09h)
                                            // Reg63[3:1]: bandwidth selection, Reg63[0]: enable/disable
        gpio_i2c_write(addr, 0x21, 0x48);
        gpio_i2c_write(addr, 0x28, 0x42);
    }

	if(0) {
		gpio_i2c_write(addr, 0x31, 0xA5);  //Analog AGC
		gpio_i2c_write(addr, 0x32, 0xEE);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x36, 0x30);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x37, 0x11);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x38, 0x75);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x39, 0x01);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x61, 0x69);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x64, 0xC6);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x65, 0x30);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x09, 0x88);  //Digital AGC
		gpio_i2c_write(addr, 0x66, 0x00);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x67, 0xF0);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1C, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1D, 0x00);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x0E, 0xB8);  //Color AGC

		gpio_i2c_write(addr, 0x2C, 0xFD);  //Sync. Slicing method

		gpio_i2c_write(addr, 0x2F, 0x14);  //Sync. Relative
		gpio_i2c_write(addr, 0x4D, 0x13);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x4F, 0x08);  //Coring
		gpio_i2c_write(addr, 0x5C, 0x30);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5D, 0x58);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5E, 0x80);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x05, 0x08);  //Video Enhancement
		gpio_i2c_write(addr, 0x57, 0x20);  //More Fixes From Richnex Luis Huang
	}
	if(0) {
		gpio_i2c_write(addr, 0x31, 0xA5);  //Analog AGC
		gpio_i2c_write(addr, 0x32, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x36, 0x31);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x37, 0x11);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x38, 0x7E);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x39, 0x04);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x61, 0x72);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x64, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x65, 0x30);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x09, 0x88);  //Digital AGC
		gpio_i2c_write(addr, 0x66, 0x00);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x67, 0xF0);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1C, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1D, 0x00);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x0E, 0xB8);  //Color AGC

		gpio_i2c_write(addr, 0x2C, 0x60);  //Sync. Slicing method

		gpio_i2c_write(addr, 0x2F, 0x14);  //Sync. Relative
		gpio_i2c_write(addr, 0x4D, 0x13);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x4F, 0x08);  //Coring
		gpio_i2c_write(addr, 0x5C, 0x30);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5D, 0x58);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5E, 0x80);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x05, 0x08);  //Video Enhancement
		gpio_i2c_write(addr, 0x57, 0x20);  //More Fixes From Richnex Luis Huang
	}
	if(1) {
		gpio_i2c_write(addr, 0x31, 0xA5);  //Analog AGC
		gpio_i2c_write(addr, 0x32, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x36, 0x31);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x37, 0x11);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x38, 0x7E);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x39, 0x04);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x61, 0x72);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x64, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x65, 0x30);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x09, 0x88);  //Digital AGC
		gpio_i2c_write(addr, 0x66, 0x00);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x67, 0xF0);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1C, 0xFF);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x1D, 0x00);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x0E, 0xB8);  //Color AGC

		gpio_i2c_write(addr, 0x2C, 0x60);  //Sync. Slicing method

		gpio_i2c_write(addr, 0x2F, 0x14);  //Sync. Relative
		gpio_i2c_write(addr, 0x4D, 0x13);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x4F, 0x08);  //Coring
		gpio_i2c_write(addr, 0x5C, 0x30);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5D, 0x58);  //More Fixes From Richnex Luis Huang
		gpio_i2c_write(addr, 0x5E, 0x80);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x05, 0x04);  //Video Enhancement
		gpio_i2c_write(addr, 0x57, 0x20);  //More Fixes From Richnex Luis Huang

		gpio_i2c_write(addr, 0x11, 0x03);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0x17, 0x0F);  //More Fixes By LeeXM

		gpio_i2c_write(addr, 0x56, 0x03);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0x63, 0x07);  //More Fixes By LeeXM

		gpio_i2c_write(addr, 0x74, 0x0C);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0x76, 0x2D);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0x78, 0x08);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0x7A, 0x09);  //More Fixes By LeeXM

		gpio_i2c_write(addr, 0x81, 0x7F);  //More Fixes By LeeXM
		gpio_i2c_write(addr, 0xD2, 0x05);  //More Fixes By LeeXM
	}

    gpio_i2c_write(addr, 0x1A, 0x03);  // blue field
    //gpio_i2c_write(addr, 0x1A, 0x83);  // snow field

    gpio_i2c_write (addr ,0x3A,0x20);   // Embedded channel ID  in SAV/EAV
    switch (dChn%4) {
        case 0:   gpio_i2c_write(addr, 0x3F, 0x20); break; // assign Channel ID as 0
        case 1:   gpio_i2c_write(addr, 0x3F, 0x21); break; // assign Channel ID as 1
        case 2:   gpio_i2c_write(addr, 0x3F, 0x22); break; // assign Channel ID as 2
        case 3:   gpio_i2c_write(addr, 0x3F, 0x23); break; // assign Channel ID as 3
    }
}

static int  rn6366_set_standard(int Grps, const char* std)
{
    int ii;

    for (ii = 0; ii < ARRAY_SIZE(RichNex_MapTBL)-1; ii ++) {
        int  Addr, Chip, nChn;

        if(!RichNex_MapGet(ii, &Chip, &nChn)) {
            if(!RN63XX_SYID[Chip]) {
                continue;
            }

            Addr = RN63XX_ADDR[Chip] + (nChn/4)*2;

            if(!gpio_i2c_probe(Addr)) {
                continue;
            }

            if(RN6318A_DEV_ID == RN63XX_SYID[Chip]) {
                rn6366_video_mode1(Addr, nChn%4, ii%4, interface_format, std);
            }
            else {
                rn6366_video_mode0(Addr, nChn%4, ii%4, interface_format, std);
            }
        }
    }

    return 0;
}

static void rn6366_video_init(int Grps, const char* std, const char* siz)
{
    int ii;

    interface_format = VIDEO_IS_960H(siz) ?RN6366_960H_FMT :RN6366_720H_FMT;

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        int Addr = RN63XX_ADDR[ii];
        int Numb = RN63XX_NUMB[ii];
        int jj;

        if(!RN63XX_SYID[ii]) {
            continue;
        }

        for(jj = 0; jj < Numb; jj ++) {
            int addr = Addr + jj*2;

            gpio_i2c_write(addr, 0x80, 0x31);//involve soft reset on I2C (0x2c)
            mdelay(10);
            if((RN63XX_SYID[ii] == RN6318_DEV_ID)
            || (RN63XX_SYID[ii] == RN6318A_DEV_ID)
            || (RN63XX_SYID[ii] == RN6304_DEV_ID)) {
                gpio_i2c_write(addr, 0xD2, 0x85);//Force to Disable Auto Clock Input Frequency Detection.
                gpio_i2c_write(addr, 0xD6, 0x37);//27MHz Clock Input.
                if(RN63XX_SYID[ii] != RN6304_DEV_ID) {
                    gpio_i2c_write(addr, 0x80, 0x36);//Force PLL & PowerD Reset.
                    mdelay(5);
                    gpio_i2c_write(addr, 0x80, 0x30);//Clear PLL & PowerD Reset.
                }
                mdelay(20);
            }
            gpio_i2c_write(addr, 0xD9, 0xE0); //set narrow anti-filiter

            gpio_i2c_write (addr, 0x82, 0x00); //disable SCLK0A
            gpio_i2c_write (addr, 0x84, 0x00); //disable SCLK0B
            gpio_i2c_write (addr, 0x86, 0x00); //disable SCLK1A
            gpio_i2c_write (addr, 0x88, 0x00); //disable SCLK1B
            gpio_i2c_write (addr, 0xF2, 0x00); //disable SCLK2A
            gpio_i2c_write (addr, 0xF4, 0x00); //disable SCLK2B
            gpio_i2c_write (addr, 0xF6, 0x00); //disable SCLK3A
            gpio_i2c_write (addr, 0xF8, 0x00); //disable SCLK3B

            gpio_i2c_write (addr, 0xFA, 0x00); //disable SCLK27M
            gpio_i2c_write (addr, 0xDA, 0x07); //disable SCLK0A low jitter output

            gpio_i2c_write (addr, 0x96, 0x0F); // GPIO
            gpio_i2c_write (addr, 0x97, 0x0F); // GPIO
        }
    }

    rn6366_set_standard(Grps, std);

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        int addr = RN63XX_ADDR[ii];

        if(!RN63XX_SYID[ii]) {
            continue;
        }

        if(interface_format == RN6366_960H_FMT) {  // 960H
            // Data Bus = {ch0,ch1,ch2,ch3}    from  VP0[7:0] {pin98,...,pin106}
            // Clock    = 144MHz (960H)        from SCLK2A {pin108}
            if(RN63XX_SYID[ii] == RN6366_DEV_ID) {
                gpio_i2c_write(addr, 0x8b, 0x02);      // configure VP0[7:0] in 4CH pixel interleaved
                gpio_i2c_write(addr, 0x8c, 0x80);      // 960H data stream on VP0[7:0]
                gpio_i2c_write(addr, 0x8a, 0x31);      // enable VP0[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write(addr, 0xf3, 0x0a);      // configure SCLK2A : base clock 36Mhz; output 4X base clock (144M)
                gpio_i2c_write(addr, 0xf2, 0xC3);      // enable SCLK2A. Drv =12mA . Inv clck.
            }
            else if(RN63XX_SYID[ii] == RN6304_DEV_ID) {
                gpio_i2c_write(addr, 0x91, 0x02);      // configure VP0[7:0] in 4CH pixel interleaved
                gpio_i2c_write(addr, 0x92, 0x80);      // 960H data stream on VP0[7:0]
                gpio_i2c_write(addr, 0x90, 0x31);      // enable VP0[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write(addr, 0x83, 0x0a);      // configure SCLK2A : base clock 36Mhz; output 4X base clock (144M)
                gpio_i2c_write(addr, 0x82, 0xC3);      // enable SCLK2A. Drv =12mA . Inv clck.
            }
            else if((RN63XX_SYID[ii] == RN6318_DEV_ID) || (RN63XX_SYID[ii] == RN6318A_DEV_ID)) {
                gpio_i2c_write (addr, 0x94, 0x02); // configure VP3[7:0] in 4CH pixel interleaved
                gpio_i2c_write (addr, 0x95, 0x80); // 960H data stream on VP3[7:0]
                gpio_i2c_write (addr, 0x93, 0x31); // enable VP3[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write (addr, 0xF7, 0x0A); // configure SCLK3A : base clock 36Mhz; output 4X base clock (144M)
                gpio_i2c_write (addr, 0xF6, 0xC3); // enable SCLK3A. Drv =12mA . Inv clck.

                gpio_i2c_write (addr+2, 0x8E, 0x02); // configure VP1[7:0] in 4CH pixel interleaved
                gpio_i2c_write (addr+2, 0x8F, 0x80); // 960H data stream on VP1[7:0]
                gpio_i2c_write (addr+2, 0x8D, 0x31); // enable VP1[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write (addr+2, 0x89, 0x0A); // configure SCLK1B : base clock 36Mhz; output 4X base clock (144M)
                gpio_i2c_write (addr+2, 0x88, 0xC3); // enable SCLK1B. Drv =12mA . Inv clck.
            }
        }
        else { // 720H
            // Data Bus = {ch0,ch1,ch2,ch3}    from  VP0[7:0] {pin98,...,pin106}
            // Clock    = 108MHz (720H)        from SCLK2A {pin108}
            if(RN63XX_SYID[ii] == RN6366_DEV_ID) {
                gpio_i2c_write(addr, 0x8b, 0x02);      // configure VP0[7:0] in 4CH pixel interleaved
                gpio_i2c_write(addr, 0x8c, 0x00);      // 720H data stream on VP0[7:0]
                gpio_i2c_write(addr, 0x8a, 0x31);      // enable VP0[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write(addr, 0xf3, 0x02);      // configure SCLK2A : base clock 27Mhz; output 4X base clock (108M)
                gpio_i2c_write(addr, 0xf2, 0xC3);      // enable SCLK2A. Drv =12mA . Inv clck.
            }
            else if(RN63XX_SYID[ii] == RN6304_DEV_ID) {
                gpio_i2c_write(addr, 0x91, 0x02);      // configure VP0[7:0] in 4CH pixel interleaved
                gpio_i2c_write(addr, 0x92, 0x00);      // 720H data stream on VP0[7:0]
                gpio_i2c_write(addr, 0x90, 0x31);      // enable VP0[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write(addr, 0x83, 0x02);      // configure SCLK2A : base clock 27Mhz; output 4X base clock (108M)
                gpio_i2c_write(addr, 0x82, 0xC3);      // enable SCLK2A. Drv =12mA . Inv clck.
            }
            else if((RN63XX_SYID[ii] == RN6318_DEV_ID) || (RN63XX_SYID[ii] == RN6318A_DEV_ID)) {
                gpio_i2c_write (addr, 0x94, 0x02); // configure VP3[7:0] in 4CH pixel interleaved
                gpio_i2c_write (addr, 0x95, 0x00); // 720H data stream on VP3[7:0]
                gpio_i2c_write (addr, 0x93, 0x31); // enable VP3[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write (addr, 0xF7, 0x02); // configure SCLK3A : base clock 27Mhz; output 4X base clock (108M)
                gpio_i2c_write (addr, 0xF6, 0xC3); // enable SCLK3A. Drv =12mA . Inv clck.

                gpio_i2c_write (addr+2, 0x8E, 0x02); // configure VP1[7:0] in 4CH pixel interleaved
                gpio_i2c_write (addr+2, 0x8F, 0x00); // 720H data stream on VP1[7:0]
                gpio_i2c_write (addr+2, 0x8D, 0x31); // enable VP1[7:0] un-scaled image (1X base clock) output; Drv = 12mA

                gpio_i2c_write (addr+2, 0x89, 0x02); // configure SCLK1B : base clock 27Mhz; output 4X base clock (108M)
                gpio_i2c_write (addr+2, 0x88, 0xC3); // enable SCLK1B. Drv =12mA . Inv clck.
            }
        }

        //---------------------------------------------------
        // Finish configuration then turn-on rquired macros
        //--------------------------------------------------
        gpio_i2c_write(addr, 0x81, 0x00);
        msleep(5);
        gpio_i2c_write(addr, 0x81, 0x3F);   // turn-on all video decoders & audio adc/dac
        gpio_i2c_write(addr+2, 0x81, 0x1F);   // turn-on all video decoders & audio adc
    }
}

static void rn6366_audio_set_samplerate(int chip, int sample_rate)
{
}

static void rn6366_audio_set_volume(int chip, unsigned int volume)
{
    int ii;
    if(!(volume < 16)) {
        volume = 15;
    }

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        unsigned char tmpRD = 0;
        if(!RN63XX_SYID[ii] || RN63XX_ACMD[ii] != 0) {
            continue;
        }

        gpio_i2c_write(RN63XX_ADDR[ii], 0xFF, 0x04);  // configure audio codec

        tmpRD = gpio_i2c_read(RN63XX_ADDR[ii], 0x5D);

        tmpRD &= 0xE0;
        tmpRD += (volume<<1);

        gpio_i2c_write(RN63XX_ADDR[ii], 0x5D, tmpRD);  // Setting DAC Output Level;

        return;
    }
}

static void rn6366_audio_set_playback(int chip, int channel)
{
    int ii;

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        if(!RN63XX_SYID[ii] || RN63XX_ACMD[ii] != 0) {
            continue;
        }

        gpio_i2c_write(RN63XX_ADDR[ii], 0xFF, 0x04);  // configure audio codec
        gpio_i2c_write(RN63XX_ADDR[ii], 0x76, 0x10);  // DAC source is from PB0A

        return;
    }
}

static void rn6366_audio_set_liveloop(int chip, int channel)
{
    int ii;
    if(channel < 0 || channel >= 16){
        return;
    }

    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        if(!RN63XX_SYID[ii] || RN63XX_ACMD[ii] != 0) {
            continue;
        }

        gpio_i2c_write(RN63XX_ADDR[ii], 0xFF, 0x04);  // configure audio codec
        gpio_i2c_write(RN63XX_ADDR[ii], 0x76, channel);  // DAC source is from PB0A

        return;
    }
}

static void rn6366_audio_init(int chip)
{
    int ii;
    for(ii = 0; ii < ARRAY_SIZE(RN63XX_ADDR); ii ++) {
        int Addr = RN63XX_ADDR[ii];
        int Numb = RN63XX_NUMB[ii];
        int jj;

        if(!RN63XX_SYID[ii]) {
            continue;
        }

        for(jj = 0; jj < Numb; jj ++) {
            int addr = Addr + jj*2;

            // (1) Sample rate 8k
            gpio_i2c_write(addr, 0xFF, 0x04);  // configure audio codec
            gpio_i2c_write(addr, 0x7A, 0x04);  // 256 clock edges for a sample period;
                                    // mask silent channel for audio mixer
            gpio_i2c_write(addr, 0x00, 0x09);  // free clock running. sample rate 8K.
                                    // fAMCLK = 256X 8KHZ
            gpio_i2c_write(addr, 0x06, 0x00);  // AUIDO_SDIV = 0 -> fASCLK = fAMCLK/1

            // configure audio codec gain
            gpio_i2c_write(addr, 0x5A, 0x6E);  // audio adc ch0 ~2 amplifier on, set adc common gain
            gpio_i2c_write(addr, 0x5B, 0x33);  // audio adc ch0,ch1 gain
            gpio_i2c_write(addr, 0x5C, 0x33);  // audio adc ch2,ch3 gain

            gpio_i2c_write(addr, 0x5D, 0x70);  // audio adc ch40 off, audio adc ch3 amplifier on, set dac gain
            //gpio_i2c_write(addr, 0x5D, 0x6D);  // audio adc ch3/40 amplifier on, set dac gain
            gpio_i2c_write(addr, 0x69, 0x02);  // audio adc ch40 gain
            gpio_i2c_write(addr, 0x5F, 0x38);  // enable audio adc ch40

            // (2) Audio Playback
            //     - ACLKP : Out ,ASYNP : Out
            //     - Standard I2S ,16bit PCM, MSB first

            // assume audio playback interface has eighteen 16bit channels with I2S protocol master mode
            gpio_i2c_write(addr, 0x0A, 0x80);   // RegA[3:0]: ACLKP IO delay , RegA[7:4]: ASYNP IO delay
            gpio_i2c_write(addr, 0x0B, 0x08);   // RegB[3:0]: ADATP IO delay

            gpio_i2c_write(addr, 0x08, 0x7F);   // left/right segment has 128 clock edges -> ADUIO_PB0_LRDIV = 127
            gpio_i2c_write(addr, 0x09, 0x70);   // I2S_STD ; each segment has 8 channels -> AUDIO_PB0_SG_NUM = 7

            gpio_i2c_write(addr, 0x67, 0x00);   // assign time slot 0 as PB0A
            gpio_i2c_write(addr, 0x68, 0x08);   // assign time slot 8 as PB0B

            // (3) Audio REC
            //     - ACLKR : out, ASYNR :out
            //     - Standard I2S ,16bit PCM, MSB first , total transferred channel= 8 (ACH0~ACH3)

            // assume audio rec/mix interface has two 16bit channels with I2S protocol master mode
            gpio_i2c_write(addr, 0x0D, 0x7F);  // left/right segment has 128 clock edges -> ADUIO_REC_LRDIV = 127
            gpio_i2c_write(addr, 0x0E, 0x0C);  // I2S_STD ; 16 audio sounds in ADATR

            if(!(addr & 0x02)) {
                gpio_i2c_write(addr, 0x20, 0x00);  // ADATR : AVIN0 in left   -> REC_SEQ0 = 0
                gpio_i2c_write(addr, 0x21, 0x01);  // ADATR : AVIN1 in left   -> REC_SEQ1 = 1
                gpio_i2c_write(addr, 0x22, 0x02);  // ADATR : AVIN2 in left   -> REC_SEQ2 = 2
                gpio_i2c_write(addr, 0x23, 0x03);  // ADATR : AVIN3 in left   -> REC_SEQ3 = 3
            }
            else {
                gpio_i2c_write(addr, 0x24, 0x00);  // ADATR : AVIN8 in left   -> REC_SEQ4 = 4
                gpio_i2c_write(addr, 0x25, 0x01);  // ADATR : AVIN8 in left   -> REC_SEQ5 = 5
                gpio_i2c_write(addr, 0x26, 0x02);  // ADATR : AVIN8 in left   -> REC_SEQ6 = 6
                gpio_i2c_write(addr, 0x27, 0x03);  // ADATR : AVIN8 in left   -> REC_SEQ7 = 7
            }

            gpio_i2c_write(addr, 0x28, 0x08);  // ADATR : AVIN4 in right  -> REC_SEQ8 = 4
            gpio_i2c_write(addr, 0x29, 0x09);  // ADATR : AVIN5 in right  -> REC_SEQ9 = 5
            gpio_i2c_write(addr, 0x2A, 0x0A);  // ADATR : AVIN6 in right  -> REC_SEQA = 6
            gpio_i2c_write(addr, 0x2B, 0x0B);  // ADATR : AVIN7 in right  -> REC_SEQB = 7
            gpio_i2c_write(addr, 0x2C, 0x0C);  // ADATR : AVINC in right  -> REC_SEQC = C
            gpio_i2c_write(addr, 0x2D, 0x0D);  // ADATR : AVIND in right  -> REC_SEQD = D
            gpio_i2c_write(addr, 0x2E, 0x0E);  // ADATR : AVINE in right  -> REC_SEQE = E
            gpio_i2c_write(addr, 0x2F, 0x0F);  // ADATR : AVINF in right  -> REC_SEQF = F

            gpio_i2c_write(addr, 0x30, 0x80);  // RegA[3:0]: ACLKR IO delay , RegA[7:4]: ASYNR IO delay
            gpio_i2c_write(addr, 0x31, 0x88);  // RegB[3:0]: ADATR IO delay , RegB[7:4]: ADATM IO delay

            if(!(addr & 0x02)) {
                if(!RN63XX_ACMD[ii]) {
                    gpio_i2c_write(addr, 0x07, 0x31);  // enable audio playback interface in 16bit master mode
#if defined(GPIO_PLAT_TYPE1)
                    if(GPIO_PROBE_VER1 == gpio_Platform_VerProbe()) {
                        gpio_i2c_write(addr, 0x0C, 0x41);  // enable ADATR pin , 16bit slave mode
                    }
                    else
#endif
                    {
                        gpio_i2c_write(addr, 0x0C, 0x71);  // enable ADATR pin , 16bit master mode
                    }
                    gpio_i2c_write(addr, 0x76, 0x10);  // DAC source is from PB0A
                    gpio_i2c_write(addr, 0x4E, 0x08);  // audio cascade first stage
                    gpio_i2c_write(addr, 0x67, 0x00);
                }
                else {
                    gpio_i2c_write(addr, 0x07, 0x00);  // disable audio playback interface
                    gpio_i2c_write(addr, 0x0C, 0x00);  // enable ADATR pin , 16bit slave mode
                    gpio_i2c_write(addr, 0x4E, 0x0E);  // audio cascade third stage
                    gpio_i2c_write(addr, 0x67, 0x00);
                }
            }
            else {
			    if(!RN63XX_ACMD[ii]) {
                    gpio_i2c_write(addr, 0x07, 0x00);  // disable audio playback interface
                    gpio_i2c_write(addr, 0x0C, 0x00);  // disable ADATR pin
                    gpio_i2c_write(addr, 0x4E, 0x05);  // audio cascade second stage
                    gpio_i2c_write(addr, 0x67, 0x40);  // swap ACASI & ACASO
			    }
			    else {
                    gpio_i2c_write(addr, 0x07, 0x00);  // disable audio playback interface
                    gpio_i2c_write(addr, 0x0C, 0x00);  // disable ADATR pin
                    gpio_i2c_write(addr, 0x4E, 0x07);  // audio cascade last stage
                    gpio_i2c_write(addr, 0x67, 0x40);  // swap ACASI & ACASO
			    }
            }
        }
    }
}

static int  rn6366_regdbg(audio_reg_xdbg_t * reg_dbg)
{
    int addr = 0;

    if(!reg_dbg
    && reg_dbg->chip >= ARRAY_SIZE(RN63XX_ADDR)) {
        return -1;
    }

    addr = RN63XX_ADDR[reg_dbg->chip];

    if(!reg_dbg->rw_flg) {  //I2C Device Write Operation
        gpio_i2c_write(addr, reg_dbg->addr, reg_dbg->value);
    }
    else {
        reg_dbg->value = gpio_i2c_read(addr, reg_dbg->addr);
    }

    return 0;
}

