

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <sched.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>

#include "jn_sdk/sdk_pri_api.h"

#include "conf.h"
#include "system.h"
#include "dvr_debug.h"
#include "sysconf.h"

#include "envfix.h"

#define DEBUG_SYSTEM
#ifdef DEBUG_SYSTEM
#define SYSTEM_TRACE(fmt...)	\
	do{printf("\033[0;37mSYSTEM->[%s]:%d ", __FUNCTION__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define SYSTEM_TRACE(fmt...)
#endif






static void write_rtc(time_t t)
{
	int utc = 0;
	int rtc;
	struct tm tm;

	if (( rtc = open ( "/dev/rtc", O_WRONLY )) < 0 ) {
		if (( rtc = open ( "/dev/misc/rtc", O_WRONLY )) < 0 ){
			printf ( "Could not access RTC err: %s\r\n", strerror(errno));
		}
	}

	tm = *( utc ? gmtime ( &t ) : localtime ( &t ));
	tm. tm_isdst = 0;
	if(ioctl(rtc, RTC_SET_TIME, &tm ) < 0){
		printf ( "Could not set the RTC time err: %s\r\n", strerror(errno));
	}

	close(rtc);
}


int SYSTEM_Init()
{
	int BSizW = VIDEO_IS_960() ?VRES_SIZE_W_EF :VRES_SIZE_W_SD;
	int BSizH = VIDEO_IS_PAL() ?VRES_SIZE_H_PL :VRES_SIZE_H_NT;
	int ii    = 0;

	VB_CONF_S stVbConf   = {0};
	MPP_SYS_CONF_S stSysConf = {0};
	memset(&stVbConf, 0, sizeof(stVbConf));

#ifdef _NVR
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[0].u32BlkSize = 352 * 288 * 2;
	stVbConf.astCommPool[0].u32BlkCnt  = 4;
#elif defined(_EXT_HDDVR)
#if   (MAX_CAM_CH == 4) && defined(_HI3520D_EXT)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	//stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 2;
	//stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	//ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (VRES_SIZE_W_HD1080P/2) * VRES_SIZE_W_HD720P * 3 / 2;// 1280*720*2 , 1280*960*1.5( 1080* 960* 1.5)
	stVbConf.astCommPool[ii].u32BlkCnt  = 7 * MAX_CAM_CH - 1;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH - 2;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;
#elif (MAX_CAM_CH == 4)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD1080P * VRES_SIZE_H_HD1080P * 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 6 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;
#endif
#if   (MAX_CAM_CH == 8) && defined(SDK_PLATFORM_HI3521)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD1080P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (VRES_SIZE_W_HD1080P/2) * VRES_SIZE_H_HD1080P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 5 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2 * MAX_CAM_CH;
	ii ++;
#elif (MAX_CAM_CH == 8)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD1080P * VRES_SIZE_H_HD1080P * 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2 * MAX_CAM_CH;
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD1080P * VRES_SIZE_H_HD1080P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2 * MAX_CAM_CH;
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P* 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD1080P * VRES_SIZE_H_HD1080P * 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD1080P * VRES_SIZE_H_HD1080P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P* 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);
	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 1 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);
	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 1 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

#endif
#if   (MAX_CAM_CH == 16) && defined(SDK_PLATFORM_HI3531)
#ifdef _HVR
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	if (JCONF_BNC_CAM() > 0) {
		stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * ((JCONF_BNC_CAM()+1)/2*2)/2;
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * ((JCONF_BNC_CAM()+1)/2*2)/2;
		strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

		stVbConf.astCommPool[ii].u32BlkSize = (VRES_SIZE_W_HD1080P/2) * VRES_SIZE_H_HD1080P * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 5 * ((JCONF_BNC_CAM()+1)/2*2)/2;
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (VRES_SIZE_W_HD1080P/2) * VRES_SIZE_H_HD1080P * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 5 * ((JCONF_BNC_CAM()+1)/2*2)/2;
		strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * JCONF_BNC_CAM();
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * JCONF_BNC_CAM();
		ii ++;
	}

	if (JCONF_BNC_CAM()  == 0) {
		stVbConf.astCommPool[ii].u32BlkSize = (1920 * 1080) * 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4;
		ii ++;
		stVbConf.astCommPool[ii].u32BlkSize = (1280 * 960) * 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4;
		ii ++;
		stVbConf.astCommPool[ii].u32BlkSize = (720 * 576) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt	= 6;
		ii ++;
		stVbConf.astCommPool[ii].u32BlkSize = (352 * 288) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt	= 6;
		ii ++;
	}
#else
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 3 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = VRES_SIZE_W_HD720P * VRES_SIZE_H_HD720P * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW) * (BSizH) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;
#endif
#endif
	
