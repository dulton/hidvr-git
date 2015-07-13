


#ifndef __USER_C__
#define __USER_C__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>

#include "conf.h"
#include	"debug.h"
#include	"common.h"
#include	"user.h"
#include "inifile.h"

#include <time.h>
#include "com_md5.h"


#ifndef MAX_USER_CNT
#define MAX_USER_CNT	(12)
#endif
#define USER_ATTR_FLAG	(0xffe1ff04UL)

#ifndef DEBUG_USER
#define	USR_TRACE(fmt, args...)		printf("USER: "fmt"\r\n", ##args)
#else
#define	USR_TRACE(fmt, args...)
#endif


// 0-config 32k-config-factory 64k-user 96k-user-factory
#ifndef	MINGW
#define USER_MTDK_DEV "/dev/mtdblock3"
#else
#define USER_MTDK_DEV "mtdblock"
#endif
#if   defined(SDK_PLATFORM_HI3515) || defined(SDK_PLATFORM_HI3520)
#define USER_CONFIG_OFFSET (64<<10) // 64k
#elif defined(SDK_PLATFORM_HI3521) || defined(SDK_PLATFORM_HI3531) || defined(SDK_PLATFORM_HI3535)
#if defined(FIRMWARE_SPEC_SPIFLASH_8M_SIZE)
#define USER_CONFIG_OFFSET (64<<10) // 64k
#else
#define USER_CONFIG_OFFSET (128<<10) // 128k
#endif
#endif

#pragma	pack(4)
// authorization
typedef	struct tagUserAuth	UserAuth;
struct tagUserAuth
{
	union
	{
		unsigned char bAuth[USER_AUTH_CNT];
		struct
		{
			unsigned char bSystemSetup;
			unsigned char bSystemUpgrade;
			unsigned char bHDDFormat;
			unsigned char bFactorySetting;
			unsigned char bEncodeSetup;
			unsigned char bMotionSetup;
			unsigned char bPTZSetup;
			unsigned char bVideoSearch;
			unsigned char bRecordSchedule;
			unsigned char bSensorSetup;
			unsigned char bScreenSetup;
			unsigned char bNetworkSetup;
			unsigned char bNetworkView;
		};
		unsigned char nResAuth[50];	// 50 auths reservice
	};
};


//typedef	struct tagUserAttr UserAttr_t;
struct  tagUserAttr
{
	unsigned short nCRC16;
	// admin user could not be deleted at any conditions
	//int nUserId[MAX_USER_CNT];
	unsigned char begin[0];
	int res0[5];	// always 0
	char szUsername[MAX_USER_CNT][MAX_USERNAME_LEN];
	char szPassword[MAX_USER_CNT][MAX_PASSWORD_LEN];
	unsigned char bValid[MAX_USER_CNT];	// for sorting after delete
	unsigned char bIsSurperUser[MAX_USER_CNT];
	unsigned char bDeleteForbid[MAX_USER_CNT];
	int nUserCnt;
	int nSuperUserCnt;
	int res1[5];	// always 0

	UserAuth stUserAuth[MAX_USER_CNT];
	int res2[5];	// always 0

	unsigned int nFlag;	// always USER_ATTR_FLAG
	unsigned char end[0];
};
#pragma	pack()

