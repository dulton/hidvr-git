

#ifndef __JA_DEFINE_H__
#define __JA_DEFINE_H__
	
#ifdef __cplusplus
extern "C" {
#endif

#include "ja_type.h"


#define JA_MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define JA_MIN2(a, b) (((a) > (b)) ? (b) : (a))
#define JA_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/* string array copy */
#define JA_SAFE_STRNCPY(_dst, _src, _def) \
	JA_STMT_START { \
		if (_dst) { \
			memset(_dst, 0, sizeof(_dst)); \
			if (_src) { \
				strncpy(_dst, _src, sizeof(_dst) - 1); \
			} else { \
				if (_def) strncpy(_dst, _def, sizeof(_dst) - 1); \
			} \
		} \
	}JA_STMT_END

/* string pointer copy */
#define JA_SAFE_STRNCPY2(_dst, _size, _src, _def) \
	JA_STMT_START { \
		if (_dst) { \
			memset(_dst, 0, _size); \
			if (_src) { \
				strncpy(_dst, _src, (_size) - 1); \
			} else { \
				if (_def) strncpy(_dst, _def, (_size) - 1); \
			} \
		} \
	}JA_STMT_END

#define JA_SAFE_DIVISION(_denominator, _numerator, _err_default) (((_numerator) == 0) ? (_err_default) : ((_denominator)/(_numerator)))
	

#ifdef __cplusplus
}
#endif
#endif /* __JA_DEFINE_H__ */


