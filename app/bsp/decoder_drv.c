#ifdef __KERNEL__

#include "drv_headers.h"

#include "gpio_drv.h"
#include "gpio_i2c_drv.h"
#include "decoder_drv.h"

static int  I2C_Grp = 0;

enum
{
    DEV_NULL = -1,
    DEV_TW2866 = 0,
    DEV_LG1702,
    DEV_MIK2455,
    DEV_NVC1700,
    DEV_NVP1108,
    DEV_CX258X8,
    DEV_TW2868,
    DEV_RN6366,
    DEV_DEV_CNT,
};

#define MAX_IC_NUM 4
static int MAX_DEV_CNT = MAX_IC_NUM;
static const unsigned char const TW2866_DEV[MAX_IC_NUM] = {0x50, 0x50, 0x54, 0x56};
static const unsigned char const LG1702_DEV[MAX_IC_NUM] = {0x70, 0x72, 0x74, 0x76};
static const unsigned char const MIK2455_DEV[MAX_IC_NUM] = {0xc0, 0xc2, 0xc4, 0xc6};
static const unsigned char const NVC1700_DEV[MAX_IC_NUM] = {0x90, 0x00, 0x00, 0x00};
static const unsigned char const NVP1108_DEV[MAX_IC_NUM] = {0x60, 0x62, 0x64, 0x66};
static const unsigned char const CX258X8_DEV[MAX_IC_NUM] = {0x8E, 0x88, 0x00, 0x00};
static const unsigned char const TW2868_DEV[MAX_IC_NUM] = {0x58, 0x00, 0x00, 0x00};
static int IC_DEV[MAX_IC_NUM]; // init when __init

#include "ad_tw2866.c"
#include "ad_lg1702.c"
#include "ad_mik2455.c"
#include "ad_nvp1108.c"
#include "ad_cx258x8.c"
#include "ad_tw2868.c"
#include "ad_rn6366.c"

static int video_open(struct inode * inode, struct file * file)
{
    return 0;
} 

static int video_close(struct inode * inode, struct file * file)
{
    return 0;
}

