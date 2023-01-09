#include "rtc.h"
#include "MCIMX6Y2.h"
#include "delay.h"
#include "stdio.h"

void RTC_Init() {
    // 设置成所有的软件都可以访问RTC寄存器
    SNVS->HPCOMR |= (1 << 31);

    RTC_DATET_TIME date_time;
    date_time.year = 2023;
    date_time.month = 1;
    date_time.day = 6;
    date_time.hour = 20;
    date_time.minute = 0;
    date_time.second = 0;
    RTC_Set(date_time);

    RTC_Enable();
}

void RTC_Enable() { 
    SNVS->LPCR |= (1 << 0);
    /// 等待设置完成
    while((SNVS->LPCR & 0x1) == 0);
}
void RTC_Disable() {  
    SNVS->LPCR &= ~(1 << 0);
    /// 等待设置完成
    while((SNVS->LPCR & 0x1) == 1);
}

void RTC_Set(RTC_DATET_TIME date_time) {
    int tmp = SNVS->LPCR;
    RTC_Disable();
    uint64_t seconds = rtc_coverdate_to_seconds(&date_time);
 	SNVS->LPSRTCMR = (unsigned int)(seconds >> 17); /* 设置高16位 */
	SNVS->LPSRTCLR = (unsigned int)(seconds << 15); /* 设置地16位 */
     if((tmp & 0x1) == 1) {
        // 在设置时已经是打开的状态,需要在设置之后打开RTC
        RTC_Enable();
    }
}
uint64_t RTC_Get() {
    uint64_t seconds = 0;
    // seconds = (((uint64_t)(SNVS->LPSRTCMR & 0x7FFF)) << 32) | (SNVS->LPSRTCLR);
    seconds = (SNVS->LPSRTCMR << 17) | (SNVS->LPSRTCLR >> 15);
    return seconds;
}

RTC_DATET_TIME RTC_GetDateTime() {
    uint64_t seconds = RTC_Get();
    RTC_DATET_TIME date_time;
    rtc_convertseconds_to_datetime(seconds, &date_time);
    return date_time;
}

/*
 * @description		: 将时间转换为秒数
 * @param - datetime: 要转换日期和时间。
 * @return 			: 转换后的秒数
 */
uint64_t rtc_coverdate_to_seconds(RTC_DATET_TIME *datetime)
{	
	unsigned short i = 0;
    uint64_t seconds = 0;
	unsigned int days = 0;
	unsigned short monthdays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
	
	for(i = 1970; i < datetime->year; i++)
	{
		days += DAYS_IN_A_YEAR; 		/* 平年，每年365天 */
		if(rtc_isleapyear(i)) days += 1;/* 闰年多加一天 		*/
	}

	days += monthdays[datetime->month];
	if(rtc_isleapyear(i) && (datetime->month >= 3)) days += 1;/* 闰年，并且当前月份大于等于3月的话加一天 */

	days += datetime->day - 1;

	seconds = days * SECONDS_IN_A_DAY + 
				datetime->hour * SECONDS_IN_A_HOUR +
				datetime->minute * SECONDS_IN_A_MINUTE +
				datetime->second;

	return seconds;	
}

void rtc_convertseconds_to_datetime(u64 seconds, RTC_DATET_TIME *datetime)
{
    u64 x;
    u64  secondsRemaining, days;
    unsigned short daysInYear;

    /* 每个月的天数       */
    unsigned char daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    secondsRemaining = seconds; /* 剩余秒数初始化 */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1; 		/* 根据秒数计算天数,加1是当前天数 */
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY; /*计算天数以后剩余的秒数 */

	/* 计算时、分、秒 */
    datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* 计算年 */
    daysInYear = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while(days > daysInYear)
    {
        /* 根据天数计算年 */
        days -= daysInYear;
        datetime->year++;

        /* 处理闰年 */
        if (!rtc_isleapyear(datetime->year))
            daysInYear = DAYS_IN_A_YEAR;
        else	/*闰年，天数加一 */
            daysInYear = DAYS_IN_A_YEAR + 1;
    }
	/*根据剩余的天数计算月份 */
    if(rtc_isleapyear(datetime->year)) /* 如果是闰年的话2月加一天 */
        daysPerMonth[2] = 29;

    for(x = 1; x <= 12; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->month = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;

}

/*
 * @description	: 判断指定年份是否为闰年，闰年条件如下:
 * @param - year: 要判断的年份
 * @return 		: 1 是闰年，0 不是闰年
 */
unsigned char rtc_isleapyear(unsigned short year)
{	
	unsigned char value=0;
	
	if(year % 400 == 0)
		value = 1;
	else 
	{
		if((year % 4 == 0) && (year % 100 != 0))
			value = 1;
		else 
			value = 0;
	}
	return value;
}