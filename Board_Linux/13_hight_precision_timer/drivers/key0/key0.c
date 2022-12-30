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
    Key0_Filter_Init();
}
void Key0_Filter_Init()
{
    /*
            EPIT1_CR寄存器
            bit0: enable epit(1: enable, 0: disable)
            bit1: 0: 计数器从上一次保留的数据开始减少, 1: 计数器从EPIT1_LR寄存器开始减少或从0xFFFFFFFF开始减少
            bit2: 比较中断全能,0：关闭,则EPIT不再与(EPIT_CMPR)进行比较,也就不会产品对应中断，1: 使用中断比较
            bit3: 0: 计数器初始值从0xFFFFFFFF开始减少，减少到0又从0xFFFFFFFF开始, 1: 计数器从EPIT1_LR寄存器从的数据开始减少
            bit15-4: 计数器分频器
            bit25-24: 时钟源选择,这里选择ipg clk,之前配置的外设时针源频率为66MHz
        */
    EPIT1->CR |= (1 << 1);      /// bit1
    EPIT1->CR |= (1 << 2);      /// bit2
    EPIT1->CR |= (1 << 3);      /// bit3
    EPIT1->CR &= ~(0xFFF << 4); /// bit15-4,设置成1分频
    EPIT1->CR &= ~(3 << 24);    /// bit25-24 清0
    EPIT1->CR |= (1 << 24);     /// bit25-24 选择ipg clk时钟源

    Interrupt_Irq_Handler_Register(EPIT1_IRQn, Key0_Epit1_Interrupt_Irq_Handler, NULL); /// 注册对应中断号的中断服务函数

    GIC_EnableIRQ(EPIT1_IRQn); /// enable epit1 irq中断
}
void Key0_Epit1_Interrupt_Irq_Handler(unsigned int gicciar, void *context)
{
    static int state = 0;
    if (GPIO_ReadValue(GPIO1, 18) == 0)
    {
        state != state;
        Beep_Set(state);
        Led_Set(state);
    }
    Key0_Filter_Stop();
    EPIT1->SR |= 1 << 0;
}
void Key0_Filter_Start(unsigned int duration)
{
    /// 开启之前先关闭之前的
    Key0_Filter_Stop();
    EPIT1->LR &= ~0xFFFFFFFF; /// 加载寄存器先清
    float frq = 1.0 / 66000000;
    int lrVal = duration / 1000.0 / frq;
    EPIT1->LR = lrVal + 1;
    EPIT1->CR |= (1 << 0);
}
void Key0_Filter_Stop()
{
    EPIT1->CR &= ~(1 << 0);
}

void Key0_Interrupt_Irq_Handler(unsigned int gicciar, void *context)
{
    Key0_Filter_Start(10);
    /// 中断处理完成后,清楚中断标志位
    GPIO_Clean_Interrupt_Flag(GPIO1, 18);
}