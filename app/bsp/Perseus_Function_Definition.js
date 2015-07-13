#include "PerseusRegistersDefine.js"

//----------------------Cx25838_Pll_Init---------------------
//---- Initialize all PLL in Cx25838
//---- The first step of all operations
//---- after this, the registers of VDEC can be accessed
//int FREQ_DIV
//   0 ... 48MHz Clock Source
//   1 ... 24MHz Clock Source
void Cx25838_Pll_Init(int DEVICE, int FREQ_DIV)
{
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_TUNE1, 0, 7,  0x42);// Set TUNE1,TUNE2 ref_count = 0x210 
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_TUNE2, 0, 7,  0x10);// Set TUNE1,TUNE2 ref_count = 0x210 
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_XTAL, 0, 7,  0x0F);
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_PLL, 0, 7,  0x1F); 
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_PLL2, 0, 7,  FREQ_DIV ?0x1E :0x0F);
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_PLL3, 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_REF, 0, 7,  0x13);
    RegMaskWrite32(DEVICE, COLA_ADC_INT5_STAB_REF, 5, 5, 0x0); // Set int5_boost = 0
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_TUNE1, 0, 7,  0x42);// Set TUNE1,TUNE2 ref_count = 0x210
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_TUNE2, 0, 7,  0x10);// Set TUNE1,TUNE2 ref_count = 0x210
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_XTAL, 0, 7,  0x0F);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_PLL, 0, 7,  0x1F);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_PLL2, 0, 7,  FREQ_DIV ?0x1E :0x0F);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_PLL3, 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_REF, 0, 7,  0x13);
    RegMaskWrite32(DEVICE, COLB_ADC_INT5_STAB_REF, 5, 5, 0x0); // Set int5_boost = 0

    // enable pll
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_PWRDN, 0, 7,  0x18);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_PWRDN, 0, 7,  0x18);

    // start tuning filter
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_TUNE3, 0, 7,  0x40);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_TUNE3, 0, 7,  0x40);

    // exit tuning
    RegMaskWrite32(DEVICE, COLA_SUP_BLK_TUNE3, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_SUP_BLK_TUNE3, 0, 7,  0x00);

    // power up all channels, clear pd_buffer
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_PWRDN_CLAMP, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_PWRDN_CLAMP, 0, 7,  0x00);

    // Enable quantizer calibration
    RegMaskWrite32(DEVICE, COLA_ADC_COM_QUANT, 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLB_ADC_COM_QUANT, 0, 7,  0x02);

    // force modulator reset
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_FB_FRCRST, 0, 7,  0x17);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_FB_FRCRST, 0, 7,  0x17);

    scripting.sleep(5);  //wait for about 5 micro second

    // start quantizer calibration
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_CAL_ATEST, 0, 7,  0x10);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_CAL_ATEST, 0, 7,  0x10);

    scripting.sleep(5);  //wait for about 5 micro second

    // exit modulator (fb) reset
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_FB_FRCRST, 0, 7,  0x07);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_FB_FRCRST, 0, 7,  0x07);

    // Select Vin1_N as single-ended input
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_INPUT, 0, 7,  0x00);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_INPUT, 0, 7,  0x00);

    RegMaskWrite32(DEVICE, COLA_CH1_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_CTRL_DAC1 , 0, 7,  0x05);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_CTRL_DAC1 , 0, 7,  0x05);

    RegMaskWrite32(DEVICE, COLA_CH1_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_CTRL_DAC23 , 0, 7,  0x67);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_CTRL_DAC23 , 0, 7,  0x67);

    RegMaskWrite32(DEVICE, COLA_CH1_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_DCSERVO_DEM , 0, 7,  0x03);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_DCSERVO_DEM , 0, 7,  0x03);

    // enable the pre_clamp in each channel for single-ended input
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_NTF_PRECLMP_EN , 0, 7,  0xF0);

    // use diode instead of resistor, so set term_en to 0, res_en to 0
    RegMaskWrite32(DEVICE, COLA_CH1_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLA_CH2_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLA_CH3_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLA_CH4_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLB_CH1_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLB_CH2_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLB_CH3_ADC_QGAIN_RES_TRM , 0, 7,  0x02);
    RegMaskWrite32(DEVICE, COLB_CH4_ADC_QGAIN_RES_TRM , 0, 7,  0x02);

    if(FREQ_DIV) {
        RegMaskWrite32(DEVICE,VID_US_PLL_INT_FRAC, 25, 30, 0x12); //108MHz for Video
        RegMaskWrite32(DEVICE,AUD_PLL_INT_FRAC, 25, 30, 0x18); // 96MHz for Audio
    }
}


