

#ifndef __COMMON_C__
#define __COMMON_C__


#define NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include "common.h"
#include <time.h>
#ifndef MINGW
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#endif
#include "environment.h"
//#include "gui.h"
#include<sys/types.h>
#include<sys/wait.h>

int IsProgAlive(const char* program)
{
	int bExisted = 0;
	FILE* fd = NULL;
	char szBuf[4000] = {""};

	if((fd = popen("/bin/ps", "r")))
	{
		memset(szBuf, 0, sizeof(szBuf));
		fread(szBuf, sizeof(szBuf), 1, fd);

		if (strstr(szBuf, program))
		{
			// is running
			printf("program %s is running\r\n", program);
			bExisted = 1;
		}
		else
		{
			// not running
			printf("program %s not running\r\n", program);
			bExisted = 0;
		}
		pclose(fd);
		fd = NULL;

	}

	return bExisted;
}

int IsDirExist(const char* stDirPath)
{

	DIR* dir = NULL;
	//struct dirent* entry = NULL;

	if(NULL == (dir = opendir(stDirPath)))
	{
		return 0;
	}

	closedir(dir);
	dir = NULL;
	return 1;

}

int IsFileExist(const char* szFilePath)
{
	return (0 == access(szFilePath, F_OK | W_OK));
}


//
// return:
// no file -> -1
// file path too long -> 0
// file exited an return -> the len of path

//#define	MAX_RET_PATHLEN	(200)
#ifndef MINGW
int GetFilePath(const char* szRootPath, const char* szFileName, const int nDepth, char* ret_szFilePath)
{
	DIR* dir = NULL;
	struct dirent* entry = NULL;
	struct stat statbuf;
	char szCurWorkPath[100] = {""};
	char *szCurWorkDir = NULL;
	int nPathLen = -1;

	if(NULL == (dir = opendir(szRootPath))){
		return -1;
	}

	// protect current working path
	szCurWorkDir = getcwd(szCurWorkPath, sizeof(szCurWorkPath) - 1);
	assert(szCurWorkDir);

	chdir(szRootPath);
	while((entry = readdir(dir)) != NULL){
		lstat(entry->d_name, &statbuf);
		if(S_ISDIR(statbuf.st_mode)){
			// is a directory
			// skip local path and upper path
			if(!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)){
				continue;
			}

			// not the bottom of directory to scan
			if(nDepth){
				char szSubPath[MAX_RET_PATHLEN + 1] = {0};	// include '\0'
				memset(szSubPath, 0, sizeof(szSubPath));
				strcpy(szSubPath, szRootPath);

				// check length is legal
				if(strlen(szSubPath) + 1 > MAX_RET_PATHLEN){
					strcpy(ret_szFilePath, "");
					return 0;
				}

				// catch it
				strcat(szSubPath, "/");

				// check length again
				if(strlen(szSubPath) + strlen(entry->d_name) > MAX_RET_PATHLEN){
					strcpy(ret_szFilePath, "");
					return 0;
				}

				// catch as a completed path
				strcat(szSubPath, entry->d_name);

				// recurse sub path
				printf("enter path \"%s\"\r\n", szSubPath);
				if((nPathLen = GetFilePath(szSubPath, szFileName, nDepth - 1, ret_szFilePath)) > 0){
					break;
				}

			}else{
				// to scan local path other file
				continue;
			}
		}else{
			// is file
//			printf("check file \"%s\"/\"%s\"\r\n", entry->d_name, szFileName);
			if(!strncasecmp(entry->d_name, szFileName, strlen(entry->d_name))){
				// recurse return here
				strcpy(ret_szFilePath, szRootPath);

				// check length is legal
				if(strlen(szRootPath) + 1 > MAX_RET_PATHLEN){
					strcpy(ret_szFilePath, "");
					return 0;
				}

				// catch it
				strcat(ret_szFilePath, "/");

				// check length again
				if(strlen(ret_szFilePath) + strlen(entry->d_name) > MAX_RET_PATHLEN){
					strcpy(ret_szFilePath, "");
					return 0;
				}

				strcat(ret_szFilePath, entry->d_name);
				nPathLen = strlen(ret_szFilePath);

				printf("file \"%s\" found!\r\n", ret_szFilePath);
				break;
			}else{
				continue;
			}
		}
	}

	// very important
	//chdir("..");
	chdir(szCurWorkPath);
	closedir(dir);
	return nPathLen;
}
#else
int GetFilePath(const char* szRootPath, const char* szFileName, const int nDepth, char* ret_szFilePath)
{
	return 0;
}
#endif

unsigned long Align4Byte(unsigned long nLen)
{
	return ((nLen % 4) ? (((nLen >> 2) + 1) << 2) : nLen);
}

