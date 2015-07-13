
#ifndef __MODEL_CONF_H__
#define __MODEL_CONF_H__

#include "version.h"
#include "ui_sel.h"

#define _JA9204RN
#define SN_HEAD "H7"
#define SDK_PLATFORM_HI3515
#define _EXT_960H

#define GPIO_PLAT_TYPE1
#define GPIO_KEYPAD_MATRIX

#define HWVER_MAJ (1)
#define HWVER_MIN (0)
#define HWVER_REV (0)
#define HWVER_EXT ""

#define MAX_REF_CH (32)
#define MAX_CAM_CH (4)
#define ALL_CAM_BITMASK (0xf)
#define MAX_AUD_CH (4)
#define ALL_AUD_BITMASK (0xf)
#define MAX_SENSOR_CH (4)
#define ALL_SENSOR_BITMASK (0xf)
#define MAX_ALARM_CH (1)
#define ALL_ALARM_BITMASK (0x1)

#define MAX_HDD_CNT (4)

#define MAX_CIF_CNT (4)
#define MAX_D1_CNT (2)

#define MAX_D1_LIVE_FPS (25)
#define MAX_CIF_LIVE_FPS (25)

#define MAX_D1_ENC_FPS (16)
#define MAX_CIF_ENC_FPS (25)
#define MAX_NET_ENC_FPS (11)
#define MAX_D1_ENC_BPS (1536)
#define MAX_CIF_ENC_BPS (512)

// enc 3enc equ 1dec 16fps at the total
#define MAX_CIF_DEC_CAPABILITY (((4 * 3) /* enc */ + 4 /* dec */) * MAX_D1_LIVE_FPS)

#define FIRMWARE_MAGIC "JUAN HI3515 JA9204RN FIRMWARE DESIGNED BY LAW"
#define FIRMWARE_FILE_NAME_PREFIX "FWHI1504H9_"

#endif //__MODEL_CONF_H__

