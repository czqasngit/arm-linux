#include "beep.h"
#include "gpio.h"
#include "const.h"

void Beep_Init()
{
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, CONST_GPIO_OUTPUT_PAD_VALUE);
    GPIO_Init(GPIO5, 1, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 0});
}

void Beep_On()
{
    GPIO_SetValue(GPIO5, 1, 0);
}
void Beep_Off()
{
    GPIO_SetValue(GPIO5, 1, 1);
}