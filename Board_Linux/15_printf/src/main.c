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
    while (1)
    {
        int a, b;
        printf("请输入两个整数,使用空格分开: \r\n");
        scanf("%d %d", &a, &b);
        printf("\r\n");
        printf("%d + %d = %d, 16进制是: %#x \r\n", a, b, a + b, a + b);
    }

    return 0;
}