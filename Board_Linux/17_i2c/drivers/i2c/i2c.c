#include "i2c.h"

void I2C_Init(I2C_Type *i2c) {
    i2c->I2CR &= ~(1 << 7); // 关闭i2c
    i2c->IFDR = 0x15;       // 设置i2c的分频值是640, 66000000 / 640 = 103.125 KHz; i2c支持的频率范围是(100kbit/s - 400kbit/s)
    i2c->I2CR |= (1 << 7);  // 使能i2c
}

char I2C_Master_Start(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction) {
    if((i2c->I2SR >> 5) & 0x1 == 1) {
        return 1;
    }
    // 设置为主机模式
    i2c->I2CR |= (1 << 5);
    // 设置为发送模式(在寻址时)
    i2c->I2CR |= (1 << 4);
    // 设置I2DR寻址,同步产生一个start信号
    // 高7位是地址,0位是读写位
    i2c->I2DR = (unsigned int)((slave_addr << 1) | direction);
}

char I2C_Master_Stop(I2C_Type *i2c) {
    
}