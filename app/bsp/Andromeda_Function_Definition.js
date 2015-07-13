#include "AndromedaRegistersDefine.js"

//int FREQ_DIV
//   0 ... 48MHz Clock Source
//   1 ... 24MHz Clock Source
void Andromeda_Colibri_Init(int DEVICE, int FREQ_DIV)
{
    // Step 1 - Setup clocks and tune analog filter
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_TUNE1, 0, 7, 0x41);
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_TUNE2, 0, 7, 0xF8);  // Ref_count=0x1F8, used by Atlas
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_XTAL, 0, 7, 0x0F);
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_PLL, 0, 7, 0x3F); 
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_PLL2, 0, 7, FREQ_DIV ?0x1E :0x0F);
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_PLL3, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_REF, 0, 7, 0x13); // default value (bit[4:3]ref_adc=0x2), Set ref_adc = 0x3 to get maximum input amplitude
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_PWRDN, 0, 7, 0x18);
    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_TUNE3, 0, 7, 0x40);

    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_TUNE1, 0, 7, 0x41);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_TUNE2, 0, 7, 0xF8);  // Ref_count=0x1F8, used by Atlas
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_XTAL, 0, 7, 0x0F);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_PLL, 0, 7, 0x3F);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_PLL2, 0, 7, FREQ_DIV ?0x1E :0x0F);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_PLL3, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_REF, 0, 7, 0x13); // default value (bit[4:3]ref_adc=0x2), Set ref_adc = 0x3 to get maximum input amplitude
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_PWRDN, 0, 7, 0x18);
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_TUNE3, 0, 7, 0x40);

    scripting.sleep(3);

    RegMaskWrite32(DEVICE,CX828_AFEA_SUP_BLK_TUNE3, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_ADC_INT5_STAB_REF, 0, 7, 0x52); // Set int5_boost = 0
    RegMaskWrite32(DEVICE,CX828_AFEB_SUP_BLK_TUNE3, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_ADC_INT5_STAB_REF, 0, 7, 0x52); // Set int5_boost = 0

    // Step 2 - power up all 8 channels, clear pd_buffer
    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_PWRDN_CLAMP, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_PWRDN_CLAMP, 0, 7, 0x00);

    // Step 3 - Enable channel calibration
    RegMaskWrite32(DEVICE,CX828_AFEA_ADC_COM_QUANT, 0, 7, 0x02);
    RegMaskWrite32(DEVICE,CX828_AFEB_ADC_COM_QUANT, 0, 7, 0x02);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_FB_FRCRST, 0, 7, 0x17);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_FB_FRCRST, 0, 7, 0x17);

    scripting.sleep(3);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_CAL_ATEST, 0, 7, 0x10);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_CAL_ATEST, 0, 7, 0x10);

    scripting.sleep(3);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_FB_FRCRST, 0, 7, 0x07);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_FB_FRCRST, 0, 7, 0x07);

    // Step 4 - Setup each channel as single-ended input
    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_NTF_PRECLMP_EN, 0, 7, 0xF0);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_INPUT, 0, 7, 0x00);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_INPUT, 0, 7, 0x00);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_DCSERVO_DEM, 0, 7, 0x03);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_DCSERVO_DEM, 0, 7, 0x03);

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_CTRL_DAC1, 0, 7, 0x05);  // default value (bit[1:0]dac23_gain=0x1). Set dac23_gain = 0x3 to get maximum input amplitude 

    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_CTRL_DAC23, 0, 7, 0x67);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_CTRL_DAC23, 0, 7, 0x67);

    // Disable for termination resistor and 100kOhm resistor to mid voltage: term_en to 0, res_en to 0
    RegMaskWrite32(DEVICE,CX828_AFEA_CH1_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH2_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH3_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEA_CH4_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH1_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH2_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH3_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    RegMaskWrite32(DEVICE,CX828_AFEB_CH4_ADC_QGAIN_RES_TRM, 0, 7, 0xf2);
    
    if(FREQ_DIV) {
        RegMaskWrite32(DEVICE,CX828_PLL8_INT_FRAC, 25, 30, 0x12); //108MHz for Video
        RegMaskWrite32(DEVICE,CX828_PLL9_INT_FRAC, 25, 30, 0x18); // 96MHz for Audio
    }
    RegMaskWrite32(DEVICE,CX828_PLL9_CTRL,0, 31, 0x00522516); // Power on PLL9, PLL9 frequency = 98.304MHz
}

void Andromeda_NTSC_Init(int DEVICE)
{
	int i;

	// Reset 8 decoders
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE, CX828_VDECA_SOFT_RST_CTRL+(0x200*i),  0, 31, 0x00008000); //Resets video decoder core, bit15=1
	scripting.sleep(5);
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE, CX828_VDECA_SOFT_RST_CTRL+(0x200*i),  0, 31, 0x00000000); 

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
        RegMaskWrite32(DEVICE, CX828_VDECA_MODE_CTRL +(0x200*i),  0, 31, 0x0004E001);  // Default value, vid_fmt_sel=4b0001  NTSC-M
		RegMaskWrite32(DEVICE, CX828_VDECA_DFE_CTRL2 +(0x200*i),  0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)
		RegMaskWrite32(DEVICE, CX828_VDECA_LUMA_CTRL +(0x200*i),  0, 31, 0x00108000);  // Set luma range = 1, default brightness/contrast
		RegMaskWrite32(DEVICE, CX828_VDECA_CHROMA_CTRL +(0x200*i),  0, 31, 0x21008080);  // Set c_ext_rng_en = 1(bit29), set c_core_sel = 1 to improve flat field SNR.
		
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL1 +(0x200*i),  0, 31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL_NS +(0x200*i),  0, 31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
        		
		RegMaskWrite32(DEVICE, CX828_VDECA_VCR_DET_CTRL +(0x200*i),  0, 31, 0xC0F00AB8); // field_phase_limit = 0xB
		RegMaskWrite32(DEVICE, CX828_VDECA_HTL_CTRL +(0x200*i),  0, 31, 0x00002040);     // Default Value
		RegMaskWrite32(DEVICE, CX828_VDECA_CRUSH_CTRL +(0x200*i),  0, 31, 0x001C1E0F); 
		
		RegMaskWrite32(DEVICE, CX828_VDECA_SC_STEP_SIZE +(0x200*i),  0, 31, 0x43E00000);  //Default value for NTSC, sc_step=0x43E00000
        RegMaskWrite32(DEVICE, CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i),  0, 31, 0x5E2D0074);//0x612D0074, Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074
		//RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x181E7014); // Default value for NTSC, v656blank=0x18,vactive=0x1e7, vblank=0x014 
		RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x1E1E001A); // Ray.C settings in Atlas was 0x1E1E001A

        RegMaskWrite32(DEVICE, CX828_VDECA_PLL_CTRL +(0x200*i),  0, 31, 0x161F1000); // Default Value	
  			
		//COMB FILTER SETTING - Default value
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0000012F); 
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E198080); 
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08);
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);
    
        ////COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA NTSC COMB FILTER SETTING
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0018012F);  // Changed lpf_bw_sel[5:4] from 0x1 to 0x2
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E192020); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);
				
		//Macrovision Setting for NTSC 
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL2 +(0x200*i),  0, 31, 0x5C450A00);  
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL3 +(0x200*i),  0, 31, 0x80000000); 
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL1 +(0x200*i),  0, 31, 0x00000A26); 		
	}
}

