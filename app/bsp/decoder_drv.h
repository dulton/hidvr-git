#ifndef __DECODER_DRV_H__
#define __DECODER_DRV_H__

#include <linux/ioctl.h>

enum {
	VIDEO_SUP_FLD1 = 0,
	VIDEO_SUP_HFD1,
	VIDEO_SUP_FLEF,
	VIDEO_SUP_HFEF,
	VIDEO_SUP_720P,
	VIDEO_SUP_1080P,
};

typedef struct
{
	int vsiz;
	int vmod;
} video_sup_t;

typedef struct video_std
{
    char std[8]; // in "pal" "ntsc" "auto"
	char siz[8]; // "720H" "960H" "FLD1" "HFD1" "FLEF" "HFEF" "720P" "1080P"
}video_std_t;

typedef enum
{
	VIDEO_MODE_1D1 = 0,
	VIDEO_MODE_2D1,
	VIDEO_MODE_4CIF,
	VIDEO_MODE_4D1,
}VIDEO_MODE;

typedef struct video_mode
{
	VIDEO_MODE mode;
}video_mode_t;

typedef struct video_loss
{
	unsigned int loss;	// 16bit use
}video_loss_t;

typedef enum
{
	VIDEO_COLOR_HUE = (1<<0),
	VIDEO_COLOR_CONTRAST = (1<<1),
	VIDEO_COLOR_BRIGHTNESS = (1<<2),
	VIDEO_COLOR_SATURATION = (1<<3),
}VIDEO_COLOR_ITEM;

#define VIDEO_MAX_COLOR_GAIN (64)
#define VIDEO_COLOR_ADJTBL0 \
        {-120,-116,-112,-108,-104,-101,-97,-93,\
        -89,-85,-81,-77,-74,-70,-66,-62,\
        -58,-54,-51,-47,-43,-39,-35,-32,\
        -28,-24,-20,-17,-13,-9,-6,-2,\
        0,\
        2,6,9,13,17,20,24,28,\
        32,35,39,43,47,51,54,58,\
        62,66,70,74,77,81,85,89,\
        93,97,101,104,108,112,116,120}
#define VIDEO_COLOR_ADJTBL1 \
        {8,12,16,20,24,27,31,35,\
        39,43,47,51,54,58,62,66,\
        70,74,77,81,85,89,93,96,\
        100,104,108,111,115,119,122,126,\
        128,\
        130,134,137,141,145,148,152,156,\
        160,163,167,171,175,179,182,186,\
        190,194,198,202,205,209,213,217,\
        221,225,229,232,236,240,244,248}
#define VIDEO_COLOR_ADJTBL0_1 \
        {-120,-116,-112,-108,-104,-100,-96,-92,\
        -88,-84,-80,-76,-72,-68,-64,-60,\
        -56,-52,-49,-46,-43,-40,-37,-34,\
        -31,-28,-25,-23,-21,-19,-17,-15,\
        -13,\
        -11,-9,-7,-5,-3,0,3,6,\
        9,12,15,18,22,26,30,34,\
        38,43,48,53,58,63,68,73,\
        78,84,90,96,102,108,114,120}

typedef struct video_color
{
	int chn;
	unsigned int items;		// use such as "VIDEO_COLOR_HUE|VIDEO_COLOR_CONTRAST"
	unsigned char hue;
	unsigned char contrast;
	unsigned char brightness;
	unsigned char saturation;
} video_color_t;

typedef struct video_preview
{
    int chn;
    int opt;
} video_preview_t;

#define VIDEO_DRV_IOC_MAGIC            'V'
#define VIDEO_DRV_DET_LOSS _IOR(VIDEO_DRV_IOC_MAGIC, 1, video_loss_t)
#define VIDEO_DRV_SET_COLOR _IOW(VIDEO_DRV_IOC_MAGIC, 2, video_color_t) 
#define VIDEO_DRV_GET_COLOR _IOWR(VIDEO_DRV_IOC_MAGIC, 3, video_color_t) 
#define VIDEO_DRV_SET_STANDARD _IOW(VIDEO_DRV_IOC_MAGIC, 4, video_std_t) 
#define VIDEO_DRV_GET_STANDARD _IOWR(VIDEO_DRV_IOC_MAGIC, 5, video_std_t)
#define VIDEO_DRV_GET_VINFO    _IOR(VIDEO_DRV_IOC_MAGIC, 6, video_sup_t)
#define VIDEO_DRV_SET_MODE _IOW(VIDEO_DRV_IOC_MAGIC, 7, video_mode_t)
#define VIDEO_DRV_SET_LIVE_CHN _IOW(VIDEO_DRV_IOC_MAGIC, 8, video_preview_t)