static DEFINE_IOCTL(video_ioctl, file, cmd, arg)
{
    int i = 0;
    unsigned int __user *argp = (unsigned int __user *)arg;
    int ret = -1;
    switch (cmd) {
    case VIDEO_DRV_SET_MODE: {
        video_mode_t mode = {0};
        if (copy_from_user(&mode, argp, sizeof(video_mode_t)))
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                if(VIDEO_MODE_2D1 == mode.mode){
                    ret = tw2866_set_mode_2d1(i);
                }else if (VIDEO_MODE_4D1 == mode.mode){
                    ret = tw2866_set_mode_4d1(i);
                }
                break;
            case DEV_LG1702:
                // default 4D1
                break;
            case DEV_MIK2455:
                if(VIDEO_MODE_2D1 == mode.mode){
                    ret = mik2455_set_mode_2d1(i);
                }else if (VIDEO_MODE_4D1 == mode.mode){
                    ret = mik2455_set_mode_4d1(i);
                }
                break;
            case DEV_NVC1700:
            case DEV_NVP1108:
                break;
            case DEV_CX258X8:
                // Need FIXME:
                break;
            case DEV_TW2868:
                break;
            }
        }
    }
        break;
    case VIDEO_DRV_SET_STANDARD: {
        int i = 0;
        video_std_t std;
        if (copy_from_user(&std, argp, sizeof(video_std_t)))
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                ret = tw2866_set_standard(i, std.std);
                break;
            case DEV_LG1702:
                ret = lg1702_set_standard(i, std.std);
                break;
            case DEV_MIK2455:
                ret = mik2455_set_standard(i, std.std);
                break;
            case DEV_NVC1700:
                nvc1700_video_init(I2C_Grp, std.std);
                msleep(100);
                nvc1700_audio_init(0);
                ret = 0;
                break;
            case DEV_NVP1108:
                nvp1108_video_init(I2C_Grp, std.std, std.siz);
                msleep(100);
                nvp1108_audio_init(0);
                ret = 0;
                break;
            case DEV_CX258X8:
                cx258x8_video_init(i, std.std, std.siz);
                msleep(100);
                cx258x8_audio_init(0);
                ret = 0;
                break;
            case DEV_TW2868:
                ret = tw2868_set_standard(i, std.std);
                break;
            case DEV_RN6366:
                rn6366_video_init(i, std.std, std.siz);
                msleep(100);
                rn6366_audio_init(0);
                ret = 0;
                break;
            }
        }
    }
        break;
    case VIDEO_DRV_GET_VINFO: {
        int i = 0;
        video_sup_t vsup;
		memset(&vsup, 0, sizeof(vsup));
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_NVP1108:
                ret = nvp1108_get_vsup(&vsup);
                break;
            case DEV_CX258X8:
                ret = cx258x8_get_vsup(&vsup);
                break;
            case DEV_RN6366:
                ret = rn6366_get_vsup(&vsup);
                break;
			default:
				vsup.vsiz = (1 << VIDEO_SUP_FLD1);
				ret = 0;
				break;
            }
        }
        if (copy_to_user(argp, &vsup, sizeof(vsup))) {
            return -1;
        }
    }
        break;
    case VIDEO_DRV_SET_LIVE_CHN: {
        video_preview_t preview;
        if (copy_from_user(&preview, argp, sizeof(preview)))
            return -1;
        if(DEV_CX258X8 == IC_DEV[0]
          && cx258x8_PreViewSource(preview.chn)) {
            ret = 0;
            break;
        }
        if(DEV_NVC1700 == IC_DEV[0]
          && nvc1700_PreViewSource(preview.chn)) {
            ret = 0;
            break;
        }
        if(DEV_TW2868 == IC_DEV[0]
          && tw2868_PreViewSource(preview.chn)) {
            ret = 0;
            break;
        }
        return -1;
    }
        break;
    case VIDEO_DRV_DET_LOSS: {
        int i;
        video_loss_t loss = {0};
        memset(&loss, 0, sizeof(video_loss_t));
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                loss.loss |= (tw2866_get_loss(i) << (TW2866_CHN_PER_CHIP*i));
                break;
            case DEV_LG1702:
                loss.loss |= (lg1702_get_loss(i) << (LG1702_CHN_PER_CHIP*i));
                break;
            case DEV_MIK2455:
                loss.loss |= (mik2455_get_loss(i) << (MIK2455_CHN_PER_CHIP*i));
                break;
            case DEV_NVC1700:
            case DEV_NVP1108:
                loss.loss |= nvp110x_get_loss(0);
                break;
            case DEV_CX258X8:
                loss.loss |= (cx258x8_get_loss(i) << (CX258X8_CHN_PER_CHIP*i));
                break;
            case DEV_TW2868:
                loss.loss |= tw2868_get_loss(0);
                break;
            case DEV_RN6366:
                loss.loss |= rn6366_get_loss(0);
                break;
            }
        }
        if (copy_to_user(argp, &loss, sizeof(video_loss_t))) {
            return -1;
        }
        ret = 0;
    }
        break;
    case VIDEO_DRV_SET_COLOR: {
        typedef void (*set_color_func)(int chip, int chn, unsigned char gain);
        set_color_func set_hue = NULL;
        set_color_func set_contrast = NULL;
        set_color_func set_brightness = NULL;
        set_color_func set_saturation = NULL;
        int chip = 0;
        int chn = 0;
        video_color_t color;
        if (copy_from_user(&color, argp, sizeof(video_color_t)))
                return -1;
        switch(IC_DEV[i]) {
        case DEV_TW2866:
            chip = color.chn / TW2866_CHN_PER_CHIP;
            chn = color.chn % TW2866_CHN_PER_CHIP;
            set_hue = tw2866_set_hue;
            set_contrast = tw2866_set_contrast;
            set_brightness = tw2866_set_brightness;
            set_saturation = tw2866_set_saturation;
            break;
        case DEV_LG1702:
            chip = color.chn / LG1702_CHN_PER_CHIP;
            chn = color.chn % LG1702_CHN_PER_CHIP;
            set_hue = lg1702_set_hue;
            set_contrast = lg1702_set_contrast;
            set_brightness = lg1702_set_brightness;
            set_saturation = lg1702_set_saturation;
            break;
        case DEV_MIK2455:
            chip = color.chn / MIK2455_CHN_PER_CHIP;
            chn = color.chn % MIK2455_CHN_PER_CHIP;
            set_hue = mik2455_set_hue;
            set_contrast = mik2455_set_contrast;
            set_brightness = mik2455_set_brightness;
            set_saturation = mik2455_set_saturation;
            break;
        case DEV_NVC1700:
        case DEV_NVP1108:
            if(color.items & VIDEO_COLOR_HUE) {
                nvp110x_set_color(color.chn, NVP110X_COLOR_HUE, color.hue);
            }
            if(color.items & VIDEO_COLOR_CONTRAST) {
                nvp110x_set_color(color.chn, NVP110X_COLOR_CON, color.contrast);
            }
            if(color.items & VIDEO_COLOR_SATURATION) {
                nvp110x_set_color(color.chn, NVP110X_COLOR_SAT, color.saturation);
            }
            if(color.items & VIDEO_COLOR_BRIGHTNESS) {
                nvp110x_set_color(color.chn, NVP110X_COLOR_BRI, color.brightness);
            }
            return 0;

        case DEV_CX258X8:
            if(color.items & VIDEO_COLOR_HUE) {
                cx258x8_set_color(color.chn, CX258X8_COLOR_HUE, color.hue);
            }
            if(color.items & VIDEO_COLOR_CONTRAST) {
                cx258x8_set_color(color.chn, CX258X8_COLOR_CON, color.contrast);
            }
            if(color.items & VIDEO_COLOR_SATURATION) {
                cx258x8_set_color(color.chn, CX258X8_COLOR_SAT, color.saturation);
            }
            if(color.items & VIDEO_COLOR_BRIGHTNESS) {
                cx258x8_set_color(color.chn, CX258X8_COLOR_BRI, color.brightness);
            }
            return 0;

        case DEV_RN6366:
            if(color.items & VIDEO_COLOR_HUE) {
                rn6366_set_color(color.chn, RN63XX_COLOR_HUE, color.hue);
            }
            if(color.items & VIDEO_COLOR_CONTRAST) {
                rn6366_set_color(color.chn, RN63XX_COLOR_CON, color.contrast);
            }
            if(color.items & VIDEO_COLOR_SATURATION) {
                rn6366_set_color(color.chn, RN63XX_COLOR_SAT, color.saturation);
            }
            if(color.items & VIDEO_COLOR_BRIGHTNESS) {
                rn6366_set_color(color.chn, RN63XX_COLOR_BRI, color.brightness);
            }
            return 0;

        case DEV_TW2868:
            if(color.items & VIDEO_COLOR_HUE) {
                tw2868_set_color(color.chn, NVP110X_COLOR_HUE, color.hue);
            }
            if(color.items & VIDEO_COLOR_CONTRAST) {
                tw2868_set_color(color.chn, NVP110X_COLOR_CON, color.contrast);
            }
            if(color.items & VIDEO_COLOR_SATURATION) {
                tw2868_set_color(color.chn, NVP110X_COLOR_SAT, color.saturation);
            }
            if(color.items & VIDEO_COLOR_BRIGHTNESS) {
                tw2868_set_color(color.chn, NVP110X_COLOR_BRI, color.brightness);
            }
            return 0;

        default:
            return -1;
        }
        if(color.items & VIDEO_COLOR_BRIGHTNESS){
            set_brightness(chip, chn, color.brightness);
        }
        if(color.items & VIDEO_COLOR_CONTRAST){
            set_contrast(chip, chn, color.contrast);
        }
        if(color.items & VIDEO_COLOR_SATURATION){
            set_saturation(chip, chn, color.saturation);
        }
        if(color.items & VIDEO_COLOR_HUE){
            set_hue(chip, chn, color.hue);
        }
        ret = 0;
    }
        break;
    case VIDEO_DRV_GET_COLOR:
        ret = 0;
        break;
    }

    return ret;
}

