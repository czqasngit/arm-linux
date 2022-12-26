#include "key0.h"
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"
#include "delay.h"
#include "interrupt.h"
#include "beep.h"
#include "led.h"
#include "delay.h"

void Key0_Init()
{
    // 将UART1_CTS_B利用为GPIO01_IO18
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);
    // 配置电气属性
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, CONST_GPIO_INPUT_PAD_VALUE);
    // 配置GPIO方向以及默认值
    GPIO_Init(GPIO1, 18, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 0, GPIO_INTERRUPT_MODE_FALLING_EDGE});
    /// 使用GPIO1的IO18对应的IRQ中断ID(GPIO1_Combined_16_31_IRQn)
    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);
    /// 注册对应IRQ中断号的中断服务函数
    Interrupt_Irq_Handler_Register(GPIO1_Combined_16_31_IRQn, (Interrupt_Irq_Handler)Key0_Interrupt_Irq_Handler, NULL);
    /// 使用GPIO01_IO18中断
    GPIO_Enable_Interrupt(GPIO1, 18);
}

void Key0_Interrupt_Irq_Handler(unsigned int gicciar, void *context)
{
    static int state = 0;
    /// 中断服务函数要求快进快出,这里没有定时器
    /// 为了处理抖动暂时使用Delay来解决
    /// 以后使用定时器来处理
    Delay(10);
    if (GPIO_ReadValue(GPIO1, 18) == 0)
    {
        if (state == 0)
        {
            state = 1;
        }
        else
        {
            state = 0;
        }
        Beep_Set(state);
        Led_Set(state);
    }
    /// 中断处理完成后,清楚中断标志位
    GPIO_Clean_Interrupt_Flag(GPIO1, 18);
}