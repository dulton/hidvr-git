#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env_common.h"
#include "generic.h"
#include "app_debug.h"


/********************************************************************************
* external interfaces
*********************************************************************************/

int NVP_env_load(lpNVP_ENV env, const char *module, int keyid)
{
	char temp[512];
	char *ptr = NULL, *pbuf;
	char *saveptr = NULL;
	int ret;
	int chn, id;

	chn = keyid/100;
	id = keyid%100;
	strncpy(temp, module, 512);
	pbuf = temp;
	while((ptr = strtok_r(pbuf, OM_AND, &saveptr)) != NULL)
	{
		if (OM_MATCH(ptr, OM_ALL)) {
			break;
		}else if (OM_MATCH(ptr, OM_PROFILE)) {
		}else if (OM_MATCH(ptr, OM_PROFILES)) {
		}else if (OM_MATCH(ptr, OM_INFO)) {
		}else if (OM_MATCH(ptr, OM_DTIME)) {
		}else if (OM_MATCH(ptr, OM_NET)) {
		}  else if (OM_MATCH(ptr, OM_VENC)) {
		}  else if (OM_MATCH(ptr, OM_VSRC)) {
		}  else if (OM_MATCH(ptr, OM_VINC)) {
		}  else if (OM_MATCH(ptr, OM_AENC)) {
		}  else if (OM_MATCH(ptr, OM_AIN)) {
		}  else if (OM_MATCH(ptr, OM_COLOR)) {
		}  else if (OM_MATCH(ptr, OM_IMG)) {
		}  else if (OM_MATCH(ptr, OM_MD)) {
		}  else if (OM_MATCH(ptr, OM_PTZ)) {
		} else {
			APP_TRACE("unknown env module: %s", ptr);
		}
		//
		pbuf = NULL;
	}

	return 0;
}

int NVP_env_save(lpNVP_ENV env, const char *module, int keyid)
{
	char temp[512];
	char *ptr = NULL, *pbuf;
	char *saveptr = NULL;
	int ret;
	int f_ret = 0;
	int chn, id;

	chn = keyid / 100;
	id = keyid % 100;
	strncpy(temp, module, 512);
	pbuf = temp;
	while((ptr = strtok_r(pbuf, OM_AND, &saveptr)) != NULL)
	{
		if (OM_MATCH(ptr, OM_ALL)) {
			break;
		}else if (OM_MATCH(ptr, OM_PROFILE)) {
		}else if (OM_MATCH(ptr, OM_PROFILES)) {
		}else if (OM_MATCH(ptr, OM_INFO)) {
			//
		}else if (OM_MATCH(ptr, OM_DTIME)) {
		}else if (OM_MATCH(ptr, OM_NET)) {
		}  else if (OM_MATCH(ptr, OM_VENC)) {
		}  else if (OM_MATCH(ptr, OM_VSRC)) {
		}  else if (OM_MATCH(ptr, OM_VINC)) {
		}  else if (OM_MATCH(ptr, OM_AENC)) {
		}  else if (OM_MATCH(ptr, OM_AIN)) {
		}  else if (OM_MATCH(ptr, OM_COLOR)) {
		}  else if (OM_MATCH(ptr, OM_IMG)) {
		}  else if (OM_MATCH(ptr, OM_MD)) {
		}  else if (OM_MATCH(ptr, OM_PTZ)) {
		} else {
			APP_TRACE("unknown env module: %s", ptr);
			f_ret = -1;
		}

		if (ret < 0)
			f_ret = -1;

		//
		pbuf = NULL;
	}

	return f_ret;
}

int NVP_env_cmd(lpNVP_CMD cmd, const char *module, int keyid)
{
	char temp[512];
	char *ptr = NULL, *pbuf;
	char *saveptr = NULL;
	int ret;
	int f_ret = 0;
	int chn, id;

	chn = keyid / 100;
	id = keyid % 100;
	strncpy(temp, module, 512);
	pbuf = temp;
	while((ptr = strtok_r(pbuf, OM_AND, &saveptr)) != NULL)
	{
		if (OM_MATCH(ptr, OM_REBOOT)) {
		}else if (OM_MATCH(ptr, OM_SYS_RESET)) {
			APP_TRACE("unknown env module: %s", ptr);
		}else if (OM_MATCH(ptr, OM_PTZ)) {
		} else {
			APP_TRACE("unknown env module: %s", ptr);
			f_ret = -1;
		}

		if (ret < 0)
			f_ret = -1;

		//
		pbuf = NULL;
	}

	return f_ret;
}

