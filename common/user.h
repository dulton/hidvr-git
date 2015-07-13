


#ifndef __USER_H__
#define __USER_H__
#ifdef __cplusplus
extern "C" {
#endif


#define MAX_FILEPATH_LEN	(50)
#define MAX_USERNAME_LEN	(20)
#define MAX_PASSWORD_LEN	(20)


typedef	enum
{
	USER_AUTH_NULL = -1,
	USER_AUTH_PLAYBACK, //0
	USER_AUTH_BACKUP,
	USER_AUTH_PTZCTRL,
	USER_AUTH_VOLUME,
	USER_AUTH_MANUALREC,
	USER_AUTH_SETTING, //5
#ifdef _NEWUI1
#ifndef TECHVISION_GUI
	USER_AUTH_SPLIT_SCREEN,
	USER_AUTH_CHANNEL_ADJUST,
#endif
	USER_AUTH_GUIDE_SETUP,
#else    
	USER_AUTH_SETTING_MISC,
	USER_AUTH_SETTING_ENCODE,
	USER_AUTH_SETTING_RECORD,
	USER_AUTH_SETTING_NETWORK,
	USER_AUTH_SETTING_SCREEN, //14
	USER_AUTH_SETTING_PTZ,
	USER_AUTH_SETTING_MOTION,
	USER_AUTH_SETTING_SENSOR,
	USER_AUTH_TOOLS,
	USER_AUTH_TOOLS_USERS,   //19
	USER_AUTH_TOOLS_SYSTEM,
	USER_AUTH_TOOLS_HDDS,
	USER_AUTH_TOOLS_FACTORYSETTING,
#endif
	USER_AUTH_REMOTE,
	USER_AUTH_REMOTE_CONFIG, //24
	USER_AUTH_REMOTE_PLAYBACK,
	USER_AUTH_CNT,
}USER_AUTH_TYPE;




struct  tagUserAttr;
typedef	struct tagUserAttr UserAttr_t;

typedef	struct tagUser	User;
struct tagUser
{

	UserAttr_t* attr;
	char file[MAX_FILEPATH_LEN];

	// crc
	unsigned short (*UpdateCRC16)(User* thiz);

	// store / restore from file
	int (*Save)(User* thiz);
	int (*Load)(User* thiz);
	int (*GetFilePath)(User* thiz, char* ret_szPath);

	int (*RestoreDefault)(User* thiz);
	int (*Reset)(User* thiz);

	int (*Print)(User* thiz);
	void (*Free)(User* thiz);

	// operation
	int (*GetUserCnt)(User* thiz);
	int (*GetSuperUserCnt)(User* thiz);
	int (*GetMaxUserCnt)(User* thiz);

	// user management
	int (*AddUser)(User* thiz, const char* username, const char* password, unsigned char is_superuser);
	int (*RemoveUser)(User* thiz, const char* username);
	int (*SetSuperUser)(User* thiz, const char* username, unsigned char set_clear);
	int (*SetPassword)(User* thiz, const char* username, const char* password);

	const char* (*GetUsername)(User* thiz, int id);
	unsigned char (*IsUserExist)(User* thiz, const char* username);
	unsigned char (*Verify)(User* thiz, const char* username, const char* password);
	unsigned char (*VerifySFT)(User* thiz, const char* username, const char* password);
	int (*GetUserPwdByName)(User *thiz, const char *username, char *userpwd, int ret_size);

	// authorization
	unsigned char (*HasAuth)(User* thiz, const char* username, USER_AUTH_TYPE which);
	unsigned char (*HasAllAuth)(User* thiz, const char* username);

	unsigned char (*IsSuperUser)(User* thiz, const char* username);
	unsigned char (*IsDeleteForbid)(User* thiz, const char* username);

	int (*SetAuth)(User* thiz, const char* username, USER_AUTH_TYPE which);
	int (*SetAllAuth)(User* thiz, const char* username);
	int (*ClearAuth)(User* thiz, const char* username, USER_AUTH_TYPE which);
	int (*ClearAllAuth)(User* thiz, const char* username);

};

#define	PUSER(ptr)	((User*)ptr)


extern User* PUserStruct(const char* file, void* arg);
extern int PUserDestruct(User* thiz, void* arg);

extern int PUserGetPwd(const char *username, char *pwd, int ret_size);

#ifdef __cplusplus
}
#endif
#endif	//__USER_H__