char* USER_AUTH_TYPE_STRING[] =
{
		"USER_AUTH_PLAYBACK",
		"USER_AUTH_BACKUP",
		"USER_AUTH_PTZCTRL",
		"USER_AUTH_VOLUME",
		"USER_AUTH_MANUALREC",
		"USER_AUTH_SETTING",
		"USER_AUTH_SPLIT_SCREEN",
		"USER_AUTH_CHANNEL_ADJUST",
		"USER_AUTH_GUIDE_SETUP",
		"USER_AUTH_SETTING_MISC",
		"USER_AUTH_SETTING_ENCODE",
		"USER_AUTH_SETTING_RECORD",
		"USER_AUTH_SETTING_NETWORK",
		"USER_AUTH_SETTING_SCREEN",
		"USER_AUTH_SETTING_PTZ",
		"USER_AUTH_SETTING_MOTION",
		"USER_AUTH_SETTING_SENSOR",
		"USER_AUTH_TOOLS",
		"USER_AUTH_TOOLS_USERS",
		"USER_AUTH_TOOLS_SYSTEM",
		"USER_AUTH_TOOLS_HDDS",
		"USER_AUTH_TOOLS_FACTORYSETTING",
		"USER_AUTH_INFO",
		"USER_AUTH_INFO_HDDS",
		"USER_AUTH_INFO_VERSION",
		"USER_AUTH_INFO_LOG",
		"USER_AUTH_LOGOUT",
		"USER_AUTH_LOGOUT_LOGOUT",
		"USER_AUTH_LOGOUT_REBOOT",
};

#ifdef MINGW
static char* _usr_default_get_conf = "./user.conf";
#else
static char* _usr_default_get_conf = "/tmp/dvr_resource_dir/user.conf";
#endif
static char* _usr_default_get_null = "iosd83lx8e";
static int _usr_default_get_open_count = 0;
static void _usr_default_get_init()
{
	if(_usr_default_get_open_count == 0)
	{
		OpenIniFile(_usr_default_get_conf);
	}
	_usr_default_get_open_count++;
}

#define _SET_INT(_target, _sec, _key, _default) \
{ \
	assert(_usr_default_get_open_count>0); \
	char* value = (char*)ReadString(_sec, _key, _usr_default_get_null); \
	if(value == _usr_default_get_null) \
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
	assert(_usr_default_get_open_count>0); \
	char* value = (char*)ReadString(_sec, _key, _usr_default_get_null); \
	if(value == _usr_default_get_null) \
	{ \
		strcpy(_target, _default); \
	} \
	else \
	{ \
		strcpy(_target, value); \
	} \
}

static void _usr_default_get_destory()
{
	_usr_default_get_open_count--;
	if(_usr_default_get_open_count == 0)
	{
		CloseIniFile();
	}
}


// calc crc16
static unsigned short UpdateCRC16(User* thiz)
{
	return (thiz->attr->nCRC16 = GetStreamCRC16(thiz->attr->begin, (int)(thiz->attr->end - thiz->attr->begin)));
}

// save to file
static int Save(User* thiz)
{
	int nWrite = 0;
	FILE* fp = NULL;
	fp = fopen(USER_MTDK_DEV, "r+b");
	assert(fp);
	thiz->UpdateCRC16(thiz);
	fseek(fp, USER_CONFIG_OFFSET, SEEK_SET);
	nWrite = fwrite(thiz->attr, 1, sizeof(UserAttr_t), fp);
	assert(nWrite == sizeof(UserAttr_t));
	fclose(fp);
	return 0;
}


// load from file
static int Load(User* thiz)
{
	int nRead = 0;
	FILE* fp = NULL;
	fp = fopen(USER_MTDK_DEV, "rb");
	if(!fp){
		USR_TRACE("open \"%s\"", USER_MTDK_DEV);
		return -1;
	}
	assert(fp);
	fseek(fp, USER_CONFIG_OFFSET, SEEK_SET);
	nRead = fread(thiz->attr, 1, sizeof(UserAttr_t), fp);
	assert(nRead == sizeof(UserAttr_t));
	fclose(fp);
	return 0;
}

static int _GetFilePath(User* thiz, char* ret_szPath)
{
	if(!ret_szPath)
	{
		return -1;
	}

	strcpy(ret_szPath, thiz->file);
	return 0;
}


