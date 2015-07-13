#ifndef GENERIC_H_
#define GENERIC_H_

#include "stdinc.h"
#include "cross.h"

#ifdef _cplusplus
extern "C" {
#endif

#define THREAD_ZEROID() ((pthread_t)NULL)
#define THREAD_IS_ZEROID(__tid) (THREAD_ZEROID() == __tid)

#define __MIN(A,B) (((A) > (B)) ? (B) : (A))

    static inline void TRACE_ARGC_ARGV(int argc, char *argv[])
    {
        int i = 0;
        printf("main(%d,", argc);
        if(argc > 0) {
            for(i = 0; i < argc; ++i) {
                printf(" \"%s\"", argv[i]);
            }
        } else {
            printf(" NULL");
        }
        printf(");\r\n");
    }

    static inline bool CHECK_DIR_EXIST(const char *dir_path)
    {
#ifdef WIN32
        return PathIsDirectory(dir_path) ? true : false;
#else
        DIR *dir = opendir(dir_path);
        if(NULL != dir) {
            closedir(dir);
            dir = NULL;
            return true;
        }
        return false;
#endif
    }

#ifdef WIN32
#define CHECK_FILE_EXIST(str_name) PathFileExists(str_name)
#else
#define CHECK_FILE_EXIST(str_name) (-1 != access(str_name, F_OK))
#endif

    static inline bool IS_FILE_EXIST(const char *filePath)
    {
#ifdef WIN32
        return PathFileExists((char *)filePath) ? true : false;
#else
        return (-1 != access(filePath, F_OK));
#endif
    }


#define GET_FILE_SIZE(str_name, s32_size) \
	do{\
		struct stat file_stat;\
		if(stat((str_name), &file_stat) < 0){ \
			(s32_size) = -1; \
		} \
		(s32_size) = (ssize_t)(file_stat.st_size); \
	}while(0)

#define STR_THE_SAME_N(str, str_match, size) \
			(strlen(str) == strlen(str_match) && 0 == strncmp(str_match, str, size))

#define STR_THE_SAME(str, str_match) \
	(strlen(str) == strlen(str_match) && 0 == strcmp(str, str_match))

#define STR_CASE_THE_SAME(str, str_match) \
	(strlen(str) == strlen(str_match) && 0 == STRCASECMP(str, str_match))

    static inline void STR_TO_UPPER(char *uri_text)
    {
        int i = 0;
        int uri_len = strlen(uri_text);
        for(i = 0; i < uri_len; ++i) {
            if(isalpha((int)uri_text[i])) {
                uri_text[i] = (char)(toupper((int)uri_text[i]));
            }
        }
    }

#define GET_HOST_BYNAME(str_domain, u32_host) \
	do{ \
		struct hostent* host_ent = gethostbyname2(str_domain, AF_INET); \
		if(!host_ent){ u32_host = 0; } \
		else{ \
			struct in_addr* const host_addr_list = (struct in_addr*)(host_ent->h_addr_list[0]); \
			struct in_addr host_addr = host_addr_list[0]; \
			u32_host = host_addr.s_addr; \
		} \
	}while(0)


#define STRUCT_ZERO(stru) do{ memset(&stru, 0, sizeof(stru)); }while(0)

#define ARRAY_ZERO(arr) do{ memset(arr, 0, sizeof(arr)); }while(0)
#define ARRAY_SIZE(arr) (sizeof(arr))
#define ARRAY_ITEM(arr) (ARRAY_SIZE(arr) / sizeof(arr[0]))

// return an hour offset formated as +/- offset hour x 100 + offset minutes
// static inline int GMT_GET()
// {
// 	char tz[32] = {""};
// 	int hour = 0, min = 0;
// 	char sign = '+';
// 	int gmt = 0;
//
// 	SNPRINTF(tz, sizeof(tz), "%s", getenv("TZ"));
// 	if(3 == sscanf(tz, "CST%c%d:%d", &sign, &hour, &min)){
// 		gmt = hour * 100 + min;
// 		if('+' == sign){
// 			gmt *= -1;
// 		}
// 	}
// 	return gmt;
// }

// an hour offset formated as +/- offset hour x 100 + offset minutes
// static inline void GMT_SET(int gmt)
// {
// 	if(gmt >= -1200 && gmt <= 1300){
// 		int const hour = abs(gmt) / 100;
// 		int const min = abs(gmt) % 100;
//
// 		if(min >= 0 && min < 60){
// 			char text[32] = {""};
// 			struct timezone tz;
// 			tz.tz_minuteswest = -1 * ((gmt >= 0 ? 1 : -1) * (hour * 60 + min));
// 			tz.tz_dsttime = 0;
// 			snprintf(text, sizeof(text), "CST%c%02d:%02d", gmt <= 0 ? '+' : '-',  hour, min); // opposite to GMT
// 			setenv("TZ", text, 1);
// 			tzset();
// 		}
// 	}
// }

// static inline int GMT_PARSE(const char *str, int defGMT)
// {
// 	int gmt = defGMT, hour = 0, min = 0;
// 	char sign = '+';
// 	if(strlen(str) == 3 && 0 == strcasecmp("GMT", str)){
// 		gmt = 0;
// 	}else if(3 == sscanf(str, "GMT%c%d:%d", &sign, &hour, &min)){
// 		if(min >= 0 && min < 60){
// 			// valid minutes
// 			gmt = ('+' == sign ? 1 : -1) * (hour * 100 + min);
// 		}
// 	}
// 	return gmt;
// }

// static inline const char *GMT_STRING(int gmt, char *result, int resultMax)
// {
// 	if(NULL != result){
// 		if(0 == gmt){
// 			snprintf(result, resultMax, "GMT");
// 		}else{
// 			snprintf(result, resultMax, "GMT%c%02d:%02d", gmt >= 0 ? '+' : '-',
// 				abs(gmt) / 100, abs(gmt) % 100);
// 		}
// 		return result;
// 	}
// 	return NULL;
// }

// static inline const char *ISO8601(struct tm *t, bool tz, char *result, int resultMax)
// {
// 	strftime(result, resultMax, "%FT%T", t);
// 	if(tz){
// 		int gmt = GMT_GET();
// 		snprintf(result + strlen(result), resultMax - strlen(result),
// 			"%c%02d:%02d", gmt >= 0 ? '+' : '-', abs(gmt) / 100, abs(gmt) % 100);
// 	}
// 	return result;
// }

// static inline int GET_DISK_CAPACITY_MB(const char *mountFolder)
// {
// 	struct statfs statFS;
// 	memset(&statFS, 0, sizeof(statFS));
// 	if(0 == statfs(mountFolder, &statFS)){
// 		return (int)((int64_t)(statFS.f_blocks) * (int64_t)(statFS.f_bsize) / (1024 * 1024));
// 	}
// 	return 0;
// }

// static inline int GET_DISK_FREE_MB(const char *mountFolder)
// {
// 	struct statfs statFS;
// 	memset(&statFS, 0, sizeof(statFS));
// 	if(0 == statfs(mountFolder, &statFS)){
// 		return (int)((int64_t)(statFS.f_bavail) * (int64_t)(statFS.f_bsize) / (1024 * 1024));
// 	}
// 	return 0;
// }

// static inline int REMOVE_FILE(const char *filePath)
// {
// 	if(0 == unlink(filePath)){
// 		if(0 == remove(filePath)){
// 			return 0;
// 		}
// 	}
// 	return -1;
// }

// static inline int TOUCH_FILE(const char *filePath)
// {
// 	FILE *fid = fopen(filePath, "a+b");
// 	if(NULL != fid){
// 		fclose(fid);
// 		fid = NULL;
// 	}
// 	return IS_FILE_EXIST(filePath) ? 0 : -1;
// }

// static inline int COPY_FILE(const char *srcFilePath, const char *dstFilePath)
// {
// 	char buf[1024] = {""};
// 	int ret = 0, err = 0;
// 	FILE *srcFID = NULL;
// 	FILE *dstFID = NULL;
//
// 	srcFID = fopen(srcFilePath, "rb");
// 	if(NULL != srcFID){
// 		dstFID = fopen(dstFilePath, "w+b");
// 		if(NULL != dstFID){
// 			while((ret = fread(buf, 1, sizeof(buf), srcFID)) > 0){
// 				ret = fwrite(buf, 1, ret, dstFID);
// 				if(ret < 0){
// 					err = 1;
// 					break;
// 				}
// 			}
// 			fclose(dstFID);
// 			dstFID = NULL;
// 		}
// 		fclose(srcFID);
// 		srcFID = NULL;
// 	}
// 	if(err){
// 		REMOVE_FILE(dstFilePath);
// 		return -1;
// 	}
// 	return 0;
// }

// static inline int MOVE_FILE(const char *srcFilePath, const char *dstFilePath)
// {
// 	if(0 == COPY_FILE(srcFilePath, dstFilePath)){
// 		return REMOVE_FILE(srcFilePath);
// 	}
// 	return -1;
// }

// static inline int MAKE_DIRECTORY(const char *directoryPath)
// {
// 	char *dirPath = strdupa(directoryPath);
// 	char *dir = alloca(strlen(dirPath) * 2);
// 	char *token = NULL;
// 	char *dirName = NULL;
//
// 	dirName = strtok_r(dirPath, "/", &token);
// 	if(NULL != dirName){
// 		strcpy(dir, dirName);
// 		strcat(dir, "/");
// 		if(0 == mkdir(dir, 0)){
// 			printf("mkdir \"%s\"\r\n", dir);
// 			while(NULL != (dirName = strtok_r(NULL, "/", &token))){
// 				strcpy(dir, dirName); // append the directory path
// 				strcat(dir, "/");
// 				if(0 == mkdir(dir, 0)){
// 					printf("mkdir \"%s\"\r\n", dir);
// 				}else{
// 					return -1;
// 				}
// 			}
// 			return 0;
// 		}
// 	}
// 	return -1;
// }

// static inline void BUILD_DATE_TIME(struct tm *result)
// {
// 	int i = 0;
// 	char *text = NULL, *token = NULL;
// 	char *const buildDate = strdupa(__DATE__);
// 	char *const buildTime = strdupa(__TIME__);
// 	const char *monthText[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",};
// 	// get date
// 	text = strtok_r(buildDate, " ", &token);
// 	for(i = 0; i < (typeof(i))(sizeof(monthText) / sizeof(monthText[0])); ++i){
// 		if(0 == strncasecmp(monthText[i], text, strlen(monthText[i]))){
// 			result->tm_mon = i;
// 			break;
// 		}
// 	}
// 	text = strtok_r(NULL, " ", &token);
// 	result->tm_mday = atoi(text);
// 	text = strtok_r(NULL, " ", &token);
// 	result->tm_year = atoi(text) - 1900;
//
// 	// get time
// 	text = strtok_r(buildTime, ":", &token);
// 	result->tm_hour = atoi(text);
// 	text = strtok_r(NULL, ":", &token);
// 	result->tm_min = atoi(text);
// 	text = strtok_r(NULL, " ", &token);
// 	result->tm_sec = atoi(text);
//
// }

#define MAKE_IMAGE   //please add this line when makeing image!!

#define ALIGN_LITTLE_ENDIAN(__val, __align) (typeof(__val))(((uint32_t)(__val) / (uint32_t)(__align)) * (uint32_t)(__align))
#define ALIGN_BIG_ENDIAN(__val, __align) (typeof(__val))((((uint32_t)(__val) + (uint32_t)(__align) - 1) / (uint32_t)(__align)) * (uint32_t)(__align))



    extern float cpu_get_status();
    extern int check_ipv4_addr(const char *pstrip);

    extern int check_mac_addr(char* str, unsigned char* buf);
    extern int http_parse_url(unsigned char *_u8ip, unsigned short *_port, char *_uri, char *_default) ;
    extern char  *_ip_2string(unsigned char *ip, char *saveptr);
    extern char  *_mac_2string(unsigned char *mac, char *saveptr);
    extern int ipstr2uint8(unsigned char *_u8ip,  char *_default);
    extern int macstr2uint8(unsigned char *_u8mac, char *_default);
    extern int kv_format_get_int(const char *content, const char *key, int * const value, int default_value);
    extern int kv_format_get_string(const char *content, const char *key, char * const value, char *default_value);
	extern int xml_format_get_string(const char *content, const char *key_attr_name, const char *key_attr_val, 
		char * const value, const char *default_value, const char *keylist, ...);
	extern int xml_format_get_int(const char *content, const char *key_attr_name, const char *key_attr_val, 
		int * value, int default_value, const char *keylist, ...);
	
#ifdef _cplusplus
};
#endif
#endif //GENERIC_H_

