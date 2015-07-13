
#ifndef __OEM_CONF_H__
#define __OEM_CONF_H__

#include "ui_sel.h"

#define _OEM_DEFAULT

// language use
#define LANGUAGE_ENGLISH "English"
#define LANGUAGE_CHINESE_MAINLAND "Chinese_Mainland"
#define LANGUAGE_CHINESE_HONGKONG "Chinese_Hongkong"
#define LANGUAGE_CHINESE_TAIWAN "Chinese_Taiwan"
#define LANGUAGE_KOREAN "Korean"
#define LANGUAGE_JAPANESE "Japanese"
#define LANGUAGE_HEBREW "Hebrew"
#define LANGUAGE_ARABIC "Arabic"
#define LANGUAGE_PERSIAN "Persian"
#define LANGUAGE_PORTUGUESE "Portuguese"
#define LANGUAGE_SPANISH "Spanish"
#define LANGUAGE_HUNGARIAN "Hungarian"
#define LANGUAGE_RUSSIAN "Russian"
#define LANGUAGE_FRENCH "French"
#define LANGUAGE_GERMAN "German"
#define LANGUAGE_THAI "Thai"
#define LANGUAGE_VIETNAMESE "Vietnamese"
#define LANGUAGE_TURKISH "Turkish"
#define LANGUAGE_ITALIAN "Italian"

#define DEVICE_NAME "DVR"

#if defined(JUAN_NEW_UI)
#if   defined(_EXT_HDDVR)
#if defined(_JA2104)
#define DEVICE_MODEL "R5104-AHD"
#endif
#if defined(_JA2108)
#define DEVICE_MODEL "R5108-AHD"
#endif
#if defined(_JA5208)
#define DEVICE_MODEL "R5208-AHD"
#endif
#if defined(_JA5204)
#define DEVICE_MODEL "R5204-AHD"
#endif
#if defined(_JA2116)
#define DEVICE_MODEL "R5116-AHD"
#endif
#elif defined(_JA2104)
	#if   defined(_HI3515A_EXT)
		#ifdef _HI3520D_RAM_LITE
			#define DEVICE_MODEL "JA-R6104"
		#else
			#define DEVICE_MODEL "JA7204S-2"
		#endif
	#elif defined(_HI3520D_EXT)
		#define DEVICE_MODEL "JA-R7204W-S1"
	#else
		#define DEVICE_MODEL "JA7204D"
	#endif
#elif defined(_JA2106)
	#ifdef _HI3520D_EXT
		#ifdef _HI3520D_RAM_LITE
			#define DEVICE_MODEL "JA-R6108"
		#else
			#define DEVICE_MODEL "JA7808S-2"
		#endif
	#else
		#define DEVICE_MODEL "JA7208D"
	#endif
#elif defined(_JA2108)
#define DEVICE_MODEL "JA-R7208W-S2"
#elif defined(_JA2116)
  #define DEVICE_MODEL "JA-R6116"
#elif defined(_JA2116D)
#define DEVICE_MODEL "JA8016"
#elif defined(_JA2124)
  #define DEVICE_MODEL "JA-R7224-S2"
#elif defined(_JA2132)
  #define DEVICE_MODEL "JA-R7232-S2"
#elif defined(_JA3116TW)
  #define DEVICE_MODEL "JA-R7216W-S2"
#else
  #error "Unknow OEM Defination!!"
#endif

#else

#if defined(_JA5208)
	#define DEVICE_MODEL "R5208-AHD"
#elif defined(_JA5204)
	#define DEVICE_MODEL "R5204-AHD"
#elif defined(_JA2104)
	#if   defined(_HI3515A_EXT)
		#ifdef _HI3520D_RAM_LITE
			#define DEVICE_MODEL "1004P"
		#else
			#define DEVICE_MODEL "JA7204S-2"
		#endif
	#elif defined(_HI3520D_EXT)
		#define DEVICE_MODEL "JA7804S-2"
	#else
		#define DEVICE_MODEL "JA7204D"
	#endif
#elif defined(_JA2106)
	#ifdef _HI3520D_EXT
		#ifdef _HI3520D_RAM_LITE
			#define DEVICE_MODEL "1008P"
		#else
			#define DEVICE_MODEL "JA7808S-2"
		#endif
	#else
		#define DEVICE_MODEL "JA7208D"
	#endif
#elif defined(_JA2108)
#define DEVICE_MODEL "JA2108"
#elif defined(_JA2116)
  #define DEVICE_MODEL "R7816"
#elif defined(_JA2116D)
#define DEVICE_MODEL "JA8016"
#elif defined(_JA2124)
  #define DEVICE_MODEL "JA7224D"
#elif defined(_JA2132)
  #define DEVICE_MODEL "JA7232D"
#elif defined(_JA3116TW)
  #define DEVICE_MODEL "R8116B"
#else
  #error "Unknow OEM Defination!!"
#endif

#endif

// default configuration
#define DEFAULT_LANGUAGE (LANGUAGE_CHINESE_MAINLAND)
#define DEFAULT_STANDARD "PAL"

#endif //__OEM_CONF_H__