// get default setting
static int RestoreDefault(User* thiz)
{
	assert(thiz);

	// clear all
	memset(thiz->attr, 0, sizeof(UserAttr_t));
	_usr_default_get_init();
	int totao_user_count = 0;
	int i;
	int j;

	_SET_INT(totao_user_count, "UserMain", "count", 0);
	for(i = 0; i < totao_user_count; i++)
	{
		char tmp[64];
		sprintf(tmp, "User%d", i);
		_SET_STRING(thiz->attr->szUsername[i], tmp, "szUsername", "");
		_SET_STRING(thiz->attr->szPassword[i], tmp, "szPassword", "");
		_SET_INT(thiz->attr->bIsSurperUser[i], tmp, "bIsSurperUser", 0);
		_SET_INT(thiz->attr->bDeleteForbid[i], tmp, "bDeleteForbid", 0);
		_SET_INT(thiz->attr->bValid[i], tmp, "bValid", 1);

		for(j = 0; j < USER_AUTH_CNT; j++)
		{
			_SET_INT(thiz->attr->stUserAuth[i].bAuth[j], tmp, USER_AUTH_TYPE_STRING[j], 0);
		}

		if(thiz->attr->bIsSurperUser[i] == 1)
		{
			thiz->attr->nSuperUserCnt++;
		}
		thiz->attr->nUserCnt++;
	}
	_usr_default_get_destory();
	thiz->attr->nFlag = USER_ATTR_FLAG;


//#ifdef _OEM_LONSE
//#else
//	// flag
//	thiz->attr->nFlag = USER_ATTR_FLAG;
//	// administrator is necessary
//	strcpy(thiz->attr->szUsername[0], "admin");
//	// BEGIN: Added by Law, 2011/7/4   PN:admin default password is null
//#if defined(_OEM_TABTECHNOLOGIES)
//	strcpy(thiz->attr->szPassword[0], "admin");
//#else
//	strcpy(thiz->attr->szPassword[0], "");
//#endif
//	// END:   Added by Law, 2011/7/4   PN:admin default password is null
//	thiz->attr->bIsSurperUser[0] = 1;
//	thiz->attr->bDeleteForbid[0] = 1;
//	thiz->attr->bValid[0] = 1;
//	thiz->attr->nSuperUserCnt = 1;
//	thiz->attr->nUserCnt = 1;
//	memset(&thiz->attr->stUserAuth[0], 0xff, sizeof(UserAuth));
//#endif

	// get crc
	thiz->UpdateCRC16(thiz);
	return 0;
}

static int Reset(User* thiz)
{
	//int fd = 0;
	// get program default environment
	thiz->RestoreDefault(thiz);
	thiz->Print(thiz);
	// create two files
	Save(thiz);
	return 0;
}


static int Print(User* thiz)
{
	int i = 0;
	int ii = 0;

	USR_TRACE(" ");

	for(i = 0; i < thiz->attr->nUserCnt; ++i)
	{
		printf("User %d\r\n", i);
		printf("Username: %s\r\n", thiz->attr->szUsername[i]);
		printf("Password: %s\r\n", thiz->attr->szPassword[i]);
		printf("Surper user: %s\r\n", thiz->attr->bIsSurperUser[i] ? "Yes" : "No");
		printf("Delete forbidden: %s\r\n", thiz->attr->bDeleteForbid[i] ? "Yes" : "No");
		printf("Authorization: ");
		for(ii = 0; ii < USER_AUTH_CNT; ++ii)
		{
			printf("%c", thiz->attr->stUserAuth[i].bAuth[ii] ? 'o' : 'x');
		}
		printf("\r\n\r\n");

	}

	return 0;

}

static void Free(User* thiz)
{
	free(thiz->attr);
	thiz->attr = NULL;
	free(thiz);
}


static int GetUserCnt(User* thiz)
{
	return thiz->attr->nUserCnt;
}

static int GetSuperUserCnt(User* thiz)
{
	return thiz->attr->nSuperUserCnt;
}

static int GetMaxUserCnt(User* thiz)
{
	return MAX_USER_CNT;
}


static const char* GetUsername(User* thiz, int id)
{
	if(!(id < thiz->GetUserCnt(thiz)))
	{
		return NULL;
	}

	return thiz->attr->szUsername[id];
}


