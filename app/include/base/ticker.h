
//
// a timer task module
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#ifndef TICKER_H_
#define TICKER_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct timeval ST_TICKER_TIME,*LP_TICKER_TIME;
extern LP_TICKER_TIME TICKER_time_in();
extern int TICKER_time_out(LP_TICKER_TIME ticker_time);

typedef void (*fTICKER_HANDLER)(void);
typedef void (*fDEALY_HANDLER1)(uint32_t lParam, uint32_t rParam);
typedef void (*fDEALY_HANDLER2)(int lParam, void *rParam);
extern int TICKER_add_task(fTICKER_HANDLER const task_handler, time_t interl, bool immediate);
extern void TICKER_del_task(fTICKER_HANDLER const task_handler);

extern int TICKER_add_delay_task(fDEALY_HANDLER1 const task_handler, time_t delay, uint32_t hash, 
	uint32_t lParam, uint32_t lSize, uint32_t rParam, uint32_t rSize);
extern int TICKER_add_delay_task2(fDEALY_HANDLER2 const task_handler, time_t delay, uint32_t hash, 
	int lparam, uint32_t rParam, uint32_t rSize);


extern int TICKER_init();
extern void TICKER_destroy();


#ifdef __cplusplus
};
#endif
#endif //TICKER_H_

