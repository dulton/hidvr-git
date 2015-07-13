
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>

#include "conf.h"
#include "environment.h"
#include "para.h"

#define DEBUG_PARA
#ifdef	DEBUG_PARA
#define	PARA_TRACE(fmt,args...) printf("\033[0;32;34mPARA: "fmt"\033[m\r\n", ##args)
#else
#define	PARA_TRACE(fmt,args...)
#endif

//#ifndef MINGW
//#define PARA_DEF_LANGUAGE (WORLDWIDE_CHINESE_MAINLAND)
//#else
//#define PARA_DEF_LANGUAGE (WORLDWIDE_ENGLISH)
//#endif

//////////////////////////////////////////////////////////////////
// global value
static char s_azPara[PARA_SIZE]; // 8k

//static WORLDWIDE_LANGUAGE s_enLanguage;
//static VOUT_STANDARD s_enStandard;
static char s_szDevName[PARA_MAX_LEN];
static char s_szDevModel[PARA_MAX_LEN];

//typedef struct _StandardMap
//{
//	const char* pStandard;
//	VOUT_STANDARD enStandard;
//}StandardMap;

//static const StandardMap s_astStandardMap[] =
//{
//	{ "PAL", VOUT_PAL },
//	{ "NTSC", VOUT_NTSC },
//};

//typedef struct _LangMap
//{
//	const char* pLang;
//	WORLDWIDE_LANGUAGE enLang;
//}LangMap;

//static const LangMap s_astLangMap[] =
//{
//	{ "ENGLISH", WORLDWIDE_ENGLISH },
//	{ "CHINESE_MAINLAND", WORLDWIDE_CHINESE_MAINLAND },
//	{ "CHINESE_HONGKONG", WORLDWIDE_CHINESE_HONGKONG },
//	{ "CHINESE_TAIWAN", WORLDWIDE_CHINESE_TAIWAN },
//	{ "KOREAN", WORLDWIDE_KOREAN },
//	{ "JAPANESE", WORLDWIDE_JAPANESE },
//	{ "ARABIC", WORLDWIDE_ARABIC },
//	{ "PERSIAN", WORLDWIDE_PERSIAN },
//	{ "HEBREW", WORLDWIDE_HEBREW },
//	{ "PORTUGUESE", WORLDWIDE_PORTUGUESE },
//	{ "SPANISH", WORLDWIDE_SPANISH },
//	{ "HUNGARIAN", WORLDWIDE_HUNGARIAN },
//	{ "RUSSIAN", WORLDWIDE_RUSSIAN },
//	{ "FRENCH", WORLDWIDE_FRENCH },
//	{ "GERMAN", WORLDWIDE_GERMAN },
//	{ "THAI", WORLDWIDE_THAI },
//	{ "VIETNAMESE", WORLDWIDE_VIETNAMESE },
//	{ "TURKISH", WORLDWIDE_TURKISH },
//	{ "ITALIAN", WORLDWIDE_ITALIAN },
//};

//////////////////////////////////////////////////////////////////
static int para_Load()
{
	int ret = 0;
	int fd = -1;
	if((fd = open(PARA_DEV, O_RDONLY)) < 0){
		printf("open \""PARA_DEV"\" failed!\r\n");
		return -1;
	}
	lseek(fd, PARA_OFFSET, SEEK_SET);
	ret = read(fd, s_azPara, PARA_SIZE);
	if(PARA_SIZE != ret){
		printf("read(%d) \""PARA_DEV"\" error!\r\n", ret);
		return -1;
	}
	close(fd);
	fd = -1;
	return ret;
}