//----------------------Cx25838_Decoder_Init---------------------
//---- Initialize all eight decoders in Cx25838 to specified format
//---- The second step of all operations
//---- after this, the registers of VDEC can be accessed
//---- 1 : NTSC 0: PAL
void Cx25838_Decoder_Init(int DEVICE,int Format)
{
    int i;
    int VDEC_OFFSET = 0x200;
    int VDEC_COUNT = 0x8;
    if (Format == 1) //NTSC
    {
    // Loop 8 times for 8 decoders
    for(i = 0; i <VDEC_COUNT; i++)
    {
        // RESET 8 video decoders
        RegMaskWrite32(DEVICE,VDECA_SOFT_RST_CTRL+(VDEC_OFFSET*i), 15, 15, 0x1);
        RegMaskWrite32(DEVICE,VDECA_SOFT_RST_CTRL+(VDEC_OFFSET*i), 15, 15, 0x0);
        RegMaskWrite32(DEVICE,VDECA_MODE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0005E001);           
        RegMaskWrite32(DEVICE,VDECA_OUT_CTRL1+(VDEC_OFFSET*i), 0, 31, 0x001306A2);            
        RegMaskWrite32(DEVICE,VDECA_OUT_CTRL_NS+(VDEC_OFFSET*i), 0, 31, 0x401306A2);     
        RegMaskWrite32(DEVICE,VDECA_LUMA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00158000);         
        RegMaskWrite32(DEVICE,VDECA_CHROMA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x21008080);        
        RegMaskWrite32(DEVICE,VDECA_CRUSH_CTRL+(VDEC_OFFSET*i), 0, 31, 0x001C1E0F);
        RegMaskWrite32(DEVICE,VDECA_HORIZ_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x5E2D0074);
        RegMaskWrite32(DEVICE,VDECA_VERT_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x1E1E001A);
        RegMaskWrite32(DEVICE,VDECA_MISC_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0000C000);
        RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
        RegMaskWrite32(DEVICE,VDECA_VSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
        RegMaskWrite32(DEVICE,VDECA_MAN_VGA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x80002018);
        RegMaskWrite32(DEVICE,VDECA_MAN_AGC_CTRL+(VDEC_OFFSET*i), 0, 31, 0x81000416);
        RegMaskWrite32(DEVICE,VDECA_DFE_CTRL1+(VDEC_OFFSET*i), 0, 31, 0xE000000A);     
        RegMaskWrite32(DEVICE,VDECA_DFE_CTRL2+(VDEC_OFFSET*i), 0, 31, 0x401040DC);     
        RegMaskWrite32(DEVICE,VDECA_DFE_CTRL3+(VDEC_OFFSET*i), 0, 31, 0xCD00028A);    
        RegMaskWrite32(DEVICE,VDECA_VCR_DET_CTRL+(VDEC_OFFSET*i), 0, 31, 0xC0F00AB8);    
        RegMaskWrite32(DEVICE,VDECA_HTL_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00002040);    
        RegMaskWrite32(DEVICE,VDECA_PLL_CTRL_FAST+(VDEC_OFFSET*i), 0, 31, 0x801FFFFF);   
        RegMaskWrite32(DEVICE,VDECA_SC_STEP_SIZE +(VDEC_OFFSET*i), 0, 31, 0x43E00000);

    // COMB FILTER SETTINGS BELOW
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFS_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001453);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFD_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001453);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_LF_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001029);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_BLEND +(VDEC_OFFSET*i), 0, 31, 0x414F4F4F);
      RegMaskWrite32(DEVICE,VDECA_COMB_MISC_CTRL +(VDEC_OFFSET*i), 0, 31, 0x02C00A0E);
      RegMaskWrite32(DEVICE,VDECA_COMB_FLAT_THRESH_CTRL +(VDEC_OFFSET*i), 0, 31, 0x00030000);
      RegMaskWrite32(DEVICE,VDECA_COMB_TEST +(VDEC_OFFSET*i), 0, 31, 0x00000000); 
    }
  }
  else  //PAL
  {
      for(i = 0; i <VDEC_COUNT; i++)
    {
      // RESET 8 video decoders
      RegMaskWrite32(DEVICE,VDECA_SOFT_RST_CTRL+(VDEC_OFFSET*i), 15, 15, 0x1);
      RegMaskWrite32(DEVICE,VDECA_SOFT_RST_CTRL+(VDEC_OFFSET*i), 15, 15, 0x0);
      RegMaskWrite32(DEVICE,VDECA_MODE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0005E004);           
      RegMaskWrite32(DEVICE,VDECA_OUT_CTRL1+(VDEC_OFFSET*i), 0, 31, 0x001306A2);            
      RegMaskWrite32(DEVICE,VDECA_OUT_CTRL_NS+(VDEC_OFFSET*i), 0, 31, 0x401306A2);     
        RegMaskWrite32(DEVICE,VDECA_LUMA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00158000);         
      RegMaskWrite32(DEVICE,VDECA_CHROMA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x21008080);        
      RegMaskWrite32(DEVICE,VDECA_CRUSH_CTRL+(VDEC_OFFSET*i), 0, 31, 0x001C1E0F);
      RegMaskWrite32(DEVICE,VDECA_HORIZ_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x612D007D);
      RegMaskWrite32(DEVICE,VDECA_VERT_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x26240022);
      RegMaskWrite32(DEVICE,VDECA_MISC_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0000C000);
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
      RegMaskWrite32(DEVICE,VDECA_VSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
      RegMaskWrite32(DEVICE,VDECA_MAN_VGA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x80002003);
      RegMaskWrite32(DEVICE,VDECA_MAN_AGC_CTRL+(VDEC_OFFSET*i), 0, 31, 0x81000A03);
      RegMaskWrite32(DEVICE,VDECA_DFE_CTRL1+(VDEC_OFFSET*i), 0, 31, 0xE000000A);     
      RegMaskWrite32(DEVICE,VDECA_DFE_CTRL2+(VDEC_OFFSET*i), 0, 31, 0x401040DC);     
      RegMaskWrite32(DEVICE,VDECA_DFE_CTRL3+(VDEC_OFFSET*i), 0, 31, 0xCD00028A);    
      RegMaskWrite32(DEVICE,VDECA_VCR_DET_CTRL+(VDEC_OFFSET*i), 0, 31, 0xC0F00AB8);    
      RegMaskWrite32(DEVICE,VDECA_HTL_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00002040);    
      RegMaskWrite32(DEVICE,VDECA_PLL_CTRL_FAST+(VDEC_OFFSET*i), 0, 31, 0x801FFFFF);   
      RegMaskWrite32(DEVICE,VDECA_SC_STEP_SIZE +(VDEC_OFFSET*i), 0, 31, 0x5411E2D0);

    // COMB FILTER SETTINGS BELOW
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFS_CFG +(VDEC_OFFSET*i), 0, 31, 0x0000142F);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFD_CFG +(VDEC_OFFSET*i), 0, 31, 0x0000302F);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_LF_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001023);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_BLEND +(VDEC_OFFSET*i), 0, 31, 0x410F2F0F);
      RegMaskWrite32(DEVICE,VDECA_COMB_MISC_CTRL +(VDEC_OFFSET*i), 0, 31, 0x04100A0E);
      RegMaskWrite32(DEVICE,VDECA_COMB_FLAT_THRESH_CTRL +(VDEC_OFFSET*i), 0, 31, 0x000F0000);
      RegMaskWrite32(DEVICE,VDECA_COMB_TEST +(VDEC_OFFSET*i), 0, 31, 0x00000000); 
    }
  }

   for (i = 0; i < 8; i ++) {
      RegMaskWrite32(DEVICE,VDECA_OUT_CTRL_NS+i*0x200, 30, 30, 0x1); //force vpres to 0
   }
}

