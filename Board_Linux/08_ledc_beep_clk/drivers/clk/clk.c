
#include "imx6ul.h"

void Clk_Init()
{
    CCM->CCGR0 = 0xFFFFFFFF;
    CCM->CCGR1 = 0xFFFFFFFF;
    CCM->CCGR2 = 0xFFFFFFFF;
    CCM->CCGR3 = 0xFFFFFFFF;
    CCM->CCGR4 = 0xFFFFFFFF;
    CCM->CCGR5 = 0xFFFFFFFF;
    CCM->CCGR6 = 0xFFFFFFFF;
}

void Clk_Freq_Init()
{
    /// CCM->CCSR的第二位判断是否选择的是pll1_main_clk(值是0)
    if ((CCM->CCSR >> 2 & 0x1) == 0)
    {
        /// 需要先切换时钟源到24MHz晶振
        CCM->CCSR &= ~(1 << 8);
        CCM->CCSR |= (1 << 2);
    }
    /// bit 13: enable register
    CCM_ANALOG->PLL_ARM |= (1 << 13);
    /// 配置
    CCM_ANALOG->PLL_ARM |= 58;
    /// 配置一分频
    CCM->CACRR = 0;
    /// 将时钟源切换回CCSR的第二位判断是否选择的是pll1_main_clk
    CCM->CCSR &= ~(1 << 2);
}