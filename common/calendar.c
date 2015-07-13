
///////////////////////////////////////////
//Solar calendar:公历
//Julian：儒略历
//Jalaali：伊朗太阳历（波斯历）
//
///////////////////////////////////////////
#include <stdio.h>
#include "calendar.h"
#define Solar_EPOCH  1721425.5  
#define Jalaali_EPOCH 	 1948320.5


int jwday(double j);
double Solar_to_Julian(int year,int  month,int  day);
float Jalaali_to_Julian(int year,int month,int day);
SDATE Julian_to_Jalaali(double jd);
SDATE Julian_to_Solar(double jd);


double mod(double, double);
int Mceil(double d);
int Mfloor(double d);
int Mmin(int a,int b);



SDATE Solar_to_Jalaali(int y,int m,int d) 
{
	double jd = Solar_to_Julian(y, m, d);
	SDATE Jadate = Julian_to_Jalaali(jd);
	return Jadate;
}



SDATE Jalaali_to_Solar(int y,int m,int d)
{
	double jd = Jalaali_to_Julian (y, m, d);
	SDATE Sodate = Julian_to_Solar(jd);
	return Sodate;
}


int Jalaali_to_Weekday(int y,int m,int d)
{
	return jwday((Jalaali_to_Julian(y,m,d)));
}


int Solar_to_Weekday(int y,int m,int d)
{
	return jwday((Solar_to_Julian(y,m,d)));
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//-----------------------------------Solar<->Julian↓↓↓↓↓↓↓↓↓

double Solar_to_Julian(int year,int  month,int  day)
{
    double jd = (Solar_EPOCH - 1) +
           (365 * (year - 1)) +
           Mfloor((year - 1) / 4) +
           (-Mfloor((year - 1) / 100)) +
           Mfloor((year - 1) / 400) +
           Mfloor((((367 * month) - 362) / 12) +
           ((month <= 2) ? 0 : (leap_Solar(year) ? -1 : -2)) 
		   +day);		   
	return jd;
}


SDATE Julian_to_Solar(double jd) {
    SDATE Sdate;
	int year,month,day;
	
	double wjd, depoch, quadricent, dqc, cent, dcent, quad, dquad,yindex, dyindex, yearday, leapadj;

    wjd = Mfloor(jd - 0.5) + 0.5;
    depoch = wjd - Solar_EPOCH;
    quadricent = Mfloor(depoch / 146097);
    dqc = mod(depoch, 146097);
    cent = Mfloor(dqc / 36524);
    dcent = mod(dqc, 36524);
    quad = Mfloor(dcent / 1461);
    dquad = mod(dcent, 1461);
    yindex = Mfloor(dquad / 365);
    year = (quadricent * 400) + (cent * 100) + (quad * 4) + yindex;
	
    if (!((cent == 4) || (yindex == 4))) {
        year++;
    }
	
    yearday = wjd - Solar_to_Julian(year, 1, 1);
    leapadj = ((wjd < Solar_to_Julian(year, 3, 1)) ? 0 :
               (leap_Solar(year) ? 1 : 2));
    month = Mfloor((((yearday + leapadj) * 12) + 373) / 367);
    day = (wjd - Solar_to_Julian(year, month, 1)) + 1;
	
	Sdate.year = year;
	Sdate.month = month;
	Sdate.day=day;
    return Sdate;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//-----------------------------------伊朗历↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

SDATE Julian_to_Jalaali(double jd)
{
    int year, month, day;
	double depoch, cycle, cyear, ycycle, aux1, aux2, yday;
	SDATE Jadate;
	
    jd = Mfloor(jd) + 0.5;

    depoch = jd - Jalaali_to_Julian(475, 1, 1);
    cycle = Mfloor(depoch / 1029983);
    cyear = mod(depoch, 1029983);
	
    if (cyear == 1029982) {
        ycycle = 2820;
    } else {
        aux1 = Mfloor(cyear / 366);
        aux2 = mod(cyear, 366);
        ycycle = Mfloor(((2134 * aux1) + (2816 * aux2) + 2815) / 1028522) + aux1 + 1;
    }
	
    Jadate.year= year = ycycle + (2820 * cycle) + 474;
	
    if (year <= 0){
        year--;
    }
	
    yday = (jd - Jalaali_to_Julian(year, 1, 1)) + 1;
	
	Jadate.month = month =(yday <= 186) ? Mceil(yday / 31) : Mceil((yday - 6) / 30);
	
    Jadate.day = (jd - Jalaali_to_Julian(year, month, 1)) + 1;
	
    return Jadate;
}

float Jalaali_to_Julian(int year,int month,int day)
{
    double epbase, epyear;

    epbase = year - ((year >= 0) ? 474 : 473);
    epyear = 474 + mod(epbase, 2820);

    return day +((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
           Mfloor(((epyear * 682) - 110) / 2816) +
           (epyear - 1) * 365 +
           Mfloor(epbase / 2820) * 1029983 +
           (Jalaali_EPOCH - 1);
}

//-----------------------------------伊朗历↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


//-----------------------------------//weekday
int jwday(double j) 
{
    return (int)mod(Mfloor((j + 1.5)), 7);
}

////////////////////////////////////////////////////////////
int Mmin(int a,int b)
{
	if(a>=b){
		return b;
	}else{
		return a;
	}
}


int Mfloor(double d)
{
	return (int)(d);
}

int Mceil(double d){

	if(((int)(d*1000000)%1000000)>0){
		return (int)d+1;
		
	}else{
		return (int)d;
	}
}

int leap_Jalaali(int year)
{
    return ((((((year - ((year > 0) ? 474 : 473)) % 2820) + 474) + 38) * 682) % 2816) < 682;
}



int  leap_Solar(int  year)
{
    return ((year % 4) == 0) &&
            (!(((year % 100) == 0) && ((year % 400) != 0)));
}


double mod(double a,double b)
{
    return a - (b * Mfloor(a / b));
}


int Solar_month_day(int y,int m)
{
	int month_day = 0;
	
	if(m<0||m>12){
		return -1;
	}
	
	switch(m){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			month_day = 31; break;
			
		case 2:
			month_day = 28+leap_Solar(y); break;
			
		default:
			month_day = 30; break;
			
	}
	
	return month_day;
}

int Jalaali_month_day(int y,int m)
{
	int month_day = 0;
	
	if(m > 0 && m <= 6){
		month_day = 31;
		
	}else if(m >= 7 && m <= 11){
		month_day = 30;
		
	}else if(m == 12){
		month_day = 29+ leap_Jalaali(y);
			
	}else{
		month_day = -1;
		
	}		
	
	return month_day;
}

