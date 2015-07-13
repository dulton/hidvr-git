


#ifndef	__ENVIRONMENT_C__
#define	__ENVIRONMENT_C__


//#define NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>

#include "common.h"
#include "para.h"
#include "environment.h"
#include "inifile.h"
#include "../app/bsp.h"


char* ENC_POSITION_MODE_STRING[] =
{
		"CUSTOM",
		"UPPER_LEFT",
		"JUST_ABOVE",
		"UPPER_RIGHT",
		"BOTTOM_LEFT",
		"JUST_BELOW",
		"BOTTOM_RIGHT",
};

char* DATE_FMT_STRING[] =
{	
		"YYYY_MM_DD",
		"MM_DD_YYYY",
		"DD_MM_YYYY"
};
char* TIME_FORMAT_STRING[] = 
{
	"TIMEFORMAT_X12",
	"TIMEFORMAT_X24",
		
};
char* VOUT_STANDARD_STRING[] =
{
		"VOUT_PAL",
		"VOUT_NTSC"
};
char* KEYMAP_TYPE_STRING[] =
{
		"KEYMAP_TYPE0",
		"KEYMAP_TYPE1",
		"KEYMAP_TYPE2",
		"KEYMAP_TYPE3",
};
char* REMOTE_TYPE_STRING[] =
{
		"REMOTE_TYPE0",
		"REMOTE_TYPE1",
};
char* WORLDWIDE_LANGUAGE_STRING[] =
{
		"WORLDWIDE_ENGLISH",
		"WORLDWIDE_CHINESE_MAINLAND",
		"WORLDWIDE_CHINESE_HONGKONG",
		"WORLDWIDE_CHINESE_TAIWAN",
		"WORLDWIDE_KOREAN",
		"WORLDWIDE_JAPANESE",
		"WORLDWIDE_ARABIC",
		"WORLDWIDE_PERSIAN",
		"WORLDWIDE_HEBREW",
		"WORLDWIDE_PORTUGUESE",
		"WORLDWIDE_SPANISH",
		"WORLDWIDE_HUNGARIAN",
		"WORLDWIDE_RUSSIAN",
		"WORLDWIDE_FRENCH",
		"WORLDWIDE_GERMAN",
		"WORLDWIDE_THAI",
		"WORLDWIDE_VIETNAMESE",
		"WORLDWIDE_TURKISH",
		"WORLDWIDE_ITALIAN",
		"WORLDWIDE_DANISH",
};
char* ENV_BOOLEAN_STRING[] =
{
		"ENV_FALSE",
		"ENV_TRUE",
};
char* ENC_MODE_STRING[] =
{
		"MODE_VIDEO",
		"MODE_AUDIO_VIDEO",
};
char* ENC_FMT_STRING[] =
{
		"ENC_FMT_QCIF",
		"ENC_FMT_CIF",
		"ENC_FMT_HD1",
		"ENC_FMT_D1",
		"ENC_FMT_WCIF",
		"ENC_FMT_960H",
		"ENC_FMT_HD720P",
		"ENC_FMT_HD1080P",
};
char* ENC_BR_VALUE_STRING[] =
{
		"ENC_BR_64KPS",
		"ENC_BR_128KPS",
		"ENC_BR_256KPS",
		"ENC_BR_384KPS",
		"ENC_BR_512KPS",
		"ENC_BR_768KPS",
		"ENC_BR_1MPS",
		"ENC_BR_1P5MPS",
		"ENC_BR_2MPS",
		"ENC_BR_3MPS",
		"ENC_BR_4MPS",
		"ENC_BR_5MPS",
		"ENC_BR_6MPS",
		"ENC_BR_8MPS",
		"ENC_BR_10MPS",
};
char* RANK_STRING[] =
{
		"RANK_HIGHEST",
		"RANK_HIGH",
		"RANK_MEDIUM",
		"RANK_LOW",
		"RANK_LOWEST",
};
char* ENC_BR_MODE_STRING[] =
{
		"ENC_BR_VARIABLE",
		"ENC_BR_CONSTANT",
		"ENC_BR_AVERAGE",
};
char* ENC_FRAMERATE_STRING[] =
{
		"FR_1FPS",
		"FR_2FPS",
		"FR_3FPS",
		"FR_4FPS",
		"FR_5FPS",
		"FR_6FPS",
		"FR_7FPS",
		"FR_8FPS",
		"FR_9FPS",
		"FR_10FPS",
		"FR_11FPS",
		"FR_12FPS",
		"FR_13FPS",
		"FR_14FPS",
		"FR_15FPS",
		"FR_16FPS",
		"FR_17FPS",
		"FR_18FPS",
		"FR_19FPS",
		"FR_20FPS",
		"FR_21FPS",
		"FR_22FPS",
		"FR_23FPS",
		"FR_24FPS",
		"FR_25FPS",
		"FR_30FPS",
};

char* SMTP_ENCRYPT_TYPE_STRING[] =
{
		"SMTP_ENCRYPT_NONE",
		"SMTP_ENCRYPT_SSL",
};

char* AUTOSWI_INTERVAL_STRING[] =
{
		"AUTOSWI_2SEC",
		"AUTOSWI_3SEC",
		"AUTOSWI_4SEC",
		"AUTOSWI_5SEC",
		"AUTOSWI_8SEC",
		"AUTOSWI_10SEC",
};
char* AUTOSWI_MODE_STRING[] =
{
		"AUTOSWI_X1",
		"AUTOSWI_X4",
		"AUTOSWI_X9",
		"AUTOSWI_X16",
};
char* VGA_RESOLUTION_STRING[] =
{
		"VGA_800x600",
		"VGA_1024x768",
		"VGA_1280x1024",
		"VGA_1366x768",
		"VGA_1440x900",
		"VGA_1920x1080P50Hz",
		"VGA_1920x1080P60Hz",
};
char* ALARM_DURATION_STRING[] =
{
		"ALARM_1SEC",
		"ALARM_2SEC",
		"ALARM_3SEC",
		"ALARM_4SEC",
		"ALARM_5SEC",
		"ALARM_8SEC",
		"ALARM_10SEC",
		"ALARM_CONT",
};
char* SENSOR_WORKMODE_STRING[] =
{
		"SENSOR_DISABLE",
		"SENSOR_NOPEN",
		"SENSOR_NCLOSE",
};
char* PTZ_PROTOCAL_STRING[] =
{
		"PELCO_D",
		"PELCO_P",
};
char* PTZ_BAUDRATE_STRING[] =
{
		"BD_2400",
		"BD_4800",
		"BD_9600",
		"BD_19200",
		"BD_38400",
		"BD_57600",
		"BD_115200",
};
char* IPCAM_PROTOCOL_NAME_STRING[] = {
	"N1",
	"ONVIF",
	"RTSP",
	"ZAVIO",
	"TST", //"Tianshitong"
	"VOC",
	"N1+ONVIF"
	"All",
};
char* VIDEO_MODE_STRING[] = {
	"VIDEO_MODE_960H",
	"VIDEO_MODE_720P",
	"VIDEO_MODE_HALF1080P",
	"VIDEO_MODE_1080P"
	"VIDEO_MODE_IPCAM",
};

char * Maintenance_Mode_STRING[] = {
    "Maintenance_Never",
    "Maintenance_Once",
    "Maintenance_Everyday",
    "Maintenance_Everyweek",
    "Maintenance_Test",
};

static char* _env_default_get_conf = "/tmp/dvr_resource_dir/environment.conf";
static char* _env_default_get_null = "xlidy3xldisd";
static int _env_default_get_open_count = 0;
static void _env_default_get_init()
{
	if(_env_default_get_open_count == 0)
	{
		OpenIniFile(_env_default_get_conf);
	}
	_env_default_get_open_count++;
}

#define _SET_INT(_target, _sec, _key, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		_target = _default; \
	} \
	else \
	{ \
		_target = atoi(value); \
	} \
}

#define _SET_STRING(_target, _sec, _key, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		strcpy(_target, _default); \
	} \
	else \
	{ \
		strcpy(_target, value); \
	} \
}

#define _SET_ENUM(_target, _sec, _key, _type, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		_target = _default; \
	} \
	else \
	{ \
		int __i; \
		for(__i = 0; __i < _type##_CNT; __i++) \
		{ \
			if(strcasecmp(_type##_STRING[__i], value) == 0) \
			{ \
				_target = (_type)__i; \
			} \
		} \
	} \
}

#define _SET_TIME(_target_h, _target_m, _sec, _key, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	int tmp[2]; \
	_target_h = 0; \
	_target_m = 0; \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		if(sscanf(_default, "%d:%d", &tmp[0], &tmp[1]) == 2) \
		{ \
			_target_h = tmp[0]; \
			_target_m = tmp[1]; \
		} \
	} \
	else \
	{ \
		if(sscanf(value, "%d:%d", &tmp[0], &tmp[1]) == 2) \
		{ \
			_target_h = tmp[0]; \
			_target_m = tmp[1]; \
		} \
	} \
}

#define _SET_IP(_target, _sec, _key, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	int tmp[4]; \
	_target[0] = 0; \
	_target[1] = 0; \
	_target[2] = 0; \
	_target[3] = 0; \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		if(sscanf(_default, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) == 4) \
		{ \
			_target[0] = tmp[0]; \
			_target[1] = tmp[1]; \
			_target[2] = tmp[2]; \
			_target[3] = tmp[3]; \
		} \
	} \
	else \
	{ \
		if(sscanf(value, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) == 4) \
		{ \
			_target[0] = tmp[0]; \
			_target[1] = tmp[1]; \
			_target[2] = tmp[2]; \
			_target[3] = tmp[3]; \
		} \
	} \
}

#define _SET_MAC(_target, _sec, _key, _default) \
{ \
	assert(_env_default_get_open_count>0); \
	int tmp[6]; \
	_target[0] = 0; \
	_target[1] = 0; \
	_target[2] = 0; \
	_target[3] = 0; \
	_target[4] = 0; \
	_target[5] = 0; \
	char* value = (char*)ReadString(_sec, _key, _env_default_get_null); \
	if(value == _env_default_get_null) \
	{ \
		if(sscanf(_default, "%x:%x:%x:%x:%x:%x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) == 6) \
		{ \
			_target[0] = tmp[0]; \
			_target[1] = tmp[1]; \
			_target[2] = tmp[2]; \
			_target[3] = rand() % 200; \
			_target[4] = rand() % 200; \
			_target[5] = rand() % 200; \
		} \
	} \
	else \
	{ \
		if(sscanf(value, "%x:%x:%x:%x:%x:%x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) == 6) \
		{ \
			_target[0] = tmp[0]; \
			_target[1] = tmp[1]; \
			_target[2] = tmp[2]; \
			_target[3] = rand() % 200; \
			_target[4] = rand() % 200; \
			_target[5] = rand() % 200; \
		} \
	} \
}

static void _env_default_get_destory()
{
	_env_default_get_open_count--;
	if(_env_default_get_open_count == 0)
	{
		CloseIniFile();
	}
}

struct tm GetBuildDateTime()
{
	struct tm tm_Build;
	memset(&tm_Build, 0, sizeof(tm_Build));
	const unsigned char MonthStr[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov","Dec"};
	unsigned char temp_str[4] = {0, 0, 0, 0}, i = 0;
	sscanf(__DATE__, "%s %2d %4d", temp_str, &(tm_Build.tm_mday), &(tm_Build.tm_year));
	sscanf(__TIME__, "%2d:%2d:%2d", &(tm_Build.tm_hour), &(tm_Build.tm_min), &(tm_Build.tm_sec));
	for (i = 0; i < 12; i++){
		if (temp_str[0] == MonthStr[i][0] && temp_str[1] == MonthStr[i][1] && temp_str[2] == MonthStr[i][2])
		{
			tm_Build.tm_mon = i;
			break;
		}
	}
	tm_Build.tm_year -= 1900;
	return tm_Build;
}

static int env_Read(int offset, void* buf, ssize_t size)
{
	int ret = 0;
	int fd = -1;
	if((fd = open(ENVIRONMENT_DEV, O_RDONLY)) < 0){
		printf("open \""ENVIRONMENT_DEV"\" failed!\r\n");
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	ret = read(fd, buf, size);
	if(size != ret){
		printf("read(%d) \""ENVIRONMENT_DEV"\" error!\r\n", ret);
		return -1;
	}
	close(fd);
	fd = -1;
	return ret;
}

static int env_Write(int offset, void* buf, ssize_t size)
{
	int ret = 0;
	int fd = -1;
	if((fd = open(ENVIRONMENT_DEV, O_WRONLY)) < 0){
		printf("open \""ENVIRONMENT_DEV"\" failed!\r\n");
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	ret = write(fd, buf, size);
	if(size != ret){
		printf("write(%d) \""ENVIRONMENT_DEV"\" error!\r\n", ret);
		return -1;
	}
	close(fd);
	fd = -1;
	return ret;
}

// operations
static void GetDevName(Sysenv* const thiz, char* ret_szDevName)
{
	if(ret_szDevName){
		const char* pName = PARA_GetDevName();
		if(pName){
			strcpy(ret_szDevName, pName);
		}
	}
}

static void GetDevModel(Sysenv* const thiz, char* ret_szDevModel)
{
	if(ret_szDevModel){
		const char* pModel = PARA_GetDevModel();
		if(pModel){
			strcpy(ret_szDevModel, pModel);
		}
	}
}

static void GetHWVersion(Sysenv* const thiz, int* ret_nMaj, int* ret_nMin, int* ret_nRev, char* ret_szExt)
{
	if(ret_nMaj){ *ret_nMaj = HWVER_MAJ; }
	if(ret_nMin){ *ret_nMin = HWVER_MIN; }
	if(ret_nRev){ *ret_nRev = HWVER_REV; }
	if(ret_szExt){ strcpy(ret_szExt, HWVER_EXT); }
}

static void GetSWVersion(Sysenv* const thiz, int* ret_nMaj, int* ret_nMin, int* ret_nRev, char* ret_szExt)
{
	if(ret_nMaj){ *ret_nMaj = SWVER_MAJ; }
	if(ret_nMin){ *ret_nMin = SWVER_MIN; }
	if(ret_nRev){ *ret_nRev = SWVER_REV; }
	char *tmp = SWVER_EXT;
	if(strlen(tmp) <= 0){
		tmp = "0";
	}
	if(ret_szExt){ sprintf(ret_szExt, "%s_%s", tmp, DEVICE_FEATURE_NO); }
}

static struct tm GetRelDateTime(Sysenv* const thiz){
	return GetBuildDateTime();
}

// crc
static ENV_BOOLEAN IsTheSameAs(Sysenv* const thiz, Sysenv* const other){
	return (0 == memcmp(&thiz->stAttr.stContent, &other->stAttr.stContent, sizeof(thiz->stAttr.stContent)) ? ENV_TRUE : ENV_FALSE );
}

static ENV_BOOLEAN Compare(Sysenv* const thiz, Sysenv* other, const char* item)
{
#define COMPARE(ssrc,dst1,dst2) (((0==strcasecmp(ssrc,dst1))&&(strlen(ssrc)==strlen(dst1)))||((0==strcmp(ssrc,dst2))&&(strlen(ssrc)==strlen(dst2))))
	// volume
	// color
	// misc
	// encode
	// record
	// network
	// screen
	// detection
	// ptz
	// sensor
	// hdd
	void* src_offset = NULL;
	void* dst_offset = NULL;
	int size = 0;
	if(COMPARE(item, "color", "col")){
		src_offset = &(thiz->stAttr.stContent.stColor);
		dst_offset = &(other->stAttr.stContent.stColor);
		size = sizeof(SysenvColor);
	}else if(COMPARE(item, "misc", "miscellaneous")){
		src_offset = &(thiz->stAttr.stContent.stGeneral);
		dst_offset = &(other->stAttr.stContent.stGeneral);
		size = sizeof(SysenvGeneral);
	}else if(COMPARE(item, "enc", "encode")){
		src_offset = &(thiz->stAttr.stContent.stEncode);
		dst_offset = &(other->stAttr.stContent.stEncode);
		size = sizeof(SysenvEnc);
	}else if(COMPARE(item, "rec", "record")){
		src_offset = &(thiz->stAttr.stContent.stRecord);
		dst_offset = &(other->stAttr.stContent.stRecord);
		size = sizeof(SysenvRec);
//	}else if(COMPARE(item, "net", "network")){
//		src_offset = &(thiz->stAttr.stContent.stNetwork);
//		dst_offset = &(other->stAttr.stContent.stNetwork);
//		size = sizeof(SysenvNetwork);
	}else if(COMPARE(item, "scr", "screen")){
		src_offset = &(thiz->stAttr.stContent.stScreen);
		dst_offset = &(other->stAttr.stContent.stScreen);
		size = sizeof(SysenvScreen);
	}else if(COMPARE(item, "det", "detection")){
		src_offset = &(thiz->stAttr.stContent.stDetection);
		dst_offset = &(other->stAttr.stContent.stDetection);
		size = sizeof(SysenvDetection);
	}else if(COMPARE(item, "ptz", "ptz")){
		src_offset = &(thiz->stAttr.stContent.stPTZ);
		dst_offset = &(other->stAttr.stContent.stPTZ);
		size = sizeof(SysenvPTZ);
	}else if(COMPARE(item, "sen", "sensor")){
		src_offset = &(thiz->stAttr.stContent.stSensor);
		dst_offset = &(other->stAttr.stContent.stSensor);
		size = sizeof(SysenvSensor);
	}else if(COMPARE(item, "telecontrol", "telecontrol")){
		src_offset = &(thiz->stAttr.stContent.stTelecontrol);
		dst_offset = &(other->stAttr.stContent.stTelecontrol);
		size = sizeof(SysenvTelecontrol);
	}else if(COMPARE(item, "ups", "ups")){
		src_offset = &(thiz->stAttr.stContent.stUps);
		dst_offset = &(other->stAttr.stContent.stUps);
		size = sizeof(SysenvUps);
	}else if(COMPARE(item, "hdd", "hdd")){
		src_offset = &(thiz->stAttr.stContent.stHDD);
		dst_offset = &(other->stAttr.stContent.stHDD);
		size = sizeof(SysenvHDD);
	}else if(COMPARE(item, "network_ip", "network_ip")){
		if(thiz->stAttr.stContent.stNetwork.bDHCPUse != other->stAttr.stContent.stNetwork.bDHCPUse
				|| memcmp(thiz->stAttr.stContent.stNetwork.au8MACAddr, other->stAttr.stContent.stNetwork.au8MACAddr, sizeof(thiz->stAttr.stContent.stNetwork.au8MACAddr)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.au8IPAddr, other->stAttr.stContent.stNetwork.au8IPAddr, sizeof(thiz->stAttr.stContent.stNetwork.au8IPAddr)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.au8Gateway, other->stAttr.stContent.stNetwork.au8Gateway, sizeof(thiz->stAttr.stContent.stNetwork.au8Gateway)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.au8SubMask, other->stAttr.stContent.stNetwork.au8SubMask, sizeof(thiz->stAttr.stContent.stNetwork.au8SubMask)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.au8DNSAddr, other->stAttr.stContent.stNetwork.au8DNSAddr, sizeof(thiz->stAttr.stContent.stNetwork.au8DNSAddr)) != 0
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}else if(COMPARE(item, "network_web", "network_web")){
		if(thiz->stAttr.stContent.stNetwork.u16PortHTTP != other->stAttr.stContent.stNetwork.u16PortHTTP)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}else if(COMPARE(item, "network_client", "network_client")){
		if(thiz->stAttr.stContent.stNetwork.u16PortClient != other->stAttr.stContent.stNetwork.u16PortClient
				|| thiz->stAttr.stContent.stNetwork.u16PortMobile != other->stAttr.stContent.stNetwork.u16PortMobile)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}else if(COMPARE(item, "network_ddns", "network_ddns")){
		if(thiz->stAttr.stContent.stNetwork.bDDNSUse != other->stAttr.stContent.stNetwork.bDDNSUse
				|| thiz->stAttr.stContent.stNetwork.enDDNSProvider != other->stAttr.stContent.stNetwork.enDDNSProvider
				|| memcmp(thiz->stAttr.stContent.stNetwork.szDDNSUrl, other->stAttr.stContent.stNetwork.szDDNSUrl, sizeof(thiz->stAttr.stContent.stNetwork.szDDNSUrl)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szDDNSUser, other->stAttr.stContent.stNetwork.szDDNSUser, sizeof(thiz->stAttr.stContent.stNetwork.szDDNSUser)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szDDNSPassword, other->stAttr.stContent.stNetwork.szDDNSPassword, sizeof(thiz->stAttr.stContent.stNetwork.szDDNSPassword)) != 0
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}else if(COMPARE(item,  "network_3g", "network_3g")){
		if(thiz->stAttr.stContent.stNetwork.b3GUse != other->stAttr.stContent.stNetwork.b3GUse
				|| memcmp(thiz->stAttr.stContent.stNetwork.sz3GAPN, other->stAttr.stContent.stNetwork.sz3GAPN, sizeof(thiz->stAttr.stContent.stNetwork.sz3GAPN)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.sz3GPIN, other->stAttr.stContent.stNetwork.sz3GPIN, sizeof(thiz->stAttr.stContent.stNetwork.sz3GPIN)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.sz3GUser, other->stAttr.stContent.stNetwork.sz3GUser, sizeof(thiz->stAttr.stContent.stNetwork.sz3GUser)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.sz3GPassword, other->stAttr.stContent.stNetwork.sz3GPassword, sizeof(thiz->stAttr.stContent.stNetwork.sz3GPassword)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.sz3GDIALNum, other->stAttr.stContent.stNetwork.sz3GDIALNum, sizeof(thiz->stAttr.stContent.stNetwork.sz3GDIALNum)) != 0
				)
		{
			return ENV_FALSE;
		}else{
			return ENV_TRUE;
		}
	}else if(COMPARE(item, "network_pppoe", "network_pppoe")){
		if(thiz->stAttr.stContent.stNetwork.bPPPoE != other->stAttr.stContent.stNetwork.bPPPoE
				|| memcmp(thiz->stAttr.stContent.stNetwork.szPPPoEUser, other->stAttr.stContent.stNetwork.szPPPoEUser, sizeof(thiz->stAttr.stContent.stNetwork.szPPPoEUser)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szPPPoEPwd, other->stAttr.stContent.stNetwork.szPPPoEPwd, sizeof(thiz->stAttr.stContent.stNetwork.szPPPoEPwd)) != 0
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
/*
ENV_BOOLEAN bSMTPUse;
char szSMTPServer[SMTP_EDITBOX_LEN+1];
unsigned short u16SMTPPort;
ENV_BOOLEAN bSMTPAnonymous;
char szSMTPUser[SMTP_EDITBOX_LEN+1];
char szSMTPPwd[SMTP_EDITBOX_LEN+1];
SMTP_ENCRYPT_TYPE enSMTPEncryptType;
char szSMTPSender[SMTP_EDITBOX_LEN+1];
char szSMTPSendee1[SMTP_EDITBOX_LEN+1];
char szSMTPSendee2[SMTP_EDITBOX_LEN+1];
char szSMTPSubject[SMTP_EDITBOX_LEN+1];
unsigned short u16SMTPInterval;
ENV_BOOLEAN bSMTPHealthEnable;
unsigned short u16SMTPHealthInterval;	
*/
	}else if(COMPARE(item, "network_smtp", "network_smtp")){
		if(thiz->stAttr.stContent.stNetwork.bSMTPUse != other->stAttr.stContent.stNetwork.bSMTPUse
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPServer, other->stAttr.stContent.stNetwork.szSMTPServer, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPServer)) != 0
				|| thiz->stAttr.stContent.stNetwork.u16SMTPPort != other->stAttr.stContent.stNetwork.u16SMTPPort
				|| thiz->stAttr.stContent.stNetwork.bSMTPAnonymous != other->stAttr.stContent.stNetwork.bSMTPAnonymous
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPUser, other->stAttr.stContent.stNetwork.szSMTPUser, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPUser)) != 0	
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPPwd, other->stAttr.stContent.stNetwork.szSMTPPwd, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPPwd)) != 0
				|| thiz->stAttr.stContent.stNetwork.enSMTPEncryptType != other->stAttr.stContent.stNetwork.enSMTPEncryptType
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPSender, other->stAttr.stContent.stNetwork.szSMTPSender, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPSender)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPSendee1, other->stAttr.stContent.stNetwork.szSMTPSendee1, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPSendee1)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPSendee2, other->stAttr.stContent.stNetwork.szSMTPSendee2, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPSendee2)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.szSMTPSubject, other->stAttr.stContent.stNetwork.szSMTPSubject, sizeof(thiz->stAttr.stContent.stNetwork.szSMTPSubject)) != 0
				|| thiz->stAttr.stContent.stNetwork.u16SMTPInterval != other->stAttr.stContent.stNetwork.u16SMTPInterval
				|| thiz->stAttr.stContent.stNetwork.bSMTPHealthEnable != other->stAttr.stContent.stNetwork.bSMTPHealthEnable
				|| thiz->stAttr.stContent.stNetwork.u16SMTPHealthInterval != other->stAttr.stContent.stNetwork.u16SMTPHealthInterval
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}else if(COMPARE(item, "network_online_upgrade", "network_online_upgrade")){
		if(thiz->stAttr.stContent.stNetwork.UsePreHost != other->stAttr.stContent.stNetwork.UsePreHost
				|| memcmp(thiz->stAttr.stContent.stNetwork.UpgradePreHost, other->stAttr.stContent.stNetwork.UpgradePreHost, sizeof(thiz->stAttr.stContent.stNetwork.UpgradePreHost)) != 0
				|| memcmp(thiz->stAttr.stContent.stNetwork.UpgradePreUrl, other->stAttr.stContent.stNetwork.UpgradePreUrl, sizeof(thiz->stAttr.stContent.stNetwork.UpgradePreUrl)) != 0
				|| thiz->stAttr.stContent.stNetwork.UpgradePrePort != other->stAttr.stContent.stNetwork.UpgradePrePort
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}
	}
