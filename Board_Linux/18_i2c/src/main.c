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
#include "ap3216c.h"

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
    AP3216C_Init();
    lcd_device_info.foreColor = 0x00FF0000;
    lcd_device_info.backColor = 0x00FFFFFF;
    
    lcd_show_string(50, 50, 260, 32, 32, "Hello I.MX6ULL");
    lcd_show_string(50, 90, 260, 24, 24, "ARM ON Linux");
    lcd_show_string(50, 130, 260, 24, 24, "Write Using C Language");

    char buffer[1024];
    while (1) {
        Led_On();
        HightPrecisionDelayMS(500);
        Led_Off();
        HightPrecisionDelayMS(500);
        // LCD_Clean_Screen(0x00FFFFFFFF);
        
        memcpy(buffer, 0, 1024);
        RTC_DATET_TIME date_time = RTC_GetDateTime();
        // printf("时间: %d-%02d-%02d %02d:%02d:%02d\r\n", date_time.year, date_time.month, date_time.day, date_time.hour, date_time.minute, date_time.second);
        sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d", date_time.year, date_time.month, date_time.day, date_time.hour, date_time.minute, date_time.second);
        // printf("%s \r\n", buffer);
        lcd_show_string(50, 170, 360, 24, 24, buffer);
        unsigned short lr = 0;
        unsigned short als = 0;
        unsigned short ps = 0;
        AP3216C_ReadData(&lr, &als, &ps);
        printf("lr: %d, als: %d, ps: %d \r\n");
    }




    return 0;
}