static int audio_open(struct inode * inode, struct file * file)
{
    return 0;
} 

static int audio_close(struct inode * inode, struct file * file)
{
    return 0;
}

static DEFINE_IOCTL(audio_ioctl, file, cmd, arg)
{
    int i = 0;
    unsigned int __user *argp = (unsigned int __user *)arg;
    int ret = -1;
    unsigned char tmp = 0;

    audio_format_t      audio_format;
    audio_playback_t    audio_playback;
    audio_da_volume_t   audio_da_volume;

    switch (cmd) {
    case AUDIO_DRV_SET_SAMPLE_RATE: {
        int rate = 0;
        if(copy_from_user(&rate, argp, sizeof(rate)))
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                ret = tw2866_set_audio_samplerate(i, rate);
                break;
            case DEV_LG1702:
                ret = lg1702_set_audio_samplerate(i, rate);
                break;
            case DEV_MIK2455:
                ret = mik2455_set_audio_samplerate(i, rate);
                break;
            case DEV_NVC1700:
                nvp110x_audio_set_samplerate(0, rate);
                nvp110x_audio_set_samplerate(1, rate);
                ret = 0;
                break;
            case DEV_NVP1108:
                nvp110x_audio_set_samplerate(i, rate);
                ret = 0;
                break;    
            case DEV_CX258X8:
                ret = cx258x8_set_audio_samplerate(i, rate);
                break;
            case DEV_TW2868:
                // ret = tw2826_set_audio_samplerate(i, rate);
                break;
            }
        }
    }
        break;
    case AUDIO_DRV_SET_AUDIO_PLAYBACK: {
        if (copy_from_user(&audio_playback, argp, sizeof(audio_playback)))
            return -1;
        if(audio_playback.chip >= MAX_DEV_CNT)
            return -1;
        if(audio_playback.chn > 24 || audio_playback.chn < 0)
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                tmp  = TW2866_BYTE_READ(audio_playback.chip, 0xe0);
                tmp &= 0xe0;
                tmp |= audio_playback.chn;
                TW2866_BYTE_WRITE(audio_playback.chip, 0xe0, tmp);
                break;
            case DEV_LG1702:
//                tmp  = LG1702_READ(audio_playback.chip, 0x007b);
//                tmp &= 0x06;
//                tmp |= audio_playback.chn;
                LG1702_WRITE(audio_playback.chip, 0x007b, audio_playback.chn);
                break;
            case DEV_MIK2455:
                tmp = MIK2455_READ(audio_playback.chip, 0xd8);
                tmp &= 0x8f;
                tmp |= ((audio_playback.chn<<4)&0x70);
                MIK2455_WRITE(audio_playback.chip, 0xd8, tmp);
                break;
            case DEV_NVC1700:
            case DEV_NVP1108:
                nvp110x_audio_set_playback(audio_playback.chip, audio_playback.chn);
                break;
            case DEV_RN6366:
                rn6366_audio_set_playback(audio_playback.chip, audio_playback.chn);
                break;    
            case DEV_CX258X8:
                //Null for cx258x8
                break;
            }
        }
        ret = 0;
    }
        break;
    case AUDIO_DRV_SET_AUDIO_DA_VOLUME: {
        if (copy_from_user(&audio_da_volume, argp, sizeof(audio_da_volume)))
            return -1;
        if(audio_da_volume.chip >= MAX_DEV_CNT)
            return -1;
        if(audio_da_volume.volume > 15 || audio_da_volume.volume < 0)
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                tmp = TW2866_BYTE_READ(audio_da_volume.chip, 0xdf);    
                tmp &= 0x0f;
                tmp |= (audio_da_volume.volume << 4);
                TW2866_BYTE_WRITE(audio_da_volume.chip, 0xdf, tmp);
                break;
            case DEV_LG1702:
                LG1702_WRITE(audio_da_volume.chip, 0x0082, audio_da_volume.volume << 4);
                break;
            case DEV_MIK2455: {
                unsigned char tmpTbl[4][3] = {
                    {0xd1, 0xf0, 0x00}, {0xd1, 0x0f, 0x04},
                    {0xd2, 0xf0, 0x00}, {0xd2, 0x0f, 0x04}
                };
                int tmpRd = 0;
                tmpRd = MIK2455_READ(audio_da_volume.chip, tmpTbl[i][0]);
                tmpRd &= tmpTbl[i][1];
                tmp = (audio_da_volume.volume << tmpTbl[i][2]) | tmpRd;
                MIK2455_WRITE(audio_da_volume.chip, tmpTbl[i][0], tmp);
            }
                break;
            case DEV_NVC1700:
            case DEV_NVP1108:
                nvp110x_audio_set_volume(i, audio_da_volume.volume);
                break;    
            case DEV_CX258X8:
                cx258x8_set_audio_volume(i, audio_da_volume.volume);
                break;
            case DEV_TW2868:
                // tw2826_set_audio_volume(i, audio_da_volume.volume);
                break;
            case DEV_RN6366:
                rn6366_audio_set_volume(i, audio_da_volume.volume);
                break;
            }
        }
        ret = 0;
    }
        break;
    case AUDIO_DRV_SET_AUDIO_FORMAT: {
        if (copy_from_user(&audio_format, argp, sizeof(audio_format)))
            return -1;
        if (audio_format.format != 0 && audio_format.format != 1)
            return -1;
        for(i = 0; i < MAX_DEV_CNT; ++i) {
            switch(IC_DEV[i]) {
            case DEV_TW2866:
                tw2866_set_audio_format(i, audio_format);
                break;
            case DEV_LG1702:
                lg1702_set_audio_format(i, audio_format);
                break;
            case DEV_MIK2455:
                mik2455_set_audio_format(i, audio_format);
                break;
            case DEV_NVC1700:
                nvp110x_set_audio_format(0, audio_format);
                nvp110x_set_audio_format(1, audio_format);
                break;
            case DEV_NVP1108:
                nvp110x_set_audio_format(i, audio_format);
                break;    
            case DEV_CX258X8:
                cx258x8_set_audio_format(i, audio_format);
                break;
            case DEV_TW2868:
                // tw2826_set_audio_format(i, audio_format);
                break;
            }
        }
        ret = 0;
    }
        break;
    case AUDIO_DRV_REG_XDBG: {
        audio_reg_xdbg_t reg_dbg;
        if (copy_from_user(&reg_dbg, argp, sizeof(reg_dbg)))
            return -1;
        if(IC_DEV[0] == DEV_CX258X8) {
            if(cx258x8_regdbg(&reg_dbg)
            || copy_to_user(argp, &reg_dbg, sizeof(reg_dbg))) {
                return -1;
            }
        }
        else if(IC_DEV[0] == DEV_RN6366) {
            if(rn6366_regdbg(&reg_dbg)
            || copy_to_user(argp, &reg_dbg, sizeof(reg_dbg))) {
                return -1;
            }
        }
        else {
            return -1;
        }
        ret = 0;
    }
       break;
    case AUDIO_DRV_CAPABILITY_LIVELOOP: {
        int capability = 0;
        if(DEV_NVP1108 == IC_DEV[0]
          || DEV_NVC1700 == IC_DEV[0]) {
            capability = 1;
        }
        if(DEV_LG1702 == IC_DEV[0]) {
            capability = 1;
        }
        if(DEV_CX258X8 == IC_DEV[0]) {
            capability = 1;
        }
        if(DEV_RN6366 == IC_DEV[0]) {
            capability = 1;
        }
        if(copy_to_user(argp, &capability, sizeof(capability))) {
            return -1;
        }
        ret = 0;
    }
        break;
    case AUDIO_DRV_SET_LIVELOOP: {
        int chn = 0;
        if(copy_from_user(&chn, argp, sizeof(chn))){
            return -1;
        }
        if(DEV_NVP1108 == IC_DEV[0]) {
            nvp110x_audio_set_liveloop(0, chn);
        }else if(DEV_NVC1700 == IC_DEV[0]) {
            nvc1700_audio_set_liveloop(0, chn);
        }else if(DEV_LG1702 == IC_DEV[0]) {
            lg1702_audio_set_liveloop(0, chn);
        }else if(DEV_CX258X8 == IC_DEV[0]) {
            cx258x8_audio_set_liveloop(0, chn);
        }else if(DEV_RN6366 == IC_DEV[0]) {
            rn6366_audio_set_liveloop(0, chn);
        } else {
            return -EPERM;
        }
        ret = 0;
    }
        break;
    case AUDIO_DRV_SET_PLAYBACK: {
        if(  DEV_NVP1108 == IC_DEV[0]
          || DEV_NVC1700 == IC_DEV[0]) {
            nvp110x_audio_set_playback(0, 0);
        }else if(DEV_LG1702 == IC_DEV[0]) {
            lg1702_audio_set_playback(0, 0);
        }else if(DEV_CX258X8 == IC_DEV[0]) {
            cx258x8_audio_set_playback(0, 0);
        }else if(DEV_RN6366 == IC_DEV[0]) {
            rn6366_audio_set_playback(0, 0);
        }else {
            return -EPERM;
        }
        ret = 0;
    }
        break;
    }

    return ret;
}