//----------------------Cx25838_Decoder_Format---------------------
//---- Initialize all eight decoders in Cx25838 to specified format
//---- The second step of all operations
//---- after this, the registers of VDEC can be accessed
//---- 1 : NTSC 0: PAL
void Cx25838_Decoder_Format(int DEVICE,int Format)
{
    int i;
    int VDEC_OFFSET = 0x200;
    int VDEC_COUNT = 0x8;
    if (Format == 1) //NTSC
    {
    // Loop 8 times for 8 decoders
    for(i = 0; i <VDEC_COUNT; i++)
    {
        // RESET 8 video decoders
        RegMaskWrite32(DEVICE,VDECA_MODE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0005E001);           
        RegMaskWrite32(DEVICE,VDECA_HORIZ_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x5E2D0074);
        RegMaskWrite32(DEVICE,VDECA_VERT_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x1E1E001A);
        RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
        RegMaskWrite32(DEVICE,VDECA_VSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
        RegMaskWrite32(DEVICE,VDECA_MAN_VGA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x80002018);
        RegMaskWrite32(DEVICE,VDECA_MAN_AGC_CTRL+(VDEC_OFFSET*i), 0, 31, 0x81000416);
        RegMaskWrite32(DEVICE,VDECA_SC_STEP_SIZE +(VDEC_OFFSET*i), 0, 31, 0x43E00000);

    // COMB FILTER SETTINGS BELOW
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFS_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001453);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFD_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001453);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_LF_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001029);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_BLEND +(VDEC_OFFSET*i), 0, 31, 0x414F4F4F);
      RegMaskWrite32(DEVICE,VDECA_COMB_MISC_CTRL +(VDEC_OFFSET*i), 0, 31, 0x02C00A0E);
      RegMaskWrite32(DEVICE,VDECA_COMB_FLAT_THRESH_CTRL +(VDEC_OFFSET*i), 0, 31, 0x00030000);
      RegMaskWrite32(DEVICE,VDECA_COMB_TEST +(VDEC_OFFSET*i), 0, 31, 0x00000000); 
    }
  }
  else  //PAL
  {
      for(i = 0; i <VDEC_COUNT; i++)
    {
      // RESET 8 video decoders
      RegMaskWrite32(DEVICE,VDECA_MODE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x0005E004);           
      RegMaskWrite32(DEVICE,VDECA_HORIZ_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x612D007D);
      RegMaskWrite32(DEVICE,VDECA_VERT_TIM_CTRL+(VDEC_OFFSET*i), 0, 31, 0x26240022);
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
      RegMaskWrite32(DEVICE,VDECA_VSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x00000000);
      RegMaskWrite32(DEVICE,VDECA_MAN_VGA_CTRL+(VDEC_OFFSET*i), 0, 31, 0x80002003);
      RegMaskWrite32(DEVICE,VDECA_MAN_AGC_CTRL+(VDEC_OFFSET*i), 0, 31, 0x81000A03);
      RegMaskWrite32(DEVICE,VDECA_SC_STEP_SIZE +(VDEC_OFFSET*i), 0, 31, 0x5411E2D0);

    // COMB FILTER SETTINGS BELOW
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFS_CFG +(VDEC_OFFSET*i), 0, 31, 0x0000142F);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_HFD_CFG +(VDEC_OFFSET*i), 0, 31, 0x0000302F);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_LF_CFG +(VDEC_OFFSET*i), 0, 31, 0x00001023);
      RegMaskWrite32(DEVICE,VDECA_COMB_2D_BLEND +(VDEC_OFFSET*i), 0, 31, 0x410F2F0F);
      RegMaskWrite32(DEVICE,VDECA_COMB_MISC_CTRL +(VDEC_OFFSET*i), 0, 31, 0x04100A0E);
      RegMaskWrite32(DEVICE,VDECA_COMB_FLAT_THRESH_CTRL +(VDEC_OFFSET*i), 0, 31, 0x000F0000);
      RegMaskWrite32(DEVICE,VDECA_COMB_TEST +(VDEC_OFFSET*i), 0, 31, 0x00000000); 
    }
  }
}


//----------------------Cx25838_Port_Enable---------------------
//---- Initialize port in Cx25838 to specified mode
//---- P_Port:
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- P_Mode:
//            0x0 Disabled ;
//            0x1 Pass-thru mode;
//            0x2 ByteInterleaved2Streams 54MHz;
//            0x3 ByteInterleaved4Streams  108MHz;
//            0x9 TilerFramer8bit from TF0;
//            0xb TilerFramer8bit from TF1;
//---- P_Source
//            0x0 from Ch_A, scaling;
//            0x1 from Ch_A, non scaling;
//            0x2 from Ch_B, scaling;
//            0x3 from Ch_B, non scaling;
//            0x4 from Ch_C, scaling;
//            0x5 from Ch_C, non scaling;
//            0x6 from Ch_D, scaling;
//            0x7 from Ch_D, non scaling;
//            0x8 from Ch_E, scaling;
//            0x9 from Ch_E, non scaling;
//            0x10 from Ch_F, scaling;
//            0x11 from Ch_F, non scaling;
//            0x12 from Ch_G, scaling;
//            0x13 from Ch_G, non scaling;
//            0x14 from Ch_H, scaling;
//            0x15 from Ch_H, non scaling;
//---- CLK_Pol
//            0x0 0 degree
//            0x1 180 degree
void Cx25838_Port_Enable(int DEVICE,int P_Port, int P_Mode, int P_Source, int CLK_Pol)
{
  int PORT_OFFSET = 0x10;
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 16, 19, P_Source);
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 8, 11, P_Mode);
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 2, 2, CLK_Pol); 
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 0, 1, 0x3);        //enable this port 
}