void Andromeda_PAL_Init(int DEVICE)
{
	int i;

	// Reset 8 decoders
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE, CX828_VDECA_SOFT_RST_CTRL+(0x200*i),  0, 31, 0x00008000); //Resets video decoder core
	scripting.sleep(5);
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE, CX828_VDECA_SOFT_RST_CTRL+(0x200*i),  0, 31, 0x00000000); 

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
		RegMaskWrite32(DEVICE, CX828_VDECA_MODE_CTRL +(0x200*i),  0, 31, 0x0004E004);  // Default value, vid_fmt_sel=4'b0100  PAL-BDGHI
		RegMaskWrite32(DEVICE, CX828_VDECA_DFE_CTRL2 +(0x200*i),  0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)
		RegMaskWrite32(DEVICE, CX828_VDECA_LUMA_CTRL +(0x200*i),  0, 31, 0x00108000);  // Set luma range = 1, default brightness/contrast
		RegMaskWrite32(DEVICE, CX828_VDECA_CHROMA_CTRL +(0x200*i),  0, 31, 0x21008080);  // Set c_ext_rng_en = 1(bit29). set c_core_sel = 1 to improve flat field SNR.
	
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL1 +(0x200*i),  0, 31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL_NS +(0x200*i),  0, 31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1

		RegMaskWrite32(DEVICE, CX828_VDECA_VCR_DET_CTRL +(0x200*i),  0, 31, 0xC0F00AB8); // field_phase_limit = 0xB, it make vcr_detect bit more reliable
		RegMaskWrite32(DEVICE, CX828_VDECA_HTL_CTRL +(0x200*i),  0, 31, 0x00002040);     // Default Value
		RegMaskWrite32(DEVICE, CX828_VDECA_CRUSH_CTRL +(0x200*i),  0, 31, 0x001C1E0F); 
		
		RegMaskWrite32(DEVICE, CX828_VDECA_SC_STEP_SIZE +(0x200*i),  0, 31, 0x5411E2D0);  // For PAL
		RegMaskWrite32(DEVICE, CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i),  0, 31, 0x672D007D); // From Ning Yang (0x632D007D)bgdel=0x5E for better Chroma/Luma Gain,
		RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x26240022); // For PAL, From Ning Yang
  	
        RegMaskWrite32(DEVICE, CX828_VDECA_PLL_CTRL +(0x200*i),  0, 31, 0x161F1000); // Default Value	
  	
		//COMB FILTER SETTING - Default value
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0000012F); 
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E198080);
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08); 
		//RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);

        //COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA PAL COMB FILTER SETTING
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0001015F);  //15F set luma lpf sel = 1 for dot in fountain
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E192020); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);
		    
        //Macrovision Setting for PAL is default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL2 +(0x200*i),  0, 31, 0x00000000);  //Default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL3 +(0x200*i),  0, 31, 0x00000000);  //Default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL1 +(0x200*i),  0, 31, 0x00000000);  //Default value
	}
}

void Andromeda_NTSC_Format(int DEVICE)
{
	int i;

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
        RegMaskWrite32(DEVICE, CX828_VDECA_MODE_CTRL +(0x200*i),  0, 31, 0x0004E001);  // Default value, vid_fmt_sel=4b0001  NTSC-M
		RegMaskWrite32(DEVICE, CX828_VDECA_DFE_CTRL2 +(0x200*i),  0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)

		RegMaskWrite32(DEVICE, CX828_VDECA_SC_STEP_SIZE +(0x200*i),  0, 31, 0x43E00000);  //Default value for NTSC, sc_step=0x43E00000
        RegMaskWrite32(DEVICE, CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i),  0, 31, 0x5E2D0074);//0x612D0074, Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074
		//RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x181E7014); // Default value for NTSC, v656blank=0x18,vactive=0x1e7, vblank=0x014 
		RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x1E1E001A); // Ray.C settings in Atlas was 0x1E1E001A

        ////COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA NTSC COMB FILTER SETTING
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0018012F);  // Changed lpf_bw_sel[5:4] from 0x1 to 0x2
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E192020); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);
				
		//Macrovision Setting for NTSC 
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL2 +(0x200*i),  0, 31, 0x5C450A00);  
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL3 +(0x200*i),  0, 31, 0x80000000); 
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL1 +(0x200*i),  0, 31, 0x00000A26); 		
	}
}

void Andromeda_PAL_Format(int DEVICE)
{
	int i;

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
		RegMaskWrite32(DEVICE, CX828_VDECA_MODE_CTRL +(0x200*i),  0, 31, 0x0004E004);  // Default value, vid_fmt_sel=4'b0100  PAL-BDGHI
		RegMaskWrite32(DEVICE, CX828_VDECA_DFE_CTRL2 +(0x200*i),  0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)

		RegMaskWrite32(DEVICE, CX828_VDECA_SC_STEP_SIZE +(0x200*i),  0, 31, 0x5411E2D0);  // For PAL
		RegMaskWrite32(DEVICE, CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i),  0, 31, 0x672D007D); // From Ning Yang (0x632D007D)bgdel=0x5E for better Chroma/Luma Gain,
		RegMaskWrite32(DEVICE, CX828_VDECA_VERT_TIM_CTRL +(0x200*i),  0, 31, 0x26240022); // For PAL, From Ning Yang

        //COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA PAL COMB FILTER SETTING
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),  0, 31, 0x0001015F);  //15F set luma lpf sel = 1 for dot in fountain
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),  0, 31, 0x1E192020); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),  0, 31, 0x080A0A08); 
		RegMaskWrite32(DEVICE, CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),  0, 31, 0x20102030);
		    
        //Macrovision Setting for PAL is default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL2 +(0x200*i),  0, 31, 0x00000000);  //Default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL3 +(0x200*i),  0, 31, 0x00000000);  //Default value
		RegMaskWrite32(DEVICE, CX828_VDECA_MV_DT_CTRL1 +(0x200*i),  0, 31, 0x00000000);  //Default value
	}
}

