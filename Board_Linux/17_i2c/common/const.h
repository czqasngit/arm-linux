#ifndef CONST_H
#define CONST_H

#define CONST_GPIO_MUX_VALUE 0x05
#define CONST_GPIO_OUTPUT_PAD_VALUE 0x10B0
/*
2、、配置UART1_CTS_B的IO属性
28 *bit 16:0 HYS 关闭
29 *bit [15:14]: 11 默认 22K 上拉
30 *bit [13]: 1 pull 功能
31 *bit [12]: 1 pull/keeper 使能
32 *bit [11]: 0 关闭开路输出
33 *bit [7:6]: 10 速度 100Mhz
34 *bit [5:3]: 000 关闭输出
35 *bit [0]: 0 低转换率
36 */
#define CONST_GPIO_INPUT_PAD_VALUE 0xF080

#endif