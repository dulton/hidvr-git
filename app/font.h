

/*
 * 2011/02/22
 * Author: Law
 */

#ifndef __FONT_H__
#define __FONT_H__

typedef enum
{
	FONT_ASC16 = 0,
	FONT_SCH16,
	FONT_TYPE_CNT,
}FONT_TYPE;

#define FONT_PATH_PREFIX "./font/"
#define FONT_ASC16_FILE FONT_PATH_PREFIX"asc16"
#define FONT_SCH16_FILE FONT_PATH_PREFIX"hzk16"
#define FONT_FILE(a) \
	(FONT_ASC16 == a ? FONT_ASC16_FILE : FONT_SCH16_FILE)
#define FONT_HEIGHT(a) \
	(FONT_ASC16 == a ? 16 : (FONT_SCH16 == a ? 16 : 16))
#define FONT_WIDTH(a) \
	(FONT_ASC16 == a ? 8 : (FONT_SCH16 == a ? 16 : 16))

extern int FONT_Init();
extern int FONT_Destroy();
extern unsigned long FONT_GetFont(FONT_TYPE enFont, unsigned char** ret_pData);
extern unsigned int FONT_GetUnitHeight(FONT_TYPE enFont);
extern unsigned int FONT_GetUnitWidth(FONT_TYPE enFont);

extern int FONT_PutText16(void* pBuffer, int nWidth, int nHeight, int nPosX, int nPosY, const char* const szText,
	unsigned int const nFgColor, unsigned int const nBgColor, unsigned int const nAlphaMask);
extern int FONT_PutText32(void* pBuffer, int nWidth, int nHeight, int nPosX, int nPosY, const char* const szText,
	unsigned int const nFgColor, unsigned int const nBgColor, unsigned int const nAlphaMask);

#endif //__FONT_H__