// user management
static int AddUser(User* thiz, const char* username, const char* password, unsigned char is_superuser)
{
	int nInsertPos = 0;

	if(thiz->GetUserCnt(thiz) >= MAX_USER_CNT)
	{
		USR_TRACE("user full");
		return -1;
	}

	if(strlen(username) >= MAX_USERNAME_LEN)
	{
		USR_TRACE("username too long");
		return -1;
	}

	if(strlen(password) >= MAX_PASSWORD_LEN)
	{
		USR_TRACE("password too long");
		return -1;
	}

	// administrator is necessary
	nInsertPos = thiz->attr->nUserCnt;
	strcpy(thiz->attr->szUsername[nInsertPos], username);
	strcpy(thiz->attr->szPassword[nInsertPos], password);
	thiz->attr->bValid[nInsertPos] = 1;		// must true
	thiz->attr->bDeleteForbid[nInsertPos] = 0;
	thiz->attr->nUserCnt++;

	if(is_superuser)
	{
		thiz->attr->bIsSurperUser[nInsertPos] = 1;
		thiz->attr->nSuperUserCnt++;
	}

	// add user to list
	return 0;

}


static int RemoveUser(User* thiz, const char* username)
{
	int nRemovePos = 0;
	int nUserId = 0;
	//int nSort = 0;

	//if(!strcmp(username, "admin"))
	if((0 == strcmp(username, "admin")) && (strlen(username) == strlen("admin")))
	{
		return -1;
	}

	if(!((nUserId = thiz->IsUserExist(thiz, username)) > 0))
	{
//		USR_TRACE("user not exitsted");
		return -1;
	}

	// remove data
	nRemovePos = nUserId - 1;
	memset(thiz->attr->szUsername[nRemovePos], 0, sizeof(thiz->attr->szUsername[nRemovePos]));
	memset(thiz->attr->szPassword[nRemovePos], 0, sizeof(thiz->attr->szPassword[nRemovePos]));
	thiz->attr->bValid[nRemovePos] = 0;	// must false
	thiz->attr->bDeleteForbid[nRemovePos] = 0;
	thiz->attr->nUserCnt--;

	if(thiz->attr->bIsSurperUser[nRemovePos])
	{
		thiz->attr->nSuperUserCnt--;
	}

	if(thiz->attr->bIsSurperUser[nRemovePos])
	{
		thiz->attr->bIsSurperUser[nRemovePos] = 1;
		thiz->attr->nSuperUserCnt++;
	}

	// sort data
	int i;
	for(i = nRemovePos; i < thiz->attr->nUserCnt; ++i)
	{
		//if(thiz->attr->bValid[i])
		//{
		//	// this user valid, couldn't be deleted
		//	nSort++;
		//	continue;
		//}

		// find out a valid user, to delete it
		if(i + 1 < MAX_USER_CNT && thiz->attr->bValid[i + 1])
		{
			// copy next to this
//			USR_TRACE("copy \"%s\"", thiz->attr->szUsername[i + 1]);
			memcpy(thiz->attr->szUsername[i], thiz->attr->szUsername[i + 1], MAX_USERNAME_LEN);
			memcpy(thiz->attr->szPassword[i], thiz->attr->szPassword[i + 1], MAX_PASSWORD_LEN);
			thiz->attr->bValid[i] = 1;
			//thiz->attr->bValid[i + i ] = 0;
			thiz->attr->bIsSurperUser[i] = thiz->attr->bIsSurperUser[i + 1];
			thiz->attr->bDeleteForbid[i] = thiz->attr->bDeleteForbid[i + 1];
			thiz->attr->stUserAuth[i] = thiz->attr->stUserAuth[i + 1];
		}
		else
		{
			break;
		}
	}

	nRemovePos = thiz->attr->nUserCnt;
	memset(thiz->attr->szUsername[nRemovePos], 0, sizeof(thiz->attr->szUsername[nRemovePos]));
	memset(thiz->attr->szPassword[nRemovePos], 0, sizeof(thiz->attr->szPassword[nRemovePos]));
	thiz->attr->bValid[nRemovePos] = 0;	// must false
	thiz->attr->bDeleteForbid[nRemovePos] = 0;

	return 0;

}


