/*
 * log.h
 *
 *  Created on: 2011-1-5
 *      Author: root
 */

#ifndef LOG_H_
#define LOG_H_


typedef enum _LOG_TYPE
{
	XLOG_TYPE_BOOT = 0,
	XLOG_TYPE_SHUTDOWN,
	XLOG_TYPE_MODIFY,
	XLOG_TYPE_RECORD,
	XLOG_TYPE_ALARM,
	XLOG_TYPE_WARNING,
}XLOG_TYPE;

#define MAX_LOG_MSG (24)

typedef struct
{
	int id;
	XLOG_TYPE type;
	time_t write_time;
	char msg[MAX_LOG_MSG];
}XLOG_MSG;

#define LOG_TMP_FILE "/tmp/dvr_log.dat"
#define LOG_TABLE "dvr_log"

void XLOG_init();
void XLOG_deinit();
int  XLOG_ready_for_work();
void XLOG_add(XLOG_TYPE _type, const char* _msg, time_t _time);
int XLOG_search(time_t _begin, time_t _end, int _type);
time_t XLOG_read_time();
void XLOG_write_time();
//int log_removeall();
//struct tm log_make_datetime(int _year, int _mon, int _day, int _hour, int _min, int _sec);
//int log_getcount(int _type, struct tm _start, struct tm _end);
//int log_get(int _type, struct tm _start, struct tm _end, int _page, int _pagesize, char*** _result, int* _row, int* _col);
//int log_get_free(char*** _result);

#endif /* LOG_H_ */
