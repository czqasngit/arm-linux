#include <stdio.h>

#define CCM_CCGR1                           *((volatile unsigned int *)0x020C406C)
#define MUX_CTL_PAD_GPIO1_IO03              *((volatile unsigned int *)0x020E0068)
#define SW_PAD_CTL_PAD_GPIO1_IO03           *((volatile unsigned int *)0x020E02F4)
#define GPIO01_GDIR                         *((volatile unsigned int *)0x0209C004)
#define GPIO01_DR                           *((volatile unsigned int *)0x0209C000)


void delay_ms(volatile unsigned int n) {
    while (n--) { }
}
void delay(volatile unsigned int n) {
    while (n--) {
        delay_ms(0x7FF);
    }
}

int main(void) {

    CCM_CCGR1 = 0xFFFFFFFF;
    MUX_CTL_PAD_GPIO1_IO03 = 0x5;
    SW_PAD_CTL_PAD_GPIO1_IO03 = 0x10B0;
    GPIO01_GDIR = 1 << 3;

    while(1) {
        delay(500);
        GPIO01_DR = 0x0;
        delay(500);
        GPIO01_DR = 0x8;
    }

    return 0;
}