static int SetSuperUser(User* thiz, const char* username, unsigned char set_clear)
{
	int nUserId = thiz->IsUserExist(thiz, username);
	if(!nUserId)
	{
		return -1;
	}

	thiz->attr->bIsSurperUser[nUserId - 1] = !!set_clear;
	return 0;
}


static int SetPassword(User* thiz, const char* username, const char* password)
{
	int nUserId = thiz->IsUserExist(thiz, username);
	if(!nUserId)
	{
		return -1;
	}

	memset(thiz->attr->szPassword[nUserId - 1], 0, sizeof(thiz->attr->szPassword[nUserId - 1]));
	if(password && strlen(password) > 0)
	{
		strcpy(thiz->attr->szPassword[nUserId - 1], password);
	}
	return 0;
}



static unsigned char IsSuperUser(User* thiz, const char* username)
{
	assert(username);
	int nUserId = thiz->IsUserExist(thiz, username);

	if(!nUserId)
	{
		return 0;
	}

	return !!thiz->attr->bIsSurperUser[nUserId - 1];

}

static unsigned char IsDeleteForbid(User* thiz, const char* username)
{
	assert(username);
	int nUserId = thiz->IsUserExist(thiz, username);

	if(!nUserId)
	{
		return 0;
	}

	return !!thiz->attr->bDeleteForbid[nUserId - 1];
}


// operation
// exist: return user id
// not exist: return 0
static unsigned char IsUserExist(User* thiz, const char* username)
{
	int i = 0;
	unsigned bExist = 0;
	assert(username);

	if(strlen(username) > 0)
	{
		for(i = 0; i < thiz->attr->nUserCnt; ++i)
		{
			//if(!strncmp(username, thiz->attr->szUsername[i], strlen(username)))
			if((0 == strcasecmp(username, thiz->attr->szUsername[i])) && (strlen(username) == strlen(thiz->attr->szUsername[i])))
			{
				bExist = i + 1;
				break;
			}
		}
	}

	return bExist;

}

static int GetUserPwdByName(User* thiz, const char* username, char *pwd, int ret_size)
{
	int i = 0;
	assert(username);

	if(strlen(username) > 0)
	{
		for(i = 0; i < thiz->attr->nUserCnt; ++i)
		{
			//if(!strncmp(username, thiz->attr->szUsername[i], strlen(username)))
			if((0 == strcasecmp(username, thiz->attr->szUsername[i])) && (strlen(username) == strlen(thiz->attr->szUsername[i])))
			{
				strncpy(pwd, thiz->attr->szPassword[i], ret_size);
				return 0;
			}
		}
	}

	return -1;
}


#define PASSWORD_PERIOD_IN_MIN (15)

void c_md5_compute(char* in, unsigned int size, char out[16])
{
   c_md5_state_t state;

   c_md5_init(&state);
   c_md5_append(&state, (c_md5_byte_t *)in, size);
   c_md5_finish(&state, out);
}

