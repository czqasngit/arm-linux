#ifndef CONST_CLK_H
#define CONST_CLK_H
#endif

/// 初始化(使能)所有I.MX6ULL外设引脚
void Clk_Init();
void Clk_Freq_Init();
/* 串口时钟配置 */
void Clk_Uart_Init();
/// @brief 初始化LCD时钟源, 24 * loopDiv / preDiv / podf(频率计算公式)
/// @param loopDiv 设置DIV_SELECT 27 ~ 54 (24MHz出来后的倍频值,被限制在650MHz - 1.3GHz, 所有DIV_SELECT的取值范围是27 - 54)
/// @param preDiv 前分频器 CSCDR2[LCDIF1_PRED] 范围 1 - 8
/// @param podf   后分频器 LCDIF1 CBCMR[LCDIF1_PODF] 范围 1 - 8
void Clk_LCD_Init(unsigned char loopDiv, unsigned char preDiv, unsigned char podf);