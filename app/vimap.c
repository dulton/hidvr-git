
#include "conf.h"
#include "bsp.h"

int VIMAP_Get(int nChn)
{
	if((nChn >= MAX_CAM_CH) || (nChn < 0)) {
		return -1;
	}

#if (0)
	int VIMAP_TBL[MAX_CAM_CH] = { 6, 7, 0, 1, 4, 5, 2, 3, };

	return VIMAP_TBL[nChn];
#endif

#if (defined(_EXT_HDDVR) && (MAX_CAM_CH == 4))
#if defined(_EXT_HDDVR_LITE)
	int  VIMAP_TBL[4] = { 0, 2, 4, 6, };
#else
	int VIMAP_TBL[4] = { 4, 8, 0,12, };
#endif
	return VIMAP_TBL[nChn];
#endif

#if (defined(_EXT_HDDVR) && (MAX_CAM_CH == 8))
#if defined(_EXT_HDDVR_LITE)
	int VIMAP_TBL[8] = { 8, 9, 12, 13, 5, 4, 0, 1, };
#else
	int VIMAP_TBL[8] = { 12, 14, 0, 2, 8, 10, 4, 6,};
#endif
	return VIMAP_TBL[nChn];
#endif

#if (defined(_EXT_HDDVR) && (MAX_CAM_CH == 16))
	int VIMAP_TBL[MAX_CAM_CH] = { 14, 15, 2, 3, 10, 11, 6, 7, 4, 5, 8, 9, 0, 1, 12, 13, };

	return VIMAP_TBL[nChn];
#endif

	return nChn;
}
