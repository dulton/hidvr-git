
#ifndef __SYSLOG_H__
#define __SYSLOG_H__


#define SYSLOG_DEFAULT_ADDR "192.168.1.250"
#define SYSLOG_DEFAULT_PORT (8080)

extern int SYSLOG_Init(const char* const szAddr, const unsigned short nPort);
extern void SYSLOG_Destroy();


extern int SYSLOG_Write(char* const szLog);


#endif //__SYSLOG_H__
