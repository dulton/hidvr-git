
#include "app_overlay.h"
#include "conf.h"
#include "environment.h"
#include "sdk/sdk_api.h"
#include "app_debug.h"
#include "calendar.h"

extern Sysenv* g_pstSysEnv;

#define APP_OVERLAY_CLOCK_NAME "clock"
#define APP_OVERLAY_TITLE_NAME "title"

int APP_OVERLAY_create_title(int vin, int stream, int x, int y, OVERLAY_FONT_SIZE_t font_size)
{
	if(NULL == sdk_enc->get_overlay_canvas(vin, stream, APP_OVERLAY_TITLE_NAME)){
		SDK_ENC_VIDEO_OVERLAY_CANVAS_t * canvas_title = NULL;
		canvas_title = sdk_enc->create_overlay_canvas(font_size * 16, font_size);
		if(NULL != canvas_title){
			char stream_title[32] = {""};
			g_pstSysEnv->GetTitle(g_pstSysEnv, vin, stream_title);
			OVERLAY_put_text(canvas_title, 0, 0, font_size, stream_title, 0);
			sdk_enc->create_overlay(vin, stream, APP_OVERLAY_TITLE_NAME, x, y, canvas_title->width, canvas_title->height, canvas_title);
			return 0;
		}
	}
	return -1;
}

static SDK_ENC_VIDEO_OVERLAY_CANVAS_t *_canvas_clock_font_16 = NULL, *_canvas_clock_font_20 = NULL;
int APP_OVERLAY_create_clock(int vin, int stream, int x, int y, OVERLAY_FONT_SIZE_t font_size)
{
	if(NULL == sdk_enc->get_overlay_canvas(vin, stream, APP_OVERLAY_CLOCK_NAME)){
		SDK_ENC_VIDEO_OVERLAY_CANVAS_t * canvas_clock = NULL;
		// check the size
		if(16 == font_size){
			if(!_canvas_clock_font_16){
				_canvas_clock_font_16 = sdk_enc->create_overlay_canvas(font_size * 20, font_size);
			}
			canvas_clock = _canvas_clock_font_16;
		}else if(20 == font_size){
			if(!_canvas_clock_font_20){
				_canvas_clock_font_20 = sdk_enc->create_overlay_canvas(font_size * 20, font_size);
			}
			canvas_clock = _canvas_clock_font_20;
		}else{
			return -1;
		}

		sdk_enc->create_overlay(vin, stream, APP_OVERLAY_CLOCK_NAME, x, y, canvas_clock->width, canvas_clock->height, canvas_clock);
		return 0;
	}
	return -1;
}

int APP_OVERLAY_release_title(int vin, int stream)
{
	if(NULL != sdk_enc->get_overlay_canvas(vin, stream, APP_OVERLAY_TITLE_NAME)){
		sdk_enc->release_overlay(vin, stream, APP_OVERLAY_TITLE_NAME);
		return 0;
	}
	return -1;
}

int APP_OVERLAY_release_clock(int vin, int stream)
{
	if(NULL != sdk_enc->get_overlay_canvas(vin, stream, APP_OVERLAY_CLOCK_NAME)){
		sdk_enc->release_overlay(vin, stream, APP_OVERLAY_CLOCK_NAME);
		return 0;
	}
	return -1;
}

int APP_OVERLAY_update_title(int vin, int stream, size_t width, size_t height)
{
	sdk_enc->update_overlay(vin, stream, APP_OVERLAY_TITLE_NAME);
	return 0;
}

int APP_OVERLAY_update_clock(int vin, int stream, size_t width, size_t height)
{
	sdk_enc->update_overlay(vin, stream, APP_OVERLAY_CLOCK_NAME);
	return 0;
}

int APP_OVERLAY_show_title(int vin, int stream, bool show)
{
	sdk_enc->show_overlay(vin, stream, APP_OVERLAY_TITLE_NAME, show);
	return 0;
}

int APP_OVERLAY_show_clock(int vin, int stream, bool show)
{
	sdk_enc->show_overlay(vin, stream, APP_OVERLAY_CLOCK_NAME, show);
	return 0;
}

