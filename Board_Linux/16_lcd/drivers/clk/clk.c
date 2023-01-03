
#include "imx6ul.h"
#include "clk.h"

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
    /// 分频器配置为1
    CCM->CACRR = 0;
    /// 将时钟源切换回CCSR的第二位判断是否选择的是pll1_main_clk
    CCM->CCSR &= ~(1 << 2);

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
    CCM_ANALOG->PFD_480 = reg;

    /// 配置外接设备的时钟频率
    /// CBCMR[PRE_PERIPH_CLK_SEL]选择PLL2->PFD2
    /// 先将19-18两个bit位清0
    /// 再将19-18两个bit位的数据写成1，选择PFD2
    CCM->CBCMR &= ~(3 << 18);
    CCM->CBCMR |= (1 << 18);
    /// CBCDR[PERIPH_CLK_SEL]多路选择器选择下路
    CCM->CBCDR &= ~(1 << 25);
    /// 读取第5个bit位(PERIPH_CLK_ SEL_BUSY),如果是1指示正忙于握手,如果是0表示握手完成
    while ((CCM->CDHIPR >> 5) & 0x1);

/* 修改 AHB_PODF 位的时候需要先禁止 AHB_CLK_ROOT 的输出，但是
 * 我没有找到关闭 AHB_CLK_ROOT 输出的的寄存器，所以就没法设置。
 * 下面设置 AHB_PODF 的代码仅供学习参考不能直接拿来使用!!
 * 内部 boot rom 将 AHB_PODF 设置为了 3 分频，即使我们不设置 AHB_PODF， * AHB_ROOT_CLK 也依旧等于 396/3=132Mhz。
 */
#if 0
    /// 将CBCDR[AHB_PODF]的12-10三个bit位清0
    CCM->CBCDR &= ~(7 << 10);
    /// 将CBCDR[AHB_PODF]的值设置成3，即3分频
    CCM->CBCDR | (2 << 10);
    /// 等待忙位检测通过
    while ((CCM->CDHIPR >> 1) & 0x1)
        ;
#endif

    /// 将9-8两个bit位的数据清0
    CCM->CBCDR &= ~(3 << 8);
    /// 设置成2分频
    CCM->CBCDR |= (1 << 8);
    /// CSCMR1[PERCLK_CLK_SEL]选择ipg clk root
    CCM->CSCMR1 &= ~(1 << 6);
    /// 将5-0 6个bit位清0,设置成1分频，此时已经是1分频了
    CCM->CSCMR1 &= ~(0x3F << 0);

    Clk_Uart_Init();
}
void Clk_Uart_Init() {
    /* 设置UART的时钟源为PLL3 480, 分频值为1,最终进入到UART的时钟为80MHz */
    CCM->CSCDR1 &= ~(1 << 6);
    CCM->CSCDR1 &= ~(0x3F); // 1分频
}
/// @brief 初始化LCD时钟源
/// @param loopDiv 设置DIV_SELECT 27 ~ 54
/// @param preDiv 
/// @param  
void Clk_LCD_Init(unsigned char loopDiv, unsigned char preDiv, unsigned char ) {
    /// LCD的计算公式里面涉及到两个寄存器用于控制小数部分的计算, 这里不使用小数，分母是1,分子是0
    CCM_ANALOG->PLL_VIDEO_NUM = 0;
    CCM_ANALOG->PLL_VIDEO_DENOM = 1; 
    // 使能这个PLL输出(启用它)
    CCM_ANALOG->PLL_VIDEO |= (1 << 13);
}