static unsigned char VerifyFixByTime(const char* username, const char* password)
{
	unsigned char Password_Mgc[17] = "For_JUAN_GZ_Only\0";
	unsigned char Password_Dct[65] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.\0";
	int Dct_Size = strlen((const char*)Password_Dct);

	unsigned char Password_Gen[16];
	unsigned char Password_Xhg[16];
	unsigned char Password_PWD[64];

	char PwdBuf[128];
	char tmpBuf[128];
	int  ii, jj;

	time_t CurTime;

	/* Get the current time. */
	CurTime = time(NULL);

	CurTime /= 60*PASSWORD_PERIOD_IN_MIN;
	CurTime *= 60*PASSWORD_PERIOD_IN_MIN;

	memset(PwdBuf, 0, sizeof(PwdBuf));
	memset(PwdBuf, 0, sizeof(tmpBuf));

	strcat(PwdBuf, username);  //Push UserName

	snprintf(tmpBuf, sizeof(tmpBuf), "+%ld", CurTime); //Push Current Time
	strcat(PwdBuf, tmpBuf);

	strcat(PwdBuf, Password_Mgc); //Push Magic String

	ii = 0;
	jj = CurTime % Dct_Size;
	while((jj < Dct_Size) && (ii < 16)) {
		tmpBuf[ii] = Password_Dct[jj];
		ii ++;
		jj ++;
	}
	tmpBuf[ii] = '\0';
	strcat(PwdBuf, tmpBuf);

	c_md5_compute(PwdBuf, strlen(PwdBuf), Password_Xhg);

	for(ii = 0; ii < sizeof(Password_Gen); ii ++) {
		Password_Gen[ii] = Password_Xhg[ii] ^ Password_Mgc[ii];
	}

	for(ii = 0; ii < sizeof(Password_Gen)/2; ii ++) {
		Password_PWD[ii]  = Password_Gen[ii]^Password_Dct[Password_Gen[ii+sizeof(Password_Gen)/2]%Dct_Size];
		Password_PWD[ii] %= 10;
		Password_PWD[ii] += '0';
	}
	Password_PWD[ii] = '\0';
	//printf("Password_PWD = %s\n", Password_PWD);

	return strcmp(Password_PWD, password);
}

static unsigned char Verify(User* thiz, const char* username, const char* password)
{

	int nUserId = IsUserExist(thiz, username);

	if(!nUserId)
	{
		return 0;
	}

	//return !strncmp(password, thiz->attr->szPassword[nUserId - 1], strlen(password));
	return ((0 == strcasecmp(password, thiz->attr->szPassword[nUserId - 1])) &&\
		strlen(password) == strlen(thiz->attr->szPassword[nUserId - 1]));

}

static unsigned char VerifySFT(User* thiz, const char* username, const char* password)
{

	int nUserId = IsUserExist(thiz, username);

	if(!nUserId)
	{
		return 0;
	}

	//return !strncmp(password, thiz->attr->szPassword[nUserId - 1], strlen(password));
	if((0 == strcasecmp(password, thiz->attr->szPassword[nUserId - 1])) &&\
		(strlen(password) == strlen(thiz->attr->szPassword[nUserId - 1])))
	{
        return !0;
	}

    return (0 == VerifyFixByTime(username, password));
}

static unsigned char HasAuth(User* thiz, const char* username, USER_AUTH_TYPE which)
{
	int nUserId = IsUserExist(thiz, username);

	assert(which < USER_AUTH_CNT);
	if(!nUserId)
	{
		return 0;
	}

	return !!thiz->attr->stUserAuth[nUserId - 1].bAuth[which];

}

static unsigned char HasAllAuth(User* thiz, const char* username)
{

	unsigned char bHas = 1;
	int i = 0;
	for(i = 0; i < USER_AUTH_CNT; ++i)
	{
		if(!HasAuth(thiz, username, (USER_AUTH_TYPE)i))
		{
			bHas = 0;
			break;
		}
	}

	return !!bHas;

}


static int SetOneAuth(User* thiz, const char* username, USER_AUTH_TYPE which, unsigned char bSet)
{

	int nUserId = IsUserExist(thiz, username);

	assert(which < USER_AUTH_CNT);
	if(!nUserId)
	{
		return -1;
	}

	thiz->attr->stUserAuth[nUserId - 1].bAuth[which] = !!bSet;
	return 0;

}


static int SetAuth(User* thiz, const char* username, USER_AUTH_TYPE which)
{
	return SetOneAuth(thiz, username, which, 1);
}