#if 0
void Andromeda_VideoDecoder_CIF_Output()
{
	int i;

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL1 +(0x200*i), 0x00110522);  // Default value, VIP 1.1, Enable insertion of ID in SAV/EAV
		RegMaskWrite32(DEVICE, CX828_VDECA_HSCALE_CTRL +(0x200*i), 0x00100000); // Low pass filter: CIF mode
		RegMaskWrite32(DEVICE, CX828_VDECA_VSCALE_CTRL +(0x200*i), 0x00090000); // Vertical Scaling Filter: 3-tap interpolation
	}
}	

void Andromeda_VideoDecoder_ID_Insertion() //Enable insertion of ID in SAV/EAV
{
	int i,temp;

	// Loop 8 times for 8 decoders
	for(i = 0; i <8; i++)
	{
		temp = Andromeda.ReadDefinedRegister(CX828_VDECA_OUT_CTRL1 +(0x200*i));  
		temp |= 0x00010000;
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL1 +(0x200*i), temp); 
		 
		temp = Andromeda.ReadDefinedRegister(CX828_VDECA_OUT_CTRL_NS +(0x200*i));  
		temp |= 0x00010000;
		RegMaskWrite32(DEVICE, CX828_VDECA_OUT_CTRL_NS +(0x200*i), temp);  
	}
}	
#endif
void Andromeda_LineLockedCameraSettings(int DEVICE)
{
    // Loop 8 times for 8 decoders
    int i;
    for(i = 0; i <8; i++)
    {
        RegMaskWrite32(DEVICE, CX828_VDECA_HTL_CTRL +(0x200*i), 0, 31, 0x00011020); // Set HTL_WIDE_EN bit to fix the horizontal line lock issue
        RegMaskWrite32(DEVICE, CX828_VDECA_PLL_CTRL +(0x200*i), 0, 31, 0x161FFFFF); // Set pll_max_offset	
    }
}

//----------------------Cx25828_omux_Enable---------------------
//---- Initialize port in Cx25838 to specified mode
//---- P_Port:
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- P_Mode:
//           0x0: Disable OMUX
//           0x1: Pass-through mode
//           0x2: 1 Streams Remux mode
//           0x3: 2 Streams Remux mode
//           0x4: 4 Streams Remux mode
//           0x5: 8 Streams Remux mode
//           Others ¨C Disable OMUX
//---- CLK_Pol
//            0x0 0 degree
//            0x1 180 degree
//---- CLK_Delay_stage_en
//            0--disable
//            1--enable
//---- CLK_Delay_stage
//            0--7 : 8 stages
//---- CLK_Phase
//            0x0: Non-delayed clock
//            0x1: Delay 0.5 cycle in 108MHz clock domain
//            0x2: Delay 1 cycle in 108MHz clock domain
//            0x3: Delay 1.5 cycle in 108MHz clock domain
//            0x4: Delay 2 cycle in 108MHz clock domain
//            0x5: Delay 2.5 cycle in 108MHz clock domain
//            0x6: Delay 3 cycle in 108MHz clock domain
//            0x7: Delay 3.5 cycle in 108MHz clock domain
void Cx25828_Omux_Enable(int DEVICE, int P_Port, int P_Mode, int CLK_Pol,int CLK_Delay_stage_en,int CLK_Delay_stage,int CLK_Phase)
{
    int PORT_OFFSET = 0x4;
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 4, 6, P_Mode);
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 17, 17, CLK_Pol); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 20, 22, CLK_Phase); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 13, 15, CLK_Delay_stage); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 7, 7, CLK_Delay_stage_en); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 18, 19, 0x3);        //enable this port 
}

//----------------------Cx25828_Omux_bypass_out---------------------
//---- Initialize port in Cx25838 to specified mode
//---- P_Port:
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- data_select:
//            0x0: Video Decoder A Scaled
//            0x1: Video Decoder A Non-Scaled
//            0x2: Video Decoder B Scaled
//            0x3: Video Decoder B Non-Scaled
//            0x4: Video Decoder C Scaled
//            0x5: Video Decoder C Non-Scaled
//            0x6: Video Decoder D Scaled
//            0x7: Video Decoder D Non-Scaled
//            0x8: Video Decoder E Scaled
//            0x9: Video Decoder E Non-Scaled
//            0xA: Video Decoder F Scaled
//            0xB: Video Decoder F Non-Scaled
//            0xC: Video Decoder G Scaled
//            0xD: Video Decoder G Non-Scaled
//            0xE: Video Decoder H Scaled
//            0xF: Video Decoder H Non-Scaled
//            0x10: Video Demux A, ID 1
//            0x11: Video Demux A, ID 2
//            0x12: Video Demux A, ID 3
//            0x13: Video Demux A, ID 4
//            0x14: Video Demux B, ID 1
//            0x15: Video Demux B, ID 2
//            0x16: Video Demux B, ID 3
//            0x17: Video Demux B, ID 4
//            0x18: Video Demux C, ID 1
//            0x19: Video Demux C, ID 2
//            0x1A: Video Demux C, ID 3
//            0x1B: Video Demux C, ID 4
//            0x1C: Video Demux D, ID 1
//            0x1D: Video Demux D, ID 2
//            0x1E: Video Demux D, ID 3
//            0x1F: Video Demux D, ID 4
void Cx25828_Omux_bypass_out(int DEVICE, int P_Port, int data_select)
{
    int PORT_OFFSET = 0x4;
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 8, 12, data_select);
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 0, 0, 0x0); //disable remux void
}

//----------------------Cx25828_Omux_remux_out---------------------
//---- Initialize port in Cx25838 to specified mode
//---- P_Port:
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- D1_CIF:
//            0--D1
//            1--CIF
//----- Pack_sel:
//            0--Group low
//            1--Group high
void Cx25828_Omux_remux_out(int DEVICE, int P_Port, int D1_CIF, int Pack_sel)
{
    int PORT_OFFSET = 0x4;
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 2, 2, D1_CIF);
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 1, 1, Pack_sel);
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A +(PORT_OFFSET*P_Port), 0, 0, 0x1); //enable remux void
}




