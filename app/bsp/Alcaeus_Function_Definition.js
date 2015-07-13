#include "Alcaeus_Registers_Definition.js"

#define writeByte(x,y,z)  RegMaskWrite32((x),(y),0,7,(z))
#define writeDword(x,y,z) RegMaskWrite32((x),(y),0,31,(z))
#define readDword(x,y)  RegMaskRead32((x),(y),0,31)

#define RES_720H                0
#define RES_960H                1

#define MAX_DECODERS            8
//#define QUAD_DECODERS           4

/* HSCALE_RATIO Settings */
#define PIXELS_WIDTH960         0x0
#define PIXELS_WIDTH720         0x0
#define PIXELS_WIDTH704         0x5D18
#define PIXELS_WIDTH360         0x100000
#define PIXELS_WIDTH352         0x10BA2F
#define PIXELS_WIDTH320         0x140000

// Default video color settings
#define SHARPNESS_DEFAULT       50
#define SATURATION_DEFAULT      5000
#define BRIGHTNESS_DEFAULT      5000
#define CONTRAST_DEFAULT        5000
#define HUE_DEFAULT             5000
#define BRIGHTNESS_NTSC_DEFAULT 6000

/* Cascade direction */
#define CASCADE_DIR_DISABLED        0x0
#define CASCADE_DIR_INPUT           0x1
#define CASCADE_DIR_OUTPUT          0x2

/* video omux mode */
#define OMUXMODE_DISABLED                         0x0
#define OMUXMODE_PASSTHROUGH                      0x1
#define OMUXMODE_1_STREAM_REMUX                   0x2
#define OMUXMODE_2_STREAM_REMUX                   0x3
#define OMUXMODE_4_STREAM_REMUX                   0x4
#define OMUXMODE_8_STREAM_REMUX                   0x5

/* dvp demux mode */
#define DVP_DEMUX_MODE_1STREAM                    0x0
#define DVP_DEMUX_MODE_2STREAMS                   0x1
#define DVP_DEMUX_MODE_3STREAMS                   0x2
#define DVP_DEMUX_MODE_4STREAMS                   0x3

/* dvp demux clock mode */
#define DVP_DEMUX_MODE_SINGLE_EDGE                0
#define DVP_DEMUX_MODE_DUAL_EDGE                  1

/* dvp demux clock polarity */
#define DVP_DEMUX_CLK_POL_NONINVERTED             0
#define DVP_DEMUX_CLK_POL_INVERTED                1

/* dvp omux clock polarity */
#define DVP_OMUX_CLK_POL_NONINVERTED              0
#define DVP_OMUX_CLK_POL_INVERTED                 1

/* dvp omux clock mode */
#define DVP_OMUX_CLK_MODE_SINGLE_EDGE             0
#define DVP_OMUX_CLK_MODE_DUAL_EDGE               1

/* dvp omux stream bandwidth */
#define DVP_OMUX_STREAM_BW_27MBYTES_1STREAM       0x0
#define DVP_OMUX_STREAM_BW_13PT5MBYTES_1STREAM    0x1

/* DVP_OMUX_PACK_ENG_SEL */
#define DVP_OMUX_PACK_ENG_SEL_LOW                 0x0
#define DVP_OMUX_PACK_ENG_SEL_HIGH                0x1

/* Video framer select */
#define VIDEO_FRAMER_0                            0x0
#define VIDEO_FRAMER_1                            0x1
#define VIDEO_FRAMER_2                            0x2
#define VIDEO_FRAMER_3                            0x3

/*
  video_expand_mode:
      0: D1 to D1
      1: 960 to 960
      2: 960 cropped to D1
      3: D1 HBI extend to 960H
      4: D1 active extend to 960H
      5: CIF to CIF
      6: Half 960H to Half 960H
      7: Half 960H HBI extend to 960H
      8: Half 960H cropped to Half D1
      9: Half D1 HBI extend to 960H
*/
typedef enum{
    VIDEO_EXTEND_D1              = 0,
    VIDEO_EXTEND_960H            = 1,
    VIDEO_EXTEND_960H_D1         = 2,
    VIDEO_EXTEND_D1_HBI_960H     = 3,
    VIDEO_EXTEND_D1_ACT_960H     = 4,
    VIDEO_EXTEND_CIF             = 5,
    VIDEO_EXTEND_HLF_960H        = 6,
    VIDEO_EXTEND_HLF_960H_HBI    = 7,
    VIDEO_EXTEND_HLF_960H_HLF_D1 = 8,
    VIDEO_EXTEND_HLF_D1_HBI_960H = 9
}VIDEO_EXTEND_MODES;

/*
 * video_frmr_mode:
 *        0: D1 NTSC Normal Frame{I}
 *        1: D1 PAL Normal Frame{I}
 *        2: E1 NTSC Normal Frame{I}
 *        3: E1 PAL Normal Frame{I}
 *        4: D1 NTSC Normal Frame{II}
 *        5: D1 PAL Normal Frame{II}
 *        6: E1 NTSC Normal Frame{II}
 *        7: E1 PAL Normal Frame{II}
 *        8: D1 NTSC Big Frame x2way
 *        9: D1 PAL Big Frame x2way
 *        A: E1 NTSC Big Frame x2way
 *        B: E1 PAL Big Frame x2way
 *        C: D1 NTSC Big Frame x4way
 *        D: D1 PAL Big Frame x4way
 *        E: E1 NTSC Big Frame x4way
 *        F: E1 PAL Big Frame x4way
*/
typedef enum{
    VIDEO_D1_NTSC_NORMAL_FRAMER_I  = 0x0,
    VIDEO_D1_PAL_NORMAL_FRAMER_I   = 0x1,
    VIDEO_E1_NTSC_NORMAL_FRAMER_I  = 0x2,
    VIDEO_E1_PAL_NORMAL_FRAMER_I   = 0x3,
    VIDEO_D1_NTSC_NORMAL_FRAMER_II = 0x4,
    VIDEO_D1_PAL_NORMAL_FRAMER_II  = 0x5,
    VIDEO_E1_NTSC_NORMAL_FRAMER_II = 0x6,
    VIDEO_E1_PAL_NORMAL_FRAMER_II  = 0x7,
    VIDEO_D1_NTSC_BIG_FRAMER_X2WAY = 0x8,
    VIDEO_D1_PAL_BIG_FRAMER_X2WAY  = 0x9,
    VIDEO_E1_NTSC_BIG_FRAMER_X2WAY = 0xA,
    VIDEO_E1_PAL_BIG_FRAMER_X2WAY  = 0xB,
    VIDEO_D1_NTSC_BIG_FRAMER_X4WAY = 0xC,
    VIDEO_D1_PAL_BIG_FRAMER_X4WAY  = 0xD,
    VIDEO_E1_NTSC_BIG_FRAMER_X4WAY = 0xE,
    VIDEO_E1_PAL_BIG_FRAMER_X4WAY  = 0xF
}VIDEO_FRAMER_MODES;

typedef enum
{
    EQ_MODE_AUTO        = 0,
    EQ_MODE_MANUAL      = 1,
    EQ_MODE_FULLMANUAL  = 2,
    EQ_MODE_DISABLE     = 3
}EQ_MODE_SEL;

typedef enum
{
    EQ_CABLE_CAT5       = 0,
    EQ_CABLE_COAX       = 1,
}EQ_CABLE_TYPE;

typedef enum{
    OUTPUT_RATE_13PT5_MHZ = 135,
    OUTPUT_RATE_27_MHZ = 270,
    OUTPUT_RATE_54_MHZ = 540,
    OUTPUT_RATE_108_MHZ = 1080,
    OUTPUT_RATE_144_MHZ = 1440
}OUTPUT_RATES;

typedef enum{
    AUDIO_RECORD_OUTPUT = 0,
    AUDIO_MIX_OUTPUT    = 1,
    AUDIO_PLAYBACK      = 2
}AUDIO_OUTPUT_INTERFACES;

typedef enum{
    AUDIO_DATA_WIDTH_8BIT  = 0,
    AUDIO_DATA_WIDTH_16BIT = 1,
    AUDIO_DATA_WIDTH_24BIT = 2
}AUDIO_DATA_WIDTH;

typedef enum{
    AUDIO_SLAVE_MODE  = 0,
    AUDIO_MASTER_MODE = 1
}AUDIO_INTERFACE_MODES;

typedef enum{
    AUDIO_ULAW_COMPRESSION = 0,
    AUDIO_ALAW_COMPRESSION = 1,
    AUDIO_UNCOMPRESSION    = 2
}AUDIO_COMPRESSION_MODES;

typedef enum{
    AUDIO_2_CHANNELS         = 0,
    AUDIO_4_CHANNELS         = 1,
    AUDIO_8_CHANNELS         = 2,
    AUDIO_16_CHANNELS        = 3,
    AUDIO_24_CHANNELS        = 4,
    AUDIO_PLAYBACK_R_CHANNEL = 5,
    AUDIO_PLAYBACK_L_CHANNEL = 6
}AUDIO_NUM_CHANNELS;

typedef enum{
    BEST_CROSS_COLOR                 = 0,
    CROSS_COLOR_MUCH_BETTER          = 1,
    CROSS_COLOR_SLIGHTLY_BETTER      = 2,
    BEST_DOT_CRAWL_AND_HANG_DOT      = 3,
    DEFAULT_VIDEO_QUALITY            = 4
}VIDEO_QUALITY;

/* Colibri operating frequency clock */
#define COLIBRI_FCLK_720MHZ_CRYSTAL_48MHZ          0x0F
#define COLIBRI_FCLK_720MHZ_CRYSTAL_24MHZ          0x1E
#define COLIBRI_FCLK_624MHZ_CRYSTAL_48MHz          0x0D
#define COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz          0x1A

