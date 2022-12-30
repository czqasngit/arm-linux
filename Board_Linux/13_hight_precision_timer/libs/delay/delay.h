#ifndef CONST_DELAY_H
#define CONST_DELAY_H
#endif

/// @brief 延时函数
/// @param 延时时间(ms)
void Delay(volatile unsigned int n);
void Delay_Init();
/// @brief 高精度延时函数
/// @param 延时值(纳秒)
void HighPrecisionDelayNS(unsigned int us);
/// @brief 高精度延时函数
/// @param 延时值(毫秒)
void HightPrecisionDelayMS(unsigned int ms);