//#ifdef _NVR
	else if(COMPARE(item, "ipcam", "ipcam")){
		src_offset = &(thiz->stAttr.stContent.stIPCamGroup);
		dst_offset = &(other->stAttr.stContent.stIPCamGroup);
		size = sizeof(SysenvIPCamGroup);
		/*if(memcmp(thiz->stAttr.stContent.stIPCamGroup.astIPCam.au8IPAddr, other->stAttr.stContent.stIPCamGroup.astIPCam.au8IPAddr, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam.au8IPAddr)) != 0
				|| memcmp(thiz->stAttr.stContent.stIPCamGroup.astIPCam.u16Port, other->stAttr.stContent.stIPCamGroup.astIPCam.u16Port, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam.u16Port)) != 0
				|| memcmp(thiz->stAttr.stContent.stIPCamGroup.astIPCam.azUsername, other->stAttr.stContent.stIPCamGroup.astIPCam.azUsername, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam.azUsername)) != 0
				|| memcmp(thiz->stAttr.stContent.stIPCamGroup.astIPCam.azPassword, other->stAttr.stContent.stIPCamGroup.astIPCam.azPassword, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam.azPassword)) != 0
				)
		{
			return ENV_FALSE;
		}
		else
		{
			return ENV_TRUE;
		}*/
	}
//#endif
	else if(COMPARE(item, "maintenance", "maintenance")){
		src_offset = &(thiz->stAttr.stContent.stMaintenance);
		dst_offset = &(other->stAttr.stContent.stMaintenance);
		size = sizeof(SysenvIPCamGroup);	
	}
	else{
		return ENV_FALSE;
	}
	if(0 == memcmp(src_offset, dst_offset, size)){
		return ENV_TRUE;
	}
	return ENV_FALSE;
#undef COMPARE
}

static unsigned int GetEnvSize(Sysenv* const thiz)
{
	return sizeof(SysenvAttr);
}

static unsigned short UpdateCRC16(Sysenv* const thiz)
{
	unsigned short crc = GetStreamCRC16((unsigned char*)&thiz->stAttr.stContent, sizeof(thiz->stAttr.stContent));
//	ENV_TRACE("%s crc = %x", __FUNCTION__, crc);
	return (thiz->stAttr.u16CRC = crc);
}

// store / restore from file
static int Save(Sysenv* const thiz)
{
	int nRet = 0;
	// update crc16 for store
	thiz->UpdateCRC16(thiz);
	nRet = env_Write(ENVIRONMENT_OFFSET, &thiz->stAttr, sizeof(thiz->stAttr));
	return nRet > 0 ? 0 : -1;
}

static int Load(Sysenv* const thiz)
{
	int nRet = 0;
	if(sizeof(thiz->stAttr) > ENVIRONMENT_SIZE)
	{
//		SysenvColor stColor;
//		SysenvGeneral stGeneral;
//		SysenvEnc stEncode;
//		SysenvIPCamGroup stIPCamGroup;
//		SysenvRec stRecord;
//		SysenvNetwork stNetwork;
//		SysenvScreen stScreen;
//		SysenvDetection stDetection;
//		SysenvPTZ stPTZ;
//		SysenvSensor stSensor;
//		SysenvTelecontrol stTelecontrol;
//		SysenvUps stUps;
//		SysenvHDD stHDD;
		ENV_TRACE("sizeof(thiz->stAttr)=%d is too large", sizeof(thiz->stAttr));
		ENV_TRACE("sizeof(SysenvColor)=%d", sizeof(SysenvColor));
		ENV_TRACE("sizeof(SysenvGeneral)=%d", sizeof(SysenvGeneral));
		ENV_TRACE("sizeof(SysenvEnc)=%d", sizeof(SysenvEnc));
//#ifdef _NVR
		ENV_TRACE("sizeof(SysenvIPCamGroup)=%d", sizeof(SysenvIPCamGroup));
//#endif
		ENV_TRACE("sizeof(SysenvRec)=%d", sizeof(SysenvRec));
		ENV_TRACE("sizeof(SysenvNetwork)=%d", sizeof(SysenvNetwork));
		ENV_TRACE("sizeof(SysenvScreen)=%d", sizeof(SysenvScreen));
		ENV_TRACE("sizeof(SysenvDetection)=%d", sizeof(SysenvDetection));
		ENV_TRACE("sizeof(SysenvPTZ)=%d", sizeof(SysenvPTZ));
		ENV_TRACE("sizeof(SysenvSensor)=%d", sizeof(SysenvSensor));
		ENV_TRACE("sizeof(SysenvTelecontrol)=%d", sizeof(SysenvTelecontrol));
		ENV_TRACE("sizeof(SysenvUps)=%d", sizeof(SysenvUps));
		ENV_TRACE("sizeof(SysenvHDD)=%d", sizeof(SysenvHDD));
		assert(0);
	}
	nRet = env_Read(ENVIRONMENT_OFFSET, &thiz->stAttr, sizeof(thiz->stAttr));
	return nRet > 0 ? 0 : -1;
}

static int SaveFactory(Sysenv* const thiz)
{
//	int nWrite = 0;
//	FILE* fp = NULL;
//	fp = fopen(ENVIRONMENT_DEV, "r+b");
//	assert(fp);
//	fseek(fp, ENVIRON_FACTORY_OFFSET, SEEK_SET);
//	nWrite = fwrite(&thiz->stAttr, 1, sizeof(SysenvAttr), fp);
//	assert(nWrite == sizeof(SysenvAttr));
//	fflush(fp);
//	fclose(fp);
	return 0;
}

static int LoadFactory(Sysenv* const thiz)
{
//	int nRead = 0;
//	FILE* fp = NULL;
//	fp = fopen(ENVIRONMENT_DEV, "rb");
//	if(!fp){
//		ENV_TRACE("open \"%s\" failed", ENVIRONMENT_DEV);
//		return -1;
//	}
//	fseek(fp, ENVIRON_FACTORY_OFFSET, SEEK_SET);
//	nRead = fread(&thiz->stAttr, 1, sizeof(SysenvAttr), fp);
//	assert(nRead == sizeof(SysenvAttr));
//	fclose(fp);
	return 0;
}

static int GetVerFix(void) //FIXME: Should Remove These Code;
{
	int  fddev = open("/dev/"GPIO_DEVICE_NAME, O_RDWR);
	if(fddev > 0) {
        int ret = ioctl(fddev, GPIO_GET_PLAT_VER, NULL);

		close(fddev);

		return ret;
	}

	return -1;
}

int  SetIrdaBuzzer(int SetOn)
{
	int  fddev = open("/dev/"GPIO_DEVICE_NAME, O_RDWR);
	if(fddev > 0) {
        int ret = ioctl(fddev, GPIO_SET_IRDA_BUZZER, &SetOn);

		close(fddev);

		return ret;
	}

	return -1;
}

#define DVR_RESOURCE_DIR        "/tmp/dvr_resource_dir/"
static int CheckEffioSupport(void) //FIXME: Should Remove These Code;
{
	if(access(DVR_RESOURCE_DIR"ForbidEffio", F_OK) == 0) {
		return 0;
	}

	int  ret = 0;
	int  fddev = open("/dev/video", O_RDWR);
	if(fddev > 0) {
		video_sup_t stVSup;

		memset(&stVSup, 0, sizeof(stVSup));

		if((ioctl(fddev, VIDEO_DRV_GET_VINFO, &stVSup) == 0)
			&& (stVSup.vsiz & (1 << VIDEO_SUP_FLEF))) {
			ret = !0;
		}
		else {
			ret = 0;
		}

		close(fddev);
	}

	return ret;
}

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

static int ReadSinglePhysicalAddress(unsigned long Address, unsigned char * Value)
{
	size_t          SystemPageSize = (size_t)getpagesize();
	unsigned char * MapAddr = NULL;

	int fd = open("/dev/mem", O_RDWR);
	if(fd < 0) {
		return -1;
	}

	MapAddr = (unsigned char *)mmap(0,
		SystemPageSize,
		PROT_READ,
		MAP_SHARED,
		fd,
		Address-(Address%SystemPageSize));
	if(MapAddr < 0) {
		close(fd);
		return -1;
	}

	*Value = *(MapAddr + Address%SystemPageSize);

	munmap(MapAddr, SystemPageSize);
	close(fd);

	return 0;
}

//Return:
//      0 Unknown Platform or CPU UnMatched
//     !0 CPU Matched
int Plat_CPU_Check(void)
{
#if   defined(_HI3515A_EXT) || defined(_HI3515C_EXT) || defined(_HI3520D_EXT)
#define HI3515A_CHIPID 0x00000001
#define HI3515C_CHIPID 0x00000002
#define HI3520D_CHIPID 0x00000003
	{
		unsigned long tmpLong = 0;
		unsigned char tmpByte = 0;

		ReadSinglePhysicalAddress(0x2005006C, &tmpByte);
		tmpLong += (tmpByte <<  0);
		ReadSinglePhysicalAddress(0x2005006D, &tmpByte);
		tmpLong += (tmpByte <<  8);
		ReadSinglePhysicalAddress(0x2005006E, &tmpByte);
		tmpLong += (tmpByte << 16);
		ReadSinglePhysicalAddress(0x2005006F, &tmpByte);
		tmpLong += (tmpByte << 24);

#if   defined(_HI3515A_EXT)
		if((HI3515A_CHIPID != tmpLong) && (HI3515C_CHIPID != tmpLong))
#elif defined(_HI3520D_EXT)
		if(HI3520D_CHIPID != tmpLong)
#endif
		{
			return 0;
		}

		return !0;
	}
#endif

	return !0;
}

ENV_BOOLEAN IsEffioSupport(void)
{
#if   !defined(_EXT_EFFIO)
	return ENV_FALSE;
#else
	static int  tmpStoredBool = -1;

	if(tmpStoredBool == -1) {
		tmpStoredBool = CheckEffioSupport();
	}

	if(tmpStoredBool == 0) {
		return ENV_FALSE;
	}

	return ENV_TRUE;
#endif
}

static int RestoreDefaultColor(Sysenv* const thiz)
{
	_env_default_get_init();
	int i;
	for(i = 0; i < MAX_CAM_CH; ++i)
	{
		{
			_SET_INT(thiz->stAttr.stContent.stColor.au8Hue[i], "Color", "au8Hue", DEFAULT_HUE);
			_SET_INT(thiz->stAttr.stContent.stColor.au8Contrast[i], "Color", "au8Contrast", DEFAULT_CONTRAST);
			_SET_INT(thiz->stAttr.stContent.stColor.au8Luminance[i], "Color", "au8Luminance", DEFAULT_BRIGHTNESS);
			_SET_INT(thiz->stAttr.stContent.stColor.au8Saturation[i], "Color", "au8Saturation", DEFAULT_SATURATION);
		}
//		printf("Hue=%d Contrast=%d Luminance=%d Saturation=%d\n",
//				thiz->stAttr.stContent.stColor.au8Hue[i],
//				thiz->stAttr.stContent.stColor.au8Contrast[i],
//				thiz->stAttr.stContent.stColor.au8Luminance[i],
//				thiz->stAttr.stContent.stColor.au8Saturation[i]);
	}

//	for(i = 0; i < MAX_CAM_CH; ++i)
//	{
//		thiz->stAttr.stContent.stColor.au8Hue[i] = DEFAULT_HUE;
//		thiz->stAttr.stContent.stColor.au8Contrast[i] = DEFAULT_CONTRAST;
//		thiz->stAttr.stContent.stColor.au8Luminance[i] = DEFAULT_BRIGHTNESS;
//		thiz->stAttr.stContent.stColor.au8Saturation[i] = DEFAULT_SATURATION;
//	}
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int ___to_be_restore = 0;
static int RestoreDefaultGeneral(Sysenv* const thiz)
{
	int i;
	_env_default_get_init();
	// miscellaneous
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.enDateFmt, "General", "enDateFmt", DATE_FMT, YYYY_MM_DD);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.enStandard, "General", "enStandard", VOUT_STANDARD, VOUT_PAL);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.enSysTimeFmt, "General", "enSysTimeFmt", TIME_FORMAT, TIMEFORMAT_X24);
	
	if(___to_be_restore == 1)
	{
		_SET_ENUM(thiz->stAttr.stContent.stGeneral.enLanguage, "General", "enLanguage", WORLDWIDE_LANGUAGE, WORLDWIDE_CHINESE_MAINLAND);
		_SET_ENUM(thiz->stAttr.stContent.stGeneral.enKey_Type, "General", "enKey_Type", KEYMAP_TYPE, DEFAULT_KEYMAP_TYPE);
		_SET_ENUM(thiz->stAttr.stContent.stGeneral.enRemote_Type, "General", "enRemote_Type", REMOTE_TYPE, DEFAULT_REMOTE_TYPE);
	}

	_SET_ENUM(thiz->stAttr.stContent.stGeneral.ChannelModeAuto, "General", "ChannelModeAuto", ENV_BOOLEAN, ENV_TRUE);
	for(i = 0; i < MAX_CAM_CH; ++i) {
		_SET_ENUM(thiz->stAttr.stContent.stGeneral.ChannelModes[i], "General", "ChannelMode", VIDEO_MODE, VIDEO_MODE_720P);
	}
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bKeyBuzzer, "General", "bKeyBuzzer", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bKeyLock, "General", "bKeyLock", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bPersianCalendar, "General", "bPersianCalendar", ENV_BOOLEAN, ENV_FALSE);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bTimeUse, "General", "bTimeUse", ENV_BOOLEAN, ENV_TRUE);
	_SET_INT(thiz->stAttr.stContent.stGeneral.nDVRId, "General", "nDVRId", 255);
	
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bGuideUse, "General", "bGuideUse", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.bEseeidUse, "General", "bEseeidUse", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.enTimerPositionMode, "General", "enTimerPositionMode", ENC_POSITION_MODE, POSITION_BOTTOM_LEFT);
	_SET_INT(thiz->stAttr.stContent.stGeneral.nXCoordinateTimer, "General", "nXCoordinateTimer", 0);
	_SET_INT(thiz->stAttr.stContent.stGeneral.nYCoordinateTimer, "General", "nYCoordinateTimer", 0);
	_SET_ENUM(thiz->stAttr.stContent.stGeneral.enEseeidPositionMode, "General", "enEseeidPositionMode", ENC_POSITION_MODE, POSITION_BOTTOM_RIGHT);
	_SET_INT(thiz->stAttr.stContent.stGeneral.nXCoordinateEseeid, "General", "nXCoordinateEseeid", 0);
	_SET_INT(thiz->stAttr.stContent.stGeneral.nYCoordinateEseeid, "General", "nYCoordinateEseeid", 0);
	_SET_STRING(thiz->stAttr.stContent.stGeneral.azSpecialFTPServerPath, "General", "azSpecialFTPServerPath", "");
	_SET_STRING(thiz->stAttr.stContent.stGeneral.Support_Web, "General", "Support_Web", "http://www.dvr163.com");
	_SET_STRING(thiz->stAttr.stContent.stGeneral.AndroidLink, "General", "AndroidLink", "");
	_SET_STRING(thiz->stAttr.stContent.stGeneral.IOSLink, "General", "IOSLink", "");
//	printf("enDateFmt=%d enStandard=%d enLanguage=%d bKeyBuzzer=%d bKeyLock=%d, nDVRId=%d\n",
//			thiz->stAttr.stContent.stGeneral.enDateFmt,
//			thiz->stAttr.stContent.stGeneral.enStandard,
//			thiz->stAttr.stContent.stGeneral.enLanguage,
//			thiz->stAttr.stContent.stGeneral.bKeyBuzzer,
//			thiz->stAttr.stContent.stGeneral.bKeyLock,
//			thiz->stAttr.stContent.stGeneral.nDVRId);

