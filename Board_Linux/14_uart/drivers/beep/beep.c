#include "beep.h"
#include "gpio.h"
#include "const.h"

void Beep_Init()
{
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, CONST_GPIO_OUTPUT_PAD_VALUE);
    GPIO_Init(GPIO5, 1, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});
}

void Beep_On()
{
    GPIO_SetValue(GPIO5, 1, 0);
}
void Beep_Off()
{
    GPIO_SetValue(GPIO5, 1, 1);
}
void Beep_Toggle()
{
    if (GPIO_ReadValue(GPIO5, 1) == 0)
    {
        Beep_Off();
    }
    else
    {
        Beep_On();
    }
}
void Beep_Set(char value)
{
    GPIO_SetValue(GPIO5, 1, value);
}