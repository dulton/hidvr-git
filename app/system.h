

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <time.h>

extern int SYSTEM_Init();
extern void SYSTEM_Destroy();
extern int SYSTEM_SetTime(struct timeval* pstVal);
// task priority
extern int SYSTEM_SetPriority(unsigned char bRealTime, int nPriority);

#endif //__SYSTEM_H__