//#if defined(_OEM_TABTECHNOLOGIES)
//		thiz->stAttr.stContent.stGeneral.enDateFmt = DD_MM_YYYY;
//#else
//		thiz->stAttr.stContent.stGeneral.enDateFmt = YYYY_MM_DD;
//#endif
//	thiz->stAttr.stContent.stGeneral.enStandard = (VOUT_STANDARD)PARA_GetStandard();
//	thiz->stAttr.stContent.stGeneral.enLanguage = (WORLDWIDE_LANGUAGE)PARA_GetLanguage();
//	printf("lang = %d\r\n", thiz->stAttr.stContent.stGeneral.enLanguage);
//	thiz->stAttr.stContent.stGeneral.bKeyBuzzer = ENV_TRUE;
//	thiz->stAttr.stContent.stGeneral.bKeyLock = ENV_TRUE;
//	thiz->stAttr.stContent.stGeneral.nDVRId = 255;
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int gMaxLargeSizeNum = MAX_D1_CNT;

int GetMaxLargeSizeNum(void)
{
	if((MAX_CAM_CH == 4) && (access(DVR_RESOURCE_DIR"E4D1", F_OK) == 0)) {
		return MAX_D1_CNT;
	}
	if((MAX_CAM_CH == 8) && (access(DVR_RESOURCE_DIR"E8D1", F_OK) == 0)) {
		return MAX_D1_CNT;
	}
#if defined(MAX_EXT_LSIZE_CNT)
	return MAX_EXT_LSIZE_CNT;
#else
	return MAX_D1_CNT;
#endif
}

static int RestoreDefaultEncode(Sysenv* const thiz)
{
	_env_default_get_init();
	int i = 0, ii = 0;
	// encode
	gMaxLargeSizeNum = GetMaxLargeSizeNum();
#if  defined(_EXT_HDDVR)
	for(i = 0; i < MAX_CAM_CH; ++i){
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenMode[i], "Encode", "aenMode", ENC_MODE, MODE_VIDEO);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenFormat[i], "Encode", "aenFormat(D1)", ENC_FMT, ENC_FMT_HD720P);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenBitrateValue[i], "Encode", "aenBitrateValue(D1)", ENC_BR_VALUE, ENC_BR_6MPS);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenLevel[i], "Encode", "aenLevel", RANK, RANK_HIGHEST);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenBitrateMode[i], "Encode", "aenBitrateMode", ENC_BR_MODE, ENC_BR_VARIABLE);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenFramerate[i], "Encode", "aenFramerate", ENC_FRAMERATE, FR_25FPS);

		for(ii = 0; ii < MAX_SUB_STREAM_CNT; ++ii){
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubFormat[i][ii], "Encode", "aenSubFormat", ENC_FMT, ENC_FMT_D1);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubLevel[i][ii], "Encode", "aenSubLevel", RANK, RANK_HIGHEST);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubBitrateMode[i][ii], "Encode", "aenSubBitrateMode", ENC_BR_MODE, ENC_BR_VARIABLE);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubBitrateValue[i][ii], "Encode", "aenSubBitrateValue", ENC_BR_VALUE, ENC_BR_512KPS);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubFramerate[i][ii], "Encode", "aenSubFramerate", ENC_FRAMERATE, FR_12FPS);
		}
	}
#else
	for(i = 0; i < MAX_CAM_CH; ++i){
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenMode[i], "Encode", "aenMode", ENC_MODE, MODE_VIDEO);
		if(i < gMaxLargeSizeNum){
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenFormat[i], "Encode", "aenFormat(D1)", ENC_FMT, ENC_FMT_D1);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenBitrateValue[i], "Encode", "aenBitrateValue(D1)", ENC_BR_VALUE, ENC_BR_2MPS);
		}else{
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenFormat[i], "Encode", "aenFormat(CIF)", ENC_FMT, ENC_FMT_CIF);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenBitrateValue[i], "Encode", "aenBitrateValue(CIF)", ENC_BR_VALUE, ENC_BR_1MPS);
		}
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenLevel[i], "Encode", "aenLevel", RANK, RANK_HIGHEST);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenBitrateMode[i], "Encode", "aenBitrateMode", ENC_BR_MODE, ENC_BR_CONSTANT);
		_SET_ENUM(thiz->stAttr.stContent.stEncode.aenFramerate[i], "Encode", "aenFramerate", ENC_FRAMERATE, FR_25FPS);
//		printf("aenMode=%d aenFormat=%d aenBitrateValue=%d aenLevel=%d aenBitrateMode=%d, aenFramerate=%d\n",
//				thiz->stAttr.stContent.stEncode.aenMode[i],
//				thiz->stAttr.stContent.stEncode.aenFormat[i],
//				thiz->stAttr.stContent.stEncode.aenBitrateValue[i],
//				thiz->stAttr.stContent.stEncode.aenLevel[i],
//				thiz->stAttr.stContent.stEncode.aenBitrateMode[i],
//				thiz->stAttr.stContent.stEncode.aenFramerate[i]);

		for(ii = 0; ii < MAX_SUB_STREAM_CNT; ++ii){
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubFormat[i][ii], "Encode", "aenSubFormat", ENC_FMT, ENC_FMT_CIF);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubLevel[i][ii], "Encode", "aenSubLevel", RANK, RANK_HIGHEST);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubBitrateMode[i][ii], "Encode", "aenSubBitrateMode", ENC_BR_MODE, ENC_BR_CONSTANT);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubBitrateValue[i][ii], "Encode", "aenSubBitrateValue", ENC_BR_VALUE, ENC_BR_384KPS);
			_SET_ENUM(thiz->stAttr.stContent.stEncode.aenSubFramerate[i][ii], "Encode", "aenSubFramerate", ENC_FRAMERATE, FR_12FPS);
		}
//		printf("aenSubFormat=%d aenSubLevel=%d aenSubBitrateMode=%d aenSubBitrateValue=%d aenSubFramerate=%d, aenFramerate=%d\n",
//				thiz->stAttr.stContent.stEncode.aenSubFormat[i][ii],
//				thiz->stAttr.stContent.stEncode.aenSubLevel[i][ii],
//				thiz->stAttr.stContent.stEncode.aenSubLevel[i][ii],
//				thiz->stAttr.stContent.stEncode.aenSubBitrateValue[i][ii],
//				thiz->stAttr.stContent.stEncode.aenSubFramerate[i][ii]);
	}
#endif
	_SET_ENUM(thiz->stAttr.stContent.stEncode.bEnableEffioEnhanced, "Encode", "bEnableEffioEnhanced", ENV_BOOLEAN, ENV_FALSE);
	_SET_ENUM(thiz->stAttr.stContent.stEncode.bFmtManualSwitch, "Encode", "bFmtManualSwitch", ENV_BOOLEAN, ENV_FALSE);
//	for(i = 0; i < MAX_CAM_CH; ++i){
//		thiz->stAttr.stContent.stEncode.aenMode[i] = MODE_VIDEO;
//		if(i < MAX_D1_CNT){
//			thiz->stAttr.stContent.stEncode.aenFormat[i] = ENC_FMT_D1;
//			thiz->stAttr.stContent.stEncode.aenBitrateValue[i] = ENC_BR_2MPS;
//		}else{
//			thiz->stAttr.stContent.stEncode.aenFormat[i] = ENC_FMT_CIF;
//			thiz->stAttr.stContent.stEncode.aenBitrateValue[i] = ENC_BR_1MPS;
//		}
//		thiz->stAttr.stContent.stEncode.aenLevel[i] = RANK_HIGHEST;
//		thiz->stAttr.stContent.stEncode.aenBitrateMode[i] = ENC_BR_CONSTANT;
//		thiz->stAttr.stContent.stEncode.aenFramerate[i] = FR_25FPS;
//
//		for(ii = 0; ii < MAX_SUB_STREAM_CNT; ++ii){
//			thiz->stAttr.stContent.stEncode.aenSubFormat[i][ii] = ENC_FMT_CIF;
//			thiz->stAttr.stContent.stEncode.aenSubLevel[i][ii] = RANK_HIGHEST;
//			thiz->stAttr.stContent.stEncode.aenSubBitrateMode[i][ii] = ENC_BR_CONSTANT;
//			thiz->stAttr.stContent.stEncode.aenSubBitrateValue[i][ii] = ENC_BR_384KPS;
//			thiz->stAttr.stContent.stEncode.aenSubFramerate[i][ii] = FR_12FPS;
//		}
//	}
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}
//#ifdef _NVR
static int RestoreDefaultIPCam(Sysenv* const thiz)
{
	_env_default_get_init();
	int i = 0;
	// encode

	_SET_INT(thiz->stAttr.stContent.stIPCamGroup.azWorkMode, "IPCam", "u16WorkMode", CHANNEL_DIRECTLINK);
	_SET_ENUM(thiz->stAttr.stContent.stIPCamGroup.bMultiNetworkSegment, "IPCam", "bMultiNetworkSegment", ENV_BOOLEAN, ENV_FALSE);
	for(i = 0; i < MAX_CAM_CH; ++i){
		char default_ip[32];
		sprintf(default_ip, "192.168.1.%d", 88+i);
		_SET_IP(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].au8IPAddr, "IPCam", "au8IPAddr", default_ip);
		_SET_MAC(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].u8MACAddr, "IPCam", "u8MACAddr", "00:05:fe:ff:ff:ff");
		_SET_INT(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].u16Port, "IPCam", "u16Port", 9080);
		_SET_ENUM(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azEnable_ch, "IPCam", "azEnable_ch", ENV_BOOLEAN, ENV_FALSE);
		_SET_INT(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azProtocolname, "IPCam", "azProtocolname", 0);
		_SET_STRING(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azUsername, "IPCam", "azUsername", "admin");
		_SET_STRING(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azPassword, "IPCam", "azPassword", "admin");
		_SET_STRING(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azModelname, "IPCam", "azModelname", "");
		_SET_INT(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].iStreamNR ,"IPCam","iStreamNR", 0);
		_SET_INT(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azStreamChoise,"IPCam","azStreamChoise", IPC_VIDEO_ONLY);
		memset(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].stPrivData, 0, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam[0].stPrivData));
	}
	printf("\n\n ############ reset syenv #######################\n\n");

//	for(i = 0; i < MAX_CAM_CH; ++i){
//		thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].au8IPAddr[0] = 192;
//		thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].au8IPAddr[1] = 168;
//		thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].au8IPAddr[2] = 1;
//		thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].au8IPAddr[3] = 200 + i;
//
//		thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].u16Port = 8840;
//		memset(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azPassword, 0, sizeof(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azPassword));
//		strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[i].azPassword, "111111");
//	}
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}
//#endif


static int RestoreDefaultHvr(Sysenv* const thiz)
{
	_env_default_get_init();
	int i = 0;
	// hvr
	
	_SET_INT(thiz->stAttr.stContent.stHvr.nBnc, "Hvr", "nBnc", MAX_CAM_CH);
	printf("\n\n ############ reset hvr #######################\n\n");

	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultRecode(Sysenv* const thiz)
{
	_env_default_get_init();
	int i, ii, iii;
	// record
	for(i = 0; i < WEEKDAY_CNT; ++i)
	{
		for(ii = 0; ii < MAX_CAM_CH; ++ii)
		{
			for(iii = 0; iii < REC_COND_CNT; ++iii)
			{
				_SET_TIME(
						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_hour,
						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_min,
						"Recode", "begin", "0:0");
				_SET_TIME(
						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_hour,
						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_min,
						"Recode", "end", "23:59");
//				printf("begin=%d:%d end=%d:%d\n",
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_hour,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_min,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_hour,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_min);
				if(0 == iii )
				{
					_SET_INT(thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type, "Recode", "type", TIMER_REC);
				}
				else
				{
					thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type = NONE_REC;
				}
			}
		}
	}

//	for(i = 0; i < WEEKDAY_CNT; ++i){
//		for(ii = 0; ii < MAX_CAM_CH; ++ii){
//			for(iii = 0; iii < REC_COND_CNT; ++iii){
//				thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_hour = 0;
//				thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_min = 0;
//				thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_hour = 23;
//				thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_min = 59;
//				if(0 == iii ){
//#if defined(_OEM_TABTECHNOLOGIES)
//					thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type = MOTION_REC;
//#else
//					thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type = TIMER_REC;
//#endif
//				}else{
//					thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type = NONE_REC;
//				}
//			}
//		}
//	}
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultNetwork(Sysenv* const thiz)
{
	_env_default_get_init();
	struct timeval tv;
	gettimeofday(&tv, 0);
	srand(tv.tv_usec);

	// network
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bDHCPUse, "Network", "bDHCPUse", ENV_BOOLEAN, ENV_FALSE);
	_SET_MAC(thiz->stAttr.stContent.stNetwork.au8MACAddr, "Network", "au8MACAddr", "00:05:fe:ff:ff:ff");
	_SET_IP(thiz->stAttr.stContent.stNetwork.au8IPAddr, "Network", "au8IPAddr", "192.168.1.114");
	_SET_IP(thiz->stAttr.stContent.stNetwork.au8SubMask, "Network", "au8SubMask", "255.255.255.0");
	_SET_IP(thiz->stAttr.stContent.stNetwork.au8Gateway, "Network", "au8Gateway", "192.168.1.1");
	_SET_IP(thiz->stAttr.stContent.stNetwork.au8DNSAddr, "Network", "au8DNSAddr", "192.168.1.1");
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16PortHTTP, "Network", "u16PortHTTP", 80);
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16PortClient, "Network", "u16PortClient", 8880);
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16PortMobile, "Network", "u16PortMobile", 8882);

	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bESeeNetUse, "Network", "bESeeNetUse", ENV_BOOLEAN, ENV_TRUE);

	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bDDNSUse, "Network", "bDDNSUse", ENV_BOOLEAN, ENV_FALSE);
	_SET_INT(thiz->stAttr.stContent.stNetwork.enDDNSProvider, "Network", "enDDNSProvider", 0);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szDDNSUrl, "Network", "szDDNSUrl", "test.dyndns.org");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szDDNSUser, "Network", "szDDNSUser", "test");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szDDNSPassword, "Network", "szDDNSPassword", "test");

	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bPPPoE, "Network", "bPPPoE", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szPPPoEUser, "Network", "szPPPoEUser", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szPPPoEPwd, "Network", "szPPPoEPwd", "");

	_SET_ENUM(thiz->stAttr.stContent.stNetwork.b3GUse, "Network", "b3GUse", ENV_BOOLEAN, ENV_FALSE);

	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bSMTPUse, "Network", "bSMTPUse", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPServer, "Network", "szSMTPServer", "");
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16SMTPPort, "Network", "u16SMTPPort", 25);
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bSMTPAnonymous, "Network", "bSMTPAnonymous", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPUser, "Network", "szSMTPUser", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPPwd, "Network", "szSMTPPwd", "");
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.enSMTPEncryptType, "Network", "enSMTPEncryptType", SMTP_ENCRYPT_TYPE, SMTP_ENCRYPT_NONE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPSender, "Network", "szSMTPSender", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPSendee1, "Network", "szSMTPSendee1", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPSendee2, "Network", "szSMTPSendee2", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szSMTPSubject, "Network", "szSMTPSubject", "DVR Report");
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16SMTPInterval, "Network", "u16SMTPInterval", 5);
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bSMTPHealthEnable, "Network", "bSMTPHealthEnable", ENV_BOOLEAN, ENV_FALSE);
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16SMTPHealthInterval, "Network", "u16SMTPHealthInterval", 30);
	
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.UsePreHost, "Network", "UsePreHost", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.UpgradePreHost, "Network", "UpgradePreHost", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.UpgradePreUrl, "Network", "szSMTPSubject", "");
	_SET_INT(thiz->stAttr.stContent.stNetwork.UpgradePrePort, "Network", "UpgradePrePort", 80);
	
	// FTP
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bFTPUse, "Network", "bFTPUse", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szFTPServer, "Network", "szFTPServer", "");
	_SET_INT(thiz->stAttr.stContent.stNetwork.u16FTPPort, "Network", "u16FTPPort", 21);
	_SET_ENUM(thiz->stAttr.stContent.stNetwork.bFTPAnonymous, "Network", "bFTPAnonymous", ENV_BOOLEAN, ENV_FALSE);
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szFTPUser, "Network", "szFTPUser", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szFTPPwd, "Network", "szFTPPwd", "");
	_SET_STRING(thiz->stAttr.stContent.stNetwork.szFTPRemoteFolder, "Network", "szFTPRemoteFolder", "/");
	_SET_INT(thiz->stAttr.stContent.stNetwork.u32FTPFileMaxSize, "Network", "u32FTPFileMaxSize", 30);

	int i, ii, iii;
	// FTP Schedule
	for(i = 0; i < WEEKDAY_CNT; ++i)
	{
		for(ii = 0; ii < MAX_CAM_CH; ++ii)
		{
			for(iii = 0; iii < REC_COND_CNT; ++iii)
			{
				_SET_TIME(
						thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_hour,
						thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_min,
						"FTPSchedule", "begin", "0:0");
				_SET_TIME(
						thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_hour,
						thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_min,
						"FTPSchedule", "end", "23:59");
//				printf("begin=%d:%d end=%d:%d\n",
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_hour,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].begin_min,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_hour,
//						thiz->stAttr.stContent.stRecord.astRecCond[i].all_conditions[ii].channel_conditions[iii].end_min);
				if(0 == iii )
				{
					_SET_INT(thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type, "FTPSchedule", "type", MOTION_REC|SENSOR_REC);
				}
				else
				{
					thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[i].all_conditions[ii].channel_conditions[iii].video_type = NONE_REC;
				}
			}
		}
	}

//	thiz->stAttr.stContent.stNetwork.bDHCPUse = ENV_FALSE;
//	if(thiz->stAttr.stContent.stNetwork.au8MACAddr[0] != 0x00
//			|| thiz->stAttr.stContent.stNetwork.au8MACAddr[1] != 0x05
//			|| thiz->stAttr.stContent.stNetwork.au8MACAddr[2] != 0xfe)
//	{
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[0] = 0x00;
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[1] = 0x05;
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[2] = 0xfe;
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[3] = rand() % 200;
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[4] = rand() % 200;
//		thiz->stAttr.stContent.stNetwork.au8MACAddr[5] = rand() % 200;
//	}

//	thiz->stAttr.stContent.stNetwork.au8IPAddr[0] = 192;
//	thiz->stAttr.stContent.stNetwork.au8IPAddr[1] = 168;
//	thiz->stAttr.stContent.stNetwork.au8IPAddr[2] = 1;
//#if defined(_OEM_TABTECHNOLOGIES)
//		thiz->stAttr.stContent.stNetwork.au8IPAddr[3] = 223;
//#else
//		thiz->stAttr.stContent.stNetwork.au8IPAddr[3] = 114;
//#endif
//	thiz->stAttr.stContent.stNetwork.au8SubMask[0] = 255;
//	thiz->stAttr.stContent.stNetwork.au8SubMask[1] = 255;
//	thiz->stAttr.stContent.stNetwork.au8SubMask[2] = 255;
//	thiz->stAttr.stContent.stNetwork.au8SubMask[3] = 0;
//	thiz->stAttr.stContent.stNetwork.au8Gateway[0] = 192;
//	thiz->stAttr.stContent.stNetwork.au8Gateway[1] = 168;
//	thiz->stAttr.stContent.stNetwork.au8Gateway[2] = 1;
//	thiz->stAttr.stContent.stNetwork.au8Gateway[3] = 1;
//	thiz->stAttr.stContent.stNetwork.au8DNSAddr[0] = 192;
//	thiz->stAttr.stContent.stNetwork.au8DNSAddr[1] = 168;
//	thiz->stAttr.stContent.stNetwork.au8DNSAddr[2] = 1;
//	thiz->stAttr.stContent.stNetwork.au8DNSAddr[3] = 1;
//	thiz->stAttr.stContent.stNetwork.u16PortHTTP = 80;
//#if defined(_OEM_TABTECHNOLOGIES)
//		thiz->stAttr.stContent.stNetwork.u16PortClient = 8888;
//#else
//		thiz->stAttr.stContent.stNetwork.u16PortClient = 8880;
//#endif
//
//	thiz->stAttr.stContent.stNetwork.u16PortMobile = 8882;

	// s
//	thiz->stAttr.stContent.stNetwork.bESeeNetUse = ENV_TRUE;

//	thiz->stAttr.stContent.stNetwork.bDDNSUse = ENV_FALSE;
//	thiz->stAttr.stContent.stNetwork.enDDNSProvider = DDNS_DYNDNS;
//	strcpy(thiz->stAttr.stContent.stNetwork.szDDNSUrl, "test.dyndns.org");
//	strcpy(thiz->stAttr.stContent.stNetwork.szDDNSUser, "test");
//	strcpy(thiz->stAttr.stContent.stNetwork.szDDNSPassword, "test");

