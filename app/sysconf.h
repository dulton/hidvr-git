

#ifndef __JA_SYS_CONF_H__
#define __JA_SYS_CONF_H__
	
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	JA_DVR = 0,
	JA_NVR,
	JA_HVR,
	JA_IPC,
	JA_DNVR,
}enJA_DEVICE_TYPE;

typedef enum {
	JA_D1,
	JA_960H,
	JA_720P,
	JA_HFHD,
	JA_960P,
	JA_1080P,
	JA_RESOL_CNT,
}enJA_SYS_RESOLUTION_TYPE;

typedef struct {
	int width;
	int height;
}stJA_SYS_RESOLUTION;

typedef struct {
	unsigned int maxPixels;
	unsigned int maxFps;
}stJA_SYS_STREAM_ATTR;

#define JA_SYS_MAX_STREAM		(5)
typedef struct _JA_SYS_CONFIG
{
	unsigned int nMaxBncCam;
	unsigned int nBncCam;
	unsigned int nNetCam;

	unsigned int nstream;
	stJA_SYS_STREAM_ATTR streamAttr[5];

	unsigned int nMaxPbChannel;
	int bMustPBFullFrame;
	unsigned int nMinPBFps;
	unsigned int nFullPBFps;
	
	enJA_DEVICE_TYPE devType;
}stJA_SYS_CONFIG, *lpJA_SYS_CONFIG;

extern stJA_SYS_CONFIG g_JSysConfig;

void JA_SYS_CONFIG_init0(int nBncCam, int nNetCam, int nstream);
void JA_SYS_CONFIG_init(const char *filePath);

stJA_SYS_RESOLUTION JA_SYS_CONFIG_getResolMap(enJA_SYS_RESOLUTION_TYPE);
stJA_SYS_RESOLUTION JA_SYS_CONFIG_getDecMaxCap(unsigned int Chn,int totalDecChn);
unsigned int JA_SYS_CONFIG_getEncMaxFps(unsigned int fmt, unsigned int streamIndex);
int JA_SYS_CONFIG_getMaxMainStream(int liveDecChn,int totalDecChn);

#define JCONF_BNC_CAM()			(g_JSysConfig.nBncCam)
#define JCONF_BNC_CAM_START()	(0)
#define JCONF_BNC_CAM_N(n)		(0 + (n))
#define JCONF_BNC_CAM_END()		(g_JSysConfig.nBncCam)
#define JCONF_NET_CAM()			(g_JSysConfig.nNetCam)
#define JCONF_NET_CAM_START()	(g_JSysConfig.nBncCam)
#define JCONF_NET_CAM_N(n)		(g_JSysConfig.nBncCam + (n))
#define JCONF_BNC_A_NET_CAM()	(g_JSysConfig.nBncCam + g_JSysConfig.nNetCam)
#define JCONF_NET_CAM_END()		JCONF_BNC_A_NET_CAM()

#define JCONF_CAM_START()	(0)
#define JCONF_CAM_END()		JCONF_BNC_A_NET_CAM()


#define JCONF_IS_BNC_CAM(chn)	(((chn) >= JCONF_BNC_CAM_START()) && ((chn) < JCONF_BNC_CAM_END()))
#define JCONF_IS_NET_CAM(chn)	(((chn) >= JCONF_NET_CAM_START()) && ((chn) < JCONF_NET_CAM_END()))

#define JCONF_STREAM_NUM()		(g_JSysConfig.nstream)

#ifdef __cplusplus
}
#endif
#endif /* __JA_SYS_CONF_H__ */



