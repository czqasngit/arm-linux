#ifndef _I2C_H
#define _I2C_H


enum I2CStatus {
    I2CStatus_OK = 0, 
    I2CStatus_BUSY = 1, 
    I2CStatus_IDLE = 2, 
    I2CStatus_NAK = 3,              /* NO ACK */
    I2CStatus_ARBI = 4,             /* 仲裁丢失 */
    I2CStatus_TIMEOUT = 5,          /* 超时 */
    I2CStatus_ACK = 6              /* ACK */          
};
typedef enum _I2CDirection {
    I2CDirection_WRITE = 0,
    I2CDirection_READ = 1
} I2CDirection;

/*
 * 主机传输结构体
 */
struct i2c_transfer
{
    unsigned char slaveAddress;      	/* 7位从机地址 			*/
    I2CDirection direction; 		/* 传输方向 			*/
    unsigned int subaddress;       		/* 寄存器地址			*/
    unsigned char subaddressSize;    	/* 寄存器地址长度 			*/
    unsigned char *volatile data;    	/* 数据缓冲区 			*/
    volatile unsigned int dataSize;  	/* 数据缓冲区长度 			*/
};


#include "imx6ul.h"
#include "MCIMX6Y2.h"

/// @brief 初始化I2C
/// @param i2c I2C设备
void I2C_Init(I2C_Type *i2c);

/// @brief 作为主机产生一个起始信号,寻址从机
/// @param i2c I2C设备
/// @param slave_addr 从机地址
/// @param direction  读写方向
/// @result 1: I2C忙, 0: I2C空闲
char I2C_Master_Start(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction);
char I2C_Master_Stop(I2C_Type *i2c);
char I2C_Restart(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction);
char I2C_Check_And_Error_Clean(I2C_Type *i2c, unsigned int state);
void I2C_Master_Send(I2C_Type *i2c, unsigned char *buffer, unsigned int size);
void I2C_Master_Receive(I2C_Type *i2c, unsigned char *buffer, unsigned int size);
char I2C_Master_Transfer(I2C_Type *base, struct i2c_transfer *xfer);
#endif