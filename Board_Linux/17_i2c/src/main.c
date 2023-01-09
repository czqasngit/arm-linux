#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "interrupt.h"
#include "key0.h"
#include "epit.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"
#include "lcdapi.h"
#include "rtc.h"

int main(void)
{
    printf("main function start \r\n");
    /// 初始化中断服务函数
    Init_Interrupt();
    Clk_Freq_Init();
    Clk_Init();
    UART1_Init();
    Led_Init();
    Beep_Init();
    // Key0_Init();
    // Epit_Init(500);
    Delay_Init();
    LCD_Init();
    RTC_Init();
    lcd_show_string(50, 50, 260, 32, 32, "Hello LCD Screen");

    while (1) {
        Led_On();
        HightPrecisionDelayMS(500);
        Led_Off();
        HightPrecisionDelayMS(500);
        RTC_DATET_TIME date_time = RTC_GetDateTime();
        printf("时间: %d-%d-%d %d:%d:%d\r\n", date_time.year, date_time.month, date_time.day, date_time.hour, date_time.minute, date_time.second);
    }

    return 0;
}