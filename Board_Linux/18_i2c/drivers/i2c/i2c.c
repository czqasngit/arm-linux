#include "i2c.h"

void I2C_Init(I2C_Type *i2c) {
    i2c->I2CR &= ~(1 << 7); // 关闭i2c
    i2c->IFDR = 0x15;       // 设置i2c的分频值是640, 66000000 / 640 = 103.125 KHz; i2c支持的频率范围是(100kbit/s - 400kbit/s)
    i2c->I2CR |= (1 << 7);  // 使能i2c
}

char I2C_Master_Start(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction) {
    if(((i2c->I2SR >> 5) & 0x1) == 1) {
        return I2CStatus_BUSY;
    }
    // 设置为主机模式
    i2c->I2CR |= (1 << 5);
    // 设置为发送模式(在寻址时)
    i2c->I2CR |= (1 << 4);
    // 设置I2DR寻址,同步产生一个start信号
    // 高7位是地址,0位是读写位
    i2c->I2DR = (unsigned int)((slave_addr << 1) | direction);
    return I2CStatus_OK;
}

char I2C_Master_Stop(I2C_Type *i2c) {
    // bit5 设置成0，从机模式,让出主机控制权
    i2c->I2CR &= ~(1 << 5);
    // bit4 设置成0，切换到接受模式(这里已经让出主机控制权了,进入到接受数据模式)
    i2c->I2CR &= ~(1 << 4);
    // 指定在收到一个字节数据后是否需要应答
    // 设置为0时需要应答,仅工作在bit4设置成接受模式,设置成1则不应答
    // 这里已经将bit4设置成了接受模式
    i2c->I2CR &= ~(1 << 3);

    // I2C忙检测超时时钟周期计时
    unsigned int timeout = 0xFFFF;
    // 等待I2C空闲
    while(((i2c->I2SR >> 5) & 0x1) == 1) {
        timeout --;
        if(timeout <= 0) {
            return I2CStatus_TIMEOUT;
        }
    }
    return I2CStatus_OK;
}

char I2C_Restart(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction) {
    // 忙,直接返回
    if(((i2c->I2SR >> 5) & 0x1) == 1) return I2CStatus_BUSY;
    // 工作在从机模式下,直接返回
    if(((i2c->I2CR >> 5) & 0x1) == 0) return I2CStatus_BUSY;
    // 设置为发送模式
    i2c->I2CR |= (1 << 4);
    // 产生一个restart信号
    i2c->I2CR |= (1 << 2);
    // 设置I2DR寻址,同步产生一个start信号
    // 高7位是地址,0位是读写位
    i2c->I2DR = (unsigned int)((slave_addr << 1) | direction);
    return I2CStatus_OK;
}

char I2C_Check_And_Error_Clean(I2C_Type *i2c, unsigned int state) {
    if(((state >> 4) & 0x1) == 1) {
        // 仲裁丢失检查
        // 先将仲裁丢失位清0,表示这一位我处理了,重置
        i2c->I2SR &= ~(1 << 4);
        // 关闭I2C
        i2c->I2CR &= ~(1 << 7);
        // 重新打开I2D
        i2c->I2CR |= (1 << 7);
        return I2CStatus_ARBI;
    } else if(((state >> 0) & 0x1) == 1) {
        return I2CStatus_NAK;
    }
    return I2CStatus_OK;
}