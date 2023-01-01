#include "uart.h"
#include "imx6ul.h"
#include "gpio.h"
#include "const.h"

/* 初始化uart, 波特率固定为115200 */
void UART1_Init()
{
    /* uart1 IO复用 */
    UART1_IO_Init();
    /* 配置UART1时先将期关闭,防止其它地方使用 */
    UART1_Disable();
    /* 配置前先进行复位,复位接受与发送状态,复位FIFOs以及相关的寄存器值,将他们恢复到默认状态 */
    UART1_Soft_Reset();
    /* 配置UART1  */
    UART1_Config();
}
void UART1_IO_Init()
{
    /* TXD, RXD 引用复用 */
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0);
    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, CONST_GPIO_OUTPUT_PAD_VALUE);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, CONST_GPIO_OUTPUT_PAD_VALUE);
}
void UART1_Disable()
{
    UART1->UCR1 &= ~(1 << 0);
}
void UART1_Enable()
{
    UART1->UCR1 |= (1 << 0);
}
void UART1_Soft_Reset()
{
    UART1->UCR2 &= ~(1 << 0);
    /// 等待复位完成
    while (((UART1->UCR2 >> 0) & 0x1) == 0)
        ;
}
void UART1_Config()
{
    UART1->UCR1 &= ~(1 < 14); /// 关闭自动波特率检测
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

#if 0
    /* 手动推算的波特率数值 */
    /* 设置波特率: 115200 */
    /* 从UART进来的时钟频率是80MHz, 设置UART1的分频值为1,那么UART1的频率就是80MHz */
    UART1->UFCR &= ~(7 << 7);
    UART1->UFCR |= (5 << 7);
    /// 设置值为: 71, bit: 0-15有效
    UART1->UBIR = 71;
    /// 设置值为: 3212, bit: 0-15有效
    UART1->UBMR = 3124;
#endif
    /* 移植NXP写好的自动推算设置波特率的函数 */
    Uart_SetBaudrate(UART1, 115200, 80000000);
}

void Uart1_SendChar(char c)
{
    // bit: 3
    // 0 Transmit is incomplete
    // 1 Transmit is complete
    while (((UART1->USR2 >> 3) & 0x1) == 0)
        ;
    // bit:8 - 0,数据位
    UART1->UTXD = c;
}
void Uart1_SendString(char *s)
{
    char *p = s;
    char c = *p;
    while (c)
    {
        c = *p;
        Uart1_SendChar(c);
        p++;
    }
}
char Uart1_ReadChar()
{
    // bit: 0
    // 0 No receive data ready
    // 1 Receive data ready
    while (((UART1->USR2 >> 0) & 0x1) == 0)
        ;
    char c = (UART1->URXD & 0xF);
    return c;
}

void Uart_SetBaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz)
{
    uint32_t numerator = 0u;   // 分子
    uint32_t denominator = 0U; // 分母
    uint32_t divisor = 0U;
    uint32_t refFreqDiv = 0U;
    uint32_t divider = 1U;
    uint64_t baudDiff = 0U;
    uint64_t tempNumerator = 0U;
    uint32_t tempDenominator = 0u;

    /* get the approximately maximum divisor */
    numerator = srcclock_hz;
    denominator = baudrate << 4;
    divisor = 1;

    while (denominator != 0)
    {
        divisor = denominator;
        denominator = numerator % denominator;
        numerator = divisor;
    }

    numerator = srcclock_hz / divisor;
    denominator = (baudrate << 4) / divisor;

    /* numerator ranges from 1 ~ 7 * 64k */
    /* denominator ranges from 1 ~ 64k */
    if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
    {
        uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
        uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
        uint32_t max = m > n ? m : n;
        numerator /= max;
        denominator /= max;
        if (0 == numerator)
        {
            numerator = 1;
        }
        if (0 == denominator)
        {
            denominator = 1;
        }
    }
    divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

    switch (divider)
    {
    case 1:
        refFreqDiv = 0x05;
        break;
    case 2:
        refFreqDiv = 0x04;
        break;
    case 3:
        refFreqDiv = 0x03;
        break;
    case 4:
        refFreqDiv = 0x02;
        break;
    case 5:
        refFreqDiv = 0x01;
        break;
    case 6:
        refFreqDiv = 0x00;
        break;
    case 7:
        refFreqDiv = 0x06;
        break;
    default:
        refFreqDiv = 0x05;
        break;
    }
    /* Compare the difference between baudRate_Bps and calculated baud rate.
     * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
     * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
     */
    tempNumerator = srcclock_hz;
    tempDenominator = (numerator << 4);
    divisor = 1;
    /* get the approximately maximum divisor */
    while (tempDenominator != 0)
    {
        divisor = tempDenominator;
        tempDenominator = tempNumerator % tempDenominator;
        tempNumerator = divisor;
    }
    tempNumerator = srcclock_hz / divisor;
    tempDenominator = (numerator << 4) / divisor;
    baudDiff = (tempNumerator * denominator) / tempDenominator;
    baudDiff = (baudDiff >= baudrate) ? (baudDiff - baudrate) : (baudrate - baudDiff);

    if (baudDiff < (baudrate / 100) * 3)
    {
        base->UFCR &= ~UART_UFCR_RFDIV_MASK;
        base->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
        base->UBIR = UART_UBIR_INC(denominator - 1); // 要先写UBIR寄存器，然后在写UBMR寄存器，3592页
        base->UBMR = UART_UBMR_MOD(numerator / divider - 1);
    }
}

void putc(char c)
{
    Uart1_SendChar(c);
}
char getc()
{
    return Uart1_ReadChar();
}

/* 引入gcc中的函数库时所需要实现的函数,看样子应该是发生异常时调用的函数 */
void raise(int sig_nr) {}