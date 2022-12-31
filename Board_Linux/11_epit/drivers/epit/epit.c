#include "epit.h"
#include "imx6ul.h"
#include "interrupt.h"
#include "led.h"
#include "beep.h"

void Epit_Init(unsigned int duration)
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
    EPIT1->CR |= (65 << 4);
    EPIT1->CR &= ~(3 << 24);    /// bit25-24 清0
    EPIT1->CR |= (1 << 24);     /// bit25-24 选择ipg clk时钟源

    EPIT1->LR &= ~0xFFFFFFFF; /// 加载寄存器先清

    EPIT1->LR = duration * 1000;
    EPIT1->CMPR = 0;    
    Interrupt_Irq_Handler_Register(EPIT1_IRQn, Epit1_Interrupt_Irq_Handler, NULL); /// 注册对应中断号的中断服务函数

    GIC_EnableIRQ(EPIT1_IRQn); /// enable epit1 irq中断

    EPIT1->CR |= (1 << 0); /// bit0: enable,最后配置完成之后再使能
}

void Epit1_Interrupt_Irq_Handler(unsigned int gicciar, void *context)
{
    static int state = 1;
    if ((EPIT1->SR & 1 << 0) == 1)
    {
        Led_Set(state);
        Beep_Set(state);
        state = !state;
    }
    EPIT1->SR |= 1 << 0;
}
