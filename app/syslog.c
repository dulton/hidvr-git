

//#define NDEBUG
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "timetick.h"
#include "syslog.h"

typedef struct SysLog SysLog;
struct SysLog
{
	// private
	int nSock;
	struct sockaddr_in stAddrIn;
	// public
	int (*Write)(SysLog* const thiz, const char* const szLog);
};

static int syslog_Write(SysLog* const thiz, const char* const szLog)
{
	if(!thiz->nSock){
		return -1;
	}
	if(szLog != 0 && strlen(szLog) > 0){
		char szTempMsg[1024] = {""};
		sprintf(szTempMsg, "%s @ %s\r\n", szLog, ASCTIME());
		sendto(thiz->nSock, szTempMsg, strlen(szTempMsg) + 1, 0, (struct sockaddr *)&(thiz->stAddrIn), strlen(szTempMsg) + 1);
	}
	return 0;
}

SysLog s_stSysLog =
{
	.nSock = 0,
	.Write = syslog_Write,
};

int SYSLOG_Init(const char* const szAddr, const unsigned short nPort)
{
	if(s_stSysLog.nSock > 0){
		return -1;
	}
	s_stSysLog.nSock  = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s_stSysLog.nSock > 0);
	memset(&(s_stSysLog.stAddrIn), 0, sizeof(struct sockaddr_in));
	s_stSysLog.stAddrIn.sin_family = AF_INET;
	s_stSysLog.stAddrIn.sin_port = htons(nPort ? nPort : SYSLOG_DEFAULT_PORT);
	s_stSysLog.stAddrIn.sin_addr.s_addr = inet_addr(szAddr ? szAddr : SYSLOG_DEFAULT_ADDR);
	return 0;
}

void SYSLOG_Destroy()
{
	if(!s_stSysLog.nSock){
		return;
	}
	close(s_stSysLog.nSock);
	s_stSysLog.nSock = 0;
}

int SYSLOG_Write(char* const szLog)
{
	return s_stSysLog.Write(&s_stSysLog, szLog);
}





