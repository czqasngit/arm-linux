#include <stdio.h>

#define CCM_CCGR1 *((volatile unsigned int *)0x020C406C)
// SW_MUX_CTL_PAD_GPIO1_IO03
#define MUX_CTL_PAD_GPIO1_IO03 *((volatile unsigned int *)0x020E0068)
// SW_PAD_CTL_PAD_GPIO1_IO03
#define SW_PAD_CTL_PAD_GPIO1_IO03 *((volatile unsigned int *)0x020E02F4)
#define GPIO01_GDIR *((volatile unsigned int *)0x0209C004)
#define GPIO01_DR *((volatile unsigned int *)0x0209C000)

void delay_ms(volatile unsigned int n)
{
    while (n--)
    {
    }
}
void delay(volatile unsigned int n)
{
    while (n--)
    {
        delay_ms(0x7FF);
    }
}

int main()
{
    /// enable CCM_CCGR1
    CCM_CCGR1 = 0xFFFFFFFF;
    /// mux GPIO01_IO03
    MUX_CTL_PAD_GPIO1_IO03 = 0x5; /// 10011
    SW_PAD_CTL_PAD_GPIO1_IO03 = 0x10B0;
    GPIO01_GDIR = 0x08;
    whilte(1)
    {
        delay(500);
        GPIO01_DR = 0x0;
        delay(500);
        GPIO01_DR = 0x8;
    }
    return 0;
}