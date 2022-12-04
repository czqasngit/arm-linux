#ifndef _GPIO_H
#define _GPIO_H
#include "imx6ul.h"
/// @brief GPIO方向
typedef enum _GPIO_DIRECTION_
{
    GPIO_DIRECTION_INPUT = 0,
    GPIO_DIRECTION_OUTPUT = 1
} GPIO_DIRECTION;
/// @brief  GPIO初始化配置
typedef struct _GPIO_CONFIG_
{
    GPIO_DIRECTION direction;
    /// 作为输出时默认的值
    uint8_t defaultOutputValue;
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
uint8_t GPIO_RedValue(GPIO_Type *base, int pin);
#endif