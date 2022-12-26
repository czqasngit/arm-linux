#ifndef _GPIO_H
#define _GPIO_H
#include "imx6ul.h"
/// @brief GPIO方向
typedef enum _GPIO_DIRECTION_
{
    GPIO_DIRECTION_INPUT = 0,
    GPIO_DIRECTION_OUTPUT = 1
} GPIO_DIRECTION;

/// GPIO触发模式
typedef enum _GPIO_INTERRUPT_MODE
{
    GPIO_INTERRUPT_MODE_NO_INTERRUPT = 0,            /* 无中断功能 */
    GPIO_INTERRUPT_MODE_LOW = 1,                     /* 低电平触发 */
    GPIO_INTERRUPT_MODE_HIGH = 2,                    /* 高电平触发 */
    GPIO_INTERRUPT_MODE_RISING_EDGE = 3,             /* 上升沿触发 */
    GPIO_INTERRUPT_MODE_FALLING_EDGE = 4,            /* 下降沿触发 */
    GPIO_INTERRUPT_MODE_RISING_AND_RALLING_EDGE = 5, /* 上升与下降沿都触发 */
} GPIO_INTERRUPT_MODE;

/// @brief  GPIO初始化配置
typedef struct _GPIO_CONFIG_
{
    GPIO_DIRECTION direction;
    /// 作为输出时默认的值
    uint8_t defaultOutputValue;
    /// 中断模式
    GPIO_INTERRUPT_MODE interruptMode;
} GPIO_CONFIG;
/// @brief 初始化GPIO
/// @param base GPIO类型基地址
/// @param pin GPIO具体引脚数字
/// @param config GPIO配置
void GPIO_Init(GPIO_Type *base, int pin, GPIO_CONFIG config);
/// @brief 向GPIO写入数据
/// @param base GPIO类型基地址
/// @param pin  GPIO具体引脚数字
/// @param value 引脚的高低电平(1/0)
void GPIO_SetValue(GPIO_Type *base, int pin, uint8_t value);
/// @brief 向GPIO写入数据
/// @param base GPIO类型基地址
/// @param pin  GPIO具体引脚数字
/// @result 引脚的高氏电平(1/0)
uint8_t GPIO_ReadValue(GPIO_Type *base, int pin);
/// 使能GPIO对应pin的中断
void GPIO_Enable_Interrupt(GPIO_Type *base, int pin);
/// 禁止GPIO对应pin的中断
void GPIO_Disable_Interrupt(GPIO_Type *base, int pin);
/// 清楚中断标志位(中断响应完成后)
void GPIO_Clean_Interrupt_Flag(GPIO_Type *base, int pin);
/// 初始化GPIO中断
void GPIO_Init_Interrupt(GPIO_Type *base, int pin, GPIO_INTERRUPT_MODE mode);
#endif