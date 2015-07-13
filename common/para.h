
#ifndef __PARA_H__
#define __PARA_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MINGW
#define PARA_DEV "/dev/mtdblock3"
#else
#define PARA_DEV "mtdblock"
#endif
#define PARA_OFFSET (0) // 0k
#define PARA_SIZE (8*1024) // 8k attention here, don't override the environment
#define PARA_MAX_LEN (128)

#define PARA_DEVNAME "devname"
#define PARA_DEVMODEL "model"
//#define PARA_LANGUAGE "language"
//#define PARA_STANDARD "standard"

// feature
#define PARA_TITLE "para"
#define PARA_HEAD "<"PARA_TITLE">\n"
#define PARA_TAIL "</"PARA_TITLE">\n"

extern void PARA_Init();
extern void PARA_Destroy();

//extern int PARA_MapLanguage(const char* szLang);
//extern const char* PARA_UmapLanguage(int enLang);

//extern int PARA_MapStandard(const char* szStandard);
//extern const char* PARA_UmapStandard(int enStandard);

//extern unsigned int PARA_GetLanguage();
//extern int PARA_SetLanguage(unsigned int enLange);

//extern unsigned int PARA_GetStandard();
//extern int PARA_SetStandard(unsigned int enStandard);

extern const char* PARA_GetDevName();
extern int PARA_SetDevName(const char* szName);

extern const char* PARA_GetDevModel();
extern int PARA_SetDevModel(const char* szModel);

extern int PARA_SettingCommit();


#ifdef __cplusplus
}
#endif

#endif //__PARA_H__