#elif defined(_JA2024NC)
//	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 704 * 576 * 2; // 4*4CIF live vi0
//	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 4 * 16;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 4636 * 2; // 16CIF live must be YUV422 2bpp
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 12;
      ++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 704 * 576 * 2; // D1 live(1-16) vi1
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 8;
	++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 288 * 2; // 4CIF live vi3 (17-20)
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 8 * 4;
	++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 288 * 2; // 4CIF live vi4 (21-24)
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 8 * 4;
	++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 704 * 576 * 2; // 1D1 live vi3/4 (17-24)
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt  = 8;
	++stVbConf.u32MaxPoolCnt;

    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 288 * 3 / 2; // VENC main stream
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6 * MAX_CAM_CH;
	++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 288 * 3 / 2; // VENC sub stream
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6 * MAX_CAM_CH;
	++stVbConf.u32MaxPoolCnt;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkSize = 384 * 288 * 3 / 2; // VENC cell-phonme stream
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 6;
	++stVbConf.u32MaxPoolCnt;

#elif defined(_JA2032NC)

	stVbConf.u32MaxPoolCnt = 7;
	stVbConf.astCommPool[0].u32BlkSize = 384 * 4636 * 2; // 16CIF live must be YUV422 2bpp
	stVbConf.astCommPool[0].u32BlkCnt  = 12;
	stVbConf.astCommPool[1].u32BlkSize = 384 * 4636 * 2; // 16CIF live must be YUV422 2bpp
	stVbConf.astCommPool[1].u32BlkCnt  = 12;
	stVbConf.astCommPool[2].u32BlkSize = 704 * 576 * 2; // D1 live(1-8) vi1
	stVbConf.astCommPool[2].u32BlkCnt  = 5;
	stVbConf.astCommPool[3].u32BlkSize = 704 * 576 * 2; // D1 live(9-16) vi3
	stVbConf.astCommPool[3].u32BlkCnt  = 5;

    stVbConf.astCommPool[4].u32BlkSize = 384 * 288 * 2; // VENC main stream
    stVbConf.astCommPool[4].u32BlkCnt = 5 * MAX_CAM_CH;
	stVbConf.astCommPool[5].u32BlkSize = 384 * 288 * 2; // VENC sub stream
    stVbConf.astCommPool[5].u32BlkCnt = 5 * MAX_CAM_CH;
	stVbConf.astCommPool[6].u32BlkSize = 384 * 288 * 2; // VENC cell-phonme stream
    stVbConf.astCommPool[6].u32BlkCnt = 5;

    //stVbConf.astCommPool[7].u32BlkSize = 704 * 576 * 2; // D1 live(9-16) vi3
	//stVbConf.astCommPool[7].u32BlkCnt  = 2;

#elif defined(_JA7216CX) || defined(_JA7216NC)
	stVbConf.u32MaxPoolCnt = 6;