//----------------------Cx25838_Port_Disable---------------------
//---- disable port in Cx25838 to specified mode
void Cx25838_Port_Disable(int DEVICE,int P_Port)
{
    int PORT_OFFSET = 0x10;
    RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 0, 1, 0x0);      
}

//----------------------Cx25838_Port_Switch_Source---------------------
//---- change the source to port in Cx25838
void Cx25838_Port_Switch_Source(int DEVICE,int P_Port, int P_Source)
{
    int PORT_OFFSET = 0x10;
    RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL +(PORT_OFFSET*P_Port), 16, 19, P_Source);      
}


//----------------------Cx25838_Sat_Adjust---------------------
//---- adjust the color saturation
//---- VDEC:
//     0x0--ChA
//     0x1--ChB
//     0x2--ChC
//     0x3--ChD
//     0x4--ChE
//     0x5--ChF
//     0x6--ChG
//     0x7--ChH
void Cx25838_Sat_Adjust(int DEVICE,int VDEC, int U_sat, int V_sat)
{
    int VDEC_OFFSET = 0x200;
    RegMaskWrite32(DEVICE,VDECA_CHROMA_CTRL +(VDEC_OFFSET*VDEC), 8, 15, V_sat);      
    RegMaskWrite32(DEVICE,VDECA_CHROMA_CTRL +(VDEC_OFFSET*VDEC), 0, 7, U_sat);
}

//----------------------Cx25838_Bright_Adjust---------------------
//---- adjust the brightness
//---- VDEC:
//     0x0--ChA
//     0x1--ChB
//     0x2--ChC
//     0x3--ChD
//     0x4--ChE
//     0x5--ChF
//     0x6--ChG
//     0x7--ChH
void Cx25838_Bright_Adjust(int DEVICE,int VDEC, int bright)
{
    int VDEC_OFFSET = 0x200;
    RegMaskWrite32(DEVICE,VDECA_LUMA_CTRL +(VDEC_OFFSET*VDEC), 0, 7, bright);    
}

//----------------------Cx25838_Contrast_Adjust---------------------
//---- adjust the contrast
//---- VDEC:
//     0x0--ChA
//     0x1--ChB
//     0x2--ChC
//     0x3--ChD
//     0x4--ChE
//     0x5--ChF
//     0x6--ChG
//     0x7--ChH
void Cx25838_Contrast_Adjust(int DEVICE,int VDEC, int cntrst)
{
    int VDEC_OFFSET = 0x200;
    RegMaskWrite32(DEVICE,VDECA_LUMA_CTRL +(VDEC_OFFSET*VDEC), 8, 15, cntrst);    
}

//----------------------Cx25838_Hue_Adjust---------------------
//---- adjust the Hue
//---- VDEC:
//     0x0--ChA
//     0x1--ChB
//     0x2--ChC
//     0x3--ChD
//     0x4--ChE
//     0x5--ChF
//     0x6--ChG
//     0x7--ChH
void Cx25838_Hue_Adjust(int DEVICE,int VDEC, int hue)
{
    int VDEC_OFFSET = 0x200;
    RegMaskWrite32(DEVICE,VDECA_CHROMA_CTRL +(VDEC_OFFSET*VDEC), 16, 23, hue);    
}


//----------------------Cx25838_DDR_Init---------------------
//---- initialize the DDR memory
int fs_ch0_fr0_base = 0x00; //
int fs_ch0_fr1_base = 0x40;
int fs_ch1_fr0_base = 0x20;
int fs_ch1_fr1_base = 0x60;
int fs_ch2_fr0_base = 0x04;
int fs_ch2_fr1_base = 0x44;
int fs_ch3_fr0_base = 0x24;
int fs_ch3_fr1_base = 0x64;
int fs_ch4_fr0_base = 0x08; //
int fs_ch4_fr1_base = 0x48;
int fs_ch5_fr0_base = 0x28;
int fs_ch5_fr1_base = 0x68;
int fs_ch6_fr0_base = 0x0C;
int fs_ch6_fr1_base = 0x4C;
int fs_ch7_fr0_base = 0x2C;
int fs_ch7_fr1_base = 0x6C;
int fs_ch8_fr0_base = 0x02; //
int fs_ch8_fr1_base = 0x42;
int fs_ch9_fr0_base = 0x22;
int fs_ch9_fr1_base = 0x62;
int fs_ch10_fr0_base = 0x06; //
int fs_ch10_fr1_base = 0x46;
int fs_ch11_fr0_base = 0x26;
int fs_ch11_fr1_base = 0x66;
int fs_ch12_fr0_base = 0x0A;
int fs_ch12_fr1_base = 0x4A;
int fs_ch13_fr0_base = 0x2A;
int fs_ch13_fr1_base = 0x6A;
int fs_ch14_fr0_base = 0x0E; //
int fs_ch14_fr1_base = 0x4E;
int fs_ch15_fr0_base = 0x2E;
int fs_ch15_fr1_base = 0x6E;
void Cx25838_DDR_Init(int DEVICE)
{
    int i;
    int CAL_DONE;
    //enable calibration ref clk
    RegMaskWrite32(DEVICE,DR_PADCAL, 0, 31, 0x8004);
    scripting.sleep(1);  

    //start calibration
    RegMaskWrite32(DEVICE,DR_PADCAL, 0, 31, 0x40008004);
    //scripting.sleep(1);
    do {
        CAL_DONE = RegMaskRead32(DEVICE, DR_PADCAL, 31, 31);
        scripting.sleep(10);
    } while(CAL_DONE == 0);

    scripting.sleep(10);  

    //disable calibration ref clk
    RegMaskWrite32(DEVICE,DR_PADCAL, 0, 31, 0x40000004);
      scripting.sleep(1);
      
    //
    //DDR2 Initialization
    //
    RegMaskWrite32(DEVICE,DR_ACCTHRSH, 0, 15, 0x0000);
    
    //Turn off bank interleave
    //Reduce Read Latency from 0x18 to 0x16
    RegMaskWrite32(DEVICE,DR_LIMIT4, 0, 5, 0x1b);
      RegMaskWrite32(DEVICE,DR_LIMIT4, 31, 31, 0x0);
    //device enable, single-end dqs, 2 counts
    
    //RegMaskWrite32(DEVICE,DR_CTRL, 0, 31, 0x640b026);
    RegMaskWrite32(DEVICE,DR_CTRL, 0, 15, 0xb026);
    
    RegMaskWrite32(DEVICE,VP_FS_INIT_CMD,31,31,0x0);//Disable Frame Store

    for(i=0; i<= 16; i++)
    {
        int fs_ch0_In_offset = 0;
        RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 1, 3, fs_ch0_In_offset);
    }
     
    // Set Frame Store Channels Base addresses
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG, 16, 23, fs_ch0_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG, 8, 15, fs_ch0_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH1_CFG, 16, 23, fs_ch1_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH1_CFG, 8, 15, fs_ch1_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH2_CFG, 16, 23, fs_ch2_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH2_CFG, 8, 15, fs_ch2_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH3_CFG, 16, 23, fs_ch3_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH3_CFG, 8, 15, fs_ch3_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH4_CFG, 16, 23, fs_ch4_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH4_CFG, 8, 15, fs_ch4_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH5_CFG, 16, 23, fs_ch5_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH5_CFG, 8, 15, fs_ch5_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH6_CFG, 16, 23, fs_ch6_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH6_CFG, 8, 15, fs_ch6_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH7_CFG, 16, 23, fs_ch7_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH7_CFG, 8, 15, fs_ch7_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG, 16, 23, fs_ch8_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG, 8, 15, fs_ch8_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH9_CFG, 16, 23, fs_ch9_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH9_CFG, 8, 15, fs_ch9_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH10_CFG, 16, 23, fs_ch10_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH10_CFG, 8, 15, fs_ch10_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH11_CFG, 16, 23, fs_ch11_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH11_CFG, 8, 15, fs_ch11_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH12_CFG, 16, 23, fs_ch12_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH12_CFG, 8, 15, fs_ch12_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH13_CFG, 16, 23, fs_ch13_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH13_CFG, 8, 15, fs_ch13_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH14_CFG, 16, 23, fs_ch14_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH14_CFG, 8, 15, fs_ch14_fr1_base);
    RegMaskWrite32(DEVICE,VP_FS_CH15_CFG, 16, 23, fs_ch15_fr0_base);
    RegMaskWrite32(DEVICE,VP_FS_CH15_CFG, 8, 15, fs_ch15_fr1_base);
    
}