static int SetAllAuth(User* thiz, const char* username)
{
	int i = 0;
	for(i = 0; i < USER_AUTH_CNT; ++i)
	{
		if(0 != SetAuth(thiz, username, (USER_AUTH_TYPE)i))
		{
			return -1;
		}
	}

	return 0;

}

static int ClearAuth(User* thiz, const char* username, USER_AUTH_TYPE which)
{
	return SetOneAuth(thiz, username, which, 0);
}

static int ClearAllAuth(User* thiz, const char* username)
{

	int i = 0;
	for(i = 0; i < USER_AUTH_CNT; ++i)
	{
		if(0 != ClearAuth(thiz, username, (USER_AUTH_TYPE)i))
		{
			return -1;
		}
	}

	return 0;

}



User UserStruct(const char* pathFile, void* arg)
{
	User self = {0};
	unsigned short nCRC16 = 0;
	// clear it
	memset(&self, 0, sizeof(self));
	assert(self.attr = (UserAttr_t*)malloc(sizeof(UserAttr_t)));

	// install operations
	self.UpdateCRC16 = UpdateCRC16;
	self.Save = Save;
	self.Load = Load;
	self.GetFilePath = _GetFilePath;

	self.RestoreDefault = RestoreDefault;
	self.Reset = Reset;
	self.Print = Print;
	self.Free = Free;

	self.GetUserCnt = GetUserCnt;
	self.GetSuperUserCnt = GetSuperUserCnt;
	self.GetMaxUserCnt = GetMaxUserCnt;
	self.GetUsername = GetUsername;

	self.AddUser = AddUser;
	self.RemoveUser = RemoveUser;
	self.SetSuperUser = SetSuperUser;
	self.SetPassword = SetPassword;

	self.IsUserExist = IsUserExist;
	self.Verify = Verify;
	self.VerifySFT = VerifySFT;
	self.GetUserPwdByName = GetUserPwdByName;

	self.IsSuperUser = IsSuperUser;
	self.IsDeleteForbid = IsDeleteForbid;

	// operation
	self.HasAuth = HasAuth;
	self.HasAllAuth = HasAllAuth;
	self.SetAuth = SetAuth;
	self.SetAllAuth = SetAllAuth;
	self.ClearAuth = ClearAuth;
	self.ClearAllAuth = ClearAllAuth;

	// load user
	if(Load(&self) < 0){
		Reset(&self);
	}else{
		// match crc16
		nCRC16 = self.attr->nCRC16;
		self.UpdateCRC16(&self);
		if(nCRC16 != self.attr->nCRC16)	{
			USR_TRACE("user crc error %04x/%04x", self.attr->nCRC16, nCRC16);
			Reset(&self);
		}
	}
	USR_TRACE("user file size %d crc %04x operation %d", sizeof(*(self.attr)), self.attr->nCRC16, sizeof(self));
	return self;
}

User* PUserStruct(const char* pathFile, void* arg)
{
	User* thiz = NULL;
	User user = {0};

	assert(thiz = (User*)malloc(sizeof(User)));

	user = UserStruct(pathFile, arg);
	memcpy(thiz, &user, sizeof(user));

	return thiz;
}

int PUserDestruct(User* thiz, void* arg)
{
	if(!thiz)
	{
		return -1;
	}

	free(thiz->attr);
	thiz->attr = NULL;
	free(thiz);

	return 0;

}

int PUserGetPwd(const char *username, char *pwd, int ret_size)
{
	User* user = PUserStruct(NULL, NULL);
	if (user == NULL) 
		return -1;
	if (user->Load(user) < 0) {
		PUserDestruct(user, NULL);
		return -1;
	}
	if (user->GetUserPwdByName && user->GetUserPwdByName(user, username, pwd, ret_size) < 0) {
		PUserDestruct(user, NULL);
		return -1;
	}
	PUserDestruct(user, NULL);
	return 0;
}


#endif	//__USER_C__
