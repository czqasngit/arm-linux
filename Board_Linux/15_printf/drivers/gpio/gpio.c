
#include "gpio.h"

void GPIO_Init(GPIO_Type *base, int pin, GPIO_CONFIG config)
{
    if (config.direction == GPIO_DIRECTION_INPUT)
    {
        base->GDIR &= ~(1 << pin);
    }
    else
    {
        base->GDIR |= (1 << pin);
        GPIO_SetValue(base, pin, config.defaultOutputValue);
    }
    GPIO_Init_Interrupt(base, pin, config.interruptMode);
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
uint8_t GPIO_ReadValue(GPIO_Type *base, int pin)
{
    uint8_t value = (base->DR >> pin) & 0x1 /*只需要对应位的引用值,其它位都舍弃*/;
    return value;
}

void GPIO_Enable_Interrupt(GPIO_Type *base, int pin)
{
    /// GPIO_IMR寄存器每一个位使能对应的GPIO中断
    base->IMR |= (1 << pin);
}

void GPIO_Disable_Interrupt(GPIO_Type *base, int pin)
{
    /// GPIO_IMR寄存器每一个位使能对应的GPIO中断
    base->IMR &= ~(1 << pin);
}

void GPIO_Clean_Interrupt_Flag(GPIO_Type *base, int pin)
{
    /// 向指定的ISR寄存器对应pin写1表示清楚当前中断
    base->ISR |= (1 << pin);
}

void GPIO_Init_Interrupt(GPIO_Type *base, int pin, GPIO_INTERRUPT_MODE mode)
{
    /// 首先将GPIO的edge_sel寄存器对应pin位清0，如果为1则会使ICR寄存器的配置无效
    base->EDGE_SEL &= ~(1 << pin);
    /// 对应ICR的索引(按2位为一个单元)
    int icrOffset = pin;
    /// 具体的icr寄存器地址
    __IO uint32_t *p_icr = NULL;
    if (pin < 16)
    {
        p_icr = &(base->ICR1);
    }
    else
    {
        p_icr = &(base->ICR2);
        icrOffset -= 16;
    }

    switch (mode)
    {
    case GPIO_INTERRUPT_MODE_NO_INTERRUPT:
        break;
    case GPIO_INTERRUPT_MODE_LOW:
        *p_icr &= ~(3 << (2 * icrOffset));
        break;
    case GPIO_INTERRUPT_MODE_HIGH:
        *p_icr &= ~(3 << (2 * icrOffset));
        *p_icr |= 1 << (2 * icrOffset);
        break;
    case GPIO_INTERRUPT_MODE_RISING_EDGE:
        *p_icr &= ~(3 << (2 * icrOffset));
        *p_icr |= 2 << (2 * icrOffset);
        break;
    case GPIO_INTERRUPT_MODE_FALLING_EDGE:
        *p_icr &= ~(3 << (2 * icrOffset));
        *p_icr |= 3 << (2 * icrOffset);
        break;
    case GPIO_INTERRUPT_MODE_RISING_AND_RALLING_EDGE:
        *p_icr &= ~(3 << (2 * icrOffset));
        base->EDGE_SEL |= (1 << pin);
        break;
    }
}