//	thiz->stAttr.stContent.stNetwork.bPPPoE = ENV_FALSE;
//	strcpy(thiz->stAttr.stContent.stNetwork.szPPPoEUser, "pppoe12345667890");
//	strcpy(thiz->stAttr.stContent.stNetwork.szPPPoEPwd, "pppoe12345667890");
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultScreen(Sysenv* const thiz)
{
	_env_default_get_init();
	int i;
	// screen
	for(i = 0; i < MAX_CAM_CH; ++i){
		char szTitle[(CHN_TITLE_LEN + 7) & ~3] = {""};
		memset(szTitle, 0, sizeof(szTitle));
		sprintf(szTitle, "%d", i + 1);
		_SET_STRING(thiz->stAttr.stContent.stScreen.aszTitle[i], "Screen", "aszTitle", "CAM");
		strcat(thiz->stAttr.stContent.stScreen.aszTitle[i], szTitle);
	}

	_SET_INT(thiz->stAttr.stContent.stScreen.nAlpha, "Screen", "nAlpha", 12);//0-16
	_SET_ENUM(thiz->stAttr.stContent.stScreen.bAutoSwi, "Screen", "bAutoSwi", ENV_BOOLEAN, ENV_FALSE);
	_SET_ENUM(thiz->stAttr.stContent.stScreen.bSkipLoss, "Screen", "bSkipLoss", ENV_BOOLEAN, ENV_FALSE);
	_SET_ENUM(thiz->stAttr.stContent.stScreen.enAutoSwiInterval, "Screen", "enAutoSwiInterval", AUTOSWI_INTERVAL, AUTOSWI_5SEC);
	_SET_ENUM(thiz->stAttr.stContent.stScreen.enAutoSwiMode, "Screen", "enAutoSwiMode", AUTOSWI_MODE, AUTOSWI_X1);
	_SET_ENUM(thiz->stAttr.stContent.stScreen.enVGAResolution, "Screen", "enVGAResolution", VGA_RESOLUTION, VGA_800x600);
	_SET_INT(thiz->stAttr.stContent.stScreen.nTvSize[TV_SIZE_LEFT], "Screen", "nTvSizeLeft", 2);//0-16
	_SET_INT(thiz->stAttr.stContent.stScreen.nTvSize[TV_SIZE_RIGHT], "Screen", "nTvSizeRight", TVSIZE_SCALAR);//0-16
	_SET_INT(thiz->stAttr.stContent.stScreen.nTvSize[TV_SIZE_TOP], "Screen", "nTvSizeTop", 0);//0-16
	_SET_INT(thiz->stAttr.stContent.stScreen.nTvSize[TV_SIZE_BOTTOM], "Screen", "nTvSizeBottom", TVSIZE_SCALAR);//0-16
	_SET_ENUM(thiz->stAttr.stContent.stScreen.bCVBSEnable, "Screen", "bCVBSEnable", ENV_BOOLEAN, ENV_FALSE);

//	thiz->stAttr.stContent.stScreen.nAlpha = 12; // 0 - 16
//	thiz->stAttr.stContent.stScreen.bAutoSwi = ENV_FALSE;
//	thiz->stAttr.stContent.stScreen.enAutoSwiInterval = AUTOSWI_5SEC;
//	thiz->stAttr.stContent.stScreen.enAutoSwiMode = AUTOSWI_X1;
//	thiz->stAttr.stContent.stScreen.enVGAResolution = VGA_800x600;

//	for(i = 0; i < MAX_CAM_CH; ++i){
//		char szTitle[(CHN_TITLE_LEN + 7) & ~3] = {""};
//		memset(szTitle, 0, sizeof(szTitle));
//#if defined(_OEM_TABTECHNOLOGIES)
//		sprintf(szTitle, "TAB%d", i + 1);
//#else
//		sprintf(szTitle, "CAM%d", i + 1);
//#endif
//		strcpy(thiz->stAttr.stContent.stScreen.aszTitle[i], szTitle);
//	}
//
//	thiz->stAttr.stContent.stScreen.nAlpha = 12; // 0 - 16
//	thiz->stAttr.stContent.stScreen.bAutoSwi = ENV_FALSE;
//	thiz->stAttr.stContent.stScreen.enAutoSwiInterval = AUTOSWI_5SEC;
//	thiz->stAttr.stContent.stScreen.enAutoSwiMode = AUTOSWI_X1;
//	thiz->stAttr.stContent.stScreen.enVGAResolution = VGA_800x600;
////	thiz->stAttr.stContent.stScreen.enVGAResolution = VGA_1024x768;

	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultMotion(Sysenv* const thiz)
{
	_env_default_get_init();
	int i, ii;
	// detection
	for(i = 0; i < MAX_CAM_CH; ++i){
		_SET_ENUM(thiz->stAttr.stContent.stDetection.aenMDSensitivity[i], "Motion", "aenMDSensitivity", RANK, RANK_HIGH);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.aenMDAlarmDuration[i], "Motion", "aenMDAlarmDuration", ALARM_DURATION, ALARM_5SEC);
		for(ii = 0; ii < VOUT_STANDARD_CNT; ++ii){
			memset(thiz->stAttr.stContent.stDetection.abMDArea[ii][i], 1, 22*18);
			memset((void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[ii][i])), 0, sizeof(CoverAreaRect));
		}
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abMDAlarm[i], "Motion", "abMDAlarm", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abMDBuzzer[i], "Motion", "abMDBuzzer", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abMDSMTP[i], "Motion", "abMDSMTP", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abMDFTP[i], "Motion", "abMDFTP", ENV_BOOLEAN, ENV_FALSE);
	}

	for(i = 0; i < MAX_CAM_CH; ++i){
		_SET_ENUM(thiz->stAttr.stContent.stDetection.aenVLAlarmDuration[i], "Motion", "aenVLAlarmDuration", ALARM_DURATION, ALARM_5SEC);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abVLAlarm[i], "Motion", "abVLAlarm", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abVLBuzzer[i], "Motion", "abVLBuzzer", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abVLSMTP[i], "Motion", "abVLSMTP", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stDetection.abVLFTP[i], "Motion", "abVLFTP", ENV_BOOLEAN, ENV_FALSE);
	};

//	// detection
//	for(i = 0; i < MAX_CAM_CH; ++i){
//		thiz->stAttr.stContent.stDetection.aenMDSensitivity[i] = RANK_HIGH;
//		thiz->stAttr.stContent.stDetection.aenMDAlarmDuration[i] = ALARM_5SEC;
//		for(ii = 0; ii < VOUT_STANDARD_CNT; ++ii){
//			memset(thiz->stAttr.stContent.stDetection.abMDArea[ii][i], 1, 22*18);
//			memset((void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[ii][i])), 0, sizeof(CoverAreaRect));
//		}
//		thiz->stAttr.stContent.stDetection.abMDAlarm[i] = 0;
//		thiz->stAttr.stContent.stDetection.abMDBuzzer[i] = 0;
//	}
//
//	for(i = 0; i < MAX_CAM_CH; ++i){
//		thiz->stAttr.stContent.stDetection.aenVLAlarmDuration[i] = ALARM_5SEC;
//		thiz->stAttr.stContent.stDetection.abVLAlarm[i] = 0;
//		thiz->stAttr.stContent.stDetection.abVLBuzzer[i] = 0;
//	};
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultSensor(Sysenv* const thiz)
{
	_env_default_get_init();
	int i, ii;
	// sensor
	for(i = 0; i < MAX_SENSOR_CH; ++i)
	{
		_SET_ENUM(thiz->stAttr.stContent.stSensor.aenWorkMode[i], "Sensor", "aenWorkMode", SENSOR_WORKMODE, SENSOR_DISABLE);
		_SET_ENUM(thiz->stAttr.stContent.stSensor.aenAlarmDuration[i], "Sensor", "aenAlarmDuration", ALARM_DURATION, ALARM_5SEC);
		for(ii = 0; ii < MAX_CAM_CH; ++ii){
			_SET_ENUM(thiz->stAttr.stContent.stSensor.abRecord[i][ii], "Sensor", "abRecord", ENV_BOOLEAN, ENV_FALSE);
		}
		_SET_ENUM(thiz->stAttr.stContent.stSensor.abAlarm[i], "Sensor", "abAlarm", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stSensor.abBuzzer[i], "Sensor", "abBuzzer", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stSensor.abSMTP[i], "Sensor", "abSMTP", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stSensor.abFTP[i], "Sensor", "abFTP", ENV_BOOLEAN, ENV_FALSE);
	}

//	// sensor
//	for(i = 0; i < MAX_SENSOR_CH; ++i)
//	{
//		thiz->stAttr.stContent.stSensor.aenWorkMode[i] = SENSOR_DISABLE;
//		thiz->stAttr.stContent.stSensor.aenAlarmDuration[i] = ALARM_5SEC;
//		for(ii = 0; ii < MAX_CAM_CH; ++ii){
//			thiz->stAttr.stContent.stSensor.abRecord[i][ii] = ENV_FALSE;
//		}
//		thiz->stAttr.stContent.stSensor.abAlarm[i] = 0;
//		thiz->stAttr.stContent.stSensor.abBuzzer[i] = 0;
//	}
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultPtz(Sysenv* const thiz)
{
	_env_default_get_init();
	int i = 0, ii = 0;
	// ptz
	for(i = 0; i < MAX_CAM_CH; ++i)
	{
		thiz->stAttr.stContent.stPTZ.au8Id[i] = 1;
		_SET_ENUM(thiz->stAttr.stContent.stPTZ.aenProtocal[i], "Ptz", "aenProtocal", PTZ_PROTOCAL, PELCO_D);
		_SET_ENUM(thiz->stAttr.stContent.stPTZ.aenBaudrate[i], "Ptz", "aenBaudrate", PTZ_BAUDRATE, BD_2400);
		thiz->stAttr.stContent.stPTZ.astTour[i].nTourPointCnt = 0;
		for(ii = 0; ii < MAX_TOUR_POINT; ++ii){
			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Preset = ii + 1;
			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Hour = 0;
			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Min = 0;
			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Sec = 10;
		}
	};

//	// ptz
//	for(i = 0; i < MAX_CAM_CH; ++i)
//	{
//		thiz->stAttr.stContent.stPTZ.au8Id[i] = 1;
//		thiz->stAttr.stContent.stPTZ.aenProtocal[i] = PELCO_D;
//		thiz->stAttr.stContent.stPTZ.aenBaudrate[i] = BD_2400;
////		thiz->stAttr.stContent.stPTZ.aenDatabit[i] = DB_8BIT;
////		thiz->stAttr.stContent.stPTZ.aenStopbit[i] = SB_ONE;
////		thiz->stAttr.stContent.stPTZ.aenParity[i] = PB_NONE;
//		thiz->stAttr.stContent.stPTZ.astTour[i].nTourPointCnt = MAX_TOUR_POINT;
//		for(ii = 0; ii < MAX_TOUR_POINT; ++ii){
//			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Preset = ii + 1;
//			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Hour = 0;
//			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Min = 0;
//			thiz->stAttr.stContent.stPTZ.astTour[i].astTourPount[ii].u8Sec = 10;
//		}
//	};
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultTelecontrol(struct _Sysenv* const thiz)
{
	_env_default_get_init();
	int i = 0, ii = 0;
	SysenvTelecontrol* const pstTelecontrol = &(thiz->stAttr.stContent.stTelecontrol);
	for(i = 0; i < sizeof(pstTelecontrol->anTeleID) / sizeof(pstTelecontrol->anTeleID[0]); ++i){
		_SET_INT(thiz->stAttr.stContent.stTelecontrol.anTeleID[i], "Telecontrol", "anTeleID", 0);
		_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.abEnable[i], "Telecontrol", "abEnable", ENV_BOOLEAN, ENV_TRUE);
		_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.aenAlarmDuration[i], "Telecontrol", "aenAlarmDuration", ALARM_DURATION, ALARM_1SEC);
		for(ii = 0; ii < sizeof(pstTelecontrol->abRecord[i]) / sizeof(pstTelecontrol->abRecord[i][0]); ++ii){
			_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.abRecord[i][ii], "Telecontrol", "abRecord", ENV_BOOLEAN, ENV_FALSE);
		}
		_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.abAlarm[i], "Telecontrol", "abAlarm", ENV_BOOLEAN, ENV_FALSE);
		_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.abBuzzer[i], "Telecontrol", "abBuzzer", ENV_BOOLEAN, ENV_TRUE);
		_SET_ENUM(thiz->stAttr.stContent.stTelecontrol.abNetworkNotice[i], "Telecontrol", "abNetworkNotice", ENV_BOOLEAN, ENV_TRUE);
	}

//	SysenvTelecontrol* const pstTelecontrol = &(thiz->stAttr.stContent.stTelecontrol);
//	for(i = 0; i < sizeof(pstTelecontrol->anTeleID) / sizeof(pstTelecontrol->anTeleID[0]); ++i){
//		pstTelecontrol->anTeleID[i] = 0;
//		pstTelecontrol->abEnable[i] = ENV_TRUE;
//		pstTelecontrol->aenAlarmDuration[i] = ALARM_1SEC;
//		for(ii = 0; ii < sizeof(pstTelecontrol->abRecord[i]) / sizeof(pstTelecontrol->abRecord[i][0]); ++ii){
//			pstTelecontrol->abRecord[i][ii] = ((i == ii) ? ENV_TRUE : ENV_FALSE);
//		}
//		pstTelecontrol->abAlarm[i] = ENV_FALSE;
//		pstTelecontrol->abBuzzer[i] = ENV_TRUE;
//		pstTelecontrol->abNetworkNotice[i] = ENV_TRUE;
//	}
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultUps(struct _Sysenv* const thiz)
{
	_env_default_get_init();
	_SET_ENUM(thiz->stAttr.stContent.stUps.bEnable, "Ups", "bEnable", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stUps.enAlarmDuration, "Ups", "enAlarmDuration", ALARM_DURATION, ALARM_1SEC);
	_SET_ENUM(thiz->stAttr.stContent.stUps.bAlarm, "Ups", "bAlarm", ENV_BOOLEAN, ENV_FALSE);
	_SET_ENUM(thiz->stAttr.stContent.stUps.bBuzzer, "Ups", "bBuzzer", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stUps.bNetworkNotice, "Ups", "bNetworkNotice", ENV_BOOLEAN, ENV_TRUE);
//	SysenvUps* const pstUps = &(thiz->stAttr.stContent.stUps);
//	pstUps->bEnable = ENV_TRUE;
//	pstUps->enAlarmDuration = ALARM_1SEC;
//	pstUps->bAlarm = ENV_FALSE;
//	pstUps->bBuzzer = ENV_TRUE;
//	pstUps->bNetworkNotice = ENV_TRUE;
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultHdd(Sysenv* const thiz)
{
	_env_default_get_init();
	_SET_ENUM(thiz->stAttr.stContent.stHDD.bOverwrite, "Hdd", "bOverwrite", ENV_BOOLEAN, ENV_TRUE);
	_SET_ENUM(thiz->stAttr.stContent.stHDD.bDeleteOldFiles, "Hdd", "Hdd", ENV_BOOLEAN, ENV_FALSE);
	_SET_INT(thiz->stAttr.stContent.stHDD.nDaysAgo, "Hdd", "nDaysAgo", 30);
//	thiz->stAttr.stContent.stHDD.bOverwrite = ENV_TRUE;
	// get crc
	UpdateCRC16(thiz);
	_env_default_get_destory();
	return 0;
}

static int RestoreDefaultMaintenance(Sysenv* const thiz)
{
    _env_default_get_init();
    _SET_ENUM(thiz->stAttr.stContent.stMaintenance.enable,"Maintenance","enable",ENV_BOOLEAN, ENV_FALSE);
    _SET_ENUM(thiz->stAttr.stContent.stMaintenance.mode, "Maintenance", "mode",Maintenance_Mode , Maintenance_Never);
    _SET_INT(thiz->stAttr.stContent.stMaintenance.schedule,"Maintenance", "schedule", 0);
    memset(&thiz->stAttr.stContent.stMaintenance.stTime, 0, sizeof(MaintenanceTime));
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.year,"Maintenance", "schedule-year", 0);
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.month,"Maintenance", "schedule-month", 0);
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.day,"Maintenance", "schedule-day", 0);
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.hour,"Maintenance", "schedule-hour", 2);
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.min,"Maintenance", "schedule-min", 0);
	_SET_INT(thiz->stAttr.stContent.stMaintenance.stTime.sec,"Maintenance", "schedule-sec", 0);
    UpdateCRC16(thiz);
    _env_default_get_destory();
    return 0;
}

static int RestoreDefault(Sysenv* const thiz)
{
	// clear all
	memset(&thiz->stAttr, 0, sizeof(SysenvAttr));
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// color
	RestoreDefaultColor(thiz);
	// general
	RestoreDefaultGeneral(thiz);
	// encode
	RestoreDefaultEncode(thiz);
//#ifdef _NVR
	// ipcam
	RestoreDefaultIPCam(thiz);
//#endif
	// hvr
	RestoreDefaultHvr(thiz);
	// record
	RestoreDefaultRecode(thiz);
	// network
	RestoreDefaultNetwork(thiz);
	// screen
	RestoreDefaultScreen(thiz);
	// detection
	RestoreDefaultMotion(thiz);
	// ptz
	RestoreDefaultPtz(thiz);
	// sensor
	RestoreDefaultSensor(thiz);
	// telecontrol
	RestoreDefaultTelecontrol(thiz);
	// ups
	RestoreDefaultUps(thiz);
	// hdd
	RestoreDefaultHdd(thiz);
	//maintenance
	RestoreDefaultMaintenance(thiz);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// get crc
	UpdateCRC16(thiz);
	return 0;
}

static int Reset(Sysenv* const thiz)
{
	ENV_TRACE("Environment reset\r\n");
	thiz->RestoreDefault(thiz);
	thiz->Print(thiz);
	// save
	thiz->Save(thiz);
	return 0;
}

static int Print(Sysenv* const thiz)
{
	int i = 0;
	ENV_TRACE("system environment:");
	printf("System Version: %d.%d.%d%s\r\n", SWVER_MAJ, SWVER_MIN, SWVER_REV, SWVER_EXT);
	//printf("Software Release: %s\r\n", thiz->stAttr.stContent.szReleaseDate);
	printf("Video Standard: %s\r\n", (VOUT_PAL == thiz->stAttr.stContent.stGeneral.enStandard) ? "PAL" : "NTSC");

	printf("Date Format: %s\r\n", (YYYY_MM_DD == thiz->stAttr.stContent.stGeneral.enDateFmt) ? "YYYY/MM/DD" : ((MM_DD_YYYY == thiz->stAttr.stContent.stGeneral.enDateFmt) ? "MM/DD/YYY" : "DD/MM/YYYY"));
	printf("DVR Number: %d\r\n", thiz->stAttr.stContent.stGeneral.nDVRId);

	printf("\r\n");
	printf("HDD Overwrite: %s\r\n", thiz->stAttr.stContent.stHDD.bOverwrite ? "Yes" : "No");
	printf("HDD DeleteOldFiles, %d DaysAgo: %s\r\n",
			thiz->stAttr.stContent.stHDD.nDaysAgo,
			thiz->stAttr.stContent.stHDD.bDeleteOldFiles ? "Yes" : "No");
	printf("Hue:\t");
	for(i = 0; i < MAX_CAM_CH; ++i){
		printf("%d", thiz->stAttr.stContent.stColor.au8Hue[i]);
	}
	printf("\r\n");
	printf("Brightness:\t");
	for(i = 0; i < MAX_CAM_CH; ++i){
		printf("%d", thiz->stAttr.stContent.stColor.au8Luminance[i]);
	}
	printf("\r\n");
	printf("Contrast:\t");
	for(i = 0; i < MAX_CAM_CH; ++i){
		printf("%d", thiz->stAttr.stContent.stColor.au8Contrast[i]);
	}
	printf("\r\n");
	printf("Saturation:\t");
	for(i = 0; i < MAX_CAM_CH; ++i){
		printf("%d", thiz->stAttr.stContent.stColor.au8Saturation[i]);
	}
	printf("\r\n");

	printf("\r\n");
	printf("Menu Alpha: %d\r\n", thiz->stAttr.stContent.stScreen.nAlpha);

	printf("\r\n");
	printf("MAC: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
		thiz->stAttr.stContent.stNetwork.au8MACAddr[0],
		thiz->stAttr.stContent.stNetwork.au8MACAddr[1],
		thiz->stAttr.stContent.stNetwork.au8MACAddr[2],
		thiz->stAttr.stContent.stNetwork.au8MACAddr[3],
		thiz->stAttr.stContent.stNetwork.au8MACAddr[4],
		thiz->stAttr.stContent.stNetwork.au8MACAddr[5]);

	printf("IP Address: %d.%d.%d.%d\r\n",
		thiz->stAttr.stContent.stNetwork.au8IPAddr[0],
		thiz->stAttr.stContent.stNetwork.au8IPAddr[1],
		thiz->stAttr.stContent.stNetwork.au8IPAddr[2],
		thiz->stAttr.stContent.stNetwork.au8IPAddr[3]);
	printf("Mask: %d.%d.%d.%d\r\n",
		thiz->stAttr.stContent.stNetwork.au8SubMask[0],
		thiz->stAttr.stContent.stNetwork.au8SubMask[1],
		thiz->stAttr.stContent.stNetwork.au8SubMask[2],
		thiz->stAttr.stContent.stNetwork.au8SubMask[3]);
	printf("Gateway: %d.%d.%d.%d\r\n",
		thiz->stAttr.stContent.stNetwork.au8Gateway[0],
		thiz->stAttr.stContent.stNetwork.au8Gateway[1],
		thiz->stAttr.stContent.stNetwork.au8Gateway[2],
		thiz->stAttr.stContent.stNetwork.au8Gateway[3]);

	printf("PPPoE User: %s\r\n", thiz->stAttr.stContent.stNetwork.szPPPoEUser);
	printf("PPPoE Password: %s\r\n", thiz->stAttr.stContent.stNetwork.szPPPoEPwd);

	printf("\r\n");
	printf("DNS: %d.%d.%d.%d\r\n",
		thiz->stAttr.stContent.stNetwork.au8DNSAddr[0],
		thiz->stAttr.stContent.stNetwork.au8DNSAddr[1],
		thiz->stAttr.stContent.stNetwork.au8DNSAddr[2],
		thiz->stAttr.stContent.stNetwork.au8DNSAddr[3]);
	printf("DDNS Enable: %s\r\n", thiz->stAttr.stContent.stNetwork.bDDNSUse ? "Yes" : "No");
	printf("DDNS URL: %s\r\n", thiz->stAttr.stContent.stNetwork.szDDNSUrl);
	printf("DDNS User: %s\r\n", thiz->stAttr.stContent.stNetwork.szDDNSUser);
	printf("DDNS Password: %s\r\n", thiz->stAttr.stContent.stNetwork.szDDNSPassword);

	printf("\r\n");
	printf("CRC: %04x\r\n", thiz->stAttr.u16CRC);
	printf("\r\n\r\n");
	return 0;
}

