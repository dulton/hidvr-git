
//#define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "timetick.h"

#define DEBUG_TIMETICK
#ifdef DEBUG_TIMETICK
#define TIMETICK_TRACE(fmt...)	\
	do{printf("\033[0;37m""TIMETICK->[%s]:%d ", __FILE__, __LINE__);printf(fmt);printf("\033[m\r\n");}while(0)
#else
#define TIMETICK_TRACE(fmt...)
#endif
#define TIMETICK_MAX_TASK (16)


typedef struct TimeTick TimeTick;
struct TimeTick
{
	// private
	time_t lNow;
	struct tm tmNow;
	char szNow[30];
	unsigned int nSecOfDay;
	unsigned char bDaemon;
	pthread_t tDaemon;
	int nTaskCnt;
	int nTaskTick[TIMETICK_MAX_TASK]; // mark task period
	int nTaskInterval[TIMETICK_MAX_TASK];
	TimeTickTask funcTask[TIMETICK_MAX_TASK]; // keep task function pointer
};

static TimeTick s_stTimetick = 
{
	// private
	.bDaemon = 0,
	.tDaemon = (pthread_t)NULL,
	.lNow = (time_t)0,
	.tmNow = {0},
	.szNow = {""},
	.nSecOfDay = 0,
	.nTaskCnt = 0,
};

time_t TIMETICK_GetTime(time_t* ret_time)
{
	if(ret_time){
		memcpy(ret_time, &(s_stTimetick.lNow), sizeof(s_stTimetick.lNow));
	}
	return s_stTimetick.lNow;
}

unsigned int TIMETICK_GetSecOfDay(unsigned int* ret_time)
{
	if(ret_time){
		memcpy(ret_time, &(s_stTimetick.nSecOfDay), sizeof(s_stTimetick.nSecOfDay));
	}
	return s_stTimetick.nSecOfDay;
}

struct tm TIMETICK_GetTMTime(struct tm* ret_time)
{
	if(ret_time){
		memcpy(ret_time, &(s_stTimetick.tmNow), sizeof(s_stTimetick.tmNow));
	}
	return s_stTimetick.tmNow;
}

const char* const TIMETICK_GetASCTime(char* ret_time)
{
	if(ret_time){
		strcpy(ret_time, s_stTimetick.szNow);
	}
	return s_stTimetick.szNow;
}

int TIMETICK_GetCurDate(int* ret_nYear, int* ret_nMonth, int* ret_nDay)
{
	if(ret_nYear){
		*ret_nYear = s_stTimetick.tmNow.tm_year + 1900;
	}
	if(ret_nMonth){
		*ret_nMonth = s_stTimetick.tmNow.tm_mon + 1;
	}
	if(ret_nDay){
		*ret_nDay = s_stTimetick.tmNow.tm_mday;
	}
	return 0;
}

int TIMETICK_GetCurTime(int* ret_nHour, int* ret_nMin, int* ret_nSec)
{
	if(ret_nHour){
		*ret_nHour = s_stTimetick.tmNow.tm_hour + 1900;
	}
	if(ret_nMin){
		*ret_nMin = s_stTimetick.tmNow.tm_min + 1;
	}
	if(ret_nSec){
		*ret_nSec = s_stTimetick.tmNow.tm_sec;
	}
	return 0;
}

static void* timetick_Daemon(void* arg)
{
	time_t lLastTick = 0;
	TIMETICK_TRACE("TIMETICK pid=%d tid=%x begin", getpid(), (unsigned int)pthread_self());
	s_stTimetick.bDaemon = TRUE;
	do
	{
		s_stTimetick.lNow = time(NULL);
		localtime_r(&s_stTimetick.lNow, &s_stTimetick.tmNow);
		asctime_r(&s_stTimetick.tmNow, s_stTimetick.szNow);
		s_stTimetick.nSecOfDay = s_stTimetick.tmNow.tm_hour * 3600 + s_stTimetick.tmNow.tm_min * 60 + s_stTimetick.tmNow.tm_sec;
		sprintf(s_stTimetick.szNow, "%04d/%02d/%02d %02d:%02d:%02d",
			s_stTimetick.tmNow.tm_year + 1900,
			s_stTimetick.tmNow.tm_mon + 1,
			s_stTimetick.tmNow.tm_mday,
			s_stTimetick.tmNow.tm_hour,
			s_stTimetick.tmNow.tm_min,
			s_stTimetick.tmNow.tm_sec);
		// task management
		if(lLastTick != s_stTimetick.lNow){
			int i = 0;
			TimeTickTask CurrentTask = NULL;
			lLastTick = s_stTimetick.lNow;
			// next tick
			for(i = 0; i < TIMETICK_MAX_TASK && s_stTimetick.bDaemon; ++i){
				if((CurrentTask = s_stTimetick.funcTask[i]) && (0 ==  --s_stTimetick.nTaskTick[i])){
					CurrentTask();
					s_stTimetick.nTaskTick[i] = s_stTimetick.nTaskInterval[i];
				}
			}
		}
		usleep(200000);
	}while(s_stTimetick.bDaemon);
	TIMETICK_TRACE("TIMETICK pid=%d tid=%x end", getpid(), (unsigned int)pthread_self());
	pthread_exit(NULL);
}

static int timetick_DaemonStart()
{
	int nRet = 0;
	pthread_t tid = (pthread_t)NULL;
	if(s_stTimetick.tDaemon){
		TIMETICK_TRACE("timetick has been inited");
		return -1;
	}
	nRet = pthread_create(&tid, 0, timetick_Daemon, NULL);
	s_stTimetick.tDaemon = tid;
	assert(0 == nRet);
	usleep(5000); // 5ms
	return 0;
}

