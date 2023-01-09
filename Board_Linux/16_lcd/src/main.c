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
    lcd_device_info.foreColor = 0x00FF0000;
    lcd_device_info.backColor = 0x00FFFFFF;
    lcd_show_string(50, 50, 260, 32, 32, "Hello I.MX6ULL");
    lcd_show_string(50, 90, 260, 24, 24, "ARM ON Linux");
    lcd_show_string(50, 130, 260, 24, 24, "Write Using C Language");
    while (1) {
        // LCD_DrawPoint(0, 0, 0x00FF0000);
        // LCD_DrawPoint(799, 0, 0x00FF0000);
        // LCD_DrawPoint(0, 479, 0x00FF0000);
        // LCD_DrawPoint(799, 479, 0x00FF0000);
        // HightPrecisionDelayMS(5000);
        // printf("Left Top %#x \r\n", LCD_GetPointColor(0, 0));
        // printf("Right Top %#x \r\n", LCD_GetPointColor(799, 0));
        // printf("Left Bottom %#x \r\n", LCD_GetPointColor(0, 479));
        // printf("Right Bottom %#x \r\n", LCD_GetPointColor(799, 479));
        Led_On();
        HightPrecisionDelayMS(300);
        Led_Off();
        HightPrecisionDelayMS(300);
    }

    return 0;
}