static unsigned char _xtal_freq_clk = COLIBRI_FCLK_720MHZ_CRYSTAL_48MHZ;

int ALCS_initColibri(int DEVICE, int ic_typ, const unsigned char freq_clk)
{
    // Step 1 - Setup clocks and tune analog filter
    int result = 0;
    _xtal_freq_clk = freq_clk;

    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_TUNE1, 0x41);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_TUNE2, !ic_typ ?0xC0 :0x20);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_XTAL, 0x0F);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_PLL, !ic_typ ?0x3F :0x3B);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_PLL2, freq_clk);  //24/48MHz crystal
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_PLL3, 0x67);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_REF, 0x13); // default value (bit[4:3]ref_adc=0x2), Set ref_adc = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_PWRDN, 0x18);
    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_TUNE3, 0x40);

    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_TUNE1, 0x41);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_TUNE2, !ic_typ ?0xC0 :0x20);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_XTAL, 0x0F);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_PLL, !ic_typ ?0x3F :0x3B);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_PLL2, freq_clk);  //24/48MHz crystal
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_PLL3, 0x67);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_REF, 0x13); // default value (bit[4:3]ref_adc=0x2), Set ref_adc = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_PWRDN, 0x18);
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_TUNE3, 0x40);

    scripting.sleep(3);

    writeByte(DEVICE, ALCS_AFEA_SUP_BLK_TUNE3, 0x00);
    writeByte(DEVICE, ALCS_AFEA_ADC_INT5_STAB_REF, 0x52); // Set int5_boost = 0
    writeByte(DEVICE, ALCS_AFEB_SUP_BLK_TUNE3, 0x00);
    writeByte(DEVICE, ALCS_AFEB_ADC_INT5_STAB_REF, 0x52); // Set int5_boost = 0

    // Step 2 - power up all 8 channels, clear pd_buffer
    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_PWRDN_CLAMP, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_PWRDN_CLAMP, 0x00);

    // Step 3 - Enable channel calibration
    writeByte(DEVICE, ALCS_AFEA_ADC_COM_QUANT, 0x02);
    writeByte(DEVICE, ALCS_AFEB_ADC_COM_QUANT, 0x02);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_FB_FRCRST, !ic_typ ?0x17 :0x14);

    scripting.sleep(3);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_CAL_ATEST, 0x10);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_CAL_ATEST, 0x10);

    scripting.sleep(3);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_FB_FRCRST, 0x07);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_FB_FRCRST, 0x07);

    // Step 4 - Setup each channel as single-ended input
    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_NTF_PRECLMP_EN, !ic_typ ?0xF0 :0x00);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_INPUT, 0x00);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_INPUT, 0x00);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_DCSERVO_DEM, !ic_typ ?0x03 :0x03);

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_CTRL_DAC1, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude

    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_CTRL_DAC23, !ic_typ ?0x67 :0x24);

    // Disable for termination resistor and 100kOhm resistor to mid voltage: term_en to 0, res_en to 0
    writeByte(DEVICE, ALCS_AFEA_CH1_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEA_CH2_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEA_CH3_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEA_CH4_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEB_CH1_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEB_CH2_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEB_CH3_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    writeByte(DEVICE, ALCS_AFEB_CH4_ADC_QGAIN_RES_TRM, !ic_typ ?0xf2 :0x02);
    
    /* Power on PLL, PLL frequency = (48e6 * 12.288)/6 = 98.304 MHz */
    writeDword(DEVICE, ALCS_PLL9_INT_FRAC, 0x189374BC);  /* Default Value */
    writeDword(DEVICE, ALCS_PLL9_CTRL,
        (freq_clk == COLIBRI_FCLK_720MHZ_CRYSTAL_24MHZ || freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz)
            ? 0x00522513
            : 0x00522516);   /* Power on PLL, PLL frequency = 98.304MHz */
    
    return result;
}

/* LPF Coeff */
unsigned long LPF_Coeff[4][4] =
{
    /*  Coeff0/1    Coeff2/3    Coeff4/5    Coeff6/7 */
    {  0xFFE9FFFC, 0x00330000, 0xFFA4FFCF, 0x013F0135  }, /* LPF 7 */
    {  0x000D000E, 0x001EFFDE, 0xFF76001D, 0x015F00F7  }, /* LPF 8 */
    {  0x000FFFE8, 0xFFDB0001, 0xFF6E005A, 0x01A800BE  }, /* LPF 9 */
    {  0xFFF6FFF3, 0xFFC6001D, 0xFF83005C, 0x01C10094  }, /* LPF 10 */
};

/* EQ UTP Coeff for D1 */
unsigned long EQ_UTP_Coeff_D1[15][4] =
{
    /*  Coeff0/1    Coeff2/3    Coeff4/5    Coeff6/7 */
    {  0xfff6ffef, 0xffefffe1, 0xffe6ffbb, 0x4a5fe3b  }, /* 200 meters */
    {  0xfff5ffe9, 0xffefffd5, 0xfff0ff9b, 0x5cffd4c  }, /* 250 meters */
    {  0xfff6ffe0, 0xfff3ffc5, 0xaff72,    0x74bfc06  }, /* 300 meters */
    {  0xfffaffd5, 0xfffeffaf, 0x3dff3a,   0x9337a49  }, /* 350 meters */
    {  0x2ffc6,    0x12ff92,   0x97feee,   0xba4f7ef  }, /* 400 meters */
    {  0x11ffb2,   0x35ff6b,   0x12afe85,  0xec974c1  }, /* 450 meters */
    {  0x29ff96,   0x6cff35,   0x211fdf3,  0x12d6f078 }, /* 500 meters */
    {  0x4fff70,   0xc3feea,   0x36ffd28,  0x1812eab5 }, /* 550 meters */
    {  0x88ff3a,   0x146fe82,  0x579fc0c,  0x1ed962f7 }, /* 600 meters */
    {  0xdefeef,   0x209fdef,  0x877fa7d,  0x27a1d895 }, /* 650 meters */
    {  0x15cfe85,  0x327fd21,  0xccdf84b,  0x3309caad }, /* 700 meters */
    {  0x214fdef,  0x4c6fbff,  0x1306f535, 0x41e03810 }, /* 750 meters */
    {  0x31ffd1d,  0x71efa65,  0x1be1f0dd, 0x55349f2b }, /* 800 meters */
    {  0x49dfbf3,  0xa79f824,  0x2866eac2, 0x6e6c7de6 }, /* 850 meters */
    {  0x6c0fa4f,  0xf41f4f5,  0x39ffe22f, 0x8f5e5174 }  /* 900 meters */
};

/* EQ UTP Coeff for Effio 960H */
unsigned long EQ_UTP_Coeff_960H[15][4] =
{
    /*  Coeff0/1    Coeff2/3    Coeff4/5    Coeff6/7 */
    {  0xfff5ffeb, 0xfff1ffdb, 0xffefffab, 0x55a7db0  }, /* 200 meters */
    {  0xfff6ffe1, 0xfff5ffc9, 0x9ff7f,    0x6f4fc56  }, /* 250 meters */
    {  0xfffbffd4, 0x2ffb2,    0x43ff41,   0x9167a67  }, /* 300 meters */
    {  0x5ffc2,    0x1bff90,   0xb0fee8,   0xbee77a6  }, /* 350 meters */
    {  0x18ffa9,   0x49ff5f,   0x16efe66,  0xfbc73bf  }, /* 400 meters */
    {  0x3aff84,   0x97ff18,   0x2aafda7,  0x14d7ee3d }, /* 450 meters */
    {  0x73ff4d,   0x118feaf,  0x4a5fc8d,  0x1bb6e67b }, /* 500 meters */
    {  0xcefefd,   0x1e5fe14,  0x7bffaeb,  0x24fc5b94 }, /* 550 meters */
    {  0x15cfe86,  0x325fd2c,  0xc87f87d,  0x3184cc47 }, /* 600 meters */
    {  0x23afdd3,  0x513fbd3,  0x13caf4df, 0x4280b6d7 }, /* 650 meters */
    {  0x38dfcc9,  0x806f9cf,  0x1eb5ef7e, 0x599118d7 }, /* 700 meters */
    {  0x592fb3b,  0xc7ff6cd,  0x2efbe782, 0x78f16ee1 }, /* 750 meters */
    {  0x89ff8ea,  0x133af251, 0x470ddba9, 0xa3b6343b }, /* 800 meters */
    {  0xd35f575,  0x1d52eba3, 0x6a74ca20, 0xde1ae24c }, /* 850 meters */
    {  0x1414f04e, 0x2c60e1b4, 0x9e41b03a, 0x7fffffff }  /* 900 meters */
};

