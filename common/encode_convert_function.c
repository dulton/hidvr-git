/*
 * test.c
 *
 *  Created on: 2011-11-11
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _CharCode
{
	union
	{
		unsigned char gb2312_char[2];
		unsigned short gb2312_code;
	}gb2312;
	union
	{
		unsigned char unicode_char[2];
		unsigned short unicode_code;
	}unicode;
	union
	{
		unsigned char utf8_char[4];
		unsigned int utf8_code;
	}utf8;
}CharCode;
#include "encode_convert_function.cxx"


int gb2312_to_utf8(char* _gb2312, char* _utf8)
{
	int char_count = sizeof(cc_sort_by_gb2312) / sizeof(CharCode); //3755 + 5(¿Õ°×)

	int search_gb2312(const void *a,const void *b)
	{
		return  *((unsigned short*)a) - ((CharCode*)b)->gb2312.gb2312_code;
	}


	char* ppppppgggggg = _gb2312;
	char* pu = _utf8;
	while(*ppppppgggggg != 0)
	{
		if( ((*ppppppgggggg)&0x80) == 0x80 )
		{
			CharCode* cp = bsearch(ppppppgggggg, cc_sort_by_gb2312, char_count, sizeof(CharCode), search_gb2312);
			if(cp != NULL)
			{
				*pu = cp->utf8.utf8_char[1];
				pu++;
				*pu = cp->utf8.utf8_char[2];
				pu++;
				*pu = cp->utf8.utf8_char[3];
				pu++;
				ppppppgggggg += 2;
			}
			else
			{
				*pu = ' ';
				pu++;
				ppppppgggggg++;
			}
		}
		else
		{
			*pu = *ppppppgggggg;
			pu++;
			ppppppgggggg++;
		}
	}
	return 0;
}


int utf8_to_gb2312(char* _utf8, char* _gb2312)
{
	int char_count = sizeof(cc_sort_by_utf8) / sizeof(CharCode); //3755 + 5(¿Õ°×)

	int search_utf8(const void *a,const void *b)
	{
		unsigned int utf8_code = 0;
		utf8_code |= *(((unsigned char*)a)) << 8;
		utf8_code |= *(((unsigned char*)a)+1) << 16;
		utf8_code |= *(((unsigned char*)a)+2) << 24;
		return  utf8_code - ((CharCode*)b)->utf8.utf8_code;
	}


	char* pu = _utf8;
	char* pg = _gb2312;
	while(*pu != 0)
	{
		if( ((*pu)&0x80) == 0x80 )
		{
			CharCode* cp = bsearch(pu, cc_sort_by_utf8, char_count, sizeof(CharCode), search_utf8);
			if(cp != NULL)
			{
				*pg = cp->gb2312.gb2312_char[0];
				pg++;
				*pg = cp->gb2312.gb2312_char[1];
				pg++;
				pu += 3;
			}
			else
			{
				*pg = ' ';
				pg++;
				pu++;
			}
		}
		else
		{
			*pg = *pu;
			pg++;
			pu++;
		}
	}
	return 0;
}

//int main(int argc, char** argv)
//{
//	{
//		char* gb2312_str = "ÈÃÈËÓôÃÆµÄgb2312×ªutf8¡£123";
//		char utf8_str[128];
//		memset(utf8_str, 0, 128);
//
//		gb2312_to_utf8(gb2312_str, utf8_str);
//		printf("%s\n", utf8_str);
//	}
//
//	{
//		char utf8_str[] = {0xe6, 0x88, 0x91, 0x31, 0x00};//ÎÒ1
//		char gb2312_str[128];
//		memset(gb2312_str, 0, 128);
//
//		utf8_to_gb2312(utf8_str, gb2312_str);
//		printf("%s\n", gb2312_str);
//
//	}
//
//	printf("db size = %d\n", sizeof(cc_sort_by_utf8));
//	return 0;
//}
