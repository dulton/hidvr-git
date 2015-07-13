

#ifndef __COMMON_H__
#define __COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif


// 循环递增递减
#define	ROUND_INCREASE(value, min, max, step)	((value + step <= max) ? (value + step) : min)
#define	ROUND_DECREASE(value, min, max, step)	((value - step >= min) ? (value - step) : max)

// 上下限递增减
#define	LIMIT_INCREASE(value, min, max, step)	((value + step <= max) ? (value + step) : max)
#define	LIMIT_DECREASE(value, min, max, step)	((value - step >= min) ? (value - step) : min)


// 位操作, 对mask置1的位操作
#define	SET_BIT(data, mask)	((data) |= (mask))
#define	CLEAR_BIT(data, mask)	((data) &= (~mask))
#define	KEEP_BIT(data, mask)	((data) &= (mask))
#define	TEST_BIT(data, mask)	(0 != KEEP_BIT(data, mask))


// 一些对齐规则
#define	ALIGN_ADDR_1K(addr)	(((addr)+ 0x3ff)& (~0x3ff))	// 1K位置对齐换算
#define	ALIGN_ADDR_4B(addr)	(((addr) + 3) & (~3))			// 4B位置对齐换算





extern int IsProgAlive(const char* program);
extern int IsDirExist(const char* stDirPath);
extern int IsFileExist(const char* szFilePath);

#define	MAX_RET_PATHLEN	(200)
extern int GetFilePath(const char* szRootPath, const char* szFileName, const int nDepth, char* ret_szFilePath);
extern unsigned long Align4Byte(unsigned long nLen);
extern unsigned long GetFileSizeEx(const char* pathFile);
extern unsigned short GetStreamCRC16(const unsigned char* stream, int nFileSize);

extern time_t my_mktime(int year, int mon, int day, int hour, int min, int sec);
extern int check_nic(char *nic);


extern int chk_value_int(char* _str, int _maxvalue, int* _ret);
extern int chk_value_string(char* _str, int _maxvalue, char** _ret);
extern int chk_value_enum(char* _str, int _maxvalue, int* _ret);
extern int chk_value_bitand(char* _str, int _maxvalue, int* _ret);
extern int chk_value_ip(char* _ip, unsigned char* _ret_ip);
extern int chk_value_mac(char* _mac, unsigned char* _ret_mac);
extern int chk_value_chnindex(char* _str);
extern int chk_value_time(char* _str, int* _hour, int* _min, int* _sec);
extern int chk_value_date(char* _str, int* _year, int* _month, int* _day);

extern unsigned short do_crc16(unsigned short reg_init, unsigned char *message, unsigned int len);

extern int system_reboot(int type);

void u64_set_bit(unsigned long long* value, int bit);
void u64_clear_bit(unsigned long long* value, int bit);
int u64_get_bit(unsigned long long value, int bit);
unsigned long long u64_xor(unsigned long long value1, unsigned long long value2);


#ifdef __cplusplus
}
#endif
#endif	//__COMMON_H__