/* EQ Coaxial Coeff for D1 */
unsigned long EQ_Coax_Coeff_D1[15][4] =
{
    /*  Coeff0/1    Coeff2/3    Coeff4/5    Coeff6/7 */
    {  0xfffbfffa, 0xfff7fff5, 0xffeeffe9, 0x2e17f7c  }, /* 200 meters */
    {  0xfffafff8, 0xfff5fff2, 0xffeaffe2, 0x329ff4c  }, /* 250 meters */
    {  0xfff9fff7, 0xfff3ffef, 0xffe7ffda, 0x37a7f15  }, /* 300 meters */
    {  0xfff8fff5, 0xfff2ffeb, 0xffe5ffd0, 0x3d3fed5  }, /* 350 meters */
    {  0xfff7fff2, 0xfff1ffe7, 0xffe5ffc6, 0x437fe8c  }, /* 400 meters */
    {  0xfff6fff0, 0xfff0ffe2, 0xffe6ffbb, 0x4a6fe37  }, /* 450 meters */
    {  0xfff6ffed, 0xfff0ffdd, 0xffe8ffae, 0x5227dd5  }, /* 500 meters */
    {  0xfff6ffea, 0xfff0ffd7, 0xffeeffa0, 0x5ac7d64  }, /* 550 meters */
    {  0xfff6ffe7, 0xfff1ffd1, 0xfff6ff8f, 0x6467ce3  }, /* 600 meters */
    {  0xfff7ffe3, 0xfff3ffc9, 0x3ff7d,    0x6f1fc4e  }, /* 650 meters */
    {  0xfff8ffdf, 0xfff7ffc1, 0x13ff67,   0x7b17ba3  }, /* 700 meters */
    {  0xfffaffda, 0xfffbffb8, 0x2aff4f,   0x8877ae0  }, /* 750 meters */
    {  0xfffcffd4, 0x2ffad,    0x47ff33,   0x9767a00  }, /* 800 meters */
    {  0xffce,     0xaffa1,    0x6cff13,   0xa81f8ff  }, /* 850 meters */
    {  0x4ffc7,    0x15ff94,   0x9bfeee,   0xbacf7da  }  /* 900 meters */
};

/* EQ Coaxial Coeff for Effio 960H */
unsigned long EQ_Coax_Coeff_960H[15][4] =
{
    /*  Coeff0/1    Coeff2/3    Coeff4/5    Coeff6/7 */
    {  0xfffafff9, 0xfff7fff4, 0xffedffe6, 0x30f7f61  }, /* 200 meters */
    {  0xfff9fff7, 0xfff5fff0, 0xffeaffdd, 0x369ff24  }, /* 250 meters */
    {  0xfff8fff4, 0xfff3ffec, 0xffe8ffd3, 0x3d07edc  }, /* 300 meters */
    {  0xfff7fff1, 0xfff2ffe7, 0xffe8ffc7, 0x444fe86  }, /* 350 meters */
    {  0xfff6ffee, 0xfff2ffe1, 0xffeaffba, 0x4c7fe22  }, /* 400 meters */
    {  0xfff6ffeb, 0xfff2ffdb, 0xffefffab, 0x55cfdab  }, /* 450 meters */
    {  0xfff6ffe7, 0xfff3ffd4, 0xfff7ff99, 0x6057d1f  }, /* 500 meters */
    {  0xfff7ffe3, 0xfff5ffcc, 0x4ff85,    0x6c4fc7b  }, /* 550 meters */
    {  0xfff8ffde, 0xfff9ffc3, 0x17ff6d,   0x79e7bbb  }, /* 600 meters */
    {  0xfffaffd8, 0xffffffb8, 0x32ff51,   0x8957ad9  }, /* 650 meters */
    {  0xfffdffd2, 0x7ffac,    0x56ff30,   0x9adf9d1  }, /* 700 meters */
    {  0x2ffca,    0x12ff9d,   0x84ff09,   0xaed789c  }, /* 750 meters */
    {  0x8ffc1,    0x20ff8c,   0xc1fedb,   0xc587733  }, /* 800 meters */
    {  0x10ffb6,   0x33ff78,   0x10efea5,  0xdf6758d  }, /* 850 meters */
    {  0x1affaa,   0x4bff60,   0x170fe65,  0xfcdf3a2  }  /* 900 meters */
};

void 
ALCS_Reset(int DEVICE)
{
    int i = 0;

    for(i = 0; i < MAX_DECODERS; i++) {
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL+(0x200*i), 0x00008000); //Resets video decoder core
    }
    writeDword(DEVICE, ALCS_CHIP_RST, 0xFFFFFF);
}

static int
ALCS_initNTSC(int DEVICE, int ic_typ, int channel)
{
    int i = channel;
    unsigned long value;
    unsigned long offset;

    if(i >= MAX_DECODERS) {
        return -1;
    }

    offset = 0x200*i;

    if(!ic_typ) {
        // Reset decoder
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00008000); //Resets video decoder core
        scripting.sleep(5);
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00000000);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E001 &0xFFFEFFFF);    /* Default value, vid_fmt_sel=4b0001  NTSC-M */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset,  0xABF4000A);    /* Close 12dB Amp. */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x00100621 | 0x30080);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x00100621 | 0x30080);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x00080000);  /* for NTSC mode, disable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00002040);
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0x001C1E0F);

        writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x43E00000);  /* Default value for NTSC, sc_step=0x43E00000 */
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x5E2D0079);/* 0x5E2D0074, Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074 */
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x1E1E001A); /* 0x1C1E7014Default value for NTSC, v656blank=0x18,vactive=0x1e7, vblank=0x014 */

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
    
        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x0018012F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x1E194020);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x080A9A08);// 0x080A0A08;
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x20102030);

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL  + offset, 0x00200210);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401060DC);      /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000012);

        /*Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x21f [9:0] */
        writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x0000221f);        /* Different for 624MHz=0x221, 720MHz=0x21f */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x6680028a);

        writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1E2BE2FE);         /* Go back to 108MHz, for 624MHz = 0x1e2be2fe */
    }
    else {
        /* reset video decoder */
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x8000);
        scripting.sleep(5);
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E001);    /* Default value, vid_fmt_sel=4b0001  NTSC-M */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x001006B1);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x001006B1);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x00080000); /* for NTSC mode, disable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00000810);     
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0xDE1C0402);     
    
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x5E2D0080);/* 0x612D0074, Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074 */
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x1E1E001A); /* 0x1C1E7014Default value for NTSC, v656blank=0x18,vactive=0x1e7, vblank=0x014 */
        writeDword(DEVICE, ALCS_VDECA_VERT_MISC_CTRL + offset, 0x10C000);     /* sync inc/reduce = 0x3F */
    
        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL_FAST + offset, 0x801F1000);
        
        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x00180127);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x10902020);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x1E199A08);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x20082020);

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL  + offset, 0x002022D0);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481060DC);      /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000002);  /* 0x00000012 */

        /*Non-Effio bp_percent [31:20]=0x668, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2*/ 
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x66800e8a);

        writeDword(DEVICE, ALCS_VDECA_FLAT_FIELD_DET + offset, 0x802091C0);      /* for 624MHz */      

        // Confirm if we are still need this setting in Alcaeus+
        value  = readDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset);
        value &= 0xEFFFFFFF;                                             /* clear bit 28, disable 12db */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset, value);

        value  = readDword(DEVICE, ALCS_VDECA_MAN_AGC_CTRL + offset);
        value |= 0x80000000;                                                
        writeDword(DEVICE, ALCS_VDECA_MAN_AGC_CTRL + offset, value);

        if ( _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz || _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_48MHz ) {
            value  = readDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset);
            value |= 0x00000100;
            writeDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset, value);      /* Colibri 624MHz mode enable */

            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x2221);       
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1E10C1A9);    
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x43E2B314);  

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x79300594);     /* sync inc/reduce = 0x3F */
        }
        else {
            /*Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x21f [9:0] */ 
            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x0000221f);          /* Different for 624MHz=0x221, 720MHz=0x21f */
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1E2BE2FE);           /* Go back to 108MHz, for 624MHz = 0x1e2be2fe */
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x43E00000);     /* Default value for NTSC, sc_step=0x43E00000 */

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x903005D4);     /* sync inc/reduce = 0x3F */
        }

    }

    return 0;
}