//----------------------Cx25838_Tiling_Mode---------------------
//---- set the video to tiling mode
//---- Format:
//            1--NTSC
//            0--PAL
//---- P_T_Port1, P_T_Port2,P_CLK_54M
//            0--port A
//            1--port B
//            2--port C
//            3--port D
void Cx25838_Tiling_Mode_Basic_Mode(int DEVICE,int Format, int P_T_Port1, int P_T_Port2, int P_CLK_54M)
{
    
    //Scale horizontal to 352 from 720
  int VDEC_OFFSET = 0x200;
  int i;
  for (i=0;i<8;i=i+1)
  {
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x10ba2f);
  }
    
  //enable frame store
  for(i = 0; i <= 8; i++) // Do this for first 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE);  // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);    // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x0);    // Select decoders as frame store channels sources
  }

  for(i = 0; i <= 8; i++) // Do this for second 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 28, 31, 0xE); // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 0, 0, 0x0);   // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 4, 7, 0x3);   // Select DVP as frame store channels sources
  }
        
    //config the T/F module
    RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x0);
    RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x0);
    
    if(Format == 1) //NTSC
    {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0xB410F700);
      RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 31, 0xB410F700);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x004160F0);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x10A170F0);    
    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_1, 0, 31, 0x004160F0);    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_2, 0, 31, 0x10A170F0);    
  }
  else             //PAL
  {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0xB411B700);
      RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 31, 0xB411B700);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x00117120);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x13917120);
    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_1, 0, 31, 0x00117120);    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_2, 0, 31, 0x13917120);        
  }
  
  RegMaskWrite32(DEVICE,VP_TF0_SRC1_CFG, 0, 31, 0x10000000);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC2_CFG, 0, 31, 0x10010000);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC3_CFG, 0, 31, 0x10020000);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC4_CFG, 0, 31, 0x10030000);      
  RegMaskWrite32(DEVICE,VP_TF1_SRC1_CFG, 0, 31, 0x11040000);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC2_CFG, 0, 31, 0x11050000);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC3_CFG, 0, 31, 0x11060000);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC4_CFG, 0, 31, 0x11070000);      
    
  //enable port
  Cx25838_Port_Enable(DEVICE, P_T_Port1, 0x9, 0x0, 0x1); //P_T_Port1 : data from T/F 0;
  Cx25838_Port_Enable(DEVICE, P_T_Port2, 0xb, 0x0, 0x1); //P_T_Port2 : data from T/F 1;
  Cx25838_Port_Enable(DEVICE, P_CLK_54M, 0x2, 0x0, 0x1); //P_CLK_54M : provide 54M clk;
  
  //enable frame store and T/F modules    
  RegMaskWrite32(DEVICE,VP_FS_INIT_CMD,31,31,0x1);//enable Frame Store
  RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x1);  //enable T/F 0
    RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x1);  //enable T/F 1
}