static int para_Save()
{
	int ret = 0;
	int fd = -1;
	if((fd = open(PARA_DEV, O_WRONLY)) < 0){
		printf("open \""PARA_DEV"\" failed!\r\n");
		return -1;
	}
	lseek(fd, PARA_OFFSET, SEEK_SET);
	ret = write(fd, s_azPara, PARA_SIZE);
	if(PARA_SIZE != ret){
		printf("write(%d) \""PARA_DEV"\" error!\r\n", ret);
		return -1;
	}
	close(fd);
	fd = -1;
	return ret;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void para_MkDefault()
{
	typedef struct _ParaItem
	{
		const char* szName; // such as model
		const char* szValue; // such as JA7204
	}ParaItem;
	const ParaItem astDefault[] =
	{
		{ PARA_DEVNAME, DEVICE_NAME },
		{ PARA_DEVMODEL, DEVICE_MODEL },
//		{ PARA_LANGUAGE, s_astLangMap[s_enLanguage].pLang },
//		{ PARA_STANDARD, s_astStandardMap[s_enStandard].pStandard },
	};

	int i = 0;
	char sz_item[PARA_MAX_LEN] = {""};
#ifndef MINGW
	char* p_config = (char*)(alloca(sizeof(s_azPara))); // alloca an area the same as the size of global para
#else
	char* p_config = (char*)(malloc(sizeof(s_azPara))); // alloca an area the same as the size of global para
#endif
	memset(p_config, 0, sizeof(s_azPara));

	strcat(p_config, PARA_HEAD);
	for(i = 0; i < (int)(sizeof(astDefault) / sizeof(astDefault[0])); ++i){
		sprintf(sz_item, "<%s>%s</%s>\n", astDefault[i].szName, astDefault[i].szValue, astDefault[i].szName);
		if(strlen(p_config) + strlen(sz_item) > sizeof(s_azPara) - sizeof(PARA_TAIL)){
			printf("config full\r\n");
			break;
		}
		// catch the string
		strcat(p_config, sz_item);
	}
	strcat(p_config, PARA_TAIL);
	memset(s_azPara, 0, sizeof(s_azPara));
	strncpy(s_azPara, p_config, strlen(p_config));
	printf("make config xml\r\n%s", s_azPara);
}

static unsigned char para_Check()
{
	if(0 == strncmp(s_azPara, PARA_HEAD, strlen(PARA_HEAD)) && strstr(s_azPara, PARA_TAIL)){
		return 1;
	}
	return 0;
}

static char* para_Make(const char* name, const char* value)
{
	static char item[PARA_MAX_LEN] = {""};
	if(!name || !value){
		return NULL;
	}
	if(!(2 * strlen(name) + strlen(value) + strlen("<></>\n") < PARA_MAX_LEN)){
		return NULL;
	}
	sprintf(item, "<%s>%s</%s>\n", name, value, name);
	return item;
}

static char* para_Parse(const char* name)
{
	static char value[PARA_MAX_LEN] = {""};
	char head[PARA_MAX_LEN] = {""};
	char tail[PARA_MAX_LEN] = {""};
	char* headpos = NULL;
	char* tailpos = NULL;

	sprintf(head, "<%s>", name);
	sprintf(tail, "</%s>\n", name);
	// find the value
	if((headpos = strstr(s_azPara, head)) && (tailpos = strstr(s_azPara, tail))){
		// valid
		// cut the tail part
		headpos += strlen(head);
		memset(value, 0, sizeof(value));
		strncpy(value, headpos,
			(unsigned int)(tailpos - headpos) < (sizeof(value) - 1) ?
			(tailpos - headpos) : (sizeof(value) - 1));
//		PARA_TRACE("value = %s", value);
		return value;
	}
	return NULL;
}

static const char* para_ParseString(const char* paraname)
{
	return para_Parse(paraname);
}

//static int para_ParseInt(const char* paraname)
//{
//	return atoi(para_Parse(paraname));
//}

//int PARA_MapStandard(const char* szStandard)
//{
//	VOUT_STANDARD enDefStandard = s_enStandard;
//	if(NULL == szStandard){
//		return enDefStandard;
//	}
//
//	int i = 0;
//	for(i = 0; i < (int)(sizeof(s_astStandardMap) / sizeof(s_astStandardMap[0])); ++i){
//		if(0 == strncasecmp(szStandard, s_astStandardMap[i].pStandard, strlen(s_astStandardMap[i].pStandard))){
//			enDefStandard = s_astStandardMap[i].enStandard;
//			break;
//		}
//	}
//	return enDefStandard;
//}

//const char* PARA_UmapStandard(int enStandard)
//{
//	int i = 0;
//	for(i = 0; i < (int)(sizeof(s_astStandardMap) / sizeof(s_astStandardMap[0])); ++i){
//		if(enStandard == s_astStandardMap[i].enStandard){
//			return s_astStandardMap[i].pStandard;
//		}
//	}
//	return s_astLangMap[0].pLang;
//}

//int PARA_MapLanguage(const char* szLang)
//{
//	WORLDWIDE_LANGUAGE enDefLang = s_enLanguage;
//	if(NULL == szLang){
//		return enDefLang;
//	}
//	int i = 0;
//	for(i = 0; i < (int)(sizeof(s_astLangMap) / sizeof(s_astLangMap[0])); ++i){
//		if(0 == strncasecmp(szLang, s_astLangMap[i].pLang, strlen(s_astLangMap[i].pLang))){
//			enDefLang = s_astLangMap[i].enLang;
//			break;
//		}
//	}
//	return enDefLang;
//}

//const char* PARA_UmapLanguage(int enLang)
//{
//	int i = 0;
//	for(i = 0; i < (int)(sizeof(s_astLangMap) / sizeof(s_astLangMap[0])); ++i){
//		if(enLang == s_astLangMap[i].enLang){
//			return s_astLangMap[i].pLang;
//		}
//	}
//	return s_astLangMap[0].pLang;
//}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void PARA_Init()
{
	const char* pText = NULL;
	// init parameter
//	s_enLanguage = (WORLDWIDE_LANGUAGE)PARA_MapLanguage(DEFAULT_LANGUAGE);
//	s_enStandard = (VOUT_STANDARD)PARA_MapStandard(DEFAULT_STANDARD);
	// load the parameter
	para_Load();
	if(!para_Check()){
		para_MkDefault();
		para_Save();
	}
	// load default device name
	if((pText = para_ParseString(PARA_DEVNAME))){
		strcpy(s_szDevName, pText);
	}
	// load default device model
	if((pText = para_ParseString(PARA_DEVMODEL))){
		strcpy(s_szDevModel, pText);
	}
//	// load default language
//	s_enLanguage = (WORLDWIDE_LANGUAGE)PARA_MapLanguage(para_ParseString(PARA_LANGUAGE));
//	// load default standard
//	s_enStandard = (VOUT_STANDARD)PARA_MapStandard(para_ParseString(PARA_STANDARD));

//	PARA_TRACE("Name: %s", s_szDevName);
//	PARA_TRACE("Model: %s", s_szDevModel);
//	PARA_TRACE("Language: %s", PARA_UmapLanguage(s_enLanguage));
}
void PARA_Destroy()
{
}


//unsigned int PARA_GetStandard()
//{
//	return (unsigned int)s_enStandard;
//}
//int PARA_SetStandard(unsigned int enStandard)
//{
//	if(enStandard < VOUT_STANDARD_CNT){
//		s_enStandard = (VOUT_STANDARD)enStandard;
//	}
//	return -1;
//}


//unsigned int PARA_GetLanguage()
//{
//	return (unsigned int)s_enLanguage;
//}
//int PARA_SetLanguage(unsigned int enLange)
//{
//	if(enLange < WORLDWIDE_LANGUAGE_CNT){
//		s_enLanguage = (WORLDWIDE_LANGUAGE)enLange;
//	}
//	return -1;
//}

const char* PARA_GetDevName()
{
	if(strlen(s_szDevName)){
		return s_szDevName;
	}
	return NULL;
}
int PARA_SetDevName(const char* szName)
{
	if(szName && strlen(szName) > 0 && strlen(szName) < sizeof(s_szDevName)){
		strcpy(s_szDevName, szName);
		return 0;
	}
	return -1;
}

const char* PARA_GetDevModel()
{
	if(strlen(s_szDevModel)){
		return s_szDevModel;
	}
	return NULL;
}
int PARA_SetDevModel(const char* szModel)
{
	if(szModel && strlen(szModel) > 0 && strlen(szModel) < sizeof(s_szDevModel)){
		strcpy(s_szDevModel, szModel);
		return 0;
	}
	return -1;
}

int PARA_SettingCommit()
{
#define CHECKPOINT(str1,str2,str3,limit) if(!(strlen(str1)+strlen(str2)+strlen(str3)<limit)){PARA_TRACE("para too long");return -1;}
#define ATTACH(srcstr,str) if(str){CHECKPOINT((srcstr),(str),PARA_TAIL, PARA_SIZE);strcat((srcstr),(str));}
	char* pText = NULL;
	char azPara[PARA_SIZE]; // 8k
	memset(azPara, 0, sizeof(azPara));
	// head
	strcat(azPara, PARA_HEAD);
	// commit name
	pText = para_Make(PARA_DEVNAME, s_szDevName);
	ATTACH(azPara, pText);
	// commit model
	pText = para_Make(PARA_DEVMODEL, s_szDevModel);
	ATTACH(azPara, pText);
//	// commit language
//	pText = para_Make(PARA_LANGUAGE, PARA_UmapLanguage(s_enLanguage));
//	ATTACH(azPara, pText);
//	// commit standard
//	pText = para_Make(PARA_STANDARD, PARA_UmapStandard(s_enStandard));
	ATTACH(azPara, pText);
	// tail
	strcat(azPara, PARA_TAIL);
	// compare diff
	if(0 == strcmp(s_azPara, azPara)){
		// the same
		return -1;
	}
	// save
	memcpy(s_azPara, azPara, sizeof(azPara));
	PARA_TRACE("\r\n%s", s_azPara);
	para_Save();
	return 0;
#undef ATTACH
#undef CHECKPOINT
}