static int
ALCS_initPAL(int DEVICE, int ic_typ, int channel)
{
    int i = channel;
    unsigned long value;
    unsigned long offset;

    if(i >= MAX_DECODERS) {
        return -1;
    }

    offset = 0x200*i;

    if(!ic_typ) {
        // Reset decoder
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset,  0x00008000); //Resets video decoder core, bit15=1
        scripting.sleep(5);
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset,  0x00000000);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset,  0x0005E004 &0xFFFEFFFF); // Default value, vid_fmt_sel=4'b0100  PAL-BDGHI, NOT effio mode = bit[8]
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset,  0xABF4000A);    /* Close 12dB Amp. */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset,  0x401040DC); // Default value, and disable clamp gating(1)
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset,  0x00108000); // Set luma range = 1, default brightness/contrast
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset,  0x3D008080); // Set c_ext_rng_en = 1(bit29), set c_core_sel = 1 to improve flat field SNR.

        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset,  0x00100621 | 0x30080);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset,  0x00100621 | 0x30080); // Default value, vip1.1 mode, id_ins_extend_en = 0x1

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x01080000);  /* for PAL mode, enable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset,  0xC0F00AB8);  // field_phase_limit = 0xB, it make vcr_detect bit more reliable
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset,  0x00002040); // Default Value
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset,  0x001C1E0F);

        writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset,  0x5411E2D0);    // For PAL
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset,  0x672D0085/*0x612D007D*/);  // From Ning Yang (0x632D007D)bgdel=0x5E for better Chroma/Luma Gain,
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset,  0x28240024);   // For PAL, From Ning Yang

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset,  0x161F1000);// Default Value
    
        //COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA PAL COMB FILTER SETTING
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset,  0x0018010F);// 0x0001015F;  // Changed lpf_bw_sel[5:4] from 0x1 to 0x2
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset,  0x1E194020);// 0x1E192020;
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset,  0x080A0A08);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset,  0x20102030);

        //Macrovision Setting for PAL is default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL2 + offset,  0x00000000); //Default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL3 + offset,  0x00000000); //Default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL1 + offset,  0x00000000);  //Default value

        //Setting for AGC (+12dB) - get video at 10mVpp input
        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL + offset,  0x00200210);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset,  0x401060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset,  0x00000002);

        //Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x21f [9:0]
        writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset,  0x0000221f);
        //Non-Effio bp_percent [31:20]=0x668, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset,  0x6680028a);

        writeDword(DEVICE, ALCS_PLL0_INT_FRAC+(0x8*i),  0x1E2BE2FE);//Go back to 108MHz
    }
    else {
        /* reset video decoder */
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x8000);
        scripting.sleep(5); 
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E004);    /* Default value, vid_fmt_sel=4'b0100  PAL-BDGHI */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x001006B1);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x001006B1);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x01080000); /* for PAL mode, enable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB, it make vcr_detect bit more reliable */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00000810);
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0xDE1C0402);

        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x26240022);  /* 0x26240022 */
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x662D0087); /* bgdel=0x5D,hactive=0x2d0, hblank=0x084 */
        writeDword(DEVICE, ALCS_VDECA_VERT_MISC_CTRL + offset, 0x10C000);     /* sync inc/reduce = 0x3F */

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL_FAST + offset, 0x801F1000);

        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x0001D326); /* 1D727 for RhodeSchwardz disc; 1C727 for AVIA disc */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x10900C90); /* default setting; 1e590890 for AVIA disc */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x30061014); /* default setting */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x1E190000); /* default of 20542030 has more ringing. This setting has a little more dots */
        writeDword(DEVICE, ALCS_VDECA_COMB_BLEND_CTRL + offset, 0x00202060); /* default of 20542030 has more ringing. This setting has a little more dots */

        /* Macrovision Setting  */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL1 + offset, 0x00000000);  /* Default value for PAL Macrovision */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL2 + offset, 0x00000000);  /* Default value for PAL Macrovision */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL3 + offset, 0x00000000);  /* Default value for PAL Macrovision */

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL + offset, 0x002022D0);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000002); /* 0x00000012 */

        //Non-Effio bp_percent [31:20]=0x668, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2 
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x66800e8a);
        writeDword(DEVICE, ALCS_VDECA_FLAT_FIELD_DET + offset, 0x4021C1C0);        

        // Confirm if we are still need this setting in Alcaeus+
        value  = readDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset);
        value &= 0xEFFFFFFF;                                             /* clear bit 28, disable 12db */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset, value);

        if ( _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz || _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_48MHz ) {
            value  = readDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset);
            value |= 0x00000100;
            writeDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset, value);      /* Colibri 624MHz mode enable */

            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x2221);       
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1E10C1A9);    
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x54153AC9);  

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x80300594);     /* use bp_avg_delay=0x80 and bp_ave_en=1. If complain about black level fluctuate 3code, set bp_ave_en=0 with be_avg_delay=0x50 */
        }
        else {
            //Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x21f [9:0] 
            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x0000221f);          /* Different for 624MHz=0x00020221, 720MHz=0x0002021F */
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1E2BE2FE);           /* Go back to 108MHz, for 624MHz = 0x1e2be2fe */
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x5411E2D0);     /* For PAL, 624MHz=0x541280B0, 720MHz=0x5411E2D0*/

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x803005D4);     /* use bp_avg_delay=0x80 and bp_ave_en=1. If complain about black level fluctuate 3code, set bp_ave_en=0 with be_avg_delay=0x50 */
        }
    }

    return 0;
}

static int
ALCS_initEffioNTSC(int DEVICE, int ic_typ, int channel)
{
    int i = channel;
    unsigned long value;
    unsigned long offset;

    if(i >= MAX_DECODERS) {
        return -1;
    }

    offset = 0x200*i;

    if(!ic_typ) {
        // Reset decoder
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00008000); //Resets video decoder core
        scripting.sleep(5);
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00000000);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E101 &0xFFFEFFFF);    /* Default value, vid_fmt_sel=4b0001  NTSC-M, effio mode = bit[8] */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset,  0xABF4000A);    /* Close 12dB Amp. */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x00100621 | 0x30080);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x00100621 | 0x30080);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x00080000);  /* for NTSC mode, disable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00002040);
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0x001C1E0F);

        writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x32e8b9c2);  /* Effio **Different for 624MHz=0x32f23723, 720MHz=0x32e8b9c2 */
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x813c00a1);/* 0x813c009b Effio bgdel=0x81 bit[31:24] ,hactive=0x3c0 bit[21:12], hblank=0x09b bit[9:0] */
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x181e7014); /* Effio v656blank_cnt =0x18 bit[31:24], vactive_cnt =0x1e7 bit[21:12], vblank_cnt =0x14  bit[9:0] */

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
    
        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x0018012F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x1E192020);//0x1E194020
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x080A9A08);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x20102030);

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL + offset, 0x00200210);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000012);

        /*Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x230 [9:0]*/
        writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x00002230);        /* Different for 624MHz=0x0002022B, 720MHz=0x00020230 */

        /*Non-Effio bp_percent [31:20]=8e8, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2*/
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x8e80028a);

        writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1D41d45b);     /* Set to 144MHz */
    }
    else {
        /* reset video decoder */
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x8000);
        scripting.sleep(5); 
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E101);    /* Default value, vid_fmt_sel=4b0001  NTSC-M, effio mode = bit[8] */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x001006B1);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x001006B1);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x00080000); /* for NTSC mode, disable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00000810);
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0xDE1C0402);    /* Sync inc/reduce = 0x3F */

        writeDword(DEVICE, ALCS_VDECA_VERT_MISC_CTRL + offset, 0x0010C000);    /* Sync inc/reduce = 0x3F */
    
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x813C00A5);/* Effio bgdel=0x81 bit[31:24] ,hactive=0x3c0 bit[21:12], hblank=0x09b bit[9:0] */
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x1E1E001A); /* Effio v656blank_cnt =0x18 bit[31:24], vactive_cnt =0x1e7 bit[21:12], vblank_cnt =0x14  bit[9:0] */

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL_FAST +  offset, 0x801F2800);
    
        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x00180127);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x10902020);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x1E199A08);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x20082020);

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL  + offset, 0x002022D0);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000002);  /* 0x00000012 */

        /*Non-Effio bp_percent [31:20]=8e8, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2*/ 
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x8e800e8a);
        writeDword(DEVICE, ALCS_VDECA_FLAT_FIELD_DET + offset, 0x8020B1C0);          

        // Confirm if we are still need this setting in Alcaeus+
        value  = readDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset);
        value &= 0xEFFFFFFF;                                             /* clear bit 28, disable 12db */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset, value);

        if ( _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz || _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_48MHz ) {
            value  = readDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset);
            value |= 0x00000100;                                                
            writeDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset, value);      /* Colibri 624MHz mode enable */

            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x222B);       
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1D89D89E);    
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x32F08ECB);  

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x79300594);      /* Sync inc/reduce = 0x3F */
        }
        else {
            /*Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x230 [9:0]*/ 
            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x00002230);           /* Different for 624MHz=0x0002022B, 720MHz=0x00020230 */
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0xe141d45b);            /* Set to 144MHz */
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x32e8b9c2);      /* Effio **Different for 624MHz=0x32f23723, 720MHz=0x32e8b9c2 */

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x903005D4);      /* Sync inc/reduce = 0x3F */
        }
    }

    return 0;
}