// color
static ENV_COLOR_VALUE GetHue(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stColor.au8Hue[channel];
}

static void SetHue(Sysenv* const thiz, int channel, ENV_COLOR_VALUE hue){
	thiz->stAttr.stContent.stColor.au8Hue[channel] = hue;
}

static ENV_COLOR_VALUE GetContrast(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stColor.au8Contrast[channel];
}

static void SetContrast(Sysenv* const thiz, int channel, ENV_COLOR_VALUE contrast){
	thiz->stAttr.stContent.stColor.au8Contrast[channel] = contrast;
}

static ENV_COLOR_VALUE GetLuminance(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stColor.au8Luminance[channel];
}

static void SetBrightness(Sysenv* const thiz, int channel, ENV_COLOR_VALUE luminance)
{
	thiz->stAttr.stContent.stColor.au8Luminance[channel] = luminance;
}
static ENV_COLOR_VALUE GetSaturation(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stColor.au8Saturation[channel];
}
static void SetSaturation(Sysenv* const thiz, int channel, ENV_COLOR_VALUE saturation){
	thiz->stAttr.stContent.stColor.au8Saturation[channel] = saturation;
}
static void GetDefaultColorValue(struct _Sysenv* const thiz, int* pHue, int* pContrast, int* pLuminance, int* pSaturation)
{
	_env_default_get_init();
	{
		_SET_INT(*pHue, "Color", "au8Hue", DEFAULT_HUE);
		_SET_INT(*pContrast, "Color", "au8Contrast", DEFAULT_CONTRAST);
		_SET_INT(*pLuminance, "Color", "au8Luminance", DEFAULT_BRIGHTNESS);
		_SET_INT(*pSaturation, "Color", "au8Saturation", DEFAULT_SATURATION);
	}
	_env_default_get_destory();
}


// misc
static DATE_FMT GetDateFmt(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enDateFmt;
}
static void SetDateFmt(Sysenv* const thiz, DATE_FMT format){
	thiz->stAttr.stContent.stGeneral.enDateFmt = format;
}
static ENV_BOOLEAN IsKeyLock(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bKeyLock;
}
static void SetKeyLock(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stGeneral.bKeyLock = set;
}
static ENV_BOOLEAN IsKeyBuzzer(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bKeyBuzzer;
}
static void SetKeyBuzzer(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stGeneral.bKeyBuzzer = set;
}
static WORLDWIDE_LANGUAGE GetLang(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enLanguage;
}
static void SetLang(Sysenv* const thiz, WORLDWIDE_LANGUAGE lang){
	thiz->stAttr.stContent.stGeneral.enLanguage = lang;
}
static VOUT_STANDARD GetStandard(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enStandard;
}
static void SetStandard(Sysenv* const thiz, VOUT_STANDARD standard){
	thiz->stAttr.stContent.stGeneral.enStandard = standard;
}
static KEYMAP_TYPE GetKey_Type(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enKey_Type;
}
static void SetKey_Type(Sysenv* const thiz, KEYMAP_TYPE key_type){
	thiz->stAttr.stContent.stGeneral.enKey_Type = key_type;
}
static REMOTE_TYPE GetRemote_Type(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enRemote_Type;
}
static void SetRemote_Type(Sysenv* const thiz, REMOTE_TYPE Remote_type){
	thiz->stAttr.stContent.stGeneral.enRemote_Type = Remote_type;
}
static unsigned int GetDVRId(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nDVRId;
}
static void SetDVRId(Sysenv* const thiz, unsigned int id){
	thiz->stAttr.stContent.stGeneral.nDVRId = id;
}
static unsigned int GetDeviceMode(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nDeviceMode;
}
static void SetDeviceMode(Sysenv* const thiz, unsigned int index){
	thiz->stAttr.stContent.stGeneral.nDeviceMode = index;
}
static unsigned int GetDeviceModeMax(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nDeviceModeMax;
}
static void SetDeviceModeMax(Sysenv* const thiz, unsigned int index){
	thiz->stAttr.stContent.stGeneral.nDeviceModeMax = index;
}
static ENC_POSITION_MODE GetTimerPositionCustom(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.enTimerPositionMode;
}
static void SetTimerPositionCustom(Sysenv* const thiz, ENC_POSITION_MODE positionMode){
	thiz->stAttr.stContent.stGeneral.enTimerPositionMode = positionMode;
}
static int GetTimerXCoordinate(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nXCoordinateTimer;
}
static void SetTimerXCoordinate(Sysenv* const thiz, int xCoordinate){
	thiz->stAttr.stContent.stGeneral.nXCoordinateTimer = xCoordinate;
}
static int GetTimerYCoordinate(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nYCoordinateTimer;
}
static void SetTimerYCoordinate(Sysenv* const thiz, int yCoordinate){
	thiz->stAttr.stContent.stGeneral.nYCoordinateTimer = yCoordinate;
}

static ENC_POSITION_MODE GetEseeidPositionCustom(Sysenv* const thiz){

	return thiz->stAttr.stContent.stGeneral.enEseeidPositionMode;
}
static void SetEseeidPositionCustom(Sysenv* const thiz, ENC_POSITION_MODE positionMode){
	thiz->stAttr.stContent.stGeneral.enEseeidPositionMode = positionMode;
}
static int GetEseeidXCoordinate(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nXCoordinateEseeid;
}
static void SetEseeidXCoordinate(Sysenv* const thiz, int xCoordinate){
	thiz->stAttr.stContent.stGeneral.nXCoordinateEseeid = xCoordinate;
}
static int GetEseeidYCoordinate(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.nYCoordinateEseeid;
}
static void SetEseeidYCoordinate(Sysenv* const thiz, int yCoordinate){
	thiz->stAttr.stContent.stGeneral.nYCoordinateEseeid = yCoordinate;
}
static void GetSpecialFTPServerPath(struct _Sysenv* const thiz, char ret_path[MAX_FTP_SERVER_PATH_LEN+1]){
	strcpy(ret_path, thiz->stAttr.stContent.stGeneral.azSpecialFTPServerPath);
}
static void SetSpecialFTPServerPath(struct _Sysenv* const thiz, const char path[MAX_FTP_SERVER_PATH_LEN+1])
{
	strcpy(thiz->stAttr.stContent.stGeneral.azSpecialFTPServerPath, path);
}
static void GetSupportWeb(Sysenv* const thiz, char ret_web[SUPPORT_WEB_LINK_LEN]){
	strncpy(ret_web, thiz->stAttr.stContent.stGeneral.Support_Web, SUPPORT_WEB_LINK_LEN);
}
static void SetSupportWeb(Sysenv* const thiz, const char web[SUPPORT_WEB_LINK_LEN]){
	strncpy(thiz->stAttr.stContent.stGeneral.Support_Web, web, SUPPORT_WEB_LINK_LEN);
}
static void GetAndroidLink(Sysenv* const thiz, char ret_link[SUPPORT_WEB_LINK_LEN]){
	strncpy(ret_link, thiz->stAttr.stContent.stGeneral.AndroidLink, SUPPORT_WEB_LINK_LEN);
}
static void SetAndroidLink(Sysenv* const thiz, const char link[SUPPORT_WEB_LINK_LEN]){
	strncpy(thiz->stAttr.stContent.stGeneral.AndroidLink, link, SUPPORT_WEB_LINK_LEN);
}
static void GetIOSLink(Sysenv* const thiz, char ret_link[SUPPORT_WEB_LINK_LEN]){
	strncpy(ret_link, thiz->stAttr.stContent.stGeneral.IOSLink, SUPPORT_WEB_LINK_LEN);
}
static void SetIOSLink(Sysenv* const thiz, const char link[SUPPORT_WEB_LINK_LEN]){
	strncpy(thiz->stAttr.stContent.stGeneral.IOSLink, link, SUPPORT_WEB_LINK_LEN);
}

//timer&eseeid width
static int GetEseeidWidth(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.sum_widthEseeid;
}
static void SetEseeidWidth(Sysenv* const thiz, int sum_width){
	thiz->stAttr.stContent.stGeneral.sum_widthEseeid = sum_width;
}
static int GetTimerWidth(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.sum_widthTimer;
}
static void SetTimerWidth(Sysenv* const thiz, int tsum_width){
	thiz->stAttr.stContent.stGeneral.sum_widthTimer = tsum_width;
}

static int  GetChannelModeAuto(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stGeneral.ChannelModeAuto;
}
static void SetChannelModeAuto(Sysenv* const thiz, int Value)
{
	thiz->stAttr.stContent.stGeneral.ChannelModeAuto = Value;
}

static int  GetChannelModes(Sysenv* const thiz, int Chnl)
{
	if((Chnl < 0) || (Chnl >= MAX_CAM_CH)) {
		return -1;
	}

	return thiz->stAttr.stContent.stGeneral.ChannelModes[Chnl];
}
static void SetChannelModes(Sysenv* const thiz, int Chnl, int Value)
{
	if((Chnl < 0) || (Chnl >= MAX_CAM_CH)) {
		return;
	}

	thiz->stAttr.stContent.stGeneral.ChannelModes[Chnl] = Value;
}


static void SetPersianCalendarUse(struct _Sysenv* const thiz, ENV_BOOLEAN bPersianCalendar){
	thiz->stAttr.stContent.stGeneral.bPersianCalendar = bPersianCalendar;
}

static ENV_BOOLEAN IsPersianCalendar(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bPersianCalendar;
}

static void SetTimeUse(struct _Sysenv* const thiz, ENV_BOOLEAN bTimeUse){
	thiz->stAttr.stContent.stGeneral.bTimeUse = bTimeUse;
}
static ENV_BOOLEAN IsTimeUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bTimeUse;
}

static TIME_FORMAT GetSysTimeFmt(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stGeneral.enSysTimeFmt;
}
static void SetSysTimeFmt(Sysenv* const thiz, TIME_FORMAT fmt)
{
	thiz->stAttr.stContent.stGeneral.enSysTimeFmt = fmt;
}


// encode
static ENC_MODE GetEncMode(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stEncode.aenMode[channel];
}
static void SetEncMode(Sysenv* const thiz, int channel, ENC_MODE mode){
	thiz->stAttr.stContent.stEncode.aenMode[channel] = mode;
}
static ENC_FMT GetEncFmt(Sysenv* const thiz, int channel)
{
	return thiz->stAttr.stContent.stEncode.aenFormat[channel];
}
static void SetEncFmt(Sysenv* const thiz, int channel, ENC_FMT format)
{
	thiz->stAttr.stContent.stEncode.aenFormat[channel] = format;
}
static ENC_PIC_LEVEL GetEncPicLevel(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stEncode.aenLevel[channel];
}
static void SetEncPicLevel(Sysenv* const thiz, int channel, ENC_PIC_LEVEL level){
	thiz->stAttr.stContent.stEncode.aenLevel[channel] = level;
}
static ENC_BR_MODE GetBitrateMode(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stEncode.aenBitrateMode[channel];
}
static void SetBitrateMode(Sysenv* const thiz, int channel, ENC_BR_MODE mode){
	thiz->stAttr.stContent.stEncode.aenBitrateMode[channel] = mode;
}
static ENC_BR_VALUE GetBitrateValue(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stEncode.aenBitrateValue[channel];
}
static void SetBitrateValue(Sysenv* const thiz, int channel, ENC_BR_VALUE bitrate){
	thiz->stAttr.stContent.stEncode.aenBitrateValue[channel] = bitrate;
}
static ENC_FRAMERATE GetEncFramerate(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stEncode.aenFramerate[channel];
}
static void SetEncFramerate(Sysenv* const thiz, int channel, ENC_FRAMERATE framerate){
	thiz->stAttr.stContent.stEncode.aenFramerate[channel] = framerate;
}

static ENC_MODE GetEncSubMode(Sysenv* const thiz, int channel, int sub){
	return MODE_VIDEO;
}
static void SetEncSubMode(Sysenv* const thiz, int channel, int sub, ENC_MODE mode){
}
static ENC_FMT GetEncSubFmt(Sysenv* const thiz, int channel, int sub)
{
	return thiz->stAttr.stContent.stEncode.aenSubFormat[channel][sub];
}
static void SetEncSubFmt(Sysenv* const thiz, int channel, int sub, ENC_FMT format)
{
	thiz->stAttr.stContent.stEncode.aenSubFormat[channel][sub] = format;
}
static ENC_PIC_LEVEL GetEncSubPicLevel(Sysenv* const thiz, int channel, int sub){
	return thiz->stAttr.stContent.stEncode.aenSubLevel[channel][sub];
}
static void SetEncSubPicLevel(Sysenv* const thiz, int channel, int sub, ENC_PIC_LEVEL level){
	thiz->stAttr.stContent.stEncode.aenSubLevel[channel][sub] = level;
}
static ENC_BR_MODE GetSubBitrateMode(Sysenv* const thiz, int channel, int sub){
	return thiz->stAttr.stContent.stEncode.aenSubBitrateMode[channel][sub];
}
static void SetSubBitrateMode(Sysenv* const thiz, int channel, int sub, ENC_BR_MODE mode){
	thiz->stAttr.stContent.stEncode.aenSubBitrateMode[channel][sub] = mode;
}
static ENC_BR_VALUE GetSubBitrateValue(Sysenv* const thiz, int channel, int sub){
	return thiz->stAttr.stContent.stEncode.aenSubBitrateValue[channel][sub];
}
static void SetSubBitrateValue(Sysenv* const thiz, int channel, int sub, ENC_BR_VALUE bitrate){
	thiz->stAttr.stContent.stEncode.aenSubBitrateValue[channel][sub] = bitrate;
}
static ENC_FRAMERATE GetEncSubFramerate(Sysenv* const thiz, int channel, int sub){
	return thiz->stAttr.stContent.stEncode.aenSubFramerate[channel][sub];
}
static void SetEncSubFramerate(Sysenv* const thiz, int channel, int sub, ENC_FRAMERATE framerate){
	thiz->stAttr.stContent.stEncode.aenSubFramerate[channel][sub] = framerate;
}

//#ifdef _NVR
// ipcam
static void GetIPCamAddr(struct _Sysenv* const thiz, int channel, unsigned char* ret_ipaddr){
	memcpy(ret_ipaddr, thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].au8IPAddr, 4);
}
static void SetIPCamAddr(struct _Sysenv* const thiz, int channel, unsigned char* ipaddr){
	memcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].au8IPAddr, ipaddr, 4);
}

static unsigned short GetIPCamPort(struct _Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].u16Port;
}

static 	void SetIPCamPort(struct _Sysenv* const thiz, int channel, unsigned short port){
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].u16Port = port;
}

static IPCAM_PROTOCOL_NAME GetIPCamProtocolname(struct _Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azProtocolname;
}
static void SetIPCamProtocolname(struct _Sysenv* const thiz, int channel, IPCAM_PROTOCOL_NAME protocol_name){
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azProtocolname = protocol_name;
}

static void GetIPCamUsername(struct _Sysenv* const thiz, int channel, const char* ret_username){
	strcpy(ret_username, thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azUsername);
}
static void SetIPCamUsername(struct _Sysenv* const thiz, int channel, char* username){
	strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azUsername, username);
}

static void GetIPCamPassword(struct _Sysenv* const thiz, int channel, const char* ret_password){
	strcpy(ret_password, thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azPassword);
}
static void SetIPCamPassword(struct _Sysenv* const thiz, int channel, char* password){
	strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azPassword, password);
}

static void GetIPCamModelname(struct _Sysenv* const thiz, int channel, const char* ret_modelname){
	strcpy(ret_modelname, thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azModelname);
}
static void SetIPCamModelname(struct _Sysenv* const thiz, int channel, char* modelname){
	strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azModelname, modelname);
}

static ENV_BOOLEAN IsEnableIPCChannel(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azEnable_ch;
}
static void SetEnableIPCChannel(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].azEnable_ch = set;
}
static void GetIPCMACAddr(Sysenv* const thiz, int channel,unsigned char ret_mac[MAC_ADDR_LEN]){
	memcpy(ret_mac, thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].u8MACAddr, MAC_ADDR_LEN);
}
static void SetIPCMACAddr(Sysenv* const thiz, int channel,const unsigned char mac[MAC_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].u8MACAddr, mac, MAC_ADDR_LEN);
}
static void GetIPCamPrivData(struct _Sysenv* const thiz,int channel, int stream, void *ret_buf,int ret_size)
{
	assert(ret_size >= MAX_IPCAM_PRIV_DATA);
	assert(stream < MAX_IPCAM_PROFILE_NR);
	assert(channel < MAX_CAM_CH);
	memcpy(ret_buf,&thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[stream],MAX_IPCAM_PRIV_DATA);
}
static void SetIPCamPrivData(struct _Sysenv* const thiz,int channel, int stream, void *data,int size)
{
	assert(size <= MAX_IPCAM_PRIV_DATA);
	assert(stream < MAX_IPCAM_PROFILE_NR);
	assert(channel < MAX_CAM_CH);
	memcpy(&thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[stream],data,size);
}
static int GetIPCamStreamNR(struct _Sysenv* const thiz,int channel)
{
	assert(channel < MAX_CAM_CH);
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].iStreamNR;
}
static void SetIPCamStreamNR(struct _Sysenv* const thiz,int channel, int nr)
{
	assert(channel < MAX_CAM_CH);
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].iStreamNR = nr;
}
static void GetIPCamRtspStream(struct _Sysenv* const thiz,int channel,char *m_stream,char *s_stream,
	int *m_transport,int *s_transport)
{
	strcpy(m_stream,thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[0].rtsp.stream_name);
	strcpy(s_stream,thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[1].rtsp.stream_name);
	*m_transport = thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[0].rtsp.transport;
	*s_transport = thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[1].rtsp.transport;
}
static void SetIPCamRtspStream(struct _Sysenv* const thiz,int channel,char *m_stream,char *s_stream,
	int m_transport,int s_transport)
{
	strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[0].rtsp.stream_name,m_stream);
	strcpy(thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[1].rtsp.stream_name,s_stream);
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[0].rtsp.transport = m_transport;
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[channel].stPrivData[1].rtsp.transport = s_transport;
}
static CHANNEL_WORKMODE GetChannelWorkMode(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stIPCamGroup.azWorkMode;
}
static void SetChannelWorkMode(struct _Sysenv* const thiz, CHANNEL_WORKMODE mode){
	thiz->stAttr.stContent.stIPCamGroup.azWorkMode = mode;
}
static ENV_BOOLEAN IsMultiNetworkSegment(Sysenv* const thiz){
	return thiz->stAttr.stContent.stIPCamGroup.bMultiNetworkSegment;
}
static void SetIsMultiNetworkSegment(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stIPCamGroup.bMultiNetworkSegment = set;
}
IPCAM_STREAM_CHOISE GetIPCamStreamChoise(struct _Sysenv* const thiz, int chn)
{
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[chn].azStreamChoise;
}
 void SetIPCamStreamChoise(struct _Sysenv* const thiz, int chn, IPCAM_STREAM_CHOISE choise)
{
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[chn].azStreamChoise = choise;
}
void UpdateIPcam(struct _Sysenv * const thiz, struct _Sysenv * target)
{
	memcpy(&thiz->stAttr.stContent.stIPCamGroup, &target->stAttr.stContent.stIPCamGroup, sizeof(SysenvIPCamGroup));
}
int GetIPcamStreamIndex(struct _Sysenv * const thiz, int chn, int stream)
{
	assert(stream < MAX_STREAM_CNT);
	assert(chn < MAX_CAM_CH);
	return thiz->stAttr.stContent.stIPCamGroup.astIPCam[chn].iStreamIndex[stream];
}
void SetIPcamStreamIndex(struct _Sysenv * const thiz, int chn, int stream, int index)
{
	assert(stream < MAX_STREAM_CNT);
	assert(chn < MAX_CAM_CH);
	if ( index > 0 && index < STREAM_ID_START_POINT){
		assert(0);
	}
	thiz->stAttr.stContent.stIPCamGroup.astIPCam[chn].iStreamIndex[stream] = index;
}