//----------------------Cx25828_Demux_Enable---------------------
//---- Initialize port in Cx25838 to specified mode
//---- P_Port:
//            0--port A
//            1--port B
//            2--port C
//            3--port D
//---- P_Mode:
//           0x0: Single Stream
//           0x1: Two Streams
//           0x2: Reserved
//           0x3: Four Streams
//---- CLK_Pol
//            0x0 0 degree
//            0x1 180 degree
//---- CLK_Delay_stage_en
//            0--disable
//            1--enable
//---- CLK_Delay_stage
//            0--7 : 8 stages
//---- dvp_id_x : 1--4
void Cx25828_Demux_Enable(int DEVICE, int P_Port, int P_Mode, int CLK_Pol,int CLK_Delay_stage_en,int CLK_Delay_stage,int dvp_id_1,int dvp_id_2,int dvp_id_3,int dvp_id_4)
{
    int PORT_OFFSET = 0x4;
    RegMaskWrite32(DEVICE, CX828_HOST_REG1, 0,31, 0x00000044);  // Enables VP input pads
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 4, 5, P_Mode);
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 1, 1, CLK_Pol); 
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 9, 11, CLK_Delay_stage); 
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 7, 7, CLK_Delay_stage_en); 
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 16, 19, dvp_id_1);   
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 20, 23, dvp_id_2);   
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 24, 27, dvp_id_3); 
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 28, 31, dvp_id_4);  
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 0, 0, 0x1);        //enable this port 
}
void Cx25828_Demux_disable(int DEVICE, int P_Port)
{
	int PORT_OFFSET = 0x4;
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A +(PORT_OFFSET*P_Port), 0, 0, 0x0);        //disable this port 
}
//----------------------Cx25828_video_pack---------------------
//---- Initialize port in Cx25838 to specified mode
//---- vpe_sel:
//            0--VPE 0
//            1--VPE 1
//            2--VPE 2
//            3--VPE 3
//            4--VPE 4
//            5--VPE 5
//            6--VPE 6
//            7--VPE 7
//            8--VPE 8
//            9--VPE 9
//            A--VPE 10
//            B--VPE 11
//            C--VPE 12
//            D--VPE 13
//            E--VPE 14
//            F--VPE 15
//---- src_sel:
//            0x0: Video Decoder A Scaled
//            0x1: Video Decoder A Non-Scaled
//            0x2: Video Decoder B Scaled
//            0x3: Video Decoder B Non-Scaled
//            0x4: Video Decoder C Scaled
//            0x5: Video Decoder C Non-Scaled
//            0x6: Video Decoder D Scaled
//            0x7: Video Decoder D Non-Scaled
//            0x8: Video Decoder E Scaled
//            0x9: Video Decoder E Non-Scaled
//            0xA: Video Decoder F Scaled
//            0xB: Video Decoder F Non-Scaled
//            0xC: Video Decoder G Scaled
//            0xD: Video Decoder G Non-Scaled
//            0xE: Video Decoder H Scaled
//            0xF: Video Decoder H Non-Scaled
//            0x10: Video Demux A, ID 1
//            0x11: Video Demux A, ID 2
//            0x12: Video Demux A, ID 3
//            0x13: Video Demux A, ID 4
//            0x14: Video Demux B, ID 1
//            0x15: Video Demux B, ID 2
//            0x16: Video Demux B, ID 3
//            0x17: Video Demux B, ID 4
//            0x18: Video Demux C, ID 1
//            0x19: Video Demux C, ID 2
//            0x1A: Video Demux C, ID 3
//            0x1B: Video Demux C, ID 4
//            0x1C: Video Demux D, ID 1
//            0x1D: Video Demux D, ID 2
//            0x1E: Video Demux D, ID 3
//            0x1F: Video Demux D, ID 4
//----- vpe_id:
//            0x0 to 0xf
//----- id_len:
//            0x0: Do NOT replace stream ID,
//            0x1: Replace 2bit stream ID
//            0x2: Replace 3bit stream ID
//            0x3: Replace 4bit stream ID
//----- strip_en
//           1--enable
//           0--disable
void Cx25828_video_pack_en(int DEVICE, int vpe_sel, int src_sel, int vpe_id, int id_len, int strip_en)
{
    int PORT_OFFSET = 0x8;
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 8, 12, src_sel);
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 4, 7, vpe_id);
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 2, 3, id_len);
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 1, 1, strip_en);
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 0, 0, 0x1); //enable vpe void
}


void Cx25828_video_pack_disable(int DEVICE, int vpe_sel, int P_Port)
{
    int PORT_OFFSET = 0x8;
    RegMaskWrite32(DEVICE, CX828_VP_VPE_0_A +(PORT_OFFSET*vpe_sel), 0, 0, 0x0); //disable vpe void
}

//----------------------Cx25828_bigframe_port_en---------------------
//---- Initialize port in Cx25838 to specified mode
//---- CLK_Pol
//            0x0 0 degree
//            0x1 180 degree
//---- CLK_Delay_stage_en
//            0--disable
//            1--enable
//---- CLK_Delay_stage
//            0--7 : 8 stages
void Cx25828_bigframe_port_en(int DEVICE, int CLK_Pol,int CLK_Delay_stage_en,int CLK_Delay_stage)
{
    RegMaskWrite32(DEVICE, CX828_HOST_REG1, 0, 7, 0x44);//POWER ON PAD
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A, 0, 0, 0x1);//enable port A demux
    RegMaskWrite32(DEVICE, CX828_VP_DEMUX_A, 4, 5, 0x0);//single stream
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A, 13, 15, CLK_Delay_stage); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_A, 7, 7, CLK_Delay_stage_en); 
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_E, 19,19,0x1);//enable data
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_E, 18,18,0x1);//enable clk
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_E, 17,17,CLK_Pol);//clock pol
    RegMaskWrite32(DEVICE, CX828_VP_OMUX_E, 0,0,0x1);//enable this void
}