//----------------------Cx25838_8CIF_Frame_Mode---------------------
//---- set the video to 8CIF frame mode for HiSilicon solution
//---- Format:
//            1--NTSC
//            0--PAL
//---- P_T_Port1
//            0--port A
//            1--port B
//            2--port C
//            3--port D
void Cx25838_8CIF_Frame_Mode(int DEVICE,int Format, int P_T_Port1)
{
    
    //Scale horizontal to 352 from 720
  int VDEC_OFFSET = 0x200;
  int i;
  for (i=0;i<8;i=i+1)
  {
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x10ba2f);
  }
    
  //enable frame store
  for(i = 0; i <= 8; i++) // Do this for first 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE);  // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);    // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x0);    // Select decoders as frame store channels sources
  }

        
    //config the T/F module
    RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x0);
    RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x0);
    
    if(Format == 1) //NTSC
    {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x580b9670);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x000260f0);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00000000);    

    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_1, 0, 31, 0x001020f0);    
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_2, 0, 31, 0x00100000);
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x580b0000);
  }
  else             //PAL
  {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x580c1670);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x00025120);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00000000);    

    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_1, 0, 31, 0x00102120);    
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_2, 0, 31, 0x00100000);
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x580c0000);
  }
  
  RegMaskWrite32(DEVICE,VP_TF0_SRC1_CFG, 0, 31, 0x10000001);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC2_CFG, 0, 31, 0x11010011);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC3_CFG, 0, 31, 0x12020011);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC4_CFG, 0, 31, 0x13030011);      
  RegMaskWrite32(DEVICE,VP_TF0_SRC5_CFG, 0, 31, 0x14040001);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC6_CFG, 0, 31, 0x15050011);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC7_CFG, 0, 31, 0x16060011);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC8_CFG, 0, 31, 0x17070011);      
    
  //enable port
  Cx25838_Port_Enable(DEVICE,P_T_Port1, 0x9, 0x0, 0x1); //P_T_Port1 : data from T/F 0;
  
  //enable frame store and T/F modules    
  RegMaskWrite32(DEVICE,VP_FS_INIT_CMD,31,31,0x1);//enable Frame Store
  RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x1);  //enable T/F 0
}






//----------------------Cx25838_Tiling_Mode_Single_Field---------------------
//---- set the video to tiling mode for Hi3515 solution 1, 
//---- Format:
//            1--NTSC
//            0--PAL
//---- P_T_Port1, P_T_Port2,P_CLK_54M
//            0--port A
//            1--port B
//            2--port C
//            3--port D
void Cx25838_Tiling_Mode_Single_Field(int DEVICE,int Format, int P_T_Port1, int P_T_Port2, int P_CLK_54M)
{
    
    //Scale horizontal to 352 from 720
  int VDEC_OFFSET = 0x200;
  int i;
  for (i=0;i<8;i=i+1)
  {
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x10ba2f);
  }
    
  //enable frame store
  for(i = 0; i <= 8; i++) // Do this for first 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE);  // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);    // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x0);    // Select decoders as frame store channels sources
  }

  for(i = 0; i <= 8; i++) // Do this for second 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 28, 31, 0xE); // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 0, 0, 0x0);   // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH8_CFG+(0x4*i), 4, 7, 0x3);   // Select DVP as frame store channels sources
  }
        
    //config the T/F module
    RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x0);
    RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x0);
    
    if(Format == 1) //NTSC
    {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0xB012F700);
      RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 31, 0xB012F700);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x0012D1E0);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00100000);    
    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_1, 0, 31, 0x0012D1E0);    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_2, 0, 31, 0x00100000);    
  }
  else             //PAL
  {
      RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0xB013B700);
      RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 31, 0xB013B700);
    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x00131240);    
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00100000);
    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_1, 0, 31, 0x00131240);    
    RegMaskWrite32(DEVICE,VP_TF1_FRAME_SIZE_2, 0, 31, 0x00100000);        
  }
  
  RegMaskWrite32(DEVICE,VP_TF0_SRC1_CFG, 0, 31, 0x10000001);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC2_CFG, 0, 31, 0x10010001);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC3_CFG, 0, 31, 0x10020001);    
  RegMaskWrite32(DEVICE,VP_TF0_SRC4_CFG, 0, 31, 0x10030001);      
  RegMaskWrite32(DEVICE,VP_TF1_SRC1_CFG, 0, 31, 0x11040001);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC2_CFG, 0, 31, 0x11050001);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC3_CFG, 0, 31, 0x11060001);    
  RegMaskWrite32(DEVICE,VP_TF1_SRC4_CFG, 0, 31, 0x11070001);      
    
  //enable port
  Cx25838_Port_Enable(DEVICE,P_T_Port1, 0x9, 0x0, 0x1); //P_T_Port1 : data from T/F 0;
  Cx25838_Port_Enable(DEVICE,P_T_Port2, 0xb, 0x0, 0x1); //P_T_Port2 : data from T/F 1;
  Cx25838_Port_Enable(DEVICE,P_CLK_54M, 0x2, 0x0, 0x1); //P_CLK_54M : provide 54M clk;
  
  //enable frame store and T/F modules    
  RegMaskWrite32(DEVICE,VP_FS_INIT_CMD,31,31,0x1);//enable Frame Store
  RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x1);  //enable T/F 0
    RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x1);  //enable T/F 1
}

//----------------------Cx25838_8CIF_Cas_Mode-----------------------
//---- Set the video to 4 CIF@108MHz cascade mode
//---- Port1,Port2
//            0--port A
//            1--port B
//            2--port C
//            3--port D
void Cx25838_8CIF_Cas_Mode(int DEVICE,int Format, int Port1, int Port2)
{
    //Scale horizontal to 352 from 720
  int VDEC_OFFSET = 0x200;
  int i;
    for (i=0;i<8;i=i+1)
  {
      RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x10ba2f);
  }
    
  //enable port
  Cx25838_Port_Enable(DEVICE,Port1, 0x3, 0x0, 0x1); 
  Cx25838_Port_Enable(DEVICE,Port2, 0x3, 0x0, 0x1); 
  
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL+(Port2*0x10), 16, 31, 0x6420);//////////
  RegMaskWrite32(DEVICE,VP_DVP_A_CNTRL+(Port1*0x10), 16, 31, 0xeca8);//////////
  
  RegMaskWrite32(DEVICE,VP_DVP_A_DEMUX+(Port1*0x10), 0, 31, 0x00321000);///////////
  RegMaskWrite32(DEVICE,VP_DVP_A_DEMUX+(Port2*0x10), 0, 31, 0x00321000);///////////
  
  RegMaskWrite32(DEVICE,VP_DVP_A_BYTE_MUX+(Port1*0x10), 0, 31, 0x00002E01);
  RegMaskWrite32(DEVICE,VP_DVP_A_BYTE_MUX+(Port2*0x10), 0, 31, 0x00002E01);
}