//	// from tongli
//	stVbConf.astCommPool[0].u32BlkSize = 3339968;
//	stVbConf.astCommPool[0].u32BlkCnt  = 10;
//	stVbConf.astCommPool[1].u32BlkSize = 813056;
//	stVbConf.astCommPool[1].u32BlkCnt  = 10;
//	stVbConf.astCommPool[2].u32BlkSize = 214016;
//	stVbConf.astCommPool[2].u32BlkCnt  = 40;
	// from yangyi
	stVbConf.astCommPool[0].u32BlkSize = 384 * 4636 * 2; // 16CIF live must be YUV422 2bpp
	stVbConf.astCommPool[0].u32BlkCnt  = 12;
	stVbConf.astCommPool[1].u32BlkSize = 704 * 576 * 2; // D1 live(1-8) vi1
	stVbConf.astCommPool[1].u32BlkCnt  = 5;
	stVbConf.astCommPool[2].u32BlkSize = 704 * 576 * 2; // D1 live(9-16) vi3
	stVbConf.astCommPool[2].u32BlkCnt  = 5;

    stVbConf.astCommPool[3].u32BlkSize = 384 * 288 * 2; // VENC main stream
    stVbConf.astCommPool[3].u32BlkCnt = 5 * MAX_CAM_CH;
	stVbConf.astCommPool[4].u32BlkSize = 384 * 288 * 2; // VENC sub stream
    stVbConf.astCommPool[4].u32BlkCnt = 5 * MAX_CAM_CH;
	stVbConf.astCommPool[5].u32BlkSize = 384 * 288 * 2; // VENC cell-phonme stream
    stVbConf.astCommPool[5].u32BlkCnt = 5;



#elif defined(_JA2016CX) || defined(_JA2016NC) || defined(_JA2016ICX)
	stVbConf.u32MaxPoolCnt = 6;
	stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 2; // D1 for full screen live
	stVbConf.astCommPool[0].u32BlkCnt  = 5 * (MAX_D1_CNT + 1);
	stVbConf.astCommPool[1].u32BlkSize = 352 * 288 * 2; // cif for live
	stVbConf.astCommPool[1].u32BlkCnt  = 5 * (MAX_CIF_CNT - MAX_D1_CNT);

	stVbConf.astCommPool[2].u32BlkSize = 704 * 576 * 3 / 2; // cif for main stream venc
	stVbConf.astCommPool[2].u32BlkCnt  = 4 * MAX_D1_CNT;
	stVbConf.astCommPool[3].u32BlkSize = 352 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[3].u32BlkCnt  = 4 * (MAX_CIF_CNT - MAX_D1_CNT);
	stVbConf.astCommPool[4].u32BlkSize = 352 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[4].u32BlkCnt  = 4 * MAX_CAM_CH;
	stVbConf.astCommPool[5].u32BlkSize = 352 * 288 * 3 / 2; // VENC cell-phonme stream
	stVbConf.astCommPool[5].u32BlkCnt  = 4;

#elif defined(_JA2008NC)
	stVbConf.u32MaxPoolCnt = 6;
	stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 2; // D1 for full screen live
	stVbConf.astCommPool[0].u32BlkCnt  = 5 * (MAX_D1_CNT + 1);
	stVbConf.astCommPool[1].u32BlkSize = 352 * 288 * 2; // cif for live
	stVbConf.astCommPool[1].u32BlkCnt  = 5 * (MAX_CIF_CNT - MAX_D1_CNT);

	stVbConf.astCommPool[2].u32BlkSize = 704 * 576 * 3 / 2; // cif for main stream venc
	stVbConf.astCommPool[2].u32BlkCnt  = 4 * MAX_D1_CNT;
	stVbConf.astCommPool[3].u32BlkSize = 352 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[3].u32BlkCnt  = 4 * (MAX_CIF_CNT - MAX_D1_CNT);
	stVbConf.astCommPool[4].u32BlkSize = 352 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[4].u32BlkCnt  = 4 * MAX_CAM_CH;
	stVbConf.astCommPool[5].u32BlkSize = 352 * 288 * 3 / 2; // VENC cell-phonme stream
	stVbConf.astCommPool[5].u32BlkCnt  = 4;