static int
ALCS_initEffioPAL(int DEVICE, int ic_typ, int channel)
{
    int i = channel;
    unsigned long value;
    unsigned long offset;

    if(i >= MAX_DECODERS) {
        return -1;
    }

    offset = 0x200*i;

    if(!ic_typ) {
        // Reset decoder
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00008000); //Resets video decoder core
        scripting.sleep(5);
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x00000000);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E104 &0xFFFEFFFF); // Default value, vid_fmt_sel=4b0001  PAL-BDGHI, effio mode = bit[8]
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset,  0xABF4000A);    /* Close 12dB Amp. */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401040DC); // Default value, and disable clamp gating(1)
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000); // Set luma range = 1, default brightness/contrast
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080); // Set c_ext_rng_en = 1(bit29), set c_core_sel = 1 to improve flat field SNR.

        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x00100621 | 0x30080);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x00100621 | 0x30080); // Default value, vip1.1 mode, id_ins_extend_en = 0x1

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x01080000);  /* for PAL mode, enable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8);  // field_phase_limit = 0xB, it make vcr_detect bit more reliable
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00002040); // Default Value
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0x001C1E0F);

        writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x3f0d6a1c);  // For Effio PAL
        //Effio bgdel=0x84 bit[31:24] ,hactive=0x3c0 bit[21:12], hblank=0x0a6 bit[9:0]
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x843c00B2/*,0x843c00a6*/);
        //Effio v656blank_cnt =0x26 bit[31:24], vactive_cnt =0x240 bit[21:12], vblank_cnt =0x022  bit[9:0]
        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x28240024/*0x26240022*/);

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);// Default Value

        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x0001016F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x1E191020);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x080A0A08);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x20102030);

        //Macrovision Setting for PAL is default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL1 + offset, 0x00000000);  //Default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL2 + offset, 0x00000000); //Default value
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL3 + offset, 0x00000000); //Default value

        //Setting for AGC (+12dB) - get video at 10mVpp input
        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL + offset, 0x00200210);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x401060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000012);


        //Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x230 [9:0]
        writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x00002230);
        //Effio bp_percent [31:20]=0x8e8, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x8e80028a);

        writeDword(DEVICE, ALCS_PLL0_INT_FRAC +(0x8*i), 0x1d41d45b);         /* Set to 144MHz */
    }
    else {
        /* reset video decoder */
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0x8000);
        scripting.sleep(5); 
        writeDword(DEVICE, ALCS_VDECA_SOFT_RST_CTRL + offset, 0);

        writeDword(DEVICE, ALCS_VDECA_MODE_CTRL + offset, 0x0005E104);    /* Default value, vid_fmt_sel=4'b0100  PAL-BDGHI, effio mode = bit[8] */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481040DC);    /* Default value, and disable clamp gating(1) */
        writeDword(DEVICE, ALCS_VDECA_LUMA_CTRL + offset, 0x00108000);    /* Set luma range = 1 */
        writeDword(DEVICE, ALCS_VDECA_CHROMA_CTRL + offset, 0x3D008080);  /* Set c_ext_rng_en = 1(bit29) */

        /* Output setting */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + offset, 0x001006B1);    /* default value, out_mode=2b01 ITU-R BT.656 */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL_NS + offset, 0x001006B1);  /* default value, out_mode=2b01 ITU-R BT.656 */

        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + offset, 0x01080000); /* for PAL mode, enable line_average: */

        writeDword(DEVICE, ALCS_VDECA_VCR_DET_CTRL + offset, 0xC0F00AB8); /* field_phase_limit = 0xB, it make vcr_detect bit more reliable */
        writeDword(DEVICE, ALCS_VDECA_HTL_CTRL + offset, 0x00000810);
        writeDword(DEVICE, ALCS_VDECA_CRUSH_CTRL + offset, 0xDE1C0402);

        writeDword(DEVICE, ALCS_VDECA_VERT_TIM_CTRL + offset, 0x28240024);  /* Effio v656blank_cnt =0x26 bit[31:24], vactive_cnt =0x240 bit[21:12], vblank_cnt =0x022  bit[9:0] */
        writeDword(DEVICE, ALCS_VDECA_HORIZ_TIM_CTRL + offset, 0x843C00B4); /* Effio bgdel=0x84 bit[31:24] ,hactive=0x3c0 bit[21:12], hblank=0x0a6 bit[9:0] */

        /* Added to Cx2684x */
        writeDword(DEVICE, ALCS_VDECA_VERT_MISC_CTRL + offset, 0x10C000);  

        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL + offset, 0x161F1000);
        writeDword(DEVICE, ALCS_VDECA_PLL_CTRL_FAST + offset, 0x801F2800);

        /* COMB FILTER SETTING Below */
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 + offset, 0x0001D326);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 + offset, 0x10900C90);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL3 + offset, 0x30061014);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL4 + offset, 0x1E190000);
        writeDword(DEVICE, ALCS_VDECA_COMB_BLEND_CTRL + offset, 0x00202060);

        /* Macrovision Setting  */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL1 + offset, 0x00000000);  /* Default value for PAL Macrovision */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL2 + offset, 0x00000000);  /* Default value for PAL Macrovision */
        writeDword(DEVICE, ALCS_VDECA_MV_DT_CTRL3 + offset, 0x00000000);  /* Default value for PAL Macrovision */

        writeDword(DEVICE, ALCS_VDECA_C2RB_DC_CTRL + offset, 0x002022D0);
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL2 + offset, 0x481060DC);
        writeDword(DEVICE, ALCS_VDECA_COL_GAIN_CTRL + offset, 0x00000002); /* 0x00000012 */

        //Non-Effio bp_percent [31:20]=0x8e8, bp_loop_gain [9:8]=2, sync_loop_gain [7:6]=2, agc_loop_gain [3:2]=2, dcc_loop_gain [1:0]=2 
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL3 + offset, 0x8e800e8a);
        writeDword(DEVICE, ALCS_VDECA_FLAT_FIELD_DET + offset, 0x8021C1C0);    

        // Confirm if we are still need this setting in Alcaeus+
        value  = readDword(DEVICE, ALCS_VDECA_DFE_CTRL1 + offset);
        value &= 0xEFFFFFFF;                                                 /* clear bit 28, disable 12db */
        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1+ offset, value);

        if ( _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz || _xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_48MHz ) {
            value  = readDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset);
            value |= 0x00000100;                                                
            writeDword(DEVICE, ALCS_AFE_DIAG_CTRL + offset, value);      /* Colibri 624MHz mode enable */

            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x222B);       
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0x1D89D89E);    
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x3F1803A0);  
    
            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x80300594);
        }
        else {
            /* Non-Effio eq_ena [16]=0, fifo_size [13:10]=8, src_decim=0x230 [9:0] */ 
            writeDword(DEVICE, ALCS_VDECA_SRC_CFG + offset, 0x00002230);        /* Different for 624MHz=0x0002022B, 720MHz=0x00020230 */
            writeDword(DEVICE, ALCS_PLL0_INT_FRAC + (0x8*i), 0xe141d45b);         /* Set to 144MHz */
            writeDword(DEVICE, ALCS_VDECA_SC_STEP_SIZE + offset, 0x3f0d6a1c);   /* For Effio PAL **Different for 624MHz=0x3f178d60, 720MHz=0x3f0d6a1c */

            writeDword(DEVICE, ALCS_VDECA_BP_MISC_CTRL + offset, 0x803005D4);
        }
    }

    return 0;
}

int
ALCS_setMacrovisionEnable(int DEVICE,
                          const unsigned char enable)
{
    unsigned long value = 0;
    int i;
    int _num_decoders = MAX_DECODERS;

    for (i=0; i < _num_decoders; i++) {
       value  = readDword(DEVICE, ALCS_VDECA_DFE_CTRL1+(0x200*i));
        if (enable) {
            value |= 0x40000000; /* set bit 30, vga_crush_en */
        }
        else {
            value &= 0xBFFFFFFF;
        }

        writeDword(DEVICE, ALCS_VDECA_DFE_CTRL1+(0x200*i), value);
    }

    return 0;
}

int
ALCS_setVideoStandard(int DEVICE,
                      int ic_typ,
                      const char * std,
                      int res[])
{
    int i;

    //ALCS_setMacrovisionEnable(DEVICE, 0); //Cannot Work Putting Here.

    for (i = 0; i < MAX_DECODERS; i++) {
        if(VIDEO_IS_NTSC(std)) {
            if(res[i] == RES_960H) {
                ALCS_initEffioNTSC(DEVICE, ic_typ, i);
            }
            else {
                ALCS_initNTSC(DEVICE, ic_typ, i);
            }
        }
        else {
            if(res[i] == RES_960H) {
                ALCS_initEffioPAL(DEVICE, ic_typ, i);
            }
            else {
                ALCS_initPAL(DEVICE, ic_typ, i);
            }
        }
    }

    return 0;
}

static int
ALCS_initDecoder(int DEVICE, int ic_typ, const char *std, int res[])
{
    unsigned long value = 0;
    int ret_val = 0;

    unsigned int id = 0;   // hard-coded
    unsigned long pll8_int_coef =
         (_xtal_freq_clk == COLIBRI_FCLK_720MHZ_CRYSTAL_24MHZ
        ||_xtal_freq_clk == COLIBRI_FCLK_624MHZ_CRYSTAL_24MHz) ? 0x12 : 0x9;

    value  = readDword(DEVICE, ALCS_PLL8_INT_FRAC);
    value &= 0x81FFFFFF;
    writeDword(DEVICE, ALCS_PLL8_INT_FRAC , value | (pll8_int_coef << 25));

    ALCS_setVideoStandard(DEVICE, ic_typ, std, res);

    value  = readDword(DEVICE, ALCS_REV_ID);
    value &= ~0x0f000000;
    value |= id << 24;
    writeDword(DEVICE, ALCS_REV_ID , value);

    return ret_val;
}

static int
ALCS_InitVPE(int DEVICE,
             const unsigned char strip_invalid_pixels)
{
    int i;
    unsigned long value = 0;

    for (i = 0; i < 8; i++) {
        value = 0x02000000 + 0x200 * i;

        writeDword(DEVICE, ALCS_VP_VPE_A0 + (0x20 * i), value);         /* the source of video packing engine: Video Decoder A Scaled */
        value &= 0xFFFFE0FD;
        value |= ((i * 2) << 8);                                                  /* set vpe src sel */
        value |= ((strip_invalid_pixels & 1) << 1);                               /* set strip invalid pixel */
        writeDword(DEVICE, ALCS_VP_VPE_A0 + (0x20 * i), value | 0x01);  /* Enable Video Packing Engine */
    }

    /* VPE8-15's source are Vin1-8, Don't strip invalid pixel */
    for (i=0; i < 8; i++) {
        value = 0x02000000 + 0x200 * i;
        writeDword(DEVICE, ALCS_VP_VPE_A8 + (0x20 * i), value);         /* the source of video packing engine: Video Decoder A Scaled */
        value &= 0xFFFFE0FD;
        value |= ((i * 2) << 8);                                                  /* set vpe src sel */
        value |= ((strip_invalid_pixels & 1) << 1);                               /* set strip invalid pixel */
        writeDword(DEVICE, ALCS_VP_VPE_A8 + (0x20 * i), value | 0x01);  /* Enable Video Packing Engine */
    }

    return 0;
}

