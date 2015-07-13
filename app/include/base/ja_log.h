

#ifndef __JA_LOG_H__
#define __JA_LOG_H__
	
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "ja_type.h"
#include "slog.h"

#define JA_DEBUG (0)
#define JA_SYNTAX "1;32"
#define JA_DEBUG_SYNTAX "1;34"
#define JA_TRACE(fmt, arg...) \
	JA_STMT_START{\
		const char* const bname = __FILE__;\
		printf("\033["JA_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}JA_STMT_END
	
#if (JA_DEBUG	 == 1)
#define JA_DEBUG(fmt, arg...) \
		JA_STMT_START{\
			const char* const bname = __FILE__;\
			printf("\033["JA_DEBUG_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
			printf(fmt, ##arg);\
			printf("\r\n");\
		}JA_STMT_END
#else
#define JA_DEBUG(fmt, arg...)
#endif

#define JA_ASSERT(exp, fmt, arg...) \
	JA_STMT_START{\
		if(!(exp)){\
			const char* const bname = __FILE__;\
			printf("\033["JA_SYNTAX"m[%12s:%4d]\033[0m assert(\"%s\") ", bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
			exit(1);\
		}\
	}JA_STMT_END


#define ja_return_if_fail(expr)		JA_STMT_START{			\
     if (expr) { } else       					\
       {								\
	 		JA_TRACE (#expr);				\
	 		return;							\
       };	\
	 }JA_STMT_END

#define ja_goto_if_fail(expr, where)		JA_STMT_START{			\
		  if (expr) { } else 						 \
			{								 \
				 JA_TRACE (#expr); 			 \
				 goto where;						 \
			};	 \
		  }JA_STMT_END
	 

#define ja_return_val_if_fail(expr,val)	JA_STMT_START{			\
		if (expr) { } else						\
		{								\
			JA_TRACE (#expr);				\
			return (val);							\
		};		\
	}JA_STMT_END

#define ja_hi_return_if_fail(expr)		JA_STMT_START{			\
	 if (0 == (expr)) { } else 						\
	   {								\
			JA_TRACE (#expr"; errcode: 0x%x", expr);				\
			return; 						\
	   };	\
	 }JA_STMT_END

#define ja_hi_goto_if_fail(expr, where)		JA_STMT_START{			\
		  if (0 == (expr)) { } else						 \
			{								 \
			  JA_TRACE (#expr"; errcode: 0x%x", expr);			  \
				 goto where;						 \
			};	 \
		  }JA_STMT_END
	 

#define ja_hi_return_val_if_fail(expr,val)	JA_STMT_START{			\
		if (0 == (expr)) { } else						\
		{								\
			JA_TRACE (#expr"; errcode: 0x%x", expr);				\
			return (val);							\
		};		\
	}JA_STMT_END	


#ifdef __cplusplus
}
#endif
#endif /* __JA_LOG_H__ */

