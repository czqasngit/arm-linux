#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "interrupt.h"
#include "key0.h"
#include "epit.h"
#include "uart.h"
#include "stdio.h"

int main(void)
{
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
    while (1) {
        HightPrecisionDelayMS(500);
    }

    return 0;
}