int ALCS_setVideoPackExtend(int DEVICE,
                                   const unsigned long vpe_sel,
                                   const unsigned long video_extend_mode)
{
    int ii;
    unsigned long value;
    
    unsigned long hact_len;
    unsigned long invld_pix_strp_en;
    unsigned long hpre_en;
    unsigned long hpst_en;
    unsigned long hpr_padding_len;
    unsigned long hpst_padding_len;
    unsigned long ln_vld_thrshld;
    
    unsigned long ParamTbls[][8] = {
        {             VIDEO_EXTEND_D1, 720, 0, 0, 0,   0,   0, 256},
        {           VIDEO_EXTEND_960H, 960, 0, 0, 0,   0,   0, 256},
        {        VIDEO_EXTEND_960H_D1, 720, 0, 0, 0,   0,   0,  64},
        {    VIDEO_EXTEND_D1_HBI_960H, 720, 0, 0, 0,   0,   0, 448},
        {    VIDEO_EXTEND_D1_ACT_960H, 720, 0, 1, 1, 120, 120, 288},
        {            VIDEO_EXTEND_CIF, 352, 1, 0, 0,   0,   0, 256},
        {       VIDEO_EXTEND_HLF_960H, 480, 1, 0, 0,   0,   0, 256},
        {   VIDEO_EXTEND_HLF_960H_HBI, 480, 1, 0, 0,   0,   0, 496},
        {VIDEO_EXTEND_HLF_960H_HLF_D1, 360, 1, 0, 0,   0,   0, 256},
        {VIDEO_EXTEND_HLF_D1_HBI_960H, 360, 1, 0, 0,   0,   0, 256},
    };

    ii = 0;
    hact_len           = ParamTbls[ii][1];
    invld_pix_strp_en  = ParamTbls[ii][2];
    hpre_en            = ParamTbls[ii][3];
    hpst_en            = ParamTbls[ii][4];
    hpr_padding_len    = ParamTbls[ii][5];
    hpst_padding_len   = ParamTbls[ii][6];
    ln_vld_thrshld     = ParamTbls[ii][7];
    for(ii = 0; ii < ARRAY_SIZE(ParamTbls); ii ++) {
        if(video_extend_mode == ParamTbls[ii][0]) {
            hact_len           = ParamTbls[ii][1];
            invld_pix_strp_en  = ParamTbls[ii][2];
            hpre_en            = ParamTbls[ii][3];
            hpst_en            = ParamTbls[ii][4];
            hpr_padding_len    = ParamTbls[ii][5];
            hpst_padding_len   = ParamTbls[ii][6];
            ln_vld_thrshld     = ParamTbls[ii][7];
            break;
        }
    }

    value   = readDword(DEVICE, ALCS_VP_VPE_E0 + (0x20 * vpe_sel));
    value  &= 0xFC000000;
    value  |= hpr_padding_len;
    value  |= (hpst_padding_len << 8);
    value  |= (hact_len << 16);
    writeDword(DEVICE, ALCS_VP_VPE_E0 + (0x20 * vpe_sel), value);

    value   = readDword(DEVICE, ALCS_VP_VPE_A0 + (0x20 * vpe_sel));
    value  &= 0xFFFF3F0D;
    value  |= (invld_pix_strp_en << 1);
    value  |= (hpre_en << 14);
    value  |= (hpst_en << 15);
    writeDword(DEVICE, ALCS_VP_VPE_A0 + (0x20 * vpe_sel), value);

    value   = readDword(DEVICE, ALCS_VP_VPE_F0 + (0x20 * vpe_sel));
    value  &= 0xFFFFFE00;
    value  |= ln_vld_thrshld;
    value |= (0x1 << 9); 
    writeDword(DEVICE, ALCS_VP_VPE_F0 + (0x20 * vpe_sel), value);

    return 0;
}

static inline int ALCS_setVideoPackEn(int DEVICE,
                            const unsigned long vpe_sel,
                            const unsigned long src_sel,
                            const unsigned long vpe_id,
                            const unsigned long id_len,
                            const unsigned long strip_en)
{
    unsigned long value = 0;

    value  = readDword(DEVICE, ALCS_VP_VPE_A0 + (0x20 * vpe_sel));
    value &= 0xfffff000;
    value |= (src_sel << 8);
    value |= (vpe_id << 4);
    value |= (id_len << 2);
    value |= (strip_en << 1);
    value |= 0x1; /* enable vpe function */

    writeDword(DEVICE, ALCS_VP_VPE_A0 + 0x20 * vpe_sel, value);

    return 0;
}

int ALCS_setVideoPackFormat(int DEVICE,
                                   const unsigned long vpe_sel,
                                   const char * std)
{
    unsigned long value;

    value   = readDword(DEVICE,  ALCS_VP_VPE_B0 + (0x20 * vpe_sel));
    value  &= 0x07FF07FF;
    value  |= (0x14 << 11);
    value  |= (0x10 << 27);
    writeDword(DEVICE, ALCS_VP_VPE_B0 + (0x20 * vpe_sel), value);

    value   = readDword(DEVICE,  ALCS_VP_VPE_C0 + (0x20 * vpe_sel));
    value  &= 0x07FF07FF;
    value  |= (0x14 << 11);
    value  |= (0x1C << 27);
    writeDword(DEVICE, ALCS_VP_VPE_C0 + (0x20 * vpe_sel), value);

    value   = readDword(DEVICE,  ALCS_VP_VPE_D0 + (0x20 * vpe_sel));
    value  &= 0x07FF07FF;
    value  |= (0x18 << 11);
    value  |= (0x1C << 27);
    writeDword(DEVICE, ALCS_VP_VPE_D0 + (0x20 * vpe_sel), value);

    if ( VIDEO_IS_NTSC(std) ) {
        value   = readDword(DEVICE,  ALCS_VP_VPE_B0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 19;
        value  |= (259 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_B0 + (0x20 * vpe_sel), value);

        value   = readDword(DEVICE,  ALCS_VP_VPE_C0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 262;
        value  |= (282 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_C0 + (0x20 * vpe_sel), value);

        value   = readDword(DEVICE,  ALCS_VP_VPE_D0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 522;
        value  |= (525 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_D0 + (0x20 * vpe_sel), value);
    }
    else
    {
        value   = readDword(DEVICE,  ALCS_VP_VPE_B0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 22;
        value  |= (310 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_B0 + (0x20 * vpe_sel), value);

        value   = readDword(DEVICE,  ALCS_VP_VPE_C0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 312;
        value  |= (335 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_C0 + (0x20 * vpe_sel), value);

        value   = readDword(DEVICE,  ALCS_VP_VPE_D0 + (0x20 * vpe_sel));
        value  &= 0xFC00FC00;
        value  |= 623;
        value  |= (625 << 16);
        writeDword(DEVICE, ALCS_VP_VPE_D0 + (0x20 * vpe_sel), value);
    }

    return 0;
}

int
ALCS_setupByteInterleaveOutput(int DEVICE,
                              int ic_typ,
                              const unsigned char num_channels, /* Max 4 channels per port, 1, 2, 4, 8 */
                              const unsigned long size,         /* 960, 720, 360, 352, 320 */
                              const unsigned long output_rate,  /* 13.5 27MHz, 54MHz, 108MHz, 144MHZ */
                              const unsigned char dvp_port,     /* port A, B, C, D */
                              unsigned long *src_sel,           /* Video source/s selected 0-7 */
                              const unsigned char strip_invalid_pixels,
                              const char * std,
                              int  res[])
{
    unsigned long value;
    int i, j;
    unsigned long output_mode = OMUXMODE_PASSTHROUGH;

    unsigned long hscale = PIXELS_WIDTH720;
    unsigned long demux_mode = 0;
    unsigned long demux_mode_edge = DVP_DEMUX_MODE_SINGLE_EDGE;

    unsigned long omux_clk_sel = 0;
    unsigned long omux_clk_pol = !ic_typ
                              ?DVP_OMUX_CLK_POL_NONINVERTED
                              :DVP_OMUX_CLK_POL_INVERTED;

    switch(num_channels) {
        case 1: demux_mode = DVP_DEMUX_MODE_1STREAM; break;
        case 2: demux_mode = DVP_DEMUX_MODE_2STREAMS; break;
        case 4:
        default: demux_mode = DVP_DEMUX_MODE_4STREAMS; break;
    }

    switch(output_rate) {
        case OUTPUT_RATE_13PT5_MHZ: /* 13.5MHz */
            if (num_channels == 2) {
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            output_mode = OMUXMODE_1_STREAM_REMUX; /* 1 stream remixed */
            break;

        case OUTPUT_RATE_27_MHZ: /* 27MHz */
            if (num_channels == 2) {
                output_mode = OMUXMODE_2_STREAM_REMUX; /* 2 stream remuxed, byte interleaved */
            }
            if (num_channels == 2 || num_channels == 4) {
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            break;

        case OUTPUT_RATE_54_MHZ: /* 54MHz*/
            if (num_channels == 4) {
                output_mode = OMUXMODE_4_STREAM_REMUX; /* 4 stream remuxed, byte interleaved */
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            else {
                output_mode = OMUXMODE_2_STREAM_REMUX; /* 2 stream remuxed, byte interleaved */
            }
            break;

        case OUTPUT_RATE_108_MHZ: /* 108MHz*/
            if (num_channels == 8)
                output_mode = OMUXMODE_8_STREAM_REMUX; /* 8 stream remuxed, byte interleaved */
            else {
                output_mode = OMUXMODE_4_STREAM_REMUX; /* 4 stream remuxed, byte interleaved */
            }
            break;
        case OUTPUT_RATE_144_MHZ: /* 144MHz*/
            if (num_channels == 4) {
                output_mode = OMUXMODE_4_STREAM_REMUX; /* 4 stream remuxed, byte interleaved */
            }
            omux_clk_sel = 1;
            break;
        case 0: /* disabled */
        default:
            output_mode = !ic_typ ?OMUXMODE_DISABLED :OMUXMODE_1_STREAM_REMUX;
            break;
    }

    if ( demux_mode_edge == DVP_DEMUX_MODE_DUAL_EDGE ) {
        omux_clk_pol = !ic_typ ?DVP_OMUX_CLK_POL_INVERTED :DVP_OMUX_CLK_POL_NONINVERTED;
    }

    if ( output_mode == OMUXMODE_PASSTHROUGH ) {
        omux_clk_pol = DVP_OMUX_CLK_POL_INVERTED; /*DVP_OMUX_CLK_POL_NONINVERTED;*/
    }

    /* Set Device ID to 0x1 */
    value  = readDword(DEVICE, ALCS_REV_ID);
    value &= 0xF0FFFFFF;
    writeDword(DEVICE, ALCS_REV_ID, value | 0x01000000);

    switch (size) {
        case 320: hscale = !ic_typ ?PIXELS_WIDTH320 :0x13E34B; break;
        case 352: hscale = 0x10A273; break;
        case 360: hscale = PIXELS_WIDTH360; break;
        case 704: hscale = PIXELS_WIDTH704; break;
        case 960: hscale = PIXELS_WIDTH960; if(0 != ic_typ) omux_clk_sel = 1;break;
        default:  hscale = PIXELS_WIDTH720; break;
    }

    for (i=0; i < 8; i++) {
        /* Scale to pixels line length */
        value  = readDword(DEVICE, ALCS_VDECA_HSCALE_CTRL + (0x200 * i));
        value &= 0xFF000000;
        writeDword(DEVICE, ALCS_VDECA_HSCALE_CTRL + (0x200 * i), value | hscale);

        /* Camera ID and Stream ID insertion */
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + (0x200 * i),0x00130521 | 0x80);

        value  = readDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + (0x200 * i));
        value &= 0x01000000;
        writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + (0x200 * i), value | 0x00080000);
    }

    /* set selected source */
    if ((num_channels < 8) && (output_mode != OMUXMODE_PASSTHROUGH)) {
        for (j=0; j < num_channels; j++) {
            ALCS_setVideoPackExtend(DEVICE, (dvp_port * 4) + j,
                (res[j] == RES_960H)
                    ? VIDEO_EXTEND_960H
                    : VIDEO_EXTEND_D1_HBI_960H);
            ALCS_setVideoPackEn(DEVICE, (dvp_port * 4) + j, (src_sel[j+dvp_port * 4] * 2),  j % 4, 0x3, strip_invalid_pixels);
            ALCS_setVideoPackFormat(DEVICE, (dvp_port * 4) + j, std);
        }
    }

    /* pad_pwr_dn = 0 to enable VP input pads */
    value  = readDword(DEVICE, ALCS_HOST_REG1);
    value &= 0xFFFFFFF7;
    writeDword(DEVICE, ALCS_HOST_REG1, value);

    value  = readDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port));
    value &= 0xFFFFFFFE;
    writeDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port), value);   /* Disable dvp_in_en */

    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port));
    value &= 0xFFF3FFFE;
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port), value);   /* Disable omux_dat_oe and omux_clk_oe and omux_remux_en */

    /* Set OMUX registers */
    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port));
    value &= !ic_typ ?0xFF9C1F09 :0xFF8C1F09;
    if ( output_mode == OMUXMODE_PASSTHROUGH ) {
        value &= 0xFFFFE0FF;
        value |= !ic_typ ?(src_sel[0] << 8) :((src_sel[0] * 2) << 8);
    }
    if ( 0 != ic_typ && demux_mode_edge == DVP_DEMUX_MODE_DUAL_EDGE ) {
        value |= (1 << 21);                                                    /* set omux clk_dly_sel_hi */
        value |= (7 << 13);                                                    /* set omux clk_dly_sel_lo */
        value |= (1 << 7);                                                     /* set omux clk_dly_en */
    }
    value |= (omux_clk_sel << 20);                                           /* set omux clock polarity */
    value |= (omux_clk_pol << 17);                                             /* set omux clock polarity */
    value |= (demux_mode_edge << 16);                                          /* set omux clock mode */
    value |= (output_mode << 4);                                               /* set omux out mode */
    value |= (DVP_OMUX_STREAM_BW_27MBYTES_1STREAM << 2);                       /* hard-coded for now */
    value |= (DVP_OMUX_PACK_ENG_SEL_LOW << 1);                                 /* hard-coded for now */
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port), value);

    /* Enable omux_remux_en */
    /* Enable omux_dat_oe and omux_clk_oe */
    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port));
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port), value | 0xC0001 );

    if(!ic_typ) {
        /* soft reset for VP */
        value  = readDword(DEVICE, ALCS_CHIP_RST);
        writeDword(DEVICE, ALCS_CHIP_RST, value | 0x4 );

        writeDword(DEVICE, ALCS_VP_VPE_MISC, 0x74d42310);
    }
    else {
        writeDword(DEVICE, ALCS_VP_VPE_MISC, 0x74d42310); 

        /* soft reset for VP */
        value = readDword(DEVICE, ALCS_CHIP_RST);
        writeDword(DEVICE, ALCS_CHIP_RST, value | 0x4 );
    }

    return 0;
}