static int timetick_DaemonStop()
{
	if(!s_stTimetick.tDaemon){
		TIMETICK_TRACE("timetick never inited");
		return -1;
	}
	s_stTimetick.bDaemon = FALSE;
	pthread_join(s_stTimetick.tDaemon, NULL);
	s_stTimetick.tDaemon = (pthread_t)NULL;
	return 0;
}

int TIMETICK_AddTask(TimeTickTask const funcTask, int const nTaskTick /* sec >= 1*/)
{
	int i = 0;
	if(!funcTask){
		return -1;
	}
	if(TIMETICK_MAX_TASK == TIMETICK_GetTaskCnt()){
		TIMETICK_TRACE("task list is full");
		return -1;
	}
	// scan one time to avoid duplicate task
	for(i = 0; i < TIMETICK_MAX_TASK; ++i){
		if(s_stTimetick.funcTask[i] == funcTask){
			TIMETICK_TRACE("duplicate task");
			return -1;
		}
	}
	for(i = 0; i < TIMETICK_MAX_TASK; ++i){
		if(NULL == s_stTimetick.funcTask[i]){
			// find 1st position
			s_stTimetick.nTaskInterval[i] = nTaskTick > 0 ? nTaskTick : 1; // keep >= 1
			s_stTimetick.nTaskTick[i] = s_stTimetick.nTaskInterval[i];
			s_stTimetick.funcTask[i] = funcTask;
			++s_stTimetick.nTaskCnt;
			TIMETICK_TRACE("add a task @ %d interval %d", i, s_stTimetick.nTaskTick[i]);
			return 0;
		}
	}
	return -1;
}

int TIMETICK_RemoveTask(TimeTickTask const funcTask)
{
	int i = 0;
	if(!TIMETICK_GetTaskCnt()){
		TIMETICK_TRACE("task list is empty");
		return -1;
	}
	for(i = 0; i < TIMETICK_MAX_TASK; ++i){
		if(!funcTask){
			int nTaskId = 0;
			if(s_stTimetick.funcTask[(nTaskId = TIMETICK_MAX_TASK - i - 1)]){
				s_stTimetick.nTaskTick[nTaskId] = 0;
				s_stTimetick.nTaskInterval[nTaskId] = 0;
				s_stTimetick.funcTask[nTaskId] = NULL;
				--s_stTimetick.nTaskCnt;
				TIMETICK_TRACE("remove a task @ %d", nTaskId);
				return 0;
			}
		}else{
			if(s_stTimetick.funcTask[i] == funcTask){
				s_stTimetick.nTaskTick[i] = 0;
				s_stTimetick.nTaskInterval[i] = 0;
				s_stTimetick.funcTask[i] = NULL;
				--s_stTimetick.nTaskCnt;
				TIMETICK_TRACE("remove a task @ %d", i);
				return 0;
			}
		}
	}
	return -1;
}

void TIMETICK_RemoveAllTask()
{
	int i = 0;
	if(!TIMETICK_GetTaskCnt()){
		TIMETICK_TRACE("task list is empty");
		return;
	}
	for(i = 0; i < TIMETICK_MAX_TASK; ++i){
		if(s_stTimetick.funcTask[i]){
			s_stTimetick.funcTask[i] = NULL;
			s_stTimetick.nTaskTick[i] = 0;
			s_stTimetick.nTaskInterval[i] = 0;
			--s_stTimetick.nTaskCnt;
			TIMETICK_TRACE("remove task @ %d", i);
		}
	}
	assert(0 == s_stTimetick.nTaskCnt);
}

unsigned int TIMETICK_GetTaskCnt()
{
	return s_stTimetick.nTaskCnt;
}

int TIMETICK_Init()
{
	TIMETICK_RemoveAllTask(&s_stTimetick);
	return timetick_DaemonStart();
}

int TIMETICK_Destroy()
{
	TIMETICK_RemoveAllTask(&s_stTimetick);
	return timetick_DaemonStop();
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
//#define TEST_TIMETICK
#ifdef TEST_TIMETICK
static void Task1Sec()
{
	time_t t = time(NULL);
	printf("task 1 second -> %s", ctime(&t));
}

static void Task5Sec()
{
	time_t t = time(NULL);
	printf("task 5 second -> %s", ctime(&t));
}

static void Task10Sec()
{
	time_t t = time(NULL);
	printf("task 10 second -> %s", ctime(&t));
}

static void Task100Sec()
{
	time_t t = time(NULL);
	printf("task 100 second -> %s", ctime(&t));
}

static void Task500Sec()
{
	time_t t = time(NULL);
	printf("task 500 second -> %s", ctime(&t));
}

#include <signal.h>

static unsigned char bTimeTick = 1;
static void Ouch(int sig)
{
	printf("Ouch!!\r\n");
	bTimeTick = 0;
}
int main(int argc, char** argv)
{
	(void)signal(SIGINT, Ouch);
	TIMETICK_Init();
	TIMETICK_AddTask(Task1Sec, 1);
	TIMETICK_AddTask(Task5Sec, 5);
	TIMETICK_AddTask(Task10Sec, 10);
	TIMETICK_AddTask(Task100Sec, 100);
	TIMETICK_AddTask(Task500Sec, 500);
	while(bTimeTick){
		usleep(10000);
	}
	TIMETICK_Destroy();
	return 0;
}
#endif