unsigned long GetFileSizeEx(const char* pathFile)
{
	// get font file size
	struct stat stStatBuf;
	if(stat(pathFile, &stStatBuf) < 0){
		perror("get file size");
		return 0;
	}
	return (unsigned long)stStatBuf.st_size;
}

unsigned short GetStreamCRC16(const unsigned char* pFile, int nFileSize)
{
	// init data all bit 1
	unsigned short fcs = 0xffff;
	unsigned char* pData = (unsigned char*)pFile;

	// CRC-ITU table
	const unsigned short u16CRC16Table[] =
	{
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
	};

	while(nFileSize > 0)
	{
		fcs = (fcs >> 8) ^ u16CRC16Table[(fcs ^ *pData) & 0xff];
		nFileSize--;
		pData++;
	}

	return ~fcs;

}

time_t my_mktime(int year, int mon, int day, int hour, int min, int sec)
{
	struct tm now_tm;
	memset(&now_tm, 0, sizeof(struct tm));
	now_tm.tm_sec = sec%60;
	now_tm.tm_min = min%60;
	now_tm.tm_hour = hour%24;
	now_tm.tm_mday = (day-1)%32 + 1;
	now_tm.tm_mon = (mon-1)%12;
	now_tm.tm_year = year - 1900;

	now_tm.tm_isdst = 0;
#ifndef MINGW
	now_tm.tm_gmtoff = 0;
	now_tm.tm_zone = "CST";
#endif
	return mktime(&now_tm);
}

#ifndef MINGW
int check_nic(char *nic)
{
	int ret = -1;
	struct ifreq ifr;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);

	strcpy(ifr.ifr_name, nic);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		ret = -2;
	}
	else
	{
		if(ifr.ifr_flags & IFF_RUNNING)
		{
			ret = 0;  // 网卡已插上网线
		}
		else
		{
			ret = -1;
		}
	}
	close(skfd);
	return ret;
}
#else
int check_nic(char *nic){return 1;}
#endif

int chk_value_int(char* _str, int _maxvalue, int* _ret)
{
	int ret = atoi(_str);
	if(_maxvalue != -1)
	{
		if(ret > _maxvalue)
		{
			return -1;
		}
	}
	if(_ret != NULL)
	{
		*_ret = ret;
	}
	return 0;
}

int chk_value_string(char* _str, int _maxvalue, char** _ret)
{
	if(_maxvalue != -1)
	{
		if((int)(strlen(_str)) >= _maxvalue)
		{
			return -1;
		}
	}
	if(_ret != NULL)
	{
		*_ret = _str;
	}
	return 0;
}

int chk_value_enum(char* _str, int _maxvalue, int* _ret)
{
	int ret_chk = chk_value_int(_str, _maxvalue - 1, _ret);
	if(*_ret == -1)
	{
		ret_chk = -1;
	}
	return ret_chk;
}

int chk_value_bitand(char* _str, int _maxvalue, int* _ret)
{
	return chk_value_int(_str, _maxvalue, _ret);
}