//----------------------Cx25838_16CIF_Mode_BigFrame---------------------
//---- set the video to tiling mode for Hi3515 solution 1, 
//---- Format:
//            1--NTSC
//            0--PAL
//---- P_T_Port, P_C_Port1, P_C_Port2
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- HACT:
//            0--352 pixel per active line
//            1--360 pixel per active line
void Cx25838_16CIF_Mode_BigFrame(int DEVICE, int Format, int P_T_Port, int P_C_Port1, int P_C_Port2, int HACT)
{
	
	//Scale horizontal to 352 from 720
  int VDEC_OFFSET = 0x200;
  int i;
  for (i=0;i<8;i=i+1)
  {
  		if (HACT==0x0)
      	RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x10ba2f);
      else
      	RegMaskWrite32(DEVICE,VDECA_HSCALE_CTRL+(VDEC_OFFSET*i), 0, 31, 0x100000);
  }
	
  //enable ports for cascade
  
  Cx25838_Port_Disable(DEVICE,P_C_Port1);
  Cx25838_Port_Disable(DEVICE,P_C_Port2);

  RegMaskWrite32(DEVICE,VP_DVP_A_DEMUX+(P_C_Port1*0x10), 0, 31, 0x00321031);///////
  RegMaskWrite32(DEVICE,VP_DVP_A_DEMUX+(P_C_Port2*0x10), 0, 31, 0x00321031);///////
  
  RegMaskWrite32(DEVICE,VP_DVP_A_BYTE_MUX+(P_C_Port1*0x10), 0, 31, 0x00002E00);
  RegMaskWrite32(DEVICE,VP_DVP_A_BYTE_MUX+(P_C_Port2*0x10), 0, 31, 0x00002E00);
  
  //enable frame store
  for(i = 0; i < 4; i++) // Do this for first 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE);  // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);    // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x0);    // Select decoders as frame store channels sources
  }

  for(i = 4; i < 8; i++) // Do this for first 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE);  // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);    // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x0);    // Select decoders as frame store channels sources
  }
  //-----------------------------------------------------------------------------------
  //ONLY FOR CASCADING AT B,C PORT, IF USE OTHER PORTS, CONTACT CONEXANT FOR SUPPORT
  for(i = 8; i < 12; i++) // Do this for second 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE); // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);   // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x3);   // Select DVP as frame store channels sources
  }
		
  for(i = 12; i < 16; i++) // Do this for second 8 FS channels
  {     
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 28, 31, 0xE); // Frame store top field, strip invalid pixels, enable channel frame store
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 0, 0, 0x0);   // Set 1KByte for each line to accomodate CIF line length
    RegMaskWrite32(DEVICE,VP_FS_CH0_CFG+(0x4*i), 4, 7, 0x2);   // Select DVP as frame store channels sources
  }
  //----------------------------------------------------------------------------------
	//config the T/F module
	RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x0);
	RegMaskWrite32(DEVICE,VP_TF1_CFG, 0, 0, 0x0);
	
	if(Format == 1) //NTSC
	{
		if (HACT==0x0)
	  	RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x580cc6f0); //352
	  else
	  	RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x5a0bc6f0); //360
	
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x000250f0);	
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00000000);
    
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_1, 0, 31, 0x001020f0);	
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_2, 0, 31, 0x00100000);
    if (HACT==0x0)
    	RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x580cc000);	
    else
    	RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x5a0bc000);	//360
  }
  else             //PAL
  {
		if (HACT==0x0)
	  	RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x580c06f0);
	  else
	  	RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 31, 0x5a0b06f0);
	
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_1, 0, 31, 0x00025120);	
    RegMaskWrite32(DEVICE,VP_TF0_FRAME_SIZE_2, 0, 31, 0x00000000);
    
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_1, 0, 31, 0x00102120);	
    RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_2, 0, 31, 0x00100000);
		if (HACT==0x0)
    	RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x580c0000);
    else
    	RegMaskWrite32(DEVICE,VP_TF0_ALT_FRAME_SIZE_3, 0, 31, 0x5a0b0000);
    	
  }
  //---------------------------------------------------------------------------------
  //ONLY FOR CASCADING AT B,C PORT, IF USE OTHER PORTS, CONTACT CONEXANT FOR SUPPORT
  RegMaskWrite32(DEVICE,VP_TF0_SRC1_CFG, 0, 31, 0x10000001);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC2_CFG, 0, 31, 0x11010011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC3_CFG, 0, 31, 0x12020011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC4_CFG, 0, 31, 0x13030011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC5_CFG, 0, 31, 0x14040011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC6_CFG, 0, 31, 0x15050011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC7_CFG, 0, 31, 0x16060011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC8_CFG, 0, 31, 0x17070011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC9_CFG, 0, 31, 0x180A0001);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC10_CFG, 0, 31, 0x190B0011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC11_CFG, 0, 31, 0x1a080011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC12_CFG, 0, 31, 0x1b090011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC13_CFG, 0, 31, 0x1c0c0011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC14_CFG, 0, 31, 0x1d0d0011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC15_CFG, 0, 31, 0x1e0e0011);	
  RegMaskWrite32(DEVICE,VP_TF0_SRC16_CFG, 0, 31, 0x1f0f0011);	
  //---------------------------------------------------------------------------------

  //enable port
  Cx25838_Port_Enable(DEVICE,P_T_Port, 0x9, 0x0, 0x1); //P_T_Port : data from T/F 0;
  
  //enable frame store and T/F modules	
  RegMaskWrite32(DEVICE,VP_FS_INIT_CMD,31,31,0x1);//enable Frame Store
  RegMaskWrite32(DEVICE,VP_TF0_CFG, 0, 0, 0x1);  //enable T/F 0
}


