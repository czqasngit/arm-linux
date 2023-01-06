#ifndef _RTC_H_
#define _RTC_H_

#include "imx6ul.h"

#define SECONDS_IN_A_MINUTE     (60)
#define SECONDS_IN_A_HOUR       (SECONDS_IN_A_MINUTE * 60)
#define SECONDS_IN_A_DAY        (24 * SECONDS_IN_A_HOUR)
#define DAYS_IN_A_YEAR          (365)
#define YEAR_RANGE_START        (1970)
#define YEAR_RANGE_END          (2099)

/* 时间结构体 */
typedef struct _RTC_DATET_TIME {
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} RTC_DATET_TIME;

void RTC_Init();
void RTC_Enable();
void RTC_Disable();
void RTC_Set(RTC_DATET_TIME date_time);
uint64_t RTC_Get();
RTC_DATET_TIME RTC_GetDateTime();
// 判断是否是润年
unsigned char rtc_isleapyear(unsigned short year);
uint64_t rtc_coverdate_to_seconds(RTC_DATET_TIME *datetime);
void rtc_convertseconds_to_datetime(u64 seconds, RTC_DATET_TIME *datetime);
#endif