//#endif

//hvr
static void SetHvrBncChn(struct _Sysenv * const thiz,int nbnc){
	if(nbnc <= MAX_CAM_CH){
		thiz->stAttr.stContent.stHvr.nBnc = nbnc;
	}
}
static int GetHvrBncChn(struct _Sysenv * const thiz){
	return thiz->stAttr.stContent.stHvr.nBnc;
}

// record
static all_rec_condition_t GetSchedule(Sysenv* const thiz, WEEKDAY weekday){
	return thiz->stAttr.stContent.stRecord.astRecCond[weekday];
}
static void SetSchedule(Sysenv* const thiz, WEEKDAY weekday, int channel, int item, rec_condition_t cond){
	thiz->stAttr.stContent.stRecord.astRecCond[weekday].all_conditions[channel].channel_conditions[item] = cond;
}

// network
static ENV_BOOLEAN IsDHCPUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.bDHCPUse;
}
static void SetDHCPUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stNetwork.bDHCPUse = set;
}
static void GetMACAddr(Sysenv* const thiz, unsigned char ret_mac[MAC_ADDR_LEN]){
	memcpy(ret_mac, thiz->stAttr.stContent.stNetwork.au8MACAddr, MAC_ADDR_LEN);
}
static void SetMACAddr(Sysenv* const thiz, const unsigned char mac[MAC_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stNetwork.au8MACAddr, mac, MAC_ADDR_LEN);
}
static void GetIPAddr(Sysenv* const thiz, unsigned char ret_ip[IP_ADDR_LEN]){
	memcpy(ret_ip, thiz->stAttr.stContent.stNetwork.au8IPAddr, IP_ADDR_LEN);
}
static void SetIPAddr(Sysenv* const thiz, const unsigned char ip[IP_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stNetwork.au8IPAddr, ip, IP_ADDR_LEN);
}
static void GetSubMask(Sysenv* const thiz, unsigned char ret_submask[IP_ADDR_LEN]){
	memcpy(ret_submask, thiz->stAttr.stContent.stNetwork.au8SubMask, IP_ADDR_LEN);
}
static void SetSubMask(Sysenv* const thiz, const unsigned char submask[IP_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stNetwork.au8SubMask, submask, IP_ADDR_LEN);
}
static void GetGateway(Sysenv* const thiz, unsigned char ret_gateway[IP_ADDR_LEN]){
	memcpy(ret_gateway, thiz->stAttr.stContent.stNetwork.au8Gateway, IP_ADDR_LEN);
}
static void SetGateway(Sysenv* const thiz, const unsigned char gateway[IP_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stNetwork.au8Gateway, gateway, IP_ADDR_LEN);
}
static void GetDNSAddr(Sysenv* const thiz, unsigned char ret_dns[IP_ADDR_LEN]){
	memcpy(ret_dns, thiz->stAttr.stContent.stNetwork.au8DNSAddr, IP_ADDR_LEN);
}
static void SetDNSAddr(Sysenv* const thiz, const unsigned char dns[IP_ADDR_LEN]){
	memcpy(thiz->stAttr.stContent.stNetwork.au8DNSAddr, dns, IP_ADDR_LEN);
}
unsigned short GetHTTPPort(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.u16PortHTTP;
}
static void SetHTTPPort(Sysenv* const thiz, unsigned short port){
	thiz->stAttr.stContent.stNetwork.u16PortHTTP = port;
}
static unsigned short GetClientPort(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.u16PortClient;
}
static void SetClientPort(Sysenv* const thiz, unsigned short port){
	thiz->stAttr.stContent.stNetwork.u16PortClient = port;
}
static unsigned short GetMobilePort(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.u16PortMobile;
}
static void SetMobilePort(Sysenv* const thiz, unsigned short port){
	thiz->stAttr.stContent.stNetwork.u16PortMobile = port;
}
static ENV_BOOLEAN IsESeeNetUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.bESeeNetUse;
}
static void SetESeeNetUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stNetwork.bESeeNetUse = set;
}
static ENV_BOOLEAN IsDDNSUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.bDDNSUse;
}
static void SetDDNSUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stNetwork.bDDNSUse = set;
}
static int GetDDNSProvider(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.enDDNSProvider;
}
static void SetDDNSProvider(Sysenv* const thiz, int provider){
	thiz->stAttr.stContent.stNetwork.enDDNSProvider = provider;
}
static void GetDDNSUrl(Sysenv* const thiz, char ret_url[DDNS_URL_LEN]){
	strncpy(ret_url, thiz->stAttr.stContent.stNetwork.szDDNSUrl, DDNS_URL_LEN);
}
static void SetDDNSUrl(Sysenv* const thiz, const char url[DDNS_URL_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.szDDNSUrl, url, DDNS_URL_LEN);
}
static void GetDDNSUser(Sysenv* const thiz, char ret_user[DDNS_USER_LEN]){
	strncpy(ret_user, thiz->stAttr.stContent.stNetwork.szDDNSUser, DDNS_USER_LEN);
}
static void SetDDNSUser(Sysenv* const thiz, const char user[DDNS_USER_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.szDDNSUser, user, DDNS_USER_LEN);
}
static void GetDDNSPwd(Sysenv* const thiz, char ret_pwd[DDNS_PWD_LEN]){
	strncpy(ret_pwd, thiz->stAttr.stContent.stNetwork.szDDNSPassword, DDNS_PWD_LEN);
}
static void SetDDNSPwd(Sysenv* const thiz, const char pwd[DDNS_PWD_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.szDDNSPassword, pwd, DDNS_PWD_LEN);
}
static ENV_BOOLEAN IsPPPoE(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.bPPPoE;
}
static void SetPPPoE(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stNetwork.bPPPoE = set;
}
static void GetPPPoEUser(Sysenv* const thiz, char ret_user[PPPOE_USER_LEN]){
	strncpy(ret_user, thiz->stAttr.stContent.stNetwork.szPPPoEUser, PPPOE_USER_LEN);
}
static void SetPPPoEUser(Sysenv* const thiz, const char user[PPPOE_USER_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.szPPPoEUser, user, PPPOE_USER_LEN);
}
static void GetPPPoEPwd(Sysenv* const thiz, char ret_pwd[PPPOE_PWD_LEN]){
	strncpy(ret_pwd, thiz->stAttr.stContent.stNetwork.szPPPoEPwd, PPPOE_PWD_LEN);
}
static void SetPPPoEPwd(Sysenv* const thiz, const char pwd[PPPOE_PWD_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.szPPPoEPwd, pwd, PPPOE_PWD_LEN);
}
static PPPOE_PROTOCOL GetPPPOEProtocol(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.enPPPOEProtocol;
}
static void SetPPPOEProtocol(Sysenv* const thiz, PPPOE_PROTOCOL protocol){
	thiz->stAttr.stContent.stNetwork.enPPPOEProtocol = protocol;
}

//Guide
static ENV_BOOLEAN IsGuideUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bGuideUse;
}
static void SetIsGuideUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stGeneral.bGuideUse = set;
}

//CVBS
static ENV_BOOLEAN IsCVBSEnable(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.bCVBSEnable;
}
static void SetCVBSEnable(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stScreen.bCVBSEnable = set;
}


static ENV_BOOLEAN IsEseeidUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stGeneral.bEseeidUse;
}
static void SetIsEseeidUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stGeneral.bEseeidUse = set;
}

//3G
static ENV_BOOLEAN Is3GUse(Sysenv* const thiz){
	return thiz->stAttr.stContent.stNetwork.b3GUse;
}
static void Set3GUse(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stNetwork.b3GUse = set;
}
static void Get3GAPN(Sysenv* const thiz, char ret_APN[NET_EDITBOX_LEN]){
	strncpy(ret_APN, thiz->stAttr.stContent.stNetwork.sz3GAPN, NET_EDITBOX_LEN);
}
static void Set3GAPN(Sysenv* const thiz, const char APN[NET_EDITBOX_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.sz3GAPN, APN, NET_EDITBOX_LEN);
}
static void Get3GPIN(Sysenv* const thiz, char ret_PIN[NET_EDITBOX_LEN]){
	strncpy(ret_PIN, thiz->stAttr.stContent.stNetwork.sz3GPIN, NET_EDITBOX_LEN);
}
static void Set3GPIN(Sysenv* const thiz, const char PIN[NET_EDITBOX_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.sz3GPIN, PIN, NET_EDITBOX_LEN);
}
static void Get3GUser(Sysenv* const thiz, char ret_user[NET_EDITBOX_LEN]){
	strncpy(ret_user, thiz->stAttr.stContent.stNetwork.sz3GUser, NET_EDITBOX_LEN);
}
static void Set3GUser(Sysenv* const thiz, const char user[NET_EDITBOX_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.sz3GUser, user, NET_EDITBOX_LEN);
}
static void Get3GPwd(Sysenv* const thiz, char ret_pwd[NET_EDITBOX_LEN]){
	strncpy(ret_pwd, thiz->stAttr.stContent.stNetwork.sz3GPassword, NET_EDITBOX_LEN);
}
static void Set3GPwd(Sysenv* const thiz, const char pwd[NET_EDITBOX_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.sz3GPassword, pwd, NET_EDITBOX_LEN);
}
static void Get3GDIALNum(Sysenv* const thiz, char ret_DIALNum[NET_EDITBOX_LEN]){
	strncpy(ret_DIALNum, thiz->stAttr.stContent.stNetwork.sz3GDIALNum, NET_EDITBOX_LEN);
}
static void Set3GDIALNum(Sysenv* const thiz, const char DIALNum[NET_EDITBOX_LEN]){
	strncpy(thiz->stAttr.stContent.stNetwork.sz3GDIALNum, DIALNum, NET_EDITBOX_LEN);
}


//Network -> SMTP
static ENV_BOOLEAN IsSMTPUse(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.bSMTPUse;
}
static void SetSMTPUse(Sysenv* const thiz, ENV_BOOLEAN set)
{
	thiz->stAttr.stContent.stNetwork.bSMTPUse = set;
}
static void GetSMTPServer(Sysenv* const thiz, char ret_server[SMTP_EDITBOX_LEN])
{
	strncpy(ret_server, thiz->stAttr.stContent.stNetwork.szSMTPServer, SMTP_EDITBOX_LEN);
}
static void SetSMTPServer(Sysenv* const thiz, const char server[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPServer, server, SMTP_EDITBOX_LEN);
}
static unsigned short GetSMTPPort(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.u16SMTPPort;
}
static void SetSMTPPort(Sysenv* const thiz, unsigned short port)
{
	thiz->stAttr.stContent.stNetwork.u16SMTPPort = port;
}
static ENV_BOOLEAN IsSMTPAnonymous(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.bSMTPAnonymous;
}
static void SetSMTPAnonymous(Sysenv* const thiz, ENV_BOOLEAN anonymous)
{
	thiz->stAttr.stContent.stNetwork.bSMTPAnonymous = anonymous;
}
static void GetSMTPUser(Sysenv* const thiz, char ret_user[SMTP_EDITBOX_LEN])
{
	strncpy(ret_user, thiz->stAttr.stContent.stNetwork.szSMTPUser, SMTP_EDITBOX_LEN);
}
static void SetSMTPUser(Sysenv* const thiz, const char user[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPUser, user, SMTP_EDITBOX_LEN);
}
static void GetSMTPPwd(Sysenv* const thiz, char ret_pwd[SMTP_EDITBOX_LEN])
{
	strncpy(ret_pwd, thiz->stAttr.stContent.stNetwork.szSMTPPwd, SMTP_EDITBOX_LEN);
}
static void SetSMTPPwd(Sysenv* const thiz, const char pwd[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPPwd, pwd, SMTP_EDITBOX_LEN);
}
static SMTP_ENCRYPT_TYPE GetSMTPEncryptType(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.enSMTPEncryptType;
}
static void SetSMTPEncryptType(Sysenv* const thiz, SMTP_ENCRYPT_TYPE type)
{
	thiz->stAttr.stContent.stNetwork.enSMTPEncryptType = type;
}
static void GetSMTPSender(Sysenv* const thiz, char ret_sender[SMTP_EDITBOX_LEN])
{
	strncpy(ret_sender, thiz->stAttr.stContent.stNetwork.szSMTPSender, SMTP_EDITBOX_LEN);
}
static void SetSMTPSender(Sysenv* const thiz, const char sender[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPSender, sender, SMTP_EDITBOX_LEN);
}
static void GetSMTPSendee1(Sysenv* const thiz, char ret_sendee1[SMTP_EDITBOX_LEN])
{
	strncpy(ret_sendee1, thiz->stAttr.stContent.stNetwork.szSMTPSendee1, SMTP_EDITBOX_LEN);
}
static void SetSMTPSendee1(Sysenv* const thiz, const char sendee1[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPSendee1, sendee1, SMTP_EDITBOX_LEN);
}
static void GetSMTPSendee2(Sysenv* const thiz, char ret_sendee2[SMTP_EDITBOX_LEN])
{
	strncpy(ret_sendee2, thiz->stAttr.stContent.stNetwork.szSMTPSendee2, SMTP_EDITBOX_LEN);
}
static void SetSMTPSendee2(Sysenv* const thiz, const char sendee2[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPSendee2, sendee2, SMTP_EDITBOX_LEN);
}
static void GetSMTPSubject(Sysenv* const thiz, char ret_subject[SMTP_EDITBOX_LEN])
{
	strncpy(ret_subject, thiz->stAttr.stContent.stNetwork.szSMTPSubject, SMTP_EDITBOX_LEN);
}
static void SetSMTPSubject(Sysenv* const thiz, const char subject[SMTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szSMTPSubject, subject, SMTP_EDITBOX_LEN);
}
static unsigned short GetSMTPInterval(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.u16SMTPInterval;
}
static void SetSMTPInterval(Sysenv* const thiz, unsigned short interval)
{
	thiz->stAttr.stContent.stNetwork.u16SMTPInterval = interval;
}
static ENV_BOOLEAN IsSMTPHealthEnable(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.bSMTPHealthEnable;
}
static void SetSMTPHealthEnable(Sysenv* const thiz, ENV_BOOLEAN enable)
{
	thiz->stAttr.stContent.stNetwork.bSMTPHealthEnable = enable;
}
static unsigned short GetSMTPHealthInterval(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.u16SMTPHealthInterval;
}
static void SetSMTPHealthInterval(Sysenv* const thiz, unsigned short interval)
{
	thiz->stAttr.stContent.stNetwork.u16SMTPHealthInterval = interval;
}

//online upgrade
static ENV_BOOLEAN IsUsePreHost(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.UsePreHost;
}
static void SetUsePreHost(Sysenv* const thiz, ENV_BOOLEAN Use)
{
	thiz->stAttr.stContent.stNetwork.UsePreHost = Use;
}
static void GetUpgradePreHost(Sysenv* const thiz, char ret_Host[ONLINE_UPGRADE_HOST_LEN])
{
	strncpy(ret_Host, thiz->stAttr.stContent.stNetwork.UpgradePreHost, ONLINE_UPGRADE_HOST_LEN);
}
static void SetUpgradePreHost(Sysenv* const thiz, const char Host[ONLINE_UPGRADE_HOST_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.UpgradePreHost, Host, ONLINE_UPGRADE_HOST_LEN);
}
static void GetUpgradePreUrl(Sysenv* const thiz, char ret_Url[ONLINE_UPGRADE_URL_LEN])
{
	strncpy(ret_Url, thiz->stAttr.stContent.stNetwork.UpgradePreUrl, ONLINE_UPGRADE_URL_LEN);
}
static void SetUpgradePreUrl(Sysenv* const thiz, const char Url[ONLINE_UPGRADE_URL_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.UpgradePreUrl, Url, ONLINE_UPGRADE_URL_LEN);
}
static unsigned short GetUpgradePrePort(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.UpgradePrePort;
}
static void SetUpgradePrePort(Sysenv* const thiz, unsigned short Port)
{
	thiz->stAttr.stContent.stNetwork.UpgradePrePort = Port;
}

//Network -> FTP
static ENV_BOOLEAN IsFTPUse(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.bFTPUse;
}
static void SetFTPUse(Sysenv* const thiz, ENV_BOOLEAN set)
{
	thiz->stAttr.stContent.stNetwork.bFTPUse = set;
}
static void GetFTPServer(Sysenv* const thiz, char ret_server[FTP_EDITBOX_LEN])
{
	strncpy(ret_server, thiz->stAttr.stContent.stNetwork.szFTPServer, FTP_EDITBOX_LEN);
}
static void SetFTPServer(Sysenv* const thiz, const char server[FTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szFTPServer, server, FTP_EDITBOX_LEN);
}
static unsigned short GetFTPPort(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.u16FTPPort;
}
static void SetFTPPort(Sysenv* const thiz, unsigned short port)
{
	thiz->stAttr.stContent.stNetwork.u16FTPPort = port;
}
static ENV_BOOLEAN IsFTPAnonymous(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.bFTPAnonymous;
}
static void SetFTPAnonymous(Sysenv* const thiz, ENV_BOOLEAN anonymous)
{
	thiz->stAttr.stContent.stNetwork.bFTPAnonymous = anonymous;
}
static void GetFTPUser(Sysenv* const thiz, char ret_user[FTP_EDITBOX_LEN])
{
	strncpy(ret_user, thiz->stAttr.stContent.stNetwork.szFTPUser, FTP_EDITBOX_LEN);
}
static void SetFTPUser(Sysenv* const thiz, const char user[FTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szFTPUser, user, FTP_EDITBOX_LEN);
}
static void GetFTPPwd(Sysenv* const thiz, char ret_pwd[FTP_EDITBOX_LEN])
{
	strncpy(ret_pwd, thiz->stAttr.stContent.stNetwork.szFTPPwd, FTP_EDITBOX_LEN);
}
static void SetFTPPwd(Sysenv* const thiz, const char pwd[FTP_EDITBOX_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szFTPPwd, pwd, FTP_EDITBOX_LEN);
}
static void GetFTPRemoteFolder(Sysenv* const thiz, char ret_folder[FTP_FOLDER_LEN])
{
	strncpy(ret_folder, thiz->stAttr.stContent.stNetwork.szFTPRemoteFolder, FTP_FOLDER_LEN);
}
static void SetFTPRemoteFolder(Sysenv* const thiz, const char folder[FTP_FOLDER_LEN])
{
	strncpy(thiz->stAttr.stContent.stNetwork.szFTPRemoteFolder, folder, FTP_FOLDER_LEN);
}
static unsigned long GetFTPFileMaxSize(Sysenv* const thiz)
{
	return thiz->stAttr.stContent.stNetwork.u32FTPFileMaxSize;
}
static void SetFTPFileMaxSize(Sysenv* const thiz, unsigned long size)
{
	thiz->stAttr.stContent.stNetwork.u32FTPFileMaxSize = size;
}
// FTP Schedule
static all_rec_condition_t GetFTPSchedule(Sysenv* const thiz, WEEKDAY weekday){
	return thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[weekday];
}
static void SetFTPSchedule(Sysenv* const thiz, WEEKDAY weekday, int channel, int item, rec_condition_t cond){
	thiz->stAttr.stContent.stNetwork.stFTPSchedule.astRecCond[weekday].all_conditions[channel].channel_conditions[item] = cond;
}