//----------------------Cx25838_Audio_Init---------------------
void Cx25838_Audio_Init(int DEVICE)
{
    //ADC
    RegMaskWrite32(DEVICE,AADC_DIG_CTRL, 0, 31,0x00031111); // 4dB gain, 8KHz Sample Rate, Enable ADC1,2,3 and 4
    RegMaskWrite32(DEVICE,AADC_ANA_CTRL, 0, 31,0xF1005555); // Power on all ADCs, DSM dither enable
    RegMaskWrite32(DEVICE,AADC_ANA_CTRL2, 0, 31,0x00000000); // Default value
    RegMaskWrite32(DEVICE,AADC_ANA_CTRL3, 0, 31,0x00000000); // Default value

    // Power on PLL, PLL frequency = (48e6 * 12.288)/6 = 98.304 MHz
    RegMaskWrite32(DEVICE,AUD_PLL_CTRL, 0, 31,0x40522716); // Power on PLL, PLL frequency = 98.304MHz
    //RegMaskWrite32(DEVICE,AUD_PLL_INT_FRAC, 0, 31,0x189374BC); // Default Value
    
    // I2S
    RegMaskWrite32(DEVICE,AP_AUD_GEN_CFG, 0, 31,0x0); // frame_sync_delay = 0,frame_sync_width = 0,  i2s_mode = 0
    RegMaskWrite32(DEVICE,AP_AUD_1_CFG, 0, 31,0x00010103);  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
    RegMaskWrite32(DEVICE,AP_AUD_2_CFG, 0, 31,0x00010103);  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
    RegMaskWrite32(DEVICE,AP_AUD_3_CFG, 0, 31,0x00010103);  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode) 
    RegMaskWrite32(DEVICE,AP_AUD_4_CFG, 0, 31,0x00010103);  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
}

//----------------------Cx25838_Audio_Sample_Rate---------------------
//---- Sample_Rate
//                0x0--48KHz
//                0x1--32KHz
//                0x2--16KHz
//                0x3--8KHz
void Cx25838_Audio_Sample_Rate(int DEVICE,int Sample_Rate)
{
    RegMaskWrite32(DEVICE,AADC_DIG_CTRL, 16, 18, Sample_Rate);
}

//----------------------Cx25838_Audio_I2S_Mode---------------------
//---- only for standard I2S mode
//---- I2S_Mode
//                0x1--Sony
//                0x0--Philips
//---- A_Port
//                0x0--Port A
//                0x1--Port B
//                0x2--Port C
//                0x3--Port D
//---- Master_Slave
//                0x1--Master
//                0x0--Slave
void Cx25838_Audio_I2S_Mode(int DEVICE,int A_Port, int I2S_Mode, int Master_Slave)
{
      int AUD_OFFSET = 0x4;
    RegMaskWrite32(DEVICE,AP_AUD_1_CFG+A_Port*AUD_OFFSET, 1, 1, Master_Slave);
    RegMaskWrite32(DEVICE,AP_AUD_1_CFG+A_Port*AUD_OFFSET, 0, 0, I2S_Mode);
}

//----------------------Cx25838_Audio_Cascade_Mode---------------------
//---- only for standard I2S mode
//---- Cascade_Mode
//                0x0--I2S
//                0x1--4:1
//                0x2--8:1
//----frame_sync_delay:
//                0x0--0bit
//                0x1--1bit
//                0x2--2bit
//---- frame_sync_width:
//                0x0--16bit
//                0x1--1bit

void Cx25838_Audio_Cascade_Mode(int DEVICE,int Cascade_Mode,int frame_sync_delay,int frame_sync_width)
{
    RegMaskWrite32(DEVICE,AP_AUD_GEN_CFG, 0, 1, Cascade_Mode); 
    RegMaskWrite32(DEVICE,AP_AUD_GEN_CFG, 4, 4, frame_sync_width); 
    RegMaskWrite32(DEVICE,AP_AUD_GEN_CFG, 8, 9, frame_sync_delay); 
}


//GPIO_num : start from 0;
//direction : 0--input; 1--output;
void Perseus_GPIO_SetDirection(int DEVICE, int GPIO_num, int direction)
{
    if ((GPIO_num < 0)||(GPIO_num > 49))
    {
        printk("invalid GPIO number");
        return;
    }
    else
    {    
        //disable the GPIO mux
        if (( GPIO_num >= 8 ) && ( GPIO_num<=16))
        {
            RegMaskWrite32(DEVICE,GPIO_MUX_CTRL,4,4,0x0);
        }
        if (( GPIO_num >= 17 ) && ( GPIO_num<=25))
        {
            RegMaskWrite32(DEVICE,GPIO_MUX_CTRL,5,5,0x0);
        }
        if (( GPIO_num >= 26 ) && ( GPIO_num<=34))
        {
            RegMaskWrite32(DEVICE,GPIO_MUX_CTRL,6,6,0x0);
        }
        if (( GPIO_num >= 35 ) && ( GPIO_num<=43))
        {
            RegMaskWrite32(DEVICE,GPIO_MUX_CTRL,7,7,0x0);
        }
        
        //set the direction
        if(GPIO_num<32)
        {
            RegMaskWrite32(DEVICE,GPIO_OE_LO,GPIO_num,GPIO_num,direction);
        }
        else
        {
            RegMaskWrite32(DEVICE,GPIO_OE_HI,GPIO_num,GPIO_num,direction);
        }
    }
}

void Perseus_GPIO_SetValue(int DEVICE, int GPIO_num, int value)
{
    if ((GPIO_num < 0)||(GPIO_num > 49))
    {
        printk("invalid GPIO number");
        return;
    }
    else
    {    
        //set the direction to output
        Perseus_GPIO_SetDirection(DEVICE, GPIO_num,0x1);
        
        //set the value
        if(GPIO_num<32)
        {
            RegMaskWrite32(DEVICE,GPIO_LO,GPIO_num,GPIO_num,value);
        }
        else
        {
            RegMaskWrite32(DEVICE,GPIO_HI,GPIO_num,GPIO_num,value);
        }
    }
}
