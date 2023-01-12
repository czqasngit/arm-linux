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

void I2C_Master_Send(I2C_Type *i2c, unsigned char *buffer, unsigned int size) {
    if(size <= 0) return;
    // I2SR的bit7: 0表示当前I2C正在发送数据, 1表示发送完成
    // 等待上一个数据发送完成
    while((((i2c->I2SR) >> 7) & 0x1) == 0);
    // 清除中断标志位
    // 这里其实可以不清除的,但是不知道不清除是否有问题
    // 做了清除也没关系,所以这里先清除
    // TODO: 有机会可以修改来测试一下
    i2c->I2SR &= ~(1 << 1);
    // 设置为发送模式
    // 在上面的函数中可能已经设置过了,但是为了保险在发送前再设置一次也没关系
    // 因为在其它函数里面可能将这个位重置了
    i2c->I2CR |= (1 << 4);
    while(size --) {
        i2c->I2DR = *buffer++;
        // NXP官方的驱动程序在这里就是使用的bit1来判断
        // 这里与上面的判断发送完成不一样，这里判断的是bit1
        // 猜测: bit7是用来判断发送寄存器是否就绪,bit1是用来判断当前发送的数据是否完成了
        // bit1 表示的是一个中断是否就绪,即发送完一个字节数据会产生一个中断,使用中断可以做到非阻塞发送
        // 所以在上面需要清除一下bit1的中断位,因为在发送数据过程中这个位会发生变化
        while((((i2c->I2SR) >> 1) & 0x1) == 0);
        i2c->I2SR &= ~(1 << 1);
        // 每发送完一个字节的数据需要等待ACK
        // 检查I2SR中的bit0位是否收到了ACK，如果是NAK(NO ACK)则返回大于0的数据，程序退出,表示发送数据过程中有错误
        // 猜测: 上面的检查i2c->I2SR的bit0是在等待到ACK之后才会就绪的
        if(I2C_Check_And_Error_Clean(i2c, i2c->I2SR)) {
            break;
        }
    }
    // 发送完后清除相关的位
    // 清除中断标志位
    i2c->I2SR &= ~(1 << 1);
    // 写完数据,交出主机控制权
    I2C_Master_Stop(i2c);
}

// 接受数据
void I2C_Master_Receive(I2C_Type *i2c, unsigned char *buffer, unsigned int size) {
    // I2SR的bit7: 0表示当前I2C正在发送数据, 1表示发送完成
    // 等待上一个数据发送完成
    while((((i2c->I2SR) >> 7) & 0x1) == 0);
    // 清除中断标记位
    i2c->I2SR &= ~(1 << 1);
    // bit4 设置成0，切换到接受模式(这里已经让出主机控制权了,进入到接受数据模式)
    i2c->I2CR &= ~(1 << 4);
    // 设置成接受到数据后会应答ACK
    i2c->I2CR &= ~(1 << 3);
    /// 只需要接受一个数据,接受完成后会发送一个NO ACK
    if(size == 1) {
        i2c->I2CR |= (1 << 3);
    }
    int dummy = i2c->I2DR;// 假读
    while (size) {
        // 开始接受数据
        // 等待数据接受完成
        while((((i2c->I2SR) >> 1) & 0x1) == 0);
        i2c->I2SR &= ~(1 << 1);
        size--;
        if(size == 1) {
            // 已经接受到倒数第二个字节数据了,将bit3设置成NO ACK，表示接受完最后一个字节后就会回复一个NO ACK，不再回复ACk
            i2c->I2CR |= (1 << 3);
        }
        *buffer = i2c->I2DR;
        buffer ++;
    }
    // 数据接受完成
    I2C_Master_Stop(i2c);
}


/*
 * @description	: I2C数据传输，包括读和写
 * @param - base: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
char I2C_Master_Transfer(I2C_Type *base, struct i2c_transfer *xfer) {
	unsigned char ret = 0;
    I2CDirection direction = xfer->direction;	
	base->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */
	/* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1)); 
    /* 读取I2C从机设备寄存器数据 */
	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == I2CDirection_READ)) {
        direction = I2CDirection_WRITE;
    }
	ret = I2C_Master_Start(base, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret) {	
		return ret;
	}
	while(!(base->I2SR & (1 << 1)));			/* 等待传输完成 */
    ret = I2C_Check_And_Error_Clean(base, base->I2SR);	/* 检查是否出现传输错误 */
    if(ret) {
      	I2C_Master_Stop(base); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址, */
    if(xfer->subaddressSize) {
        do {
			base->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
            base->I2DR =  ((xfer->subaddress) >> (8 * xfer->subaddressSize)); //向I2DR寄存器写入子地址
			while(!(base->I2SR & (1 << 1)));  	/* 等待传输完成 */
            /* 检查是否有错误发生 */
            ret = I2C_Check_And_Error_Clean(base, base->I2SR);
            if(ret) {
             	I2C_Master_Stop(base); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2CStatus_OK));

        if(xfer->direction == I2CDirection_READ) { 		/* 读取数据 */
            base->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            I2C_Restart(base, xfer->slaveAddress, I2CDirection_READ); /* 发送重复开始信号和从机地址 */
    		while(!(base->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = I2C_Check_And_Error_Clean(base, base->I2SR);
            if(ret) {
             	ret = I2CStatus_ACK;
                I2C_Master_Stop(base); 		/* 发送停止信号 */
                return ret;  
            }
           	          
        }
    }	
    /* 发送数据 */
    if ((xfer->direction == I2CDirection_WRITE) && (xfer->dataSize > 0)) {
    	I2C_Master_Send(base, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == I2CDirection_READ) && (xfer->dataSize > 0)) {
       	I2C_Master_Receive(base, xfer->data, xfer->dataSize);
	}
	return 0;	
}
