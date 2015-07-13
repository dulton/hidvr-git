#ifndef	__DDNS_H__
#define	__DDNS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "environment.h"

#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <resolv.h>

void XDDNS_init();
void* ddns_proc(void* arg);
void XDDNS_deinit();
void XDDNS_update();
int XDDNS_test( int provider_num, char *host, char *usr, char *pwd );
int GetDDNSProvider(int ProviderNum, char Provider[DDNS_URL_LEN]);
int GetDDNSProviderCount();

#ifdef __cplusplus
};
#endif

#endif

