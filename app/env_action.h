/*
 * env_action.h
 *
 *  Created on: 2011-4-6
 *      Author: root
 */

#ifndef ENV_ACTION_H_
#define ENV_ACTION_H_

typedef enum _SMTP_TYPE
{
	SMTP_TYPE_NORMAL = 1,
	SMTP_TYPE_TEST,
	SMTP_TYPE_ABNORMAL,
	SMTP_TYPE_SENSOR_ALRAM,
	SMTP_TYPE_VIDEO_LOSS,
	SMTP_TYPE_MOTION_DETECT,
}SMTP_TYPE;


//void XENVACT_miniweb_init();
//void XENVACT_miniweb_deinit();
//void XENVACT_miniweb_reload();
//
//void XENVACT_avstreamsrv_init();
//void XENVACT_avstreamsrv_deinit();
//void XENVACT_avstreamsrv_reload();

void XENVACT_set_ip();
int  XENVACT_PPPSetReady(char * PPPSetFile);
void XENVACT_PPP_Bgn(void);
void XENVACT_PPP_End(void);

int PTZ_INIT();
void PTZ_RELOAD();
void OSD_INIT();
void OSD_RELOAD(unsigned char bTime, unsigned char bTitle);
void OVERLAY_INIT();
void OVERLAY_RELOAD();
void GPIO_INIT();
void GPIO_RELOAD();
void VIDEO_INIT();
void VIDEO_RELOAD();

void XENVACT_init_volumn();
void XENVACT_change_volumn(int _volumn);

void XENVACT_SMTP_Start(int mode);
void XENVACT_SMTP_Stop(void);
void XENVACT_SMTP_Add(SMTP_TYPE type, unsigned short detail);

#endif /* ENV_ACTION_H_ */
