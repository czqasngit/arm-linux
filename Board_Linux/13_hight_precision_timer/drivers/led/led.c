
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"

void Led_Init()
{
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, CONST_GPIO_OUTPUT_PAD_VALUE);
    GPIO_Init(GPIO1, 3, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});
}

void Led_On()
{
    GPIO_SetValue(GPIO1, 3, 0);
}
void Led_Off()
{
    GPIO_SetValue(GPIO1, 3, 1);
}
void Led_Toggle()
{
    if (GPIO_ReadValue(GPIO1, 3) == 0)
    {
        Led_Off();
    }
    else
    {
        Led_On();
    }
}
void Led_Set(char value)
{
    GPIO_SetValue(GPIO1, 3, value);
}