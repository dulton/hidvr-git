#ifndef __ENVFIX_H__
#define __ENVFIX_H__

#include "environment.h"

extern Sysenv* g_pstSysEnv;
#define VIDEO_IS_PAL() (VOUT_PAL == g_pstSysEnv->GetStandard(g_pstSysEnv))
#if   defined(_EXT_EFFIO)
#define VIDEO_IS_960() (ENV_TRUE == IsEffioSupport())
#else
#define VIDEO_IS_960() (0 == 1)
#endif

#endif //__ENVFIX_H__