#elif defined(_JA9204RN)
	stVbConf.u32MaxPoolCnt = 5;
	stVbConf.astCommPool[0].u32BlkSize = 960 * 576 * 3 / 2; // D1 for full screen live
	stVbConf.astCommPool[0].u32BlkCnt  = 5 * (MAX_D1_CNT + 1);
	stVbConf.astCommPool[1].u32BlkSize = 480 * 288 * 3 / 2; // cif for live
	stVbConf.astCommPool[1].u32BlkCnt  = 5 * (MAX_CIF_CNT - MAX_D1_CNT);

	stVbConf.astCommPool[2].u32BlkSize = 960 * 576 * 3 / 2; // cif for main stream venc
	stVbConf.astCommPool[2].u32BlkCnt  = 4 * MAX_D1_CNT;
	stVbConf.astCommPool[3].u32BlkSize = 480 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[3].u32BlkCnt  = 4 * (MAX_CIF_CNT - MAX_D1_CNT);
	stVbConf.astCommPool[4].u32BlkSize = 480 * 288 * 3 / 2; // cif for sub stream venc
	stVbConf.astCommPool[4].u32BlkCnt  = 4 * MAX_CAM_CH;

#elif defined(_JA3116TW)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 1.5 * (MAX_D1_CNT) * (VIDEO_IS_PAL() ?1 :1.2f);
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * (MAX_D1_CNT + 1) * (VIDEO_IS_PAL() ?1 :1.2f);
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2.5 * (MAX_CAM_CH + 1) * (VIDEO_IS_PAL() ?1 :1.2f);
	ii ++;
	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 2 * (MAX_CAM_CH + 1) * (VIDEO_IS_PAL() ?1 :1.2f);
	strcpy(stVbConf.astCommPool[ii ++].acMmzName, MMZ_ZONE_NAME1);

#elif defined(_JA2104) || defined(_JA2106) || defined(_JA2116)
	#if   defined(_HI3520D_RAM_LITE) && (MAX_CAM_CH == 4)
	 	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
		stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_D1_CNT + 2;
	 	ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * (MAX_CIF_CNT - MAX_D1_CNT) + 3 * MAX_CAM_CH + 2;
	 	ii ++;
	#elif defined(_HI3520D_RAM_LITE) && (MAX_CAM_CH == 8)
		stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
		stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * (MAX_D1_CNT + 1);
	 	ii ++;
 
		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * (MAX_CIF_CNT - MAX_D1_CNT) + 2 * MAX_CAM_CH;
		ii ++;
	#elif defined(_HI3515A_EXT) && defined(_JA2104)
		stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
		stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 9 * (MAX_D1_CNT + 1);
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
		stVbConf.astCommPool[ii].u32BlkCnt  = 6 * (MAX_CIF_CNT - MAX_D1_CNT) + 10 * (MAX_CAM_CH);
		ii ++;
	#elif defined(_HI3520D_EXT) && defined(_JA2104)
		stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
		stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2; // D1 for full screen live
		stVbConf.astCommPool[ii].u32BlkCnt  = 7 * (MAX_D1_CNT + 1);
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2; // cif for live
		stVbConf.astCommPool[ii].u32BlkCnt  = 6 * (MAX_CIF_CNT - MAX_D1_CNT);
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2; // cif for live
		stVbConf.astCommPool[ii].u32BlkCnt  = 4 * (MAX_CAM_CH);
		ii ++;
	#else
		stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
		stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2; // D1 for full screen live
		stVbConf.astCommPool[ii].u32BlkCnt  = 5 * (MAX_D1_CNT + 1) + 3;
		ii ++;

		stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2; // cif for live
		stVbConf.astCommPool[ii].u32BlkCnt  = 6 * (MAX_CIF_CNT - MAX_D1_CNT) + 4 * (MAX_CAM_CH);
		ii ++;
	#endif