void i2c_dev_Probe(void)
{
    int i, j;
    int RetPrb;
//    gpio_rst_device();
    mdelay(100);

    for(j = 0; j < I2C_Grp; j ++) {
        printk("I2C -------------- PROBING I2C Device%d\n", j);
        for(i = 0; i < 0xFF; i += 2) {
            RetPrb = gpio_i2c_probeX(j, i);
            udelay(5);
            if(RetPrb) {
                printk("I2C -------------- PROBED : 0x%02X \n", i);
            }
        }
    }
}

static void i2c_dev_detect(void)
{
    int i = 0;
    for(i = 0; i < MAX_DEV_CNT; ++i) {
        if(tw2866_check(i)) {
            IC_DEV[i] = DEV_TW2866;
        }else if(lg1702_check(i)) {
            IC_DEV[i] = DEV_LG1702;
        }else if(mik2455_check(i)) {
            IC_DEV[i] = DEV_MIK2455;
        }else if(nvc1700_check(0)) {
            IC_DEV[0]   = DEV_NVC1700;
            MAX_DEV_CNT = 1;
        }else if(nvp1108_check_all(0)) {
            printk("nvp110x at Addr: 0x%X Found\n", NVP1108_DEV[i]);
            IC_DEV[i] = DEV_NVP1108;
            MAX_DEV_CNT = 1;
        }else if(cx258x8_check(i)) {
            printk("cx258x8 at Addr: 0x%X Found\n", CX258X8_DEV[i]);
            IC_DEV[0]   = DEV_CX258X8;
            MAX_DEV_CNT = 1;
        }else if(rn6366_check(i)) {
            IC_DEV[0]   = DEV_RN6366;
            MAX_DEV_CNT = 1;
        }else if(tw2868_check(i)) {
            printk("tw2868 at Addr: 0x%X Found\n", TW2868_DEV[i]);
            IC_DEV[i]   = DEV_TW2868;
            MAX_DEV_CNT = 1;
        }else{
            IC_DEV[i] = DEV_NULL;
        }
    }
#if 1
    printk("Device Detected:\t");
    for(i = 0; i < MAX_DEV_CNT; ++i) {
        printk((IC_DEV[i] == DEV_NULL) ?  "x" : "o");
    }
    printk("\r\n");
#endif
}

