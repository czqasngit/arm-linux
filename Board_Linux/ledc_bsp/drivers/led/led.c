
#include "imx6ul.h"

void Led_Init()
{
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10B0);
    GPIO1->GDIR |= (1 << 3);
}

void Led_On()
{
    GPIO1->DR &= ~(1 << 3);
}
void Led_Off()
{
    GPIO1->DR |= (1 << 3);
}