void AndromedaDecoder_NTSC_Init(int DEVICE)
{
	int i;
	
	// Reset 8 decoders
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE,CX828_VDECA_SOFT_RST_CTRL+(0x200*i), 0, 31, 0x00008000); //Resets video decoder core, bit15=1
	scripting.sleep(20);
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE,CX828_VDECA_SOFT_RST_CTRL+(0x200*i), 0, 31, 0x00000000); 

	// Don't loop...
	for(i = 0; i <8; i++)
	{
		//Andromeda.registers.registers.WriteDefinedRegister(DC_CTRL_A +(0xC*i), 0, 31, 0x00200206);  // Default value
		//RegMaskWrite32(DEVICE,VID_A_PLL_CTRL +(0x8*i), 0, 31, 0x00422714);  // Default value
		//RegMaskWrite32(DEVICE,VID_A_PLL_INT_FRAC +(0x8*i), 0, 31, 0x0F2BE2FE);  // Default value

		RegMaskWrite32(DEVICE,CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x0004E001);  // Default value, vid_fmt_sel=4b0001  NTSC-M
		RegMaskWrite32(DEVICE,CX828_VDECA_DFE_CTRL2 +(0x200*i), 0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)
		RegMaskWrite32(DEVICE,CX828_VDECA_LUMA_CTRL +(0x200*i), 0, 31, 0x00108000);  // Set luma range = 1
		RegMaskWrite32(DEVICE,CX828_VDECA_CHROMA_CTRL +(0x200*i), 0, 31, 0x21008080);  // Set c_ext_rng_en = 1(bit29), set c_core_sel = 1 to improve flat field SNR.
		
		RegMaskWrite32(DEVICE,CX828_VDECA_VCR_DET_CTRL +(0x200*i), 0, 31, 0xC0F00AB8); // field_phase_limit = 0xB
		RegMaskWrite32(DEVICE,CX828_VDECA_HTL_CTRL +(0x200*i), 0, 31, 0x00002040); 
		RegMaskWrite32(DEVICE,CX828_VDECA_CRUSH_CTRL +(0x200*i), 0, 31, 0x001C1E0F); 
		
		RegMaskWrite32(DEVICE,CX828_VDECA_SC_STEP_SIZE +(0x200*i), 0, 31, 0x43E00000);  //Default value for NTSC, sc_step=0x43E00000
		RegMaskWrite32(DEVICE,CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i), 0, 31, 0x5e2D0074);//0x612D0074, Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074
		RegMaskWrite32(DEVICE,CX828_VDECA_VERT_TIM_CTRL +(0x200*i), 0, 31, 0x1E1E001A); // 0x1C1E7014Default value for NTSC, v656blank=0x18,vactive=0x1e7, vblank=0x014

        RegMaskWrite32(DEVICE,CX828_VDECA_PLL_CTRL +(0x200*i),0,31, 0x161F1000); // Default Value	
  			
		//COMB FILTER SETTING - Default value
		//RegMaskWrite32(DEVICE, VDECA_COMB_FILT_CTRL1  +(0x200*i), 0x0000012F); 
		//RegMaskWrite32(DEVICE, VDECA_COMB_FILT_CTRL2  +(0x200*i), 0x1E198080); 
		//RegMaskWrite32(DEVICE, VDECA_COMB_FILT_CTRL3  +(0x200*i), 0x080A0A08);
		//RegMaskWrite32(DEVICE, VDECA_COMB_FILT_CTRL4  +(0x200*i), 0x20102030);
    
        ////COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA NTSC COMB FILTER SETTING
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i),0,31, 0x0018012F);  // Changed lpf_bw_sel[5:4] from 0x1 to 0x2
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i),0,31, 0x1E192020); 
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i),0,31, 0x080A0A08); 
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i),0,31, 0x20102030);
 
 		// Macrovision Setting 
       	RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL1 +(0x200*i), 0, 31, 0x00000A26);  // Needed for NTSC Macrovision
	   	RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL2 +(0x200*i), 0, 31, 0x5C450A00);  // Needed for NTSC Macrovision
       	RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL3 +(0x200*i), 0, 31, 0x80000000);  // Needed for NTSC Macrovision  		
		//to enable 12dB auto gain
        RegMaskWrite32(DEVICE,CX828_VDECA_C2RB_DC_CTRL +(0x200*i),0,31, 0x00200210);
        RegMaskWrite32(DEVICE,CX828_VDECA_DFE_CTRL2 +(0x200*i),0,31, 0x401060DC);
        RegMaskWrite32(DEVICE,CX828_VDECA_COL_GAIN_CTRL +(0x200*i),0,31, 0x00000002);

		// Output setting
      	RegMaskWrite32(DEVICE,CX828_VDECA_OUT_CTRL1 +(0x200*i), 0, 31, 0x001006b2);    // vip1.1 mode, id_ins_extend_en = 0x1
       	RegMaskWrite32(DEVICE,CX828_VDECA_OUT_CTRL_NS +(0x200*i), 0, 31, 0x001006b2);  // vip1.1 mode, id_ins_extend_en = 0x1
	}
}

void AndromedaDecoder_PAL_Init(int DEVICE)
{
	int i;
	
	// Reset the decoder. Don't loop...
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE,CX828_VDECA_SOFT_RST_CTRL+(0x200*i), 0, 31, 0x00008000); //Resets video decoder core, bit15=1
	scripting.sleep(20);
	for(i = 0; i <8; i++)  RegMaskWrite32(DEVICE,CX828_VDECA_SOFT_RST_CTRL+(0x200*i), 0, 31, 0x00000000); 

	// Don't loop...
	for(i = 0; i <8; i++)
	{
		RegMaskWrite32(DEVICE,CX828_VDECA_MODE_CTRL +(0x200*i), 0, 31, 0x0004E004);  // Default value, vid_fmt_sel=4'b0100  PAL-BDGHI
		RegMaskWrite32(DEVICE,CX828_VDECA_DFE_CTRL2 +(0x200*i), 0, 31, 0x401040DC);  // Default value, and disable clamp gating(1)
		RegMaskWrite32(DEVICE,CX828_VDECA_LUMA_CTRL +(0x200*i), 0, 31, 0x00108000);  // Set luma range = 1
		RegMaskWrite32(DEVICE,CX828_VDECA_CHROMA_CTRL +(0x200*i), 0, 31, 0x21008080);  // Set c_ext_rng_en = 1(bit29, set c_core_sel = 1 to improve flat field SNR.
	
		RegMaskWrite32(DEVICE,CX828_VDECA_OUT_CTRL1 +(0x200*i),0,31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1
		RegMaskWrite32(DEVICE,CX828_VDECA_OUT_CTRL_NS +(0x200*i),0,31, 0x00100632);  // Default value, vip1.1 mode, id_ins_extend_en = 0x1


		RegMaskWrite32(DEVICE,CX828_VDECA_VCR_DET_CTRL +(0x200*i), 0, 31, 0xC0F00AB8); // field_phase_limit = 0xB, it make vcr_detect bit more reliable
		RegMaskWrite32(DEVICE,CX828_VDECA_HTL_CTRL +(0x200*i), 0, 31, 0x00002040); 
		RegMaskWrite32(DEVICE,CX828_VDECA_CRUSH_CTRL +(0x200*i), 0, 31, 0x001C1E0F); 
		
		RegMaskWrite32(DEVICE,CX828_VDECA_SC_STEP_SIZE +(0x200*i), 0, 31, 0x5411E2D0);  //Default value for NTSC, sc_step=0x43E00000
		RegMaskWrite32(DEVICE,CX828_VDECA_HORIZ_TIM_CTRL +(0x200*i), 0, 31, 0x672D007D); // From Ning Yang (0x632D007D)bgdel=0x61 for better Chroma/Luma Gain,
		RegMaskWrite32(DEVICE,CX828_VDECA_VERT_TIM_CTRL +(0x200*i), 0, 31, 0x26240022); // From Ning Yang  		
		//RegMaskWrite32(DEVICE,CX828_VDECA_VERT_TIM_CTRL +(0x200*i), 0, 31, 0x28240026); // Default value for NTSC, v656blank=0x26,vactive=0x240, vblank=0x022
 		
	
        //COMB FILTER SETTING - Ray.C Taken from Dongsheng optimized FPGA PAL COMB FILTER SETTING
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL1  +(0x200*i), 0,31,0x0001015F);  //15F set luma lpf sel = 1 for dot in fountain
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL2  +(0x200*i), 0,31,0x1E192020); 
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL3  +(0x200*i), 0,31,0x080A0A08); 
		RegMaskWrite32(DEVICE,CX828_VDECA_COMB_FILT_CTRL4  +(0x200*i), 0,31,0x20102030);
		    
		//to enable 12dB auto gain
        RegMaskWrite32(DEVICE,CX828_VDECA_C2RB_DC_CTRL +(0x200*i),0,31, 0x00200210);
        RegMaskWrite32(DEVICE,CX828_VDECA_DFE_CTRL2 +(0x200*i),0,31, 0x401060DC);
        RegMaskWrite32(DEVICE,CX828_VDECA_COL_GAIN_CTRL +(0x200*i),0,31, 0x00000002);
    
        //Macrovision Setting for PAL is default value
        RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL2 +(0x200*i), 0,31,0x00000000);  //Default value
        RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL3 +(0x200*i), 0,31,0x00000000);  //Default value
        RegMaskWrite32(DEVICE,CX828_VDECA_MV_DT_CTRL1 +(0x200*i), 0,31,0x00000000);  //Default value
	}
}