static struct file_operations video_fops =
{
    .owner    =    THIS_MODULE,
    .ioctl    =    video_ioctl,
    .open    =    video_open,
    .release    =    video_close,
};

static struct miscdevice video_dev = {
    .minor    =    MISC_DYNAMIC_MINOR,
    .name    =    "video",
    .fops    =    &video_fops,
};

static struct file_operations audio_fops =
{
    .owner        =    THIS_MODULE,
    .ioctl        =    audio_ioctl,
    .open        =    audio_open,
    .release    =    audio_close,
};

static struct miscdevice audio_dev = {
    .minor    =    MISC_DYNAMIC_MINOR,
    .name    =    "audio",
    .fops    =    &audio_fops,
};

static int dev_init(void)
{
    int ret = 0;
    int i   = 0;

    I2C_Grp = gpio_i2c_lines();

    // register misc device
    if ((ret = misc_register(&video_dev))) {
        printk(KERN_ERR" could not register Video devices\n");
        return ret;
    }
    if ((ret = misc_register(&audio_dev))) {
        printk(KERN_ERR" could not register Audio devices\n");
        return ret;
    }

    i2c_dev_Probe();
    i2c_dev_detect();

    for(i = 0; i < MAX_DEV_CNT; ++i){
        if(DEV_TW2866 == IC_DEV[i]){
            printk("TW2866 init @ %d\r\n", i);
            tw2866_video_init(i, "pal");
            tw2866_audio_init(i);
            tw2866_audio_cascade(i);
        }else if(DEV_LG1702 == IC_DEV[i]){
            printk("LG1702 init @ %d\r\n", i);
            lg1702_video_init(i, "pal");
            lg1702_audio_init(i);
        }else if(DEV_MIK2455 == IC_DEV[i]){
            printk("MIK2455 init @ %d\r\n", i);
            mik2455_video_init(i, "pal");
            mik2455_audio_init(i);
        }else if(DEV_NVC1700 == IC_DEV[i]){
            // printk("NVC1700 init @ %d\r\n", i);
            // nvc1700_video_init(I2C_Grp, "pal");
            // nvc1700_audio_init(i);
        }else if(DEV_NVP1108 == IC_DEV[i]){
            // printk("NVP1108 init @ %d\r\n", i);
            // nvp1108_video_init(I2C_Grp, "pal");
            // nvp1108_audio_init(i);
        }else if(DEV_RN6366 == IC_DEV[i]) {
            // printk("RN6366 init @ %d\r\n", i);
            // rn6366_video_init(i, "pal");
            // rn6366_audio_init(i);
        }else if(DEV_CX258X8 == IC_DEV[i]){
            // printk("CX258X8 init @ %d\r\n", i);
            // cx258x8_video_init(i, "pal");
            // cx258x8_audio_init(i);
        }else if(DEV_TW2868 == IC_DEV[i]) {
            printk("TW2868 init @ %d\r\n", i);
            tw2868_video_init(i, "pal");
        }
    }

    printk(KERN_INFO"%s init successful!\n", __func__);
    return 0;
}