void APP_OVERLAY_task() // per second
{
	int i = 0, ii = 0;
	char clock_text[128] = {""};
	time_t t_now = 0;
	struct tm tm_now = {0};
	size_t text_width = 0, text_height = 0;
	char* weekday_map_chs[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六",};
	char* weekday_map_eng[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT",};
	char* weekday_map_per[] = {"Yek","Dos","Ses","Cha","Pan","Jom","Sha"};
	int hour = 0;
	const char* date_format_yyyymmdd = "%04d/%02d/%02d";
	const char* date_format_mmddyyyy = "%02d/%02d/%04d";
	const char* date_format_ddmmyyyy = "%02d/%02d/%04d";
	const char* time_symbol_prefix = "";
	const char* time_symbol_suffix = "";
	const char* weekday = "";
	const char * const time_lable[]={" AM"," PM"};
	TIME_FORMAT time_fmt=g_pstSysEnv->GetSysTimeFmt(g_pstSysEnv);

	// all the clock overlay is reference to these 2 canvas
	// erase the canvas firstly
	if(_canvas_clock_font_20){
		_canvas_clock_font_20->erase_rect(_canvas_clock_font_20, 0, 0, 0, 0);
	}
	if(_canvas_clock_font_16){
		_canvas_clock_font_16->erase_rect(_canvas_clock_font_16, 0, 0, 0, 0);
	}

	time(&t_now);
	localtime_r(&t_now, &tm_now);
	hour = tm_now.tm_hour % 12;

	switch(g_pstSysEnv->GetLang(g_pstSysEnv)){
		case WORLDWIDE_CHINESE_MAINLAND:
			if(hour < 6){
				time_symbol_prefix = "凌晨";
			}else if(hour < 12){
				time_symbol_prefix = "上午";
			}else if(hour < 18){
				time_symbol_prefix = "下午";
			}else if(hour < 20){
				time_symbol_prefix = "傍晚";
			}else{
				time_symbol_prefix = "晚上";
			}
			weekday = weekday_map_chs[tm_now.tm_wday];
			date_format_yyyymmdd = "%04d年%02d月%02d日";
			date_format_mmddyyyy = "%02d月%02d日%04d年";
			date_format_ddmmyyyy = "%02d日%02d月%04d年";
			break;

		default:
			time_symbol_suffix = hour < 12 ? "a.m." : "p.m.";
			if(g_pstSysEnv->IsPersianCalendar(g_pstSysEnv)){
				weekday = weekday_map_per[tm_now.tm_wday];	//display persian weekdays
			}else{
				weekday = weekday_map_eng[tm_now.tm_wday];
			}
			date_format_yyyymmdd = "%04d/%02d/%02d";
			date_format_mmddyyyy = "%02d/%02d/%04d";
			date_format_ddmmyyyy = "%02d/%02d/%04d";
			break;
	}

	// FIXME:
	hour = tm_now.tm_hour;
	time_symbol_prefix = "";
	time_symbol_suffix = "";

	// the date
	SDATE cdate;
	cdate.year = tm_now.tm_year + 1900;
	cdate.month =  tm_now.tm_mon + 1;
	cdate.day = tm_now.tm_mday;

	if(g_pstSysEnv->IsPersianCalendar(g_pstSysEnv)){  //伊朗历
		cdate = Solar_to_Jalaali(cdate.year,cdate.month,cdate.day);
	}

	
	switch(g_pstSysEnv->GetDateFmt(g_pstSysEnv)){
		case YYYY_MM_DD:
			snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), date_format_yyyymmdd,
				cdate.year,cdate.month,cdate.day);
			break;
		case MM_DD_YYYY:
			snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), date_format_mmddyyyy,
				cdate.month, cdate.day,cdate.year);
			break;
		case DD_MM_YYYY:
			snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), date_format_ddmmyyyy,
				cdate.day,cdate.month,cdate.year);
			break;
		default:
			// invalid format
			return;
	}

	// the time
	strcat(clock_text, " ");

	// add symbol prefix
	snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", time_symbol_prefix);
	// add time clock
	if(TIMEFORMAT_X24 == time_fmt)
	{
		snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%02d:%02d:%02d",
			hour, tm_now.tm_min, tm_now.tm_sec);
//		snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", time_symbol_suffix);
	}else if(TIMEFORMAT_X12 == time_fmt)
	{
		if(hour >= 0 && hour < 12)
		{	
			if(0 == hour)
			{
				snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%02d:%02d:%02d",
				hour+12, tm_now.tm_min, tm_now.tm_sec);
			}else{
				snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%02d:%02d:%02d",
				hour, tm_now.tm_min, tm_now.tm_sec);
			}			
			snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", time_lable[0]);
		}else if(hour >= 12 && hour < 24){
			if(12 == hour)
			{
				snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%02d:%02d:%02d",
				hour, tm_now.tm_min, tm_now.tm_sec);
			}else{
				snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%02d:%02d:%02d",
				hour-12, tm_now.tm_min, tm_now.tm_sec);
			}			
			snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", time_lable[1]);
		}
		
	}

	snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", time_symbol_suffix);

	// weekday
	strcat(clock_text, " ");
	snprintf(clock_text + strlen(clock_text), sizeof(clock_text) - strlen(clock_text), "%s", weekday);

	// put text
	text_width = OVERLAY_put_text(_canvas_clock_font_20, 0, 0, OVERLAY_FONT_SIZE_20, clock_text, 0);
	text_width = OVERLAY_put_text(_canvas_clock_font_16, 0, 0, OVERLAY_FONT_SIZE_16, clock_text, 0);

	for(i = 0; i < MAX_CAM_CH; ++i){
		for(ii = 0; ii < 2; ++ii){
			sdk_enc->update_overlay(i, ii, APP_OVERLAY_CLOCK_NAME);
		}
	}
}

int APP_OVERLAY_init()
{
	OVERLAY_init();
    OVERLAY_load_font(OVERLAY_FONT_SIZE_16, "font/asc16", "font/hzk16");
    OVERLAY_load_font(OVERLAY_FONT_SIZE_20, "font/asc20", "font/hzk20");
	return 0;
}

void APP_OVERLAY_destroy()
{
	if(_canvas_clock_font_16){
		sdk_enc->release_overlay_canvas(_canvas_clock_font_16);
		_canvas_clock_font_16 = NULL;
	}
	if(_canvas_clock_font_20){
		sdk_enc->release_overlay_canvas(_canvas_clock_font_20);
		_canvas_clock_font_20 = NULL;
	}
}


