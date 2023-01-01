#include "uart.h"
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"
/* 初始化uart, 波特率固定为115200 */
void UART1_Init() {
    /* uart1 IO复用 */
    UART1_IO_Init();
    /* 配置UART1时先将期关闭,防止其它地方使用 */
    UART1_Disable();
    /* 配置前先进行复位,复位接受与发送状态,复位FIFOs以及相关的寄存器值,将他们恢复到默认状态 */
    UART1_Soft_Reset();
    /* 配置UART1  */
    UART1_Config();
} 
void UART1_IO_Init() {
    /* TXD, RXD 引用复用 */
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0);
    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, CONST_GPIO_OUTPUT_PAD_VALUE);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, CONST_GPIO_OUTPUT_PAD_VALUE);
}
void UART1_Disable() {
    UART1->UCR1 &= ~(1 << 0);
}
void UART1_Enable() {
    UART1->UCR1 |= (1 << 0);
}
void UART1_Soft_Reset() {
    UART1->UCR2 &= ~(1 << 0);
    /// 等待复位完成
    while(((UART1->UCR2 >> 0) & 0x1) == 0);
}
void UART1_Config() {
    /* 配置UART1的数据位,奇偶校验,停止位等等 */
    /// 接受使能
    UART1->UCR2 |= (1 << 1);
    /// 发送使能
    UART1->UCR2 |= (1 << 2);
    /// 数据位设置成8(0:7, 1:8)
    UART1->UCR2 |= (1 << 5);
    /// 设置1位停止位(0:1, 1: 2)
    UART1->UCR2 &= ~(1 << 6);
    /// 禁止奇偶校验
    UART1->UCR2 &= ~(1 << 8);
    /// 忽略RTS引脚,发射机会忽略来自RTS引用的信号数据
    UART1->UCR2 |= (1 << 14);
    // RXD Muxed Input Selected. Selects proper input pins for serial and Infrared input signal. 
    // NOTE: In this chip, UARTs are used in MUXED mode, so that this bit should always be set.
    UART1->UCR2 |= (1 << 2);

    /* 设置波特率: 115200 */
    /* 从UART进来的时钟频率是80MHz, 设置UART1的分频值为1,那么UART1的频率就是80MHz */ 
    UART1->UFCR &= ~(7 << 7);
    UART1->UFCR |= (5 << 7);
    /// 设置值为: 71, bit: 0-15有效
    UART1->UBIR = 71;
    /// 设置值为: 3212, bit: 0-15有效
    UART1->UBMR = 3124;
}

void Uart1_SendChar(char c) {
    // bit: 3
    // 0 Transmit is incomplete
    // 1 Transmit is complete
    while(((UART1->USR2 >> 3) & 0x1) == 0);
    // bit:8 - 0,数据位
    UART1->UTXD = c;
} 
char Uart1_ReadChar() {
    // bit: 0
    // 0 No receive data ready
    // 1 Receive data ready
    while(((UART1->USR2 >> 0) & 0x1) == 0);
    char c = (UART1->URXD & 0xF);
    return c;
}