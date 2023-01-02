#ifndef CONST_CLK_H
#define CONST_CLK_H
#endif

/// 初始化(使能)所有I.MX6ULL外设引脚
void Clk_Init();
void Clk_Freq_Init();
/* 串口时钟配置 */
void Clk_Uart_Init();
void Clk_LCD_Init();