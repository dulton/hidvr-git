#ifndef __STDINC_WIN_HEAD_FILE__
#define __STDINC_WIN_HEAD_FILE__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <assert.h>
#include <malloc.h>
#include <WinSock2.h>
#include <IPHlpApi.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"iphlpapi.lib")

#endif