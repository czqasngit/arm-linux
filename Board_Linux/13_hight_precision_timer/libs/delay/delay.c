#include "delay.h"
#include "imx6ul.h"

void delay_ms(volatile unsigned int n)
{
    /// 指令空转n个来达到延时的目的
    while (n--)
    {
    }
}
void Delay(volatile unsigned int n)
{
    while (n--)
    {
        delay_ms(0x7FF);
    }
}

void Delay_Init()
{
    /* 复位GPT定时器, bit15写入1复位*/
    GPT1->CR |= 1 << 15;
    /* 等待复位完成,复位完成后bit15的值是0 */
    while ((GPT1->CR >> 15) & 0x1)
    {
    }
    /// bit0:11
    GPT1->PR &= ~(0x7FF << 0);
    /// 进入GPT1的时钟源为66MHz
    /// 这里设置GPT1的频率为66分频,频率则为1MHz, 一个计数是1us
    GPT1->PR |= (65 << 0);

    /// 如果要使用比较中断,则需要设置OCR的比较寄存器值
    /// 当CNT的值与相应的比较寄存器值一样的时候,则会触发对应中断
    /// 这里不使用,设置成0xFFFFFFFF
    GPT1->OCR[0] = 0xFFFFFFFF;
    /// 关闭比较中断
    GPT1->CR &= ~(3 << 20);
    /// 设置GPT的时钟源为ipg_clk
    GPT1->CR &= ~(3 << 6);
    GPT1->CR |= 1 << 6;
    /// 使能GPT
    GPT1->CR |= 1 << 0;
}
/// GPT的频率是1MHz,一个计数则表示走过1us
void HighPrecisionDelayNS(unsigned int us)
{

    unsigned int lastCNTValue = GPT1->CNT;
    unsigned int totalCNTValue = 0;
    while (1)
    {
        unsigned int cntValue = GPT1->CNT;
        if (cntValue > lastCNTValue)
        {
            /// 计数器向上递增,如果当前的值大于上一次的值,表示没有溢出
            /// 总共计数累加
            totalCNTValue += cntValue - lastCNTValue;
        }
        else
        {
            /// 溢出了
            totalCNTValue += 0xFFFFFFFF - lastCNTValue;
            totalCNTValue += cntValue;
        }
        lastCNTValue = cntValue;
        /// 如果总计数已经大于给定的数据,则退出表示延时完成
        if (totalCNTValue > us)
            break;
    }
}
void HightPrecisionDelayMS(unsigned int ms)
{
    unsigned int total = ms;
    while (total > 0)
    {
        HighPrecisionDelayNS(1000);
        total--;
    }
}