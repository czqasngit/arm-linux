
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
    if (((CCM->CCSR >> 2) & 0x1) == 0)
    {
        /// 需要先切换时钟源到24MHz晶振
        CCM->CCSR &= ~(1 << 8);
        CCM->CCSR |= (1 << 2);
    }
    /// bit 13: enable register
    /// 配置(696MHz)
    CCM_ANALOG->PLL_ARM = (1 << 13) | (58);
    /// 将时钟源切换回CCSR的第二位判断是否选择的是pll1_main_clk
    CCM->CCSR &= ~(1 << 2);
    /// 分频器配置为1
    CCM->CACRR = 0;

    /// 配置PPL2(528MHz)的PFD0 ~ FPD3
    int reg = CCM_ANALOG->PFD_528;
    /// PFD0: 352MHz
    reg |= (27 << 0);
    /// PFD1: 594MHz
    reg |= (16 << 8);
    /// PFD2: 396MHz
    reg |= (24 << 16);
    /// PFD3: 297MHz
    reg |= (32 << 24);
    CCM_ANALOG->PFD_528 = reg;
    /// 配置PPL3(480MHz)的PFD0 ~ FPD3
    reg = CCM_ANALOG->PFD_480;
    /// PFD0: 720MHz
    reg |= (12 << 0);
    /// PFD1: 540MHz
    reg |= (16 << 8);
    /// PFD2: 508.2MHz
    reg |= (17 << 16);
    // PFD3: 454.7MHz
    reg |= (19 << 24);
}