/*******************************************************************************************************
 *
 * ALCS_setupByteInterleaveInput
 * setup Alcaeus as input port
 *
 *******************************************************************************************************/
int
ALCS_setupByteInterleaveInput(int DEVICE,
                              int ic_typ,
                              const unsigned char num_channels, /* Max 4 channels per port, 1, 2, 4 */
                              const unsigned long size,         /* 720, 352, 320 */
                              const unsigned long output_rate,  /* 13.5MHz, 27MHz, 54MHz, 108MHz */
                              const unsigned char dvp_port,     /* port A, B, C, D */
                              unsigned long *src_sel)
{
    int i;
    unsigned long value = 0;
    unsigned long hscale = PIXELS_WIDTH720;
    unsigned char strip_invalid_pixels = 0;

    unsigned long demux_mode = DVP_DEMUX_MODE_1STREAM;
    unsigned long demux_mode_edge = DVP_DEMUX_MODE_SINGLE_EDGE;

    unsigned long stream_id = 0;

    switch (num_channels) {
        case 1: demux_mode = DVP_DEMUX_MODE_1STREAM; break;
        case 2: demux_mode = DVP_DEMUX_MODE_2STREAMS; break;
        case 4: demux_mode = DVP_DEMUX_MODE_4STREAMS; break;
    }

    switch (output_rate) {
        case OUTPUT_RATE_13PT5_MHZ:  /* 13.5 MHz */
            if (num_channels == 2) {
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            break;
        case OUTPUT_RATE_27_MHZ:    /* 27 MHz */
            if (num_channels == 2 || num_channels == 4) {
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            break;
        case OUTPUT_RATE_54_MHZ:    /* 54 MHz */
            if (num_channels == 4) {
                demux_mode_edge = DVP_DEMUX_MODE_DUAL_EDGE;
            }
            break;
        case OUTPUT_RATE_108_MHZ:   /* 108 MHz */
        default:
            demux_mode_edge = DVP_DEMUX_MODE_SINGLE_EDGE;
            break;
    }

    if(!ic_typ) {
        switch (size) {
            case 320:
                strip_invalid_pixels = 1;
                hscale = PIXELS_WIDTH320;
                break;
            case 352:
                strip_invalid_pixels = 1;
                hscale = PIXELS_WIDTH352;
                break;
            case 360:
                hscale = PIXELS_WIDTH360;
                break;
            case 704:
                hscale = PIXELS_WIDTH704;
                break;
            default:
                hscale = PIXELS_WIDTH720;
                break;
        }

        for (i=0; i < 8; i++) {
            /* Scale to pixels line length */
            value  = readDword(DEVICE, ALCS_VDECA_HSCALE_CTRL + (0x200 * i));
            value &= 0xFF000000;
            writeDword(DEVICE, ALCS_VDECA_HSCALE_CTRL + (0x200 * i), value | hscale);

            /* Camera ID and Stream ID insertion */
            writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + (0x200 * i),0x00130521 | 0x80);

            value  = readDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + (0x200 * i));
            value &= 0x01000000;
            writeDword(DEVICE, ALCS_VDECA_VSCALE_CTRL + (0x200 * i), value | 0x00080000);
        }
    }
    else {
        switch (size) {
            case 320:
            case 352:
                strip_invalid_pixels = 1;
                break;
        }

        for (i=0; i < 8; i++) {
            /* Camera ID and Stream ID insertion */
            writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1 + (0x200 * (src_sel[0]+i)),0x00130521 | 0x80);
        }
    }

    for (i = 0; i < num_channels; i++) {
        value  = readDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port));
        value &= 0x0000ffff;
        stream_id |= (src_sel[i] << (16 + i*4));
        writeDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port), value );
    }

    ALCS_InitVPE(DEVICE, strip_invalid_pixels);

    value  = readDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port));
    value &= 0xFFFFFFFE;
    writeDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port), value);   /* Disable dvp_in_en */

    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port));
    value &= 0xFFF3FFFE;
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port), value);   /* Disable omux_dat_oe and omux_clk_oe and omux_remux_en */

    /* Set DEMUX registers */
    value  = readDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port));
    value &= 0x0000FFC9;                                                       /* Set stream ids to 0 */
    value |= (demux_mode << 4);                                                /* Set demux mode, hard-coded */
    value |= (demux_mode_edge << 2);                                           /* Set clock mode */
    value |= (DVP_DEMUX_CLK_POL_NONINVERTED << 1);                             /* Set clock polarity */
    writeDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port), value);

    /* Set OMUX registers */
    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port));
    value &= 0xFFF0FF88;
    value |= (DVP_OMUX_CLK_POL_NONINVERTED << 17);                             /* set omux clock polarity */
    value |= (DVP_OMUX_CLK_MODE_DUAL_EDGE << 16);                              /* set omux clock mode, hard-coded for now */
    value |= (OMUXMODE_4_STREAM_REMUX << 4);                                   /* set omux out mode, hard-coded for now */
    value |= (DVP_OMUX_STREAM_BW_27MBYTES_1STREAM << 2);                       /* hard-coded for now */
    value |= (DVP_OMUX_PACK_ENG_SEL_HIGH << 1);                                /* hard-coded for now */
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * dvp_port), value);

    value  = readDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port));
    writeDword(DEVICE, ALCS_VP_DEMUX_A + (0x04 * dvp_port), value | 1); /* Enable dvp_in_en */

    return 0;
}