#elif defined(_JA2108) || defined(_JA2116D)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 12 * (MAX_D1_CNT + 1);
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 12 * (MAX_CIF_CNT - MAX_D1_CNT);
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2; // cif for live
	stVbConf.astCommPool[ii].u32BlkCnt  = 5 * (MAX_CAM_CH);
	ii ++;

#elif defined(_JA2124) || defined(_JA2132)
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[ii].u32BlkSize = 720 * BSizH * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 9 * ((MAX_CAM_CH/16) * 4);
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = BSizW * BSizH * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 6 * ((MAX_CAM_CH%16)/4)+6;
	ii ++;

	stVbConf.astCommPool[ii].u32BlkSize = (BSizW/2) * (BSizH/2) * 3 / 2;
	stVbConf.astCommPool[ii].u32BlkCnt  = 4 * MAX_CAM_CH;
	ii ++;

#else
	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;
	stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 3 / 2;
	stVbConf.astCommPool[0].u32BlkCnt  = 8 * (MAX_D1_CNT);
	stVbConf.astCommPool[1].u32BlkSize = 352 * 288 * 3 / 2;
	stVbConf.astCommPool[1].u32BlkCnt  = 8 * (MAX_CIF_CNT - MAX_D1_CNT) + 2;
#endif

	SYSTEM_Destroy();
	SYSTEM_SetTime(NULL);
	DVR_ASSERT(HI_MPI_VB_SetConf(&stVbConf));
	DVR_ASSERT(HI_MPI_VB_Init());
	stSysConf.u32AlignWidth = 16;
	DVR_ASSERT(HI_MPI_SYS_SetConf(&stSysConf));
	DVR_ASSERT(HI_MPI_SYS_Init());
	return 0;
}

void SYSTEM_Destroy()
{
	DVR_ASSERT(HI_MPI_SYS_Exit());
	DVR_ASSERT(HI_MPI_VB_Exit());
}

// if arg=NULL, time stamp will sync to system clock
int SYSTEM_SetTime(struct timeval* pstVal)
{
	int nRet = 0;
	unsigned long long u64Pts = 0;
	struct timeval stVal = {0};
	if(pstVal){
		stVal = *pstVal;
		nRet = settimeofday(pstVal, NULL);
		assert(0 == nRet);
		write_rtc(pstVal->tv_sec);
		sleep(2);
	}else{
		gettimeofday(&stVal, NULL);
	}
	u64Pts = stVal.tv_sec;
	u64Pts *= 1000000;
	u64Pts += stVal.tv_usec;
	DVR_ASSERT(HI_MPI_SYS_InitPtsBase(u64Pts));
	return 0;
}

// nPriority = (0, 99) low -> high
//
int SYSTEM_SetPriority(unsigned char bRealTime, int nPriority)
{
	int nRet = 0;
	struct sched_param stSched = {0};
	if(!(nPriority >= 0 && nPriority < 100)){
		return -1;
	}
	if(bRealTime){
		// real time
		stSched.sched_priority = nPriority;
		nRet = sched_setscheduler(getpid(), SCHED_RR, &stSched);
		if(0 != nRet){
			SYSTEM_TRACE("set realtime task @ %u error: %s", getpid(), strerror(errno));
			return -1;
		}
	}else{
		// normal task
		nPriority = (99 - nPriority) / 5;
//		stSched.sched_priority = nPriority;
//		// reset to normal task
//		nRet = sched_setscheduler(getpid(), SCHED_OTHER, &stSched);
//		if(0 != nRet){
//			SYSTEM_TRACE("set normal task @ %u error: %s", getpid(), strerror(errno));
//			return -1;
//		}
		// set priority
		nRet = setpriority(PRIO_PROCESS, getpid(), nPriority);
		if(0 != nRet){
			SYSTEM_TRACE("set normal task @ %u error: %s", getpid(), strerror(errno));
			return -1;
		}
	}
	return -1;
}