//----------------------Cx25828_Audio_AADC_Init---------------------
//---- Initialize AADC PLL and analog part in Cx25838
//---- The first step of all local AADC operations
//---- FsRate:(uniform fs rate to following settings)
//       0x0-8K
//       0x1-16K
//       0x2-32K
//       0x4-48K
//       0x5-44.1K

void Cx25828_Audio_AADC_Init(int DEVICE, int FsRate)
{
    //sample rate change
    if (FsRate == 0x0) //8K
		RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 20,22, 0x3);
	else if (FsRate == 0x1) //16K
		     RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 20,22, 0x2);
		   else if (FsRate == 0x2) //32K
		   	    RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 20,22, 0x1);
		   	    else if (FsRate == 0x4) //48K
		   	    	RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 20,22, 0x0);
		   	    	  else //44.1K
		   	    	  	RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 20,22, 0x4);

    RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL1, 0,17, 0x11111); //enable all channels
    RegMaskWrite32(DEVICE,CX828_AADC_DIG_CTRL2, 11,12, 0x0); //gain = -4db
    RegMaskWrite32(DEVICE,CX828_AADC_ANA_CTRL1, 0, 31, 0xF0005555); // Power on all ADCs, DSM dither enable,1dB
    RegMaskWrite32(DEVICE,CX828_AADC_ANA_CTRL2, 0, 31, 0x00000000); // Default value
    RegMaskWrite32(DEVICE,CX828_AADC_ANA_CTRL3, 0, 31, 0x00000000); // Default value
    RegMaskWrite32(DEVICE,CX828_AADC_ANA_CTRL4, 0, 31, 0x00000002); // Power on ADC 9, GAIN = 1dB
}

//----------------------Cx25828_Audio_ADAC_Init---------------------
//---- Initialize ADAC PLL and Analog part in Cx25838
//---- The first step of all local ADAC operations
//---- FsRate:(uniform fs rate to following settings)
//       0x0-8K
//       0x1-16K
//       0x2-32K
//       0x4-48K
//       0x5-44.1K
//---- Aout
//     00000-audio adc 1
//     00001-audio adc 2
//     00010-audio adc 3
//     00011-audio adc 4
//     00100-audio adc 5
//     00101-audio adc 6
//     00110-audio adc 7
//     00111-audio adc 8
//     01000-audio adc 9
//     01001-audio_mixed
//     01010-audio_pb
//     01011-cascade data 0
//     01100-cascade data 1
//     01101-cascade data 2
//     01110-cascade data 3
//     01111-cascade data 4
//     10000-cascade data 5
//     10001-cascade data 6
//     10010-cascade data 7
//     10011-cascade data 8
//     10100-cascade data 9
//     10101-cascade data 10
//     10110-cascade data 11
//     10111-cascade data 12
//     11000-cascade data 13
//     11001-cascade data 14
//     11010-cascade data 15
//     11011-cascade data 16
//     11100-cascade data 17
//     11101-cascade data 18
//     11110-cascade data 19
//     11111-cascade data 20
//---- Gain_En:
//       0x0--disable the output gain
//       0x1--enable the output gain
//---- Gain:
//       Audio gain for adac output. 2 integer and 4 fractional part, 
//       0~3.9375 with resolution 0.0625. default 1.0
void Cx25828_Audio_ADAC_Init(int DEVICE, int FsRate, int Aout, int Gain_En, int Gain)
{
	RegMaskWrite32(DEVICE, CX828_ADAC_CTRL0, 0, 31, 0x230); //power up the ADAC
	RegMaskWrite32(DEVICE, CX828_AP_CFG2, 4, 8, Aout);//choose the source to ADAC
	RegMaskWrite32(DEVICE, CX828_AP_CFG3, 2, 2, Gain_En);
	RegMaskWrite32(DEVICE, CX828_AP_CFG3, 17, 17, Gain_En);
	RegMaskWrite32(DEVICE, CX828_AUD_GAIN2, 8, 13, Gain);
}

//----------------------Cx25828_Audio_Rec_Cfg---------------------
//---- Config the Record port of Cx25828
//---- FsRate:(uniform fs rate to following settings)
//       0x0-8K
//       0x1-16K
//       0x2-32K
//       0x4-48K
//       0x5-44.1K
//---- ChNum:
//       0x0-2 channel
//       0x1-4 channel
//       0x2-8 channel
//       0x3-16 channel
//       0x4-24 channel
//---- I2S_PCM:
//       0x0-I2S
//       0x1-PCM
//---- Master_Slave:
//       0x0-Slave
//       0x1-Master
//---- DataWidth:
//       0x0-8bit
//       0x1-16bit
//       0x2-24bit
//---- BitShift:
//       0x0-0bit
//       0x1-1bit
//       0x2-2bit

