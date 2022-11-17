#include "delay.h"

void delay_ms(volatile unsigned int n)
{
    /// 指令空转n个来达到延时的目的
    while (n--)
    {
    }
}
void Delay(volatile unsigned int n)
{
    while (n--)
    {
        delay_ms(0x7FF);
    }
}