static void dev_exit(void)
{
    misc_deregister(&video_dev);
    misc_deregister(&audio_dev);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_AUTHOR("Law");
MODULE_DESCRIPTION("VIDEO for HI3515");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("GPL");


#else //__KERNEL__

#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>

// nValue: 0 - 16, other : ignor
int VIDEODRV_SetColor(int nFileD, int nChn, int nHue, int nBrightness, int nContrast, int nSaturation)
{
    int nRet = 0;
    video_color_t stColor = {0};
    memset(&stColor, 0, sizeof(stColor));
    stColor.chn= nChn;
    stColor.items = 0;
    if(nHue >= 0 && nHue <= VIDEO_MAX_COLOR_GAIN){
        stColor.items |= VIDEO_COLOR_HUE;
        stColor.hue = nHue;
    }
    if(nBrightness >= 0 && nBrightness <= VIDEO_MAX_COLOR_GAIN){
        stColor.items |= VIDEO_COLOR_BRIGHTNESS;
        stColor.brightness = nBrightness;
    }
    if(nContrast >= 0 && nContrast <= VIDEO_MAX_COLOR_GAIN){
        stColor.items |= VIDEO_COLOR_CONTRAST;
        stColor.contrast = nContrast;
    }
    if(nSaturation >= 0 && nSaturation <= VIDEO_MAX_COLOR_GAIN){
        stColor.items |= VIDEO_COLOR_SATURATION;
        stColor.saturation = nSaturation;
    }
    if(!stColor.items){
        return -1;
    }
    if(nFileD){
        if((nRet = ioctl(nFileD, VIDEO_DRV_SET_COLOR, &stColor))){
            fprintf(stderr, "%s error->%s\r\n", __func__, strerror(errno));
            return -1;
        }
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

// set the audio output volume
int AUDIODRV_SetVolume(int nFileD, int nVolume)
{
    if(nFileD){
        int nRet = 0;
        audio_da_volume_t stVol;
        if(nVolume < 0){
            nVolume = 0;
        }
        if(nVolume >= 8){
            nVolume = 7;
        }
        stVol.chip = 0;
        stVol.volume = nVolume * 2;
        if((nRet = ioctl(nFileD, AUDIO_DRV_SET_AUDIO_DA_VOLUME, &stVol))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
        return 0;
    }
    return -1;
}

// set the audio sample rate 
// return the sample rate actually setting by front ad
int AUDIODRV_SetSampleRate(int nFileD, int nRate)
{
    if(nFileD){
        int nRet = 0;
        if((nRet = ioctl(nFileD, AUDIO_DRV_SET_SAMPLE_RATE, &nRate))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
//        return nRate;
        return 8000;
    }
    return -1;
}

int AUDIODRV_IsCapLiveLoop(int nFileD)
{
    if(nFileD){
        int nRet = 0;
        int bCapability = 0;
        if((nRet = ioctl(nFileD, AUDIO_DRV_CAPABILITY_LIVELOOP, &bCapability))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
        return bCapability;
    }
    return 0;
}

// nChannel = 2, 4, 8, 16
// return the real channel you can set
int AUDIODRV_SetI2SMode(int nFileD, int nChannel)
{
    if(nFileD){
        int nRet = 0;
        if((nRet = ioctl(nFileD, AUDIO_DRV_IOC_MAGIC, &nChannel))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
        return nChannel;
    }
    return -1;
}

int AUDIODRV_SetLiveLoop(int nFileD, int nChannel)
{
    printf("set live loop = %d\r\n", nChannel);
    if(nFileD){
        int nRet = 0;
        if((nRet = ioctl(nFileD, AUDIO_DRV_SET_LIVELOOP, &nChannel))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
        return 0;
    }
    return -1;
}

int AUDIODRV_SetPlayback(int nFileD)
{
    printf("set playback\r\n");
    if(nFileD){
        int nRet = 0;
        if((nRet = ioctl(nFileD, AUDIO_DRV_SET_PLAYBACK, NULL))){
            fprintf(stderr, "%s err->%s\r\n", __func__, strerror(errno));
            return -1;
        }
        return 0;
    }
    return -1;
}

#endif //__KERNEL__

