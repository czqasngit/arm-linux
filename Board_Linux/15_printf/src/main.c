#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "interrupt.h"
#include "key0.h"
#include "epit.h"
#include "uart.h"

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
    while (1)
    {
        // HightPrecisionDelayMS(50);
        // Led_On();
        // Beep_On();
        // HightPrecisionDelayMS(50);
        // Led_Off();
        // Beep_Off();
        Uart1_SendString("请输入一个字符: ");
        char c = Uart1_ReadChar();
        Uart1_SendString("你输入的字符是: " + c);
        Uart1_SendString("\r\n");
    }

    return 0;
}