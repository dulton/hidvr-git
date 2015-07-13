#ifndef __Ja_calendar__   
#define __Ja_calendar__   

#ifdef __cplusplus
extern "C" {
#endif 



typedef struct{
	int h;
	int m;
	int s;
}STIME;


typedef struct{
	int year;
	int month;
	int day;
}SDATE;

		
SDATE Solar_to_Jalaali(int y,int m,int d);  //太阳历->伊朗太阳历（波斯历）
SDATE Jalaali_to_Solar(int y,int m,int d);  //伊朗历->太阳历

int Jalaali_to_Weekday(int y,int m,int d);	//伊朗历求星期
int Solar_to_Weekday(int y,int m,int d);	//太阳历求星期

int leap_Jalaali(int year);
int leap_Solar(int year);

int Solar_month_day(int year,int month);
int Jalaali_month_day(int y,int m);

#ifdef __cplusplus
}
#endif
#endif