// screen
static void GetTitle(Sysenv* const thiz, int channel, char ret_title[CHN_TITLE_LEN]){
	strncpy(ret_title, thiz->stAttr.stContent.stScreen.aszTitle[channel], CHN_TITLE_LEN);
}
static const char* GetTitleEx(Sysenv* const thiz, int channel){
	return (const char*)(thiz->stAttr.stContent.stScreen.aszTitle[channel]);
}
static void SetTitle(Sysenv* const thiz, int channel, char title[CHN_TITLE_LEN]){
	memset(thiz->stAttr.stContent.stScreen.aszTitle[channel], 0, sizeof(thiz->stAttr.stContent.stScreen.aszTitle[channel]));
	strncpy(thiz->stAttr.stContent.stScreen.aszTitle[channel], title, CHN_TITLE_LEN);
}
static int GetAlpha(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.nAlpha;
}
static void SetAlpha(Sysenv* const thiz, int alpha){
	thiz->stAttr.stContent.stScreen.nAlpha = alpha;
}
static ENV_BOOLEAN IsAutoSwi(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.bAutoSwi;
}
static ENV_BOOLEAN IsSkipLoss(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.bSkipLoss;
}
static void SetAutoSwi(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stScreen.bAutoSwi = set;
}
static void SetSkipLoss(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stScreen.bSkipLoss = set;
}
static AUTOSWI_INTERVAL GetAutoSwiInterval(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.enAutoSwiInterval;
}
static void SetAutoSwiInterval(Sysenv* const thiz, AUTOSWI_INTERVAL interval){
	thiz->stAttr.stContent.stScreen.enAutoSwiInterval = interval;
}
static AUTOSWI_MODE GetAutoSwiMode(Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.enAutoSwiMode;
}
static void SetAutoSwiMode(Sysenv* const thiz, AUTOSWI_MODE mode){
	thiz->stAttr.stContent.stScreen.enAutoSwiMode = mode;
}
static VGA_RESOLUTION GetVGAResolution(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stScreen.enVGAResolution;
}
static void SetVGAResolution(struct _Sysenv* const thiz, VGA_RESOLUTION resolution){
	thiz->stAttr.stContent.stScreen.enVGAResolution = resolution;
}
static int GetTvSize(struct _Sysenv* const thiz, TvSize_E nType){
	if(nType >= 0 && nType < TV_SIZE_CNT) {
		return thiz->stAttr.stContent.stScreen.nTvSize[nType];
	}

	return -1;
}
static void SetTvSize(struct _Sysenv* const thiz, TvSize_E nType, int nVal){
	if(nType >= 0 && nType < TV_SIZE_CNT) {
		thiz->stAttr.stContent.stScreen.nTvSize[nType] = nVal;
	}
}

// detection
static MD_SENSITIVITY GetMDSensitivity(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.aenMDSensitivity[channel];
}
static void SetMDSensitivity(Sysenv* const thiz, int channel, MD_SENSITIVITY sensitivity){
	thiz->stAttr.stContent.stDetection.aenMDSensitivity[channel] = sensitivity;
}
static ALARM_DURATION GetMDAlarmDuration(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.aenMDAlarmDuration[channel];
}
static void SetMDAlarmDuration(Sysenv* const thiz, int channel, ALARM_DURATION duration){
	thiz->stAttr.stContent.stDetection.aenMDAlarmDuration[channel] = duration;
}
static void GetMDArea(Sysenv* const thiz, int channel, unsigned char ret_area[18][22]){
#if   defined(_EXT_HDDVR)
	memcpy(ret_area, thiz->stAttr.stContent.stDetection.abMDArea[VOUT_PAL][channel], 22 * 18);
#else
	memcpy(ret_area, thiz->stAttr.stContent.stDetection.abMDArea[thiz->GetStandard(thiz)][channel], 22 * 18);
#endif
}
static void SetMDArea(Sysenv* const thiz, int channel, unsigned char area[18][22]){
#if   defined(_EXT_HDDVR)
	memcpy(thiz->stAttr.stContent.stDetection.abMDArea[VOUT_PAL][channel], area, 22*18);
#else
	memcpy(thiz->stAttr.stContent.stDetection.abMDArea[thiz->GetStandard(thiz)][channel], area, 22*18);
#endif
}
static void GetVCArea(Sysenv* const thiz, int channel, CoverAreaRect *ret_area){
#if   defined(_EXT_HDDVR)
	memcpy(ret_area, (void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[VOUT_PAL][channel])), sizeof(CoverAreaRect));
#else
	memcpy(ret_area, (void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[thiz->GetStandard(thiz)][channel])), sizeof(CoverAreaRect));
#endif
}
static void SetVCArea(Sysenv* const thiz, int channel, CoverAreaRect *ret_area){
#if   defined(_EXT_HDDVR)
	memcpy((void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[VOUT_PAL][channel])), ret_area, sizeof(CoverAreaRect));
#else
	memcpy((void *)(&(thiz->stAttr.stContent.stDetection.abVCArea[thiz->GetStandard(thiz)][channel])), ret_area, sizeof(CoverAreaRect));
#endif
}

static ENV_BOOLEAN IsMDAlarm(Sysenv* const thiz, int md_channel){
	return thiz->stAttr.stContent.stDetection.abMDAlarm[md_channel];
}
static void SetMDAlarm(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abMDAlarm[channel] = set;
}
static ENV_BOOLEAN IsMDBuzzer(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abMDBuzzer[channel];
}
static void SetMDBuzzer(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abMDBuzzer[channel] = set;
}
static ENV_BOOLEAN IsMDSMTP(Sysenv* const thiz, int md_channel){
	return thiz->stAttr.stContent.stDetection.abMDSMTP[md_channel];
}
static void SetMDSMTP(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abMDSMTP[channel] = set;
}
static ENV_BOOLEAN IsMDFTP(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abMDFTP[channel];
}
static void SetMDFTP(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abMDFTP[channel] = set;
}
static ALARM_DURATION GetVLAlarmDuration(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.aenVLAlarmDuration[channel];
}
static void SetVLAlarmDuration(Sysenv* const thiz, int channel, ALARM_DURATION duration){
	thiz->stAttr.stContent.stDetection.aenVLAlarmDuration[channel] = duration;
}
static ENV_BOOLEAN IsVLAlarm(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abVLAlarm[channel];
}
static void SetVLAlarm(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abVLAlarm[channel] = set;
}
static ENV_BOOLEAN IsVLBuzzer(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abVLBuzzer[channel];
}
static void SetVLBuzzer(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abVLBuzzer[channel] = set;
}
static ENV_BOOLEAN IsVLSMTP(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abVLSMTP[channel];
}
static void SetVLSMTP(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abVLSMTP[channel] = set;
}
static ENV_BOOLEAN IsVLFTP(Sysenv* const thiz, int channel){
	return thiz->stAttr.stContent.stDetection.abVLFTP[channel];
}
static void SetVLFTP(Sysenv* const thiz, int channel, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stDetection.abVLFTP[channel] = set;
}


// ptz
static unsigned char GetPTZId(Sysenv* const thiz, int ptz_channel){
	return thiz->stAttr.stContent.stPTZ.au8Id[ptz_channel];
}
static void SetPTZId(Sysenv* const thiz, int ptz_channel, unsigned char id){
	thiz->stAttr.stContent.stPTZ.au8Id[ptz_channel] = id;
}
static PTZ_PROTOCAL GetPTZProtocal(Sysenv* const thiz, int ptz_channel){
	return thiz->stAttr.stContent.stPTZ.aenProtocal[ptz_channel];
}
static void SetPTZProtocal(Sysenv* const thiz, int ptz_channel, PTZ_PROTOCAL protocal){
	thiz->stAttr.stContent.stPTZ.aenProtocal[ptz_channel] = protocal;
}
static PTZ_BAUDRATE GetPTZBaudrate(Sysenv* const thiz, int ptz_channel){
	return thiz->stAttr.stContent.stPTZ.aenBaudrate[ptz_channel];
}
static void SetPTZBaudrate(Sysenv* const thiz, int ptz_channel, PTZ_BAUDRATE baudrate){
	thiz->stAttr.stContent.stPTZ.aenBaudrate[ptz_channel] = baudrate;
}
//static PTZ_DATABIT GetPTZDatabit(Sysenv* const thiz, int ptz_channel){
//	return thiz->stAttr.stContent.stPTZ.aenDatabit[ptz_channel];
//}
//static void SetPTZDatabit(Sysenv* const thiz, int ptz_channel, PTZ_DATABIT databit){
//	thiz->stAttr.stContent.stPTZ.aenDatabit[ptz_channel] = databit;
//}
//static PTZ_STOPBIT GetPTZStopbit(Sysenv* const thiz, int ptz_channel){
//	return thiz->stAttr.stContent.stPTZ.aenStopbit[ptz_channel];
//}
//static void SetPTZStopbit(Sysenv* const thiz, int ptz_channel, PTZ_STOPBIT stopbit){
//	thiz->stAttr.stContent.stPTZ.aenStopbit[ptz_channel] = stopbit;
//}
//static PTZ_PARITY GetPTZParity(Sysenv* const thiz, int ptz_channel){
//	return thiz->stAttr.stContent.stPTZ.aenParity[ptz_channel];
//}
//static void SetPTZParity(Sysenv* const thiz, int ptz_channel, PTZ_PARITY parity){
//	thiz->stAttr.stContent.stPTZ.aenParity[ptz_channel] = parity;
//}
static Tour GetPTZTour(Sysenv* const thiz, int ptz_channel)
{
	return thiz->stAttr.stContent.stPTZ.astTour[ptz_channel];
}

static void SetPTZTour(Sysenv* const thiz, int ptz_channel, Tour tour)
{
	thiz->stAttr.stContent.stPTZ.astTour[ptz_channel] = tour;
}
static void PrintPTZTour(Sysenv* const thiz, int ptz_channel)
{
	int i = 0;
	if(ptz_channel < MAX_CAM_CH){
		printf("channel %d tour state\r\n", ptz_channel);
		printf("total point %d\r\n", thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].nTourPointCnt);
		for(i = 0; i < thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].nTourPointCnt; ++i){
			printf("preset %d keeping %02d:%02d:%02d\r\n",
				thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].astTourPount[i].u8Preset,
				thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].astTourPount[i].u8Hour,
				thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].astTourPount[i].u8Min,
				thiz->stAttr.stContent.stPTZ.astTour[ptz_channel].astTourPount[i].u8Sec);
		}
	}
}
// sensor
static SENSOR_WORKMODE GetSensorWorkMode(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.aenWorkMode[sensor];
}
static void SetSensorWorkMode(Sysenv* const thiz, int sensor, SENSOR_WORKMODE mode){
	thiz->stAttr.stContent.stSensor.aenWorkMode[sensor] = mode;
}
static ALARM_DURATION GetSensorAlarmDuration(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.aenAlarmDuration[sensor];
}
static void SetSensorAlarmDuration(Sysenv* const thiz, int sensor, ALARM_DURATION duration){
	thiz->stAttr.stContent.stSensor.aenAlarmDuration[sensor] = duration;
}
static ENV_BOOLEAN IsSensorCamRec(Sysenv* const thiz, int sensor, int camera){
	return thiz->stAttr.stContent.stSensor.abRecord[sensor][camera];
}
static void SetSensorCamRec(Sysenv* const thiz, int sensor, int camera, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stSensor.abRecord[sensor][camera] = set;
}
static ENV_BOOLEAN IsSensorAlarm(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.abAlarm[sensor];
}
static void SetSensorAlarm(Sysenv* const thiz, int sensor, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stSensor.abAlarm[sensor] = set;
}
static ENV_BOOLEAN IsSensorBuzzer(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.abBuzzer[sensor];
}
static void SetSensorBuzzer(Sysenv* const thiz, int sensor, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stSensor.abBuzzer[sensor] = set;
}
static ENV_BOOLEAN IsSensorSMTP(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.abSMTP[sensor];
}
static void SetSensorSMTP(Sysenv* const thiz, int sensor, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stSensor.abSMTP[sensor] = set;
}
static ENV_BOOLEAN IsSensorFTP(Sysenv* const thiz, int sensor){
	return thiz->stAttr.stContent.stSensor.abFTP[sensor];
}
static void SetSensorFTP(Sysenv* const thiz, int sensor, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stSensor.abFTP[sensor] = set;
}

// telecontrol
static unsigned int GetTelecontrolId(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.anTeleID[telecontrol];
}
static void SetTelecontrolId(struct _Sysenv* const thiz, int telecontrol, unsigned int id){
	thiz->stAttr.stContent.stTelecontrol.anTeleID[telecontrol] = id;
}
static ENV_BOOLEAN IsTelecontrolEnable(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.abEnable[telecontrol];
}
static void SetTelecontrolEnable(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stTelecontrol.abEnable[telecontrol] = enable;
}
static ALARM_DURATION GetTelecontrolAlarmDuration(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.aenAlarmDuration[telecontrol];
}
static void SetTelecontrolAlarmDuration(struct _Sysenv* const thiz, int telecontrol, ALARM_DURATION duration){
	thiz->stAttr.stContent.stTelecontrol.aenAlarmDuration[telecontrol] = duration;
}
static ENV_BOOLEAN IsTelecontrolRecord(struct _Sysenv* const thiz, int telecontrol, int camera){
	return thiz->stAttr.stContent.stTelecontrol.abRecord[telecontrol][camera];
}
static void SetTelecontrolRecord(struct _Sysenv* const thiz, int telecontrol, int camera, ENV_BOOLEAN record){
	thiz->stAttr.stContent.stTelecontrol.abRecord[telecontrol][camera] = record;
}
static ENV_BOOLEAN IsTelecontrolAlarm(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.abAlarm[telecontrol];
}
static void SetTelecontrolAlarm(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stTelecontrol.abAlarm[telecontrol] = enable;
}
static ENV_BOOLEAN IsTelecontrolBuzzer(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.abBuzzer[telecontrol];
}
static void SetTelecontrolBuzzer(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stTelecontrol.abBuzzer[telecontrol] = enable;
}
static ENV_BOOLEAN IsTelecontrolNetworkNotice(struct _Sysenv* const thiz, int telecontrol){
	return thiz->stAttr.stContent.stTelecontrol.abNetworkNotice[telecontrol];
}
static void SetTelecontrolNetworkNotice(struct _Sysenv* const thiz, int telecontrol, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stTelecontrol.abNetworkNotice[telecontrol] = enable;
}

// ups
static ENV_BOOLEAN IsUpsEnable(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stUps.bEnable;
}
static void SetUpsEnable(struct _Sysenv* const thiz, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stUps.bEnable = enable;
}
static ALARM_DURATION GetUpsAlarmDuration(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stUps.enAlarmDuration;
}
static void SetUpsAlarmDuration(struct _Sysenv* const thiz, ALARM_DURATION duration){
	thiz->stAttr.stContent.stUps.enAlarmDuration = duration;
}
static ENV_BOOLEAN IsUpsAlarm(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stUps.bAlarm;
}
static void SetUpsAlarm(struct _Sysenv* const thiz, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stUps.bAlarm = enable;
}
static ENV_BOOLEAN IsUpsBuzzer(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stUps.bBuzzer;
}
static void SetUpsBuzzer(struct _Sysenv* const thiz, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stUps.bBuzzer = enable;
}
static ENV_BOOLEAN IsUpsNetworkNotice(struct _Sysenv* const thiz){
	return thiz->stAttr.stContent.stUps.bNetworkNotice;
}
static void SetUpsNetworkNotice(struct _Sysenv* const thiz, ENV_BOOLEAN enable){
	thiz->stAttr.stContent.stUps.bNetworkNotice = enable;
}

// hdd
static ENV_BOOLEAN IsHDDOverwrite(Sysenv* const thiz){
	return thiz->stAttr.stContent.stHDD.bOverwrite;
}
static void SetHDDOverwrite(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stHDD.bOverwrite = set;
}
static ENV_BOOLEAN IsHDDDeleteOldFiles(Sysenv* const thiz){
	return thiz->stAttr.stContent.stHDD.bDeleteOldFiles;
}
static void SetHDDDeleteOldFiles(Sysenv* const thiz, ENV_BOOLEAN set){
	thiz->stAttr.stContent.stHDD.bDeleteOldFiles = set;
}
static int GetHDDDaysAgo(Sysenv* const thiz){
	return thiz->stAttr.stContent.stHDD.nDaysAgo;
}
static void SetHDDDaysAgo(Sysenv* const thiz, int set){
	thiz->stAttr.stContent.stHDD.nDaysAgo = set;
}

//maintenance
static ENV_BOOLEAN IsMaintenance(Sysenv* const thiz)
{
    return thiz->stAttr.stContent.stMaintenance.enable;
}
static Maintenance_Mode GetMaintenanceMode(Sysenv* const thiz)
{
    return thiz->stAttr.stContent.stMaintenance.mode;
}
static void SetMaintenanceMode(Sysenv* const thiz, Maintenance_Mode mode)
{
    thiz->stAttr.stContent.stMaintenance.mode=mode;
    if(mode==Maintenance_Never)
    {
        thiz->stAttr.stContent.stMaintenance.enable=ENV_TRUE;
    }
    else
    {
        thiz->stAttr.stContent.stMaintenance.enable=ENV_FALSE;
    }
}
static void GetMaintenanceSchedule(Sysenv* const thiz, int* schedule)
{
    *schedule=thiz->stAttr.stContent.stMaintenance.schedule;
}
static void SetMaintenanceSchedule(Sysenv* const thiz, int schedule)
{
    thiz->stAttr.stContent.stMaintenance.schedule=schedule;
}
static void GetMaintenanceDate(Sysenv* const thiz, int *year, int *month, int *day)
{
    *year=thiz->stAttr.stContent.stMaintenance.stTime.year;
    *month=thiz->stAttr.stContent.stMaintenance.stTime.month;
    *day=thiz->stAttr.stContent.stMaintenance.stTime.day;
}
static void SetMaintenanceDate(Sysenv* const thiz, int year, int month, int day)
{
    thiz->stAttr.stContent.stMaintenance.stTime.year=year;
    thiz->stAttr.stContent.stMaintenance.stTime.month=month;
    thiz->stAttr.stContent.stMaintenance.stTime.day=day;
}
static void GetMaintenanceTime(Sysenv* const thiz, int *hour, int *min, int *sec)
{
    *hour=thiz->stAttr.stContent.stMaintenance.stTime.hour;
    *min=thiz->stAttr.stContent.stMaintenance.stTime.min;
    *sec=thiz->stAttr.stContent.stMaintenance.stTime.sec;
}
static void SetMaintenanceTime(Sysenv* const thiz, int hour, int min, int sec)
{
    thiz->stAttr.stContent.stMaintenance.stTime.hour=hour;
    thiz->stAttr.stContent.stMaintenance.stTime.min=min;
    thiz->stAttr.stContent.stMaintenance.stTime.sec=sec;
}

