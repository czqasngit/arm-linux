#define MUX_CTL_PAD_GPIO1_IO03 *((volatile unsigned int *)0x020E0068)
#define SW_PAD_CTL_PAD_GPIO1_IO03 *((volatile unsigned int *)0x020E02F4)
#define GPIO01_GDIR *((volatile unsigned int *)0x0209C004)
#define GPIO01_DR *((volatile unsigned int *)0x0209C000)

void Led_Init()
{
    MUX_CTL_PAD_GPIO1_IO03 = 0x5;
    SW_PAD_CTL_PAD_GPIO1_IO03 = 0x10B0;
    GPIO01_GDIR = 1 << 3;
}

void Led_On()
{
    GPIO01_DR = 0x0;
}
void Led_Off()
{
    GPIO01_DR = 0x8;
}