/*******************************************************************************************************
 *
 * ALCS_setUvSuppress
 *
 *******************************************************************************************************/
int
ALCS_setUvSuppress(int DEVICE,
                   const unsigned char value)
{
    int result = !0;
    unsigned long temp = 0;
    int i;
    int _num_decoders = MAX_DECODERS;

    for (i=0; i < _num_decoders; i++) {
        temp  = readDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1+(0x200*i));
        temp &= 0xFFF9FFFF;
        temp |= ((value & 3) << 17);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1+(0x200*i), temp);
    }

    return result;
}

/*******************************************************************************************************
 *
 * ALCS_setup16CifBigFrame
 * Setup Alcaeus as 16 cif big frame interleave
 *
 *******************************************************************************************************/
int
ALCS_setup16CifBigFrame(int DEVICE, unsigned long standard)
{
    int i, result = !0;
    unsigned long value = 0;

    /* Set OMUX registers */
    /* Port B */
    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * 1));
    value |= (DVP_OMUX_CLK_POL_INVERTED << 17);                             /* set omux clock polarity */
    value |= (DVP_OMUX_STREAM_BW_13PT5MBYTES_1STREAM << 2);
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * 1), value);

    /* Port C */
    value  = readDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * 2));
    value |= (DVP_OMUX_CLK_POL_INVERTED << 17);                             /* set omux clock polarity */
    value |= (DVP_OMUX_STREAM_BW_13PT5MBYTES_1STREAM << 2);
    writeDword(DEVICE, ALCS_VP_OMUX_A + (0x04 * 2), value);

    value = readDword(DEVICE, ALCS_VP_OMUX_A);
    writeDword(DEVICE, ALCS_VP_OMUX_A,  value & 0xfffffffe); /* disable remux function on PA */

    value  = readDword(DEVICE, ALCS_VP_DEMUX_A);
    value &= 0x0000ffcc;            /* set to Single Stream input */
    value |= 0x32100000;
    writeDword(DEVICE, ALCS_VP_DEMUX_A, value);
    writeDword(DEVICE, ALCS_VP_DEMUX_A, value | 1); /* enable port */

    value  = readDword(DEVICE, ALCS_HOST_REG1);
    value &= 0xff00;
    value |= 0x44; /* power on pad */
    writeDword(DEVICE, ALCS_HOST_REG1, value);

    value  = readDword(DEVICE, ALCS_VP_OMUX_A);
    value &= 0xffff1ff7;
    value  |= (1 << 7);
    value |= (2 << 13);
    writeDword(DEVICE, ALCS_VP_OMUX_A, value);

    value  = readDword(DEVICE, ALCS_VP_OMUX_E);
    value &= 0xfff1ffff;
    value |= (1 << 17); /* clock polarity */
    value |= (1 << 18); /* enable clk */
    value |= (1 << 19); /* enable data */
    value |= 0x01;      /* enable */
    writeDword(DEVICE, ALCS_VP_OMUX_E, value);

    for (i=0; i < MAX_DECODERS; i++) {
        value  = readDword(DEVICE, ALCS_VDECA_OUT_CTRL1+(0x200*i));
        writeDword(DEVICE, ALCS_VDECA_OUT_CTRL1+(0x200*i), value & 0xFFFFFFEF);
    }

    return result;
}

/******************************************************************************
 *
 * ALCS_setDRE()
 *
 ******************************************************************************/
int
ALCS_setDRE(int DEVICE,
            unsigned long *coe_curve,
            const unsigned char decoder)
{
    int result = !0;
    int i = 0;
    unsigned long en_state;
    unsigned long addr_offset;
    unsigned long addr_start;
    unsigned long wr_data;

    /* This bitmap indicates which channel will go through DRE module. */
    unsigned long channel_bitmap[8] = {
        0x0,   /* Bit 0:  Video Decoder A */
        0x2,   /* Bit 2:  Video Decoder B */
        0x4,   /* Bit 4:  Video Decoder C */
        0x6,   /* Bit 6:  Video Decoder D */
        0x8,   /* Bit 8:  Video Decoder E */
        0xA,   /* Bit 10: Video Decoder F */
        0xC,   /* Bit 12: Video Decoder G */
        0xE    /* Bit 14: Video Decoder H */
    };

    /* disable DRE */
    en_state = readDword( DEVICE, ALCS_VP_DRE_0);
    writeDword(DEVICE, ALCS_VP_DRE_0, 0 );

    addr_offset = decoder << 8;
    addr_start  = addr_offset;
    wr_data     = 0;

    writeDword(DEVICE, ALCS_VP_DRE_1, ( (0x3 << 28) + (addr_start << 16) + 0x0)  );

    for (i = 1; i < 257; i++) {
        wr_data = wr_data + (coe_curve[i-1] << ( ( (i-1) % 4 ) * 8 ) );

        if ( (i % 4) == 0 ) {
            writeDword(DEVICE, ALCS_VP_DRE_2, wr_data );
            wr_data = 0;
        }
    }
    
    /* enable DRE */
    writeDword(DEVICE, ALCS_VP_DRE_0, (0x3 << channel_bitmap[decoder] ) | en_state );
    
    return result;
}

/******************************************************************************
 *
 * ALCS_disableDRE()
 *
 ******************************************************************************/
int
ALCS_disableDRE(int DEVICE,
                const unsigned char decoder)
{
    int result = !0;
    unsigned long value = 0;

    /* This bitmap indicates which channel will go through DRE module. */
    unsigned long channel_bitmap[8] = {
        0x0,   /* Bit 0:  Video Decoder A */
        0x2,   /* Bit 2:  Video Decoder B */
        0x4,   /* Bit 4:  Video Decoder C */
        0x6,   /* Bit 6:  Video Decoder D */
        0x8,   /* Bit 8:  Video Decoder E */
        0xA,   /* Bit 10: Video Decoder F */
        0xC,   /* Bit 12: Video Decoder G */
        0xE    /* Bit 14: Video Decoder H */
    };

    /* disable DRE */
    value   = readDword( DEVICE, ALCS_VP_DRE_0);
    value  &= ~(0x1 << channel_bitmap[decoder]);
    writeDword(DEVICE, ALCS_VP_DRE_0, value );

    return result;
}

/******************************************************************************
    video_quality:
        BEST_CROSS_COLOR                    0
        CROSS_COLOR_MUCH_BETTER             1
        CROSS_COLOR_SLIGHTLY_BETTER         2
        BEST_DOT_CRAWL_AND_HANG_DOT         3
        DEFAULT_VIDEO_QUALITY               4
 ******************************************************************************/
int
ALCS_setVideoQuality(int DEVICE,
                     const char * std,
                     int res,
                     const unsigned long video_quality,
                     const unsigned char decoder)
{
    int result = !0;
    
    /* COMB FILTER SETTING Below */
    if ( video_quality == BEST_DOT_CRAWL_AND_HANG_DOT ) {
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0001016F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E192020);
    }
    else if ( video_quality == BEST_CROSS_COLOR ) {
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0018010F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E198020);
    }
    else if ( video_quality == CROSS_COLOR_SLIGHTLY_BETTER ) {
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0001016F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E198020);
    }
    else if ( video_quality == CROSS_COLOR_MUCH_BETTER ) {
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0018016F);
        writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E194020);
    }
    else {//DEFAULT_VIDEO_QUALITY
        if (VIDEO_IS_NTSC(std)) {
            if(res == RES_960H) {
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0018012F);             
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E192020);
            }
            else {
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0018012F);             
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E194020);
            }
        }
        else {
            if(res == RES_960H) {
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0001016F);             
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E191020);
            }
            else {
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL1 +(0x200*decoder), 0x0018010F);            
                writeDword(DEVICE, ALCS_VDECA_COMB_FILT_CTRL2 +(0x200*decoder), 0x1E194020);
            }
        }
    }

    return result;
}


/******************************************************************************
 *
 * ALCS_setLowPassFilter()
 *
 ******************************************************************************/
int
ALCS_setLowPassFilter(int DEVICE,
                     const unsigned long lpf_sel,
                     const unsigned char decoder)
{
    unsigned long value;
 
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF0+(0x200*decoder), LPF_Coeff[lpf_sel][0] );
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF1+(0x200*decoder), LPF_Coeff[lpf_sel][1] ); 
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF2+(0x200*decoder), LPF_Coeff[lpf_sel][2] ); 
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF3+(0x200*decoder), LPF_Coeff[lpf_sel][3] ); 

    /* enable equalizer */
    value  = readDword( DEVICE, ALCS_VDECA_SRC_CFG + (0x200 * decoder));
    value |= 0x00010000;
    writeDword(DEVICE, ALCS_VDECA_SRC_CFG + (0x200 * decoder), value );

    return 0;
}

/******************************************************************************
 *
 * ALCS_disableLowPassFilter()
 *
 ******************************************************************************/
int
ALCS_disableLowPassFilter(int DEVICE,
                         const unsigned char decoder)
{
    unsigned long value;

    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF0 + (0x200 * decoder), 0x0 );
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF1 + (0x200 * decoder), 0x0 );
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF2 + (0x200 * decoder), 0x0 );
    writeDword(DEVICE, ALCS_VDECA_EQ_FIR_COEFF3 + (0x200 * decoder), 0x0 );

    /* disable equalizer */
    value  = readDword( DEVICE, ALCS_VDECA_SRC_CFG + (0x200 * decoder));
    value &= 0xfffeffff;
    writeDword(DEVICE, ALCS_VDECA_SRC_CFG + (0x200 * decoder), value );

    return 0;
}
