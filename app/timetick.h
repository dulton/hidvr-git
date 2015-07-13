
#ifndef __TIMETICK_H__
#define __TIMETICK_H__

#include <time.h>

typedef void (*TimeTickTask)(void);

extern time_t TIMETICK_GetTime(time_t* ret_time);
extern unsigned int TIMETICK_GetSecOfDay(unsigned int* ret_time);
extern struct tm TIMETICK_GetTMTime(struct tm* ret_time);
extern const char* const TIMETICK_GetASCTime(char* ret_time);
extern int TIMETICK_GetCurDate(int* ret_nYear, int* ret_nMonth, int* ret_nDay);
extern int TIMETICK_GetCurTime(int* ret_nHour, int* ret_nMin, int* ret_nSec);
extern int TIMETICK_AddTask(TimeTickTask const funcTask, int const nTaskTick /* sec >= 1*/);
extern int TIMETICK_RemoveTask(TimeTickTask const funcTask);
extern void TIMETICK_RemoveAllTask();
extern unsigned int TIMETICK_GetTaskCnt();

#define TIME() TIMETICK_GetTime(NULL)
#define GETTIMEOFDAY() TIMETICK_GetSecOfDay(NULL)
#define LOCALTIME() TIMETICK_GetTMTime(NULL)
#define ASCTIME() TIMETICK_GetASCTime(NULL)
#define GETCURDATE(ret_nYear, ret_nMonth, ret_nDay) TIMETICK_GetCurDate(ret_nYear, ret_nMonth, ret_nDay)
#define GETCURTIME(ret_nHour, ret_nMin, ret_nSec) TIMETICK_GetCurTime(ret_nHour, ret_nMin, ret_nSec)

extern int TIMETICK_Init();
extern int TIMETICK_Destroy();

#endif //__TIMETICK_H__

