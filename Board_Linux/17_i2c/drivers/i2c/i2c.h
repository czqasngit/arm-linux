#ifndef _I2C_H
#define _I2C_H


enum I2CStatus {
    I2CStatus_OK = 0, 
    I2CStatus_BUSY = 1, 
    I2CStatus_IDLE = 2, 
    I2CStatus_NAK = 3, 
    I2CStatus_ARBI = 4,           /* 仲裁丢失 */
    I2CStatus_TIMEOUT = 5,        /* 超时 */
    I2CStatus_ACK = 7             
}
enum I2CDirection {
    I2CDirection_WRITE = 0,
    I2CDirection_READ = 1
}
#include "imx6ul.h"
#include "MCIMX6Y2.h"

/// @brief 初始化I2C
/// @param i2c I2C设备
void I2C_Init(I2C_Type *i2c);
/// @brief 作为主机产生一个起始信号,寻址从机
/// @param i2c I2C设备
/// @param slave_addr 从机地址
/// @param direction  读写方向
/// @result 0: I2C忙, 1: I2C空闲
char I2C_Master_Start(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction);
#endif