Sysenv SysenvStruct(const char* pathFile, const char* pathFileFactory, void* arg)
{
	PARA_Init(); // para data init

	unsigned short nCRC16 = 0;
	Sysenv self;
	// clear it
	memset(&self, 0, sizeof(self));

	// install operations
	// crc
	self.IsTheSameAs = IsTheSameAs;
	self.Compare = Compare;
	self.GetEnvSize = GetEnvSize;
	self.GetDevName = GetDevName;
	self.GetDevModel = GetDevModel;
	self.GetHWVersion = GetHWVersion;
	self.GetSWVersion = GetSWVersion;
	self.GetRelDateTime = GetRelDateTime;
	self.UpdateCRC16 = UpdateCRC16;
	// store / restore from file
	self.Save = Save;
	self.Load = Load;
	self.SaveFactory = SaveFactory;
	self.LoadFactory = LoadFactory;
	self.RestoreDefaultColor = RestoreDefaultColor;
	self.RestoreDefaultGeneral = RestoreDefaultGeneral;
	self.RestoreDefaultEncode = RestoreDefaultEncode;
//#ifdef _NVR
	self.RestoreDefaultIPCam = RestoreDefaultIPCam;
//#endif
	//hvr
	self.RestoreDefaultHvr = RestoreDefaultHvr;

	self.RestoreDefaultRecode = RestoreDefaultRecode;
	self.RestoreDefaultNetwork = RestoreDefaultNetwork;
	self.RestoreDefaultScreen = RestoreDefaultScreen;
	self.RestoreDefaultMotion = RestoreDefaultMotion;
	self.RestoreDefaultSensor = RestoreDefaultSensor;
	self.RestoreDefaultPtz = RestoreDefaultPtz;
	self.RestoreDefaultTelecontrol = RestoreDefaultTelecontrol;
	self.RestoreDefaultUps = RestoreDefaultUps;
	self.RestoreDefaultHdd = RestoreDefaultHdd;
	self.RestoreDefault = RestoreDefault;
	self.Reset = Reset;
	self.Print = Print;
	// color
	self.GetHue = GetHue;
	self.SetHue = SetHue;
	self.GetContrast = GetContrast;
	self.SetContrast = SetContrast;
	self.GetLuminance = GetLuminance;
	self.SetBrightness = SetBrightness;
	self.GetSaturation = GetSaturation;
	self.SetSaturation = SetSaturation;
	self.GetDefaultColorValue = GetDefaultColorValue;
	// misc
	self.GetDateFmt = GetDateFmt;
	self.SetDateFmt = SetDateFmt;
	self.IsKeyLock = IsKeyLock;
	self.SetKeyLock = SetKeyLock;
	self.IsKeyBuzzer = IsKeyBuzzer;
	self.SetKeyBuzzer = SetKeyBuzzer;
	self.GetLang = GetLang;
	self.SetLang = SetLang;
	self.GetStandard = GetStandard;
	self.SetStandard = SetStandard;
	self.GetKey_Type = GetKey_Type;
	self.SetKey_Type = SetKey_Type;
	self.GetRemote_Type = GetRemote_Type;
	self.SetRemote_Type = SetRemote_Type;
	self.GetDVRId = GetDVRId;
	self.SetDVRId = SetDVRId;
	self.GetDeviceMode = GetDeviceMode;
	self.SetDeviceMode = SetDeviceMode;
	self.GetDeviceModeMax = GetDeviceModeMax;
	self.SetDeviceModeMax = SetDeviceModeMax;
	self.GetTimerPositionCustom = GetTimerPositionCustom;	
	self.SetTimerPositionCustom = SetTimerPositionCustom;
	self.GetTimerXCoordinate = GetTimerXCoordinate;
	self.SetTimerXCoordinate = SetTimerXCoordinate;
	self.GetTimerYCoordinate = GetTimerYCoordinate;
	self.SetTimerYCoordinate = SetTimerYCoordinate;

	self.GetEseeidPositionCustom = GetEseeidPositionCustom;	
	self.SetEseeidPositionCustom = SetEseeidPositionCustom;
	self.GetEseeidXCoordinate = GetEseeidXCoordinate;
	self.SetEseeidXCoordinate = SetEseeidXCoordinate;
	self.GetEseeidYCoordinate = GetEseeidYCoordinate;
	self.SetEseeidYCoordinate = SetEseeidYCoordinate;
	self.GetSpecialFTPServerPath = GetSpecialFTPServerPath;
	self.SetSpecialFTPServerPath = SetSpecialFTPServerPath;
	self.GetSupportWeb = GetSupportWeb;
	self.SetSupportWeb = SetSupportWeb;
	self.GetAndroidLink = GetAndroidLink;
	self.SetAndroidLink = SetAndroidLink;
	self.GetIOSLink = GetIOSLink;
	self.SetIOSLink = SetIOSLink;

	self.GetSysTimeFmt=GetSysTimeFmt;
	self.SetSysTimeFmt=SetSysTimeFmt;
	
//timer&eseeid width
	self.GetEseeidWidth = GetEseeidWidth;
	self.SetEseeidWidth = SetEseeidWidth;
	self.GetTimerWidth = GetTimerWidth;
	self.SetTimerWidth = SetTimerWidth;

	self.GetChannelModeAuto = GetChannelModeAuto;
	self.SetChannelModeAuto = SetChannelModeAuto;
	self.GetChannelModes = GetChannelModes;
	self.SetChannelModes = SetChannelModes;

	self.SetPersianCalendarUse = SetPersianCalendarUse;
	self.IsPersianCalendar = IsPersianCalendar;
	self.SetTimeUse = SetTimeUse;
	self.IsTimeUse = IsTimeUse;

	// encode
	self.GetEncMode = GetEncMode;
	self.SetEncMode = SetEncMode;
	self.GetEncFmt = GetEncFmt;
	self.SetEncFmt = SetEncFmt;
	self.GetEncPicLevel = GetEncPicLevel;
	self.SetEncPicLevel = SetEncPicLevel;
	self.GetBitrateMode = GetBitrateMode;
	self.SetBitrateMode = SetBitrateMode;
	self.GetBitrateValue = GetBitrateValue;
	self.SetBitrateValue = SetBitrateValue;
	self.GetEncFramerate = GetEncFramerate;
	self.SetEncFramerate = SetEncFramerate;

	self.GetEncSubMode = GetEncSubMode;
	self.SetEncSubMode = SetEncSubMode;
	self.GetEncSubFmt = GetEncSubFmt;
	self.SetEncSubFmt = SetEncSubFmt;
	self.GetEncSubPicLevel = GetEncSubPicLevel;
	self.SetEncSubPicLevel = SetEncSubPicLevel;
	self.GetSubBitrateMode = GetSubBitrateMode;
	self.SetSubBitrateMode = SetSubBitrateMode;
	self.GetSubBitrateValue = GetSubBitrateValue;
	self.SetSubBitrateValue = SetSubBitrateValue;
	self.GetEncSubFramerate = GetEncSubFramerate;
	self.SetEncSubFramerate = SetEncSubFramerate;
	
//#ifdef _NVR
	// ipcam
	self.GetIPCamAddr = GetIPCamAddr;
	self.SetIPCamAddr = SetIPCamAddr;
	self.GetIPCamPort = GetIPCamPort;
	self.SetIPCamPort = SetIPCamPort;GetIPCamProtocolname;
	self.GetIPCamProtocolname= GetIPCamProtocolname;
	self.SetIPCamProtocolname= SetIPCamProtocolname;
	self.GetIPCamUsername= GetIPCamUsername;
	self.SetIPCamUsername= SetIPCamUsername;
	self.GetIPCamModelname= GetIPCamModelname;
	self.SetIPCamModelname= SetIPCamModelname;
	self.GetIPCamPassword = GetIPCamPassword;
	self.SetIPCamPassword = SetIPCamPassword;
	self.IsEnableIPCChannel= IsEnableIPCChannel;
	self.SetEnableIPCChannel= SetEnableIPCChannel;
	self.GetIPCMACAddr = GetIPCMACAddr;
	self.SetIPCMACAddr = SetIPCMACAddr;
	self.GetIPCamPrivData = GetIPCamPrivData;
	self.SetIPCamPrivData = SetIPCamPrivData;
	self.GetIPCamStreamNR = GetIPCamStreamNR;
	self.SetIPCamStreamNR = SetIPCamStreamNR;
	self.GetIPCRtspStream = GetIPCamRtspStream;
	self.SetIPCRtspStream = SetIPCamRtspStream;
	self.GetChannelWorkMode = GetChannelWorkMode;
	self.SetChannelWorkMode = SetChannelWorkMode;
	self.IsMultiNetworkSegment = IsMultiNetworkSegment;
	self.SetIsMultiNetworkSegment = SetIsMultiNetworkSegment;
	self.GetIPCamStreamChoise = GetIPCamStreamChoise;
	self.SetIPCamStreamChoise = SetIPCamStreamChoise;
	self.UpdateIPcam = UpdateIPcam;
	self.SetIPCamStreamIndex = SetIPcamStreamIndex;
	self.GetIPCamStreamIndex = GetIPcamStreamIndex;
//#endif

	//hvr
	self.SetHvrBncChn = SetHvrBncChn;
	self.GetHvrBncChn = GetHvrBncChn;
	
	// record
	self.GetSchedule = GetSchedule;
	self.SetSchedule = SetSchedule;
	// network
	self.IsDHCPUse = IsDHCPUse;
	self.SetDHCPUse = SetDHCPUse;
	self.GetMACAddr = GetMACAddr;
	self.SetMACAddr = SetMACAddr;
	self.GetIPAddr = GetIPAddr;
	self.SetIPAddr = SetIPAddr;
	self.GetSubMask = GetSubMask;
	self.SetSubMask = SetSubMask;
	self.GetGateway = GetGateway;
	self.SetGateway = SetGateway;
	self.GetDNSAddr = GetDNSAddr;
	self.SetDNSAddr = SetDNSAddr;
	self.GetHTTPPort = GetHTTPPort;
	self.SetHTTPPort = SetHTTPPort;
	self.GetClientPort = GetClientPort;
	self.SetClientPort = SetClientPort;
	self.GetMobilePort = GetMobilePort;
	self.SetMobilePort = SetMobilePort;
	self.IsESeeNetUse = IsESeeNetUse;
	self.SetESeeNetUse = SetESeeNetUse;
	self.IsDDNSUse = IsDDNSUse;
	self.SetDDNSUse = SetDDNSUse;
	self.GetDDNSProvider = GetDDNSProvider;
	self.SetDDNSProvider = SetDDNSProvider;
	self.GetDDNSUrl = GetDDNSUrl;
	self.SetDDNSUrl = SetDDNSUrl;
	self.GetDDNSUser = GetDDNSUser;
	self.SetDDNSUser = SetDDNSUser;
	self.GetDDNSPwd = GetDDNSPwd;
	self.SetDDNSPwd = SetDDNSPwd;
	self.IsPPPoE = IsPPPoE;
	self.SetPPPoE = SetPPPoE;
	self.GetPPPoEUser = GetPPPoEUser;
	self.SetPPPoEUser = SetPPPoEUser;
	self.GetPPPoEPwd = GetPPPoEPwd;
	self.SetPPPoEPwd = SetPPPoEPwd;
	self.GetPPPOEProtocol= GetPPPOEProtocol;
	self.SetPPPOEProtocol= SetPPPOEProtocol;
	//guide
	self.IsGuideUse = IsGuideUse;	
	self.SetIsGuideUse = SetIsGuideUse;
	self.IsEseeidUse = IsEseeidUse;	
	self.SetIsEseeidUse = SetIsEseeidUse;

	self.Is3GUse = Is3GUse;
	self.Set3GUse = Set3GUse;
	self.Get3GAPN = Get3GAPN;
	self.Set3GAPN = Set3GAPN;
	self.Get3GPIN = Get3GPIN;
	self.Set3GPIN = Set3GPIN;
	self.Get3GUser = Get3GUser;
	self.Set3GUser = Set3GUser;
	self.Get3GPwd = Get3GPwd;
	self.Set3GPwd = Set3GPwd;
	self.Get3GDIALNum = Get3GDIALNum;
	self.Set3GDIALNum = Set3GDIALNum;
	// E-Mail/SMTP
	self.IsSMTPUse = IsSMTPUse;
	self.SetSMTPUse = SetSMTPUse;
	self.GetSMTPServer = GetSMTPServer;
	self.SetSMTPServer = SetSMTPServer;
	self.GetSMTPPort = GetSMTPPort;
	self.SetSMTPPort = SetSMTPPort;	
	self.IsSMTPAnonymous = IsSMTPAnonymous;
	self.SetSMTPAnonymous = SetSMTPAnonymous;	
	self.GetSMTPUser = GetSMTPUser;
	self.SetSMTPUser = SetSMTPUser;	
	self.GetSMTPPwd = GetSMTPPwd;
	self.SetSMTPPwd = SetSMTPPwd;		
	self.GetSMTPEncryptType = GetSMTPEncryptType;
	self.SetSMTPEncryptType = SetSMTPEncryptType;
	self.GetSMTPSender = GetSMTPSender;
	self.SetSMTPSender = SetSMTPSender;			
	self.GetSMTPSendee1 = GetSMTPSendee1;
	self.SetSMTPSendee1 = SetSMTPSendee1;				
	self.GetSMTPSendee2 = GetSMTPSendee2;
	self.SetSMTPSendee2 = SetSMTPSendee2;				
	self.GetSMTPSubject = GetSMTPSubject;
	self.SetSMTPSubject = SetSMTPSubject;			
	self.GetSMTPInterval = GetSMTPInterval;
	self.SetSMTPInterval = SetSMTPInterval;		
	self.IsSMTPHealthEnable = IsSMTPHealthEnable;
	self.SetSMTPHealthEnable = SetSMTPHealthEnable;	
	self.GetSMTPHealthInterval = GetSMTPHealthInterval;
	self.SetSMTPHealthInterval = SetSMTPHealthInterval;
	// Network-->FTP
	self.IsFTPUse = IsFTPUse;
	self.SetFTPUse = SetFTPUse;
	self.GetFTPServer = GetFTPServer;
	self.SetFTPServer = SetFTPServer;
	self.GetFTPPort = GetFTPPort;
	self.SetFTPPort = SetFTPPort;	
	self.IsFTPAnonymous = IsFTPAnonymous;
	self.SetFTPAnonymous = SetFTPAnonymous;	
	self.GetFTPUser = GetFTPUser;
	self.SetFTPUser = SetFTPUser;	
	self.GetFTPPwd = GetFTPPwd;
	self.SetFTPPwd = SetFTPPwd;		
	self.GetFTPRemoteFolder = GetFTPRemoteFolder;
	self.SetFTPRemoteFolder = SetFTPRemoteFolder;			
	self.GetFTPFileMaxSize = GetFTPFileMaxSize;
	self.SetFTPFileMaxSize = SetFTPFileMaxSize;	
	// Network-->FTP-->FTPSchedule
	self.GetFTPSchedule = GetFTPSchedule;
	self.SetFTPSchedule = SetFTPSchedule;
	//online upgrade
	self.IsUsePreHost = IsUsePreHost;
	self.SetUsePreHost = SetUsePreHost;
	self.GetUpgradePreHost = GetUpgradePreHost;
	self.SetUpgradePreHost = SetUpgradePreHost;
	self.GetUpgradePreUrl = GetUpgradePreUrl;
	self.SetUpgradePreUrl = SetUpgradePreUrl;
	self.GetUpgradePrePort = GetUpgradePrePort;
	self.SetUpgradePrePort = SetUpgradePrePort;
	// screen
	self.GetTitle = GetTitle;
	self.GetTitleEx = GetTitleEx;
	self.SetTitle = SetTitle;
	self.GetAlpha = GetAlpha;
	self.SetAlpha = SetAlpha;
	self.IsAutoSwi = IsAutoSwi;
	self.IsSkipLoss = IsSkipLoss;
	self.SetAutoSwi = SetAutoSwi;
	self.SetSkipLoss = SetSkipLoss;
	self.GetAutoSwiInterval = GetAutoSwiInterval;
	self.SetAutoSwiInterval = SetAutoSwiInterval;
	self.GetAutoSwiMode = GetAutoSwiMode;
	self.SetAutoSwiMode = SetAutoSwiMode;
	self.GetVGAResolution = GetVGAResolution;
	self.SetVGAResolution = SetVGAResolution;
	self.SetVGAResolution = SetVGAResolution;
	self.GetTvSize = GetTvSize;
	self.SetTvSize = SetTvSize;
	self.SetCVBSEnable = SetCVBSEnable;
	self.IsCVBSEnable = IsCVBSEnable;

	// detection
	self.GetMDSensitivity = GetMDSensitivity;
	self.SetMDSensitivity = SetMDSensitivity;
	self.GetMDAlarmDuration = GetMDAlarmDuration;
	self.SetMDAlarmDuration = SetMDAlarmDuration;
	self.GetMDArea = GetMDArea;
	self.SetMDArea = SetMDArea;
	self.GetVCArea = GetVCArea;
	self.SetVCArea = SetVCArea;
	self.IsMDAlarm = IsMDAlarm;
	self.SetMDAlarm = SetMDAlarm;
	self.IsMDBuzzer = IsMDBuzzer;
	self.SetMDBuzzer = SetMDBuzzer;
	self.IsMDSMTP = IsMDSMTP;
	self.SetMDSMTP = SetMDSMTP;
	self.IsMDFTP = IsMDFTP;
	self.SetMDFTP = SetMDFTP;
	self.GetVLAlarmDuration = GetVLAlarmDuration;
	self.SetVLAlarmDuration = SetVLAlarmDuration;
	self.IsVLAlarm = IsVLAlarm;
	self.SetVLAlarm = SetVLAlarm;
	self.IsVLBuzzer = IsVLBuzzer;
	self.SetVLBuzzer = SetVLBuzzer;
	self.IsVLSMTP = IsVLSMTP;
	self.SetVLSMTP = SetVLSMTP;
	self.IsVLFTP= IsVLFTP;
	self.SetVLFTP = SetVLFTP;
	// ptz
	self.GetPTZId = GetPTZId;
	self.SetPTZId = SetPTZId;
	self.GetPTZProtocal = GetPTZProtocal;
	self.SetPTZProtocal = SetPTZProtocal;
	self.GetPTZBaudrate = GetPTZBaudrate;
	self.SetPTZBaudrate = SetPTZBaudrate;
//	self.GetPTZDatabit = GetPTZDatabit;
//	self.SetPTZDatabit = SetPTZDatabit;
//	self.GetPTZStopbit = GetPTZStopbit;
//	self.SetPTZStopbit = SetPTZStopbit;
//	self.GetPTZParity = GetPTZParity;
//	self.SetPTZParity = SetPTZParity;
	self.GetPTZTour = GetPTZTour;
	self.SetPTZTour = SetPTZTour;
	self.PrintPTZTour = PrintPTZTour;
	// sensor
	self.GetSensorWorkMode = GetSensorWorkMode;
	self.SetSensorWorkMode = SetSensorWorkMode;
	self.GetSensorAlarmDuration = GetSensorAlarmDuration;
	self.SetSensorAlarmDuration = SetSensorAlarmDuration;
	self.IsSensorCamRec = IsSensorCamRec;
	self.SetSensorCamRec = SetSensorCamRec;
	self.IsSensorAlarm = IsSensorAlarm;
	self.SetSensorAlarm = SetSensorAlarm;
	self.IsSensorBuzzer = IsSensorBuzzer;
	self.SetSensorBuzzer = SetSensorBuzzer;
	self.IsSensorSMTP = IsSensorSMTP;
	self.SetSensorSMTP = SetSensorSMTP;
	self.IsSensorFTP = IsSensorFTP;
	self.SetSensorFTP = SetSensorFTP;
	// telecontrol
	self.GetTelecontrolId = GetTelecontrolId;
	self.SetTelecontrolId = SetTelecontrolId;
	self.IsTelecontrolEnable = IsTelecontrolEnable;
	self.SetTelecontrolEnable = SetTelecontrolEnable;
	self.GetTelecontrolAlarmDuration = GetTelecontrolAlarmDuration;
	self.SetTelecontrolAlarmDuration = SetTelecontrolAlarmDuration;
	self.IsTelecontrolRecord = IsTelecontrolRecord;
	self.SetTelecontrolRecord = SetTelecontrolRecord;
	self.IsTelecontrolAlarm = IsTelecontrolAlarm;
	self.SetTelecontrolAlarm = SetTelecontrolAlarm;
	self.IsTelecontrolBuzzer = IsTelecontrolBuzzer;
	self.SetTelecontrolBuzzer = SetTelecontrolBuzzer;
	self.IsTelecontrolNetworkNotice = IsTelecontrolNetworkNotice;
	self.SetTelecontrolNetworkNotice = SetTelecontrolNetworkNotice;

	// ups
	self.IsUpsEnable = IsUpsEnable;
	self.SetUpsEnable = SetUpsEnable;
	self.GetUpsAlarmDuration = GetUpsAlarmDuration;
	self.SetUpsAlarmDuration = SetUpsAlarmDuration;
	self.IsUpsAlarm = IsUpsAlarm;
	self.SetUpsAlarm = SetUpsAlarm;
	self.IsUpsBuzzer = IsUpsBuzzer;
	self.SetUpsBuzzer = SetUpsBuzzer;
	self.IsUpsNetworkNotice = IsUpsNetworkNotice;
	self.SetUpsNetworkNotice = SetUpsNetworkNotice;

	// hdd
	self.IsHDDOverwrite = IsHDDOverwrite;
	self.SetHDDOverwrite = SetHDDOverwrite;
	self.IsHDDDeleteOldFiles = IsHDDDeleteOldFiles;
	self.SetHDDDeleteOldFiles = SetHDDDeleteOldFiles;
	self.GetHDDDaysAgo = GetHDDDaysAgo;
	self.SetHDDDaysAgo = SetHDDDaysAgo;

	//maintenance
	self.IsMaintenance=IsMaintenance;
	self.GetMaintenanceMode=GetMaintenanceMode;
	self.SetMaintenanceMode=SetMaintenanceMode;
	self.GetMaintenanceSchedule=GetMaintenanceSchedule;
	self.SetMaintenanceSchedule=SetMaintenanceSchedule;
	self.GetMaintenanceDate=GetMaintenanceDate;
	self.SetMaintenanceDate=SetMaintenanceDate;
	self.GetMaintenanceTime=GetMaintenanceTime;
	self.SetMaintenanceTime=SetMaintenanceTime;

	Load(&self);
	// check crc is ok
	nCRC16 = self.stAttr.u16CRC;
	self.UpdateCRC16(&self);
	ENV_TRACE("check 2nd crc = %x", self.stAttr.u16CRC);
	if(nCRC16 != self.stAttr.u16CRC){
		ENV_TRACE("env config crc error %04x/%04x", self.stAttr.u16CRC, nCRC16);
		___to_be_restore = 1;
		Reset(&self);
		___to_be_restore = 0;
	}
//	ENV_TRACE("enviroment size %dKB(%dB) crc %04x operation %d",
//		1 + (sizeof(self.stAttr)>>10), sizeof(self.stAttr), self.stAttr.u16CRC, sizeof(self));
	return self;
}

Sysenv* PSysenvStruct(const char* pathFile, const char* pathFileFactory, void* arg)
{
	Sysenv* thiz = NULL;
	Sysenv self;
	if(!(thiz = (Sysenv*)calloc(sizeof(Sysenv), 1))){
		return NULL;
	}
	self = SysenvStruct(pathFile, pathFileFactory, arg);
	memcpy(thiz, &self, sizeof(self));
	PARA_Destroy();
	return thiz;
}

int PSysenvDestruct(Sysenv* thiz, void* arg)
{
	if(!thiz){
		return -1;
	}
	free(thiz);
	PARA_Destroy();
	return 0;
}

#endif	//__ENVIRONMENT_C__

