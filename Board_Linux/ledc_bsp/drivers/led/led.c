
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"

void Led_Init()
{
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, CONST_GPIO_INPUT_PAD_VALUE);
    GPIO_Init(GPIO1, 3, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 0});
}

void Led_On()
{
    GPIO_SetValue(GPIO1, 3, 0);
}
void Led_Off()
{
    GPIO_SetValue(GPIO1, 3, 1);
}