/*
 * std.c
 *
 *  Created on: 2010-11-15
 *      Author: root
 */
#include <time.h>

#include "std.h"

struct tm xxxt;


struct tm get_now()
{
	time_t tmp = time(NULL);
	struct tm tmpp;
	localtime_r(&tmp, &tmpp);
	return tmpp;
}