void Cx25828_Audio_Rec_Cfg(int DEVICE, int FsRate, int ChNum, int I2S_PCM, int Master_Slave, int DataWidth, int BitShift)
{
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x0);

    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 12, 13, I2S_PCM);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 8, 9, BitShift);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 4, 6, ChNum);
    
    if (DataWidth == 0x0)//8bit
        RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0x0);//set to 8bit
    else if (DataWidth == 0x1)//16bit
           RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0x5);//set to 16bit
         else if (DataWidth == 0x2)//24bit
         	  RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0xa);//set to 24bit
         	  
         	  
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 20, 20, 0x1);//msb
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 19, 19, Master_Slave);
    
    if (Master_Slave == 0x0)
        RegMaskWrite32(DEVICE, CX828_AP_CFG1, 18, 18, 0x0);//wclk generated externally
    else
    	RegMaskWrite32(DEVICE, CX828_AP_CFG1, 18, 18, 0x1);//wclk generated internally
    	
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 16, 17, 0x0);//wclk&bclk polarity
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 18, 18, 0x0);//wclk no delay
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 11, 11, 0x0);//left adjusted
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 10, 10, 0x0);//no encoded
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 0, 0, 0x1);//sdat output
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 20, 23, 0x1);//12.288
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 18, 18, 0x1);//bclk calcuated by bit_width,fs_rate,and ch_num
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 13, 13, 0x0);//normal mode

    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 28, 30, FsRate);
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x1);
}

//----------------------Cx25828_Audio_Rec_Rmap---------------------
//---- map the Record port of Cx25828
//---- ChOrder: 
//     0x0(ch1) to 0x17
//---- Rmap:
//     00000-audio adc 1
//     00001-audio adc 2
//     00010-audio adc 3
//     00011-audio adc 4
//     00100-audio adc 5
//     00101-audio adc 6
//     00110-audio adc 7
//     00111-audio adc 8
//     01000-audio adc 9
//     01001-audio_mixed
//     01010-audio_pb
//     01011-cascade data 0
//     01100-cascade data 1
//     01101-cascade data 2
//     01110-cascade data 3
//     01111-cascade data 4
//     10000-cascade data 5
//     10001-cascade data 6
//     10010-cascade data 7
//     10011-cascade data 8
//     10100-cascade data 9
//     10101-cascade data 10
//     10110-cascade data 11
//     10111-cascade data 12
//     11000-cascade data 13
//     11001-cascade data 14
//     11010-cascade data 15
//     11011-cascade data 16
//     11100-cascade data 17
//     11101-cascade data 18
//     11110-cascade data 19
//     11111-cascade data 20
void Cx25828_Audio_Rec_Rmap(int DEVICE, int ChOrder, int Rmap)
{
	int Reg_Addr = CX828_AP_RMAP0 + 0x4*(ChOrder/6);//to the register address
	int Bit_Addr_Lo = (ChOrder - 0x6*(ChOrder/6))*0x5;
	int Bit_Addr_Hi = Bit_Addr_Lo + 0x4;
	
	RegMaskWrite32(DEVICE, Reg_Addr, Bit_Addr_Lo, Bit_Addr_Hi, Rmap);
}


//----------------------Cx25828_Audio_Mix_Cfg---------------------
//---- Config the Mix port of Cx25828
//---- FsRate:(uniform fs rate to following settings)
//       0x0-8K
//       0x1-16K
//       0x2-32K
//       0x4-48K
//       0x5-44.1K
//---- ChNum:
//       0x0-2 channel
//       0x1-4 channel
//       0x2-8 channel
//       0x3-16 channel
//       0x4-24 channel
//---- I2S_PCM:
//       0x0-I2S
//       0x1-PCM
//---- Master_Slave:
//       0x0-Slave
//       0x1-Master
//---- DataWidth:
//       0x0-8bit
//       0x1-16bit
//       0x2-24bit
//---- BitShift:
//       0x0-0bit
//       0x1-1bit
//       0x2-2bit

void Cx25828_Audio_Mix_Cfg(int DEVICE, int FsRate, int ChNum, int I2S_PCM, int Master_Slave, int DataWidth, int BitShift)
{
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x0);

    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 12, 13, I2S_PCM);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 10, 11, BitShift);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 4, 6, ChNum);
    
    if (DataWidth == 0x0)//8bit
    {
        RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0x0);//set to 8bit
        RegMaskWrite32(DEVICE, CX828_AP_CFG1, 10, 11, 0x0);//set to 8bit
    }
    else if (DataWidth == 0x1)//16bit
        {
            RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0x5);//set to 16bit
            RegMaskWrite32(DEVICE, CX828_AP_CFG1, 10, 11, 0x1);//set to 16bit
        }
        else if (DataWidth == 0x2)//24bit
            {
                RegMaskWrite32(DEVICE, CX828_AP_CFG0, 0, 3, 0xa);//set to 24bit
                RegMaskWrite32(DEVICE, CX828_AP_CFG1, 10, 11, 0x2);//set to 24bit
            }
         	  
         	  
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 12, 12, 0x1);//msb
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 19, 19, Master_Slave);
    if (Master_Slave == 0x0)
        RegMaskWrite32(DEVICE, CX828_AP_CFG1, 18, 18, 0x0);//wclk generated externally
    else
    	RegMaskWrite32(DEVICE, CX828_AP_CFG1, 18, 18, 0x1);//wclk generated internally
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 16, 17, 0x0);//wclk&bclk polarity
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 18, 18, 0x0);//wclk no delay
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 14, 14, 0x0);//left adjusted
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 13, 13, 0x0);//no encoded
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 1, 1, 0x1);//sdat output
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 20, 23, 0x1);//12.288
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 18, 18, 0x1);//bclk calcuated by bit_width,fs_rate,and ch_num
    RegMaskWrite32(DEVICE, CX828_AP_CFG3, 13, 13, 0x0);//normal mode

    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 28, 30, FsRate);
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x1);
}

