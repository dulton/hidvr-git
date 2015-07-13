
#ifndef __CONF_H__
#define __CONF_H__

#include "../conf.h"
#ifndef __MODEL_CONF_H__
#error "Never Config!!"
#endif

#include "../oem.h"
#ifndef __OEM_CONF_H__
#error "OEM Never Config!!"
#endif

#ifndef MAX_ENC_CH
#define MAX_ENC_CH (MAX_CAM_CH)
#endif
#ifndef MAX_DEC_CH
#define MAX_DEC_CH (MAX_CAM_CH)
#endif
#ifndef MAX_PLAYBACK_CH
#define MAX_PLAYBACK_CH (MAX_DEC_CH)
#endif

#endif //__CONF_H__

