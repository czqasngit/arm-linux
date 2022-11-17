
#include "gpio.h"

void GPIO_Init(GPIO_Type *base, int pin, GPIO_CONFIG config)
{
    if (config.direction == GPIO_DIRECTION_INPUT)
    {
        base->GDIR = ~(1 << pin);
        GPIO_SetValue(base, pin, config.defaultOutputValue);
    }
    else
    {
        base->GDIR |= (1 << pin);
    }
}
void GPIO_SetValue(GPIO_Type *base, int pin, uint8_t value)
{
    if (value == 0)
    {
        base->DR &= ~(1 << pin);
    }
    else
    {
        base->DR |= (1 << pin);
    }
}
uint8_t GPIO_RedValue(GPIO_Type *base, int pin)
{
    uint8_t value = (base->DR >> pin) & 0x1 /*只需要对应位的引用值,其它位都舍弃*/;
    return value;
}