//----------------------Cx25828_Audio_Mix_Rmap---------------------
//---- map the Mix port of Cx25828
//---- ChOrder: 
//     0x0(ch1) to 0x17
//---- Mmap:
//     00000-audio adc 1
//     00001-audio adc 2
//     00010-audio adc 3
//     00011-audio adc 4
//     00100-audio adc 5
//     00101-audio adc 6
//     00110-audio adc 7
//     00111-audio adc 8
//     01000-audio adc 9
//     01001-audio_mixed
//     01010-audio_pb
//     01011-cascade data 0
//     01100-cascade data 1
//     01101-cascade data 2
//     01110-cascade data 3
//     01111-cascade data 4
//     10000-cascade data 5
//     10001-cascade data 6
//     10010-cascade data 7
//     10011-cascade data 8
//     10100-cascade data 9
//     10101-cascade data 10
//     10110-cascade data 11
//     10111-cascade data 12
//     11000-cascade data 13
//     11001-cascade data 14
//     11010-cascade data 15
//     11011-cascade data 16
//     11100-cascade data 17
//     11101-cascade data 18
//     11110-cascade data 19
//     11111-cascade data 20
void Cx25828_Audio_Mix_Mmap(int DEVICE, int ChOrder, int Mmap)
{
	int Reg_Addr = CX828_AP_MMAP0 + 0x4*(ChOrder/6);//to the register address
	int Bit_Addr_Lo = (ChOrder - 0x6*(ChOrder/6))*0x5;
	int Bit_Addr_Hi = Bit_Addr_Lo + 0x4;
	
	RegMaskWrite32(DEVICE, Reg_Addr, Bit_Addr_Lo, Bit_Addr_Hi, Mmap);
}


//----------------------Cx25828_Audio_PB_Cfg---------------------
//---- Config the playback port of Cx25828
//---- FsRate:(uniform fs rate to following settings)
//       0x0-8K
//       0x1-16K
//       0x2-32K
//       0x4-48K
//       0x5-44.1K
//---- I2S_PCM:
//       0x0-I2S
//       0x1-PCM
//---- Master_Slave:
//       0x0-Slave
//       0x1-Master
//---- DataWidth:
//       0x0-8bit
//       0x1-16bit
//       0x2-24bit
//---- BitShift:
//       0x0-0bit
//       0x1-1bit
//       0x2-2bit
//---- LRCh
//       0x0-Right Channel
//       0x1-Left Channel

void Cx25828_Audio_PB_Cfg(int DEVICE, int FsRate, int LRCh, int I2S_PCM, int Master_Slave, int DataWidth, int BitShift)
{
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x0);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 29, 29, I2S_PCM);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 24, 25, BitShift);
    RegMaskWrite32(DEVICE, CX828_AP_CFG0, 20, 22, 0x0);  //2channels
    
    if (DataWidth == 0x0)//8bit
        RegMaskWrite32(DEVICE, CX828_AP_CFG0, 16, 19, 0x0);//set to 8bit
    else if (DataWidth == 0x1)//16bit
            RegMaskWrite32(DEVICE, CX828_AP_CFG0, 16, 19, 0x5);//set to 16bit
         else if (DataWidth == 0x2)//24bit
         	  RegMaskWrite32(DEVICE, CX828_AP_CFG0, 16, 19, 0xa);//set to 24bit
         	  
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 25, 25, LRCh);  //choose left channel
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 24, 24, 0x1);  //MSB
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 23, 23, Master_Slave);
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 22, 22, 0x0);  //wclk polarity
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 21, 21, 0x0);  //bclk polarity

    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 28, 30, FsRate);  //SET FREQ TO 8KHz
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 20, 20, 0x0);  //sdat delay = 0
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 19, 19, 0x0);  //wclk delay = 0
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 17, 17, 0x0);  //left aligned
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 16, 16, 0x0);  //no encoded

    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x1);
}


//----------------------Cx25828_Audio_Mixer_En---------------------
//---- Enable the Mixer of Cx25828
//---- Mixer_Gain:
//       Mixer result shift bit:
//       000-no gain
//       001-/2
//       010-/4
//       011-/8
//       100-*2
//       101-*4
//       110-*8

void Cx25828_Audio_Mixer_Cfg(int DEVICE, int Mixer_Gain)
{
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x0);
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 24, 24, 0x1);//enable mixer
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 0, 2, Mixer_Gain);//enable mixer
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x1);
}

//----------------------Cx25828_Audio_Mixer_Ratio---------------------
//---- set the ratio for each channels
//---- ChOrder: 
//     0x0(ch1) to 0xa(ch11)
//---- Ratio:
//Audio mixer ratio
//2 integer bits, 4 fractional bits, 0-3.9375 with step of 0.0625, default is 0.125.
void Cx25828_Audio_Mixer_Ratio(int DEVICE, int ChOrder, int Ratio)
{
	int Reg_Addr = CX828_AP_RATIO0 + 0x4*(ChOrder/4);//to the register address
	int Bit_Addr_Lo = (ChOrder - 0x4*(ChOrder/4))*0x8;
	int Bit_Addr_Hi = Bit_Addr_Lo + 0x5;
	RegMaskWrite32(DEVICE, Reg_Addr, Bit_Addr_Lo, Bit_Addr_Hi, Ratio);
}

//----------------------Cx25828_Audio_MCLK_En---------------------
//---- set the MCLK output for
//---- Multi:
//     0x0: 256*fs
//     0x1: 512*fs
void Cx25828_Audio_MCLK_En(int DEVICE, int Multi)
{
	RegMaskWrite32(DEVICE,CX828_AP_CFG3,4,4,Multi);
	RegMaskWrite32(DEVICE,CX828_AP_CFG3,11,11,0x1); //polarity
	RegMaskWrite32(DEVICE,CX828_AP_CFG2,25,25,0x1); //enable
}
//----------------------Cx25828_Audio_MCLK_Disable---------------------
//---- disable the MCLK output for
void Cx25828_Audio_MCLK_Disable(int DEVICE)
{
	RegMaskWrite32(DEVICE,CX828_AP_CFG2,25,25,0x0); //disable
}


//----------------------Cx25828_Audio_Cas_Cfg---------------------
//---- Config the Cascade port of Cx25828
//---- Cas_OE:
//       0x0-input
//       0x1-output
//---- DataWidth:
//       0x0-8bit
//       0x1-16bit
//       0x2-24bit
//---- BitShift:
//       0x0-0bit
//       0x1-1bit
//       0x2-2bit

void Cx25828_Audio_Cas_Cfg(int DEVICE, int Cas_OE, int DataWidth, int BitShift)
{
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x0);
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 4, 5, BitShift);
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 2, 3, DataWidth);
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 30, 30, Cas_OE);
    RegMaskWrite32(DEVICE, CX828_AP_CFG1, 29, 29, 0x1); //msb
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 22, 22, 0x0); //0bit delay
    RegMaskWrite32(DEVICE, CX828_AUD_CAP_DLY, 12, 20, 0x7);//to compensate the delay
    RegMaskWrite32(DEVICE, CX828_AP_CFG2, 23, 23, 0x1);
}
