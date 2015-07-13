
#ifndef __MODEL_CONF_H__
#define __MODEL_CONF_H__

#include "version.h"
#include "ui_sel.h"

#define _JA2008NC
#define SN_HEAD "I1"
#define SDK_PLATFORM_HI3520

#define GPIO_PLAT_TYPE3
#define GPIO_KEYPAD_PANEL

#define HWVER_MAJ (1)
#define HWVER_MIN (0)
#define HWVER_REV (0)
#define HWVER_EXT ""

#define MAX_REF_CH (32)
#define MAX_CAM_CH (8)
#define ALL_CAM_BITMASK (0xff)
#define MAX_AUD_CH (8)
#define ALL_AUD_BITMASK (0xff)
#define MAX_SENSOR_CH (4)
#define ALL_SENSOR_BITMASK (0xf)
#define MAX_ALARM_CH (1)
#define ALL_ALARM_BITMASK (0x1)

#define MAX_HDD_CNT (4)

#define MAX_CIF_CNT (8)
#define MAX_D1_CNT (8)

#define MAX_D1_LIVE_FPS (25)
#define MAX_CIF_LIVE_FPS (25)

#define MAX_D1_ENC_FPS (16)
#define MAX_CIF_ENC_FPS (25)
#define MAX_NET_ENC_FPS (16)
#define MAX_D1_ENC_BPS (2048)
#define MAX_CIF_ENC_BPS (512)

#define MAX_ENC_CH (8)
#define MAX_DEC_CH (1)
#define MAX_PLAYBACK_CH (1)

// enc 3enc equ 1dec 16fps at the total
#define MAX_CIF_DEC_CAPABILITY (((8 * 3) /* enc */ + 4 /* dec */) * MAX_D1_LIVE_FPS)

#define FIRMWARE_MAGIC "JUAN HI3520 JA2008NC FIRMWARE DESIGNED BY LAW"
#define FIRMWARE_FILE_NAME_PREFIX "FWHI2008A_"

#endif //__MODEL_CONF_H__

