
#ifndef __MODEL_CONF_H__
#define __MODEL_CONF_H__

#include "version.h"
#include "ui_sel.h"

#define _JA7216NC
#define SN_HEAD "H3"
#define SDK_PLATFORM_HI3515

#define GPIO_PLAT_TYPE1
#define GPIO_KEYPAD_MATRIX

#define HWVER_MAJ (1)
#define HWVER_MIN (0)
#define HWVER_REV (0)
#define HWVER_EXT ""

#define MAX_REF_CH (32)
#define MAX_CAM_CH (16)
#define ALL_CAM_BITMASK (0xffff)
#define MAX_AUD_CH (2)
#define ALL_AUD_BITMASK (0x3)
#define MAX_SENSOR_CH (4)
#define ALL_SENSOR_BITMASK (0xf)
#define MAX_ALARM_CH (1)
#define ALL_ALARM_BITMASK (0x1)

#define MAX_HDD_CNT (4)

#define MAX_CIF_CNT (16)
#define MAX_D1_CNT (0)

#define MAX_D1_LIVE_FPS (25)
#define MAX_CIF_LIVE_FPS (25)

#define MAX_D1_ENC_FPS (16)
#define MAX_CIF_ENC_FPS (16)
#define MAX_NET_ENC_FPS (11)
#define MAX_D1_ENC_BPS (2048)
#define MAX_CIF_ENC_BPS (512)

// enc 3enc equ 1dec 16fps at the total
#define MAX_CIF_DEC_CAPABILITY (((4 * 3) /* enc */ + 4 /* dec */) * MAX_D1_LIVE_FPS)

#define FIRMWARE_MAGIC "JUAN HI3515 R7216NC FIRMWARE DESIGNED BY LAW"
#define FIRMWARE_FILE_NAME_PREFIX "FWHI1516B_"

#endif //__MODEL_CONF_H__