int chk_value_ip(char* _ip, unsigned char* _ret_ip)
{
	int ret;
	int tmp[IP_ADDR_LEN];
	ret = sscanf(_ip, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
	if(ret == IP_ADDR_LEN)
	{
		int i;
		for(i = 0; i < IP_ADDR_LEN; i++)
		{
			_ret_ip[i] = tmp[i] & 0xff;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

int chk_value_mac(char* _mac, unsigned char* _ret_mac)
{
	int ret;
	int tmp[MAC_ADDR_LEN];
	ret = sscanf(_mac, "%x:%x:%x:%x:%x:%x", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
	if(ret == MAC_ADDR_LEN)
	{
		int i;
		for(i = 0; i < MAC_ADDR_LEN; i++)
		{
			_ret_mac[i] = tmp[i] & 0xff;
		}
		_ret_mac[0] = _ret_mac[0] & 0xfc;
		return 0;
	}
	else
	{
		return -1;
	}
}
int chk_value_chnindex(char* _str)
{
	return 0;
}

int chk_value_time(char* _str, int* _hour, int* _min, int* _sec)
{
	int ret;
	int hour, min, sec;
	ret = sscanf(_str, "%d:%d:%d", &hour, &min, &sec);
	if(ret == 3)
	{
		if(_hour != NULL)
		{
			*_hour = hour;
		}
		if(_min != NULL)
		{
			*_min = min;
		}
		if(_sec != NULL)
		{
			*_sec = sec;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

int chk_value_date(char* _str, int* _year, int* _month, int* _day)
{
	int ret;
	int year, month, day;
	ret = sscanf(_str, "%d-%d-%d", &year, &month, &day);
	if(ret == 3)
	{
		if(_year != NULL)
		{
			*_year = year;
		}
		if(_month != NULL)
		{
			*_month = month;
		}
		if(_day != NULL)
		{
			*_day = day;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

// http://blog.hjenglish.com/codeworm/archive/2007/02/08/617082.html
unsigned short do_crc16(unsigned short reg_init, unsigned char *message, unsigned int len)
{
	const unsigned short crc16_ccitt_table[] =
	{
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
	};
	unsigned short crc_reg = reg_init;
	while (len--){
		crc_reg = (crc_reg >> 8) ^ crc16_ccitt_table[(crc_reg ^ *message++) & 0xff];
	}
	return crc_reg;
}

int system_reboot(int type)
{
	int rlt = 0;
	
	printf("Reboot in application!\n");
	#ifdef MINGW
		rlt = system("reboot");
	#else
		rlt = reboot(RB_AUTOBOOT);
	#endif
	return rlt;
}


void u64_set_bit(unsigned long long* value, int bit)
{
	unsigned long llhigh = 0;
	unsigned long lllow = 0;
	
	lllow = *value & 0xFFFFFFFFUL;
	llhigh = *value / 0x100000000ULL;
	if(bit < 32)
		lllow |= (1 << bit);
	else
		llhigh |= (1 << (bit -32));
	*value = llhigh * 0x100000000ULL + lllow;
}

void u64_clear_bit(unsigned long long* value, int bit)
{
	unsigned long llhigh = 0;
	unsigned long lllow = 0;
	
	lllow = *value & 0xFFFFFFFFUL;
	llhigh = *value / 0x100000000ULL;
	if(bit < 32)
		lllow &= ~(1 << bit);
	else
		llhigh &= ~(1 << (bit -32));
	*value = llhigh * 0x100000000ULL + lllow;
}


int u64_get_bit(unsigned long long value, int bit)
{
	unsigned long llhigh = 0;
	unsigned long lllow = 0;
	int rlt = 0;
	
	lllow = value & 0xFFFFFFFFUL;
	llhigh = value / 0x100000000ULL;
	if(bit < 32)
		rlt = lllow & (1 << bit);
	else
		rlt = llhigh &(1 << (bit - 32));
	if(0 == rlt)
		return 0;
	else
		return 1;	
}

unsigned long long u64_xor(unsigned long long value1, unsigned long long value2)
{
	unsigned long llhigh1 = 0, lllow1 = 0;
	unsigned long llhigh2 = 0, lllow2 = 0;
	unsigned long rethigh = 0, retlow = 0;
	unsigned long long rlt = 0;
	
	lllow1 = value1 & 0xFFFFFFFFUL;
	llhigh1 = value1 / 0x100000000ULL;
	lllow2 = value2 & 0xFFFFFFFFUL;
	llhigh2 = value2 / 0x100000000ULL;
	retlow = lllow1 ^ lllow2;
	rethigh = llhigh1 ^ llhigh2;
	rlt = rethigh * 0x100000000ULL + retlow;

	return rlt;
}


#endif	//__COMMON_C__

typedef void (*sighandler_t)(int);
#define MAXLINE 1024
int pox_system(const char *cmd_line)
{
#if 0
	int ret = 0;	
	sighandler_t old_handler;
	
	old_handler = signal(SIGCHLD, SIG_DFL);
	ret = system(cmd_line);
	signal(SIGCHLD, old_handler);
	
	return ret;
#else
	char result_buf[MAXLINE], command[MAXLINE];
	int rc = 0; // 用于接收命令返回值
	FILE *fp;

	/*将要执行的命令写入buf*/
	sprintf(command, "%s", cmd_line);

	/*执行预先设定的命令，并读出该命令的标准输出*/
	fp = popen(command, "r");
	if(NULL == fp)
	{
		printf("pox_system:popen(%s) fail!!!\r\n", command);
		return -1;
	}
	while(fgets(result_buf, sizeof(result_buf), fp) != NULL)
	{
		/*为了下面输出好看些，把命令返回的换行符去掉*/
		if('\n' == result_buf[strlen(result_buf)-1])
		{
			result_buf[strlen(result_buf)-1] = '\0';
		}
		printf("%s\r\n",result_buf);
	}

	/*等待命令执行完毕并关闭管道及文件指针*/
	rc = pclose(fp);
	if(-1 == rc)
	{
		printf("pox_system:pclose(%s) fail!!!\r\n", command);
		return -2;
	}
	else
	{
		printf("pox_system:command=%s,child status=%d,return=%d\r\n", command, rc, WEXITSTATUS(rc));
	}

	return 0;
#endif
}