typedef struct __audio_format_t
{
	unsigned char format;   /* 0:i2s; 1:dsp */
} audio_format_t;

typedef struct __audio_output_t
{
	unsigned char PlaybackOrLoop;   /*0:playback; 1:loop*/
	unsigned char channel;          /*[0, 15]*/
	unsigned char reserve[6];
} audio_output_t;

typedef enum __audio_samplerate_t
{
	SAMPLE_RATE_8000,
	SAMPLE_RATE_16000,
	SAMPLE_RATE_32000,
	SAMPLE_RATE_44100,
	SAMPLE_RATE_48000,
	SAMPLE_RATE_BUTT
} audio_samplerate_t;

typedef struct __audio_playback_t
{
    unsigned int chip;
    unsigned int chn;
} audio_playback_t;

typedef struct __audio_da_mute_t
{
    unsigned int chip;
} audio_da_mute_t;

typedef struct __audio_da_volume_t
{
    unsigned int chip;
    unsigned int volume;
} audio_da_volume_t;

typedef struct __audio_reg_rdbg_t
{
	unsigned int chip;
    unsigned int rw_flg;
    unsigned int addr;
    unsigned int value;
} audio_reg_xdbg_t;

#define AUDIO_DRV_IOC_MAGIC				'A'
#define AUDIO_DRV_SET_SAMPLE_RATE		_IOW(AUDIO_DRV_IOC_MAGIC, 1, audio_samplerate_t)
#define AUDIO_DRV_SET_AUDIO_PLAYBACK	_IOW(AUDIO_DRV_IOC_MAGIC, 2, audio_playback_t) 
#define AUDIO_DRV_SET_ADUIO_DA_MUTE		_IOW(AUDIO_DRV_IOC_MAGIC, 3, audio_da_mute_t)
#define AUDIO_DRV_SET_AUDIO_DA_VOLUME	_IOW(AUDIO_DRV_IOC_MAGIC, 4, audio_da_volume_t) 
#define AUDIO_DRV_SET_AUDIO_FORMAT		_IOW(AUDIO_DRV_IOC_MAGIC, 5, audio_format_t)
#define AUDIO_DRV_REG_XDBG				_IOR(AUDIO_DRV_IOC_MAGIC, 6, audio_reg_xdbg_t)
#define AUDIO_DRV_CFG_CHN				_IOW(AUDIO_DRV_IOC_MAGIC, 7, int)

#define AUDIO_DRV_CAPABILITY_LIVELOOP	_IOR(AUDIO_DRV_IOC_MAGIC, 8, int) // with capability will return true(1) @ arg else return false(0)
#define AUDIO_DRV_SET_I2S_MODE			_IOWR(AUDIO_DRV_IOC_MAGIC, 9, int)
#define AUDIO_DRV_SET_LIVELOOP			_IOW(AUDIO_DRV_IOC_MAGIC, 10, int)
#define AUDIO_DRV_SET_PLAYBACK			_IO(AUDIO_DRV_IOC_MAGIC, 11)

#ifdef __KERNEL__

#define VIDEO_IS_PAL(str) (strlen(str)==strlen("pal") && !strcasecmp(str,"pal"))
#define VIDEO_IS_NTSC(str) (strlen(str)==strlen("ntsc") && !strcasecmp(str,"ntsc"))
#define VIDEO_IS_720H(str) (0 == strcasecmp(str, "720h"))
#define VIDEO_IS_960H(str) (0 == strcasecmp(str, "960h"))

#else //__KERNEL__

extern int VIDEODRV_SetColor(int nFileD, int nChn, int nHue, int nBrightness, int nContrast, int nSaturation);

extern int AUDIODRV_SetVolume(int nFileD, int nVolume);
extern int AUDIODRV_SetSampleRate(int nFileD, int nRate);
extern int AUDIODRV_IsCapLiveLoop(int nFileD);
extern int AUDIODRV_SetI2SMode(int nFileD, int nChannel);
extern int AUDIODRV_SetLiveLoop(int nFileD, int nChannel);
extern int AUDIODRV_SetPlayback(int nFileD);

#endif //__KERNEL__

#endif	//__DECODER_DRV_H__


