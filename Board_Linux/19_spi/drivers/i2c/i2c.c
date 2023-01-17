#include "i2c.h"
#include "stdio.h"

void I2C_Init(I2C_Type *i2c) {
    // i2c->I2CR &= ~(1 << 7); // 关闭i2c
    // i2c->IFDR = 0x15;       // 设置i2c的分频值是640, 66000000 / 640 = 103.125 KHz; i2c支持的频率范围是(100kbit/s - 400kbit/s)
    // i2c->I2CR |= (1 << 7);  // 使能i2c
    /* 1、配置I2C */
	i2c->I2CR &= ~(1 << 7); /* 要访问I2C的寄存器，首先需要先关闭I2C */

    /* 设置波特率为100K
     * I2C的时钟源来源于IPG_CLK_ROOT=66Mhz
 	 * IC2 时钟 = PERCLK_ROOT/dividison(IFDR寄存器)
	 * 设置寄存器IFDR,IFDR寄存器参考IMX6UL参考手册P1260页，表29-3，
	 * 根据表29-3里面的值，挑选出一个还是的分频数，比如本例程我们
	 * 设置I2C的波特率为100K， 因此当分频值=66000000/100000=660.
	 * 在表29-3里面查找，没有660这个值，但是有640，因此就用640,
	 * 即寄存器IFDR的IC位设置为0X15
	 */
	i2c->IFDR = 0X15 << 0;

	/*
     * 设置寄存器I2CR，开启I2C
     * bit[7] : 1 使能I2C,I2CR寄存器其他位其作用之前，此位必须最先置1
	 */
	i2c->I2CR |= (1<<7);
    printf("I2C init complete \r\n");
}

char I2C_Master_Start(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction) {
    // if(((i2c->I2SR >> 5) & 0x1) == 1) {
    //     return I2CStatus_BUSY;
    // }
    // // 设置为主机模式
    // i2c->I2CR |= (1 << 5);
    // // 设置为发送模式(在寻址时)
    // i2c->I2CR |= (1 << 4);
    // // 设置I2DR寻址,同步产生一个start信号
    // // 高7位是地址,0位是读写位
    // i2c->I2DR = (unsigned int)((slave_addr << 1) | direction);
    // // printf("I2C_Master_Start\r\n");
    // return I2CStatus_OK;
    if(i2c->I2SR & (1 << 5))			/* I2C忙 */
		return 1;

	/*
     * 设置寄存器I2CR
     * bit[5]: 1 主模式
     * bit[4]: 1 发送
	 */
	i2c->I2CR |=  (1 << 5) | (1 << 4);

	/*
     * 设置寄存器I2DR
     * bit[7:0] : 要发送的数据，这里写入从设备地址
     *            参考资料:IMX6UL参考手册P1249
	 */ 
	i2c->I2DR = ((unsigned int)slave_addr << 1) | ((direction == I2CDirection_READ)? 1 : 0);
	return 0;
}

char I2C_Master_Stop(I2C_Type *i2c) {
    // // bit5 设置成0，从机模式,让出主机控制权
    // i2c->I2CR &= ~(1 << 5);
    // // bit4 设置成0，切换到接受模式(这里已经让出主机控制权了,进入到接受数据模式)
    // i2c->I2CR &= ~(1 << 4);
    // // 指定在收到一个字节数据后是否需要应答
    // // 设置为0时需要应答,仅工作在bit4设置成接受模式,设置成1则不应答
    // // 这里已经将bit4设置成了接受模式
    // i2c->I2CR &= ~(1 << 3);

    // // I2C忙检测超时时钟周期计时
    // unsigned int timeout = 0xFFFF;
    // // 等待I2C空闲
    // while(((i2c->I2SR >> 5) & 0x1) == 1) {
    //     timeout --;
    //     if(timeout <= 0) {
    //         return I2CStatus_TIMEOUT;
    //     }
    // }
    // // printf("I2C_Master_Stop\r\n");
    // return I2CStatus_OK;

	unsigned short timeout = 0xffff;

	/*
	 * 清除I2CR的bit[5:3]这三位
	 */
	i2c->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

	/* 等待忙结束 */
	while((i2c->I2SR & (1 << 5)))
	{
		timeout--;
		if(timeout == 0)	/* 超时跳出 */
			return I2CStatus_TIMEOUT;
	}
	return I2CStatus_OK;
}

char I2C_Restart(I2C_Type *i2c, unsigned int slave_addr, I2CDirection direction) {
    /* 
        I2C忙并且工作在从模式,跳出
        在从模式下，又是忙的状态表示当前I2C正以从机模式工作，不能产生restart信号，
        如果只是忙但是工作在主机模式下,是可以产生restart信号的
     */
    // if(((i2c->I2SR >> 5) & 0x1) == 1 && ((i2c->I2CR >> 5) & 0x1) == 0) return I2CStatus_BUSY;
    // // 设置为发送模式
    // i2c->I2CR |= (1 << 4);
    // // 产生一个restart信号
    // i2c->I2CR |= (1 << 2);
    // // 设置I2DR寻址,同步产生一个start信号
    // // 高7位是地址,0位是读写位
    // i2c->I2DR = (unsigned int)((slave_addr << 1) | (direction == I2CDirection_READ ? 1 : 0));
    // // printf("I2C_Restart\r\n");

    // return I2CStatus_OK;
    /* I2C忙并且工作在从模式,跳出 */
	if(i2c->I2SR & (1 << 5) && (((i2c->I2CR) & (1 << 5)) == 0))		
		return 1;

	/*
     * 设置寄存器I2CR
     * bit[4]: 1 发送
     * bit[2]: 1 产生重新开始信号
	 */
	i2c->I2CR |=  (1 << 4) | (1 << 2);

	/*
     * 设置寄存器I2DR
     * bit[7:0] : 要发送的数据，这里写入从设备地址
     *            参考资料:IMX6UL参考手册P1249
	 */ 
	i2c->I2DR = ((unsigned int)slave_addr << 1) | ((direction == I2CDirection_READ)? 1 : 0);
	
	return 0;
}

char I2C_Check_And_Error_Clean(I2C_Type *i2c, unsigned int state) {
    // if(((state >> 4) & 0x1) == 1) {
    //     // 仲裁丢失检查
    //     // 先将仲裁丢失位清0,表示这一位我处理了,重置
    //     i2c->I2SR &= ~(1 << 4);
    //     // 关闭I2C
    //     i2c->I2CR &= ~(1 << 7);
    //     // 重新打开I2D
    //     i2c->I2CR |= (1 << 7);
    //     return I2CStatus_ARBI;
    // } else if(((state >> 0) & 0x1) == 1) {
    //     return I2CStatus_NAK;
    // }
    // // printf("I2C_Check_And_Error_Clean\r\n");

    // return I2CStatus_OK;

    /* 检查是否发生仲裁丢失错误 */
	if(state & (1<<4))
	{
		i2c->I2SR &= ~(1<<4);		/* 清除仲裁丢失错误位 			*/

		i2c->I2CR &= ~(1 << 7);	/* 先关闭I2C 				*/
		i2c->I2CR |= (1 << 7);		/* 重新打开I2C 				*/
		return I2CStatus_ARBI;
	} 
	else if(state & (1 << 0))     	/* 没有接收到从机的应答信号 */
	{
		return I2CStatus_NAK;		/* 返回NAK(No acknowledge) */
	}
	return I2CStatus_OK;
}

void I2C_Master_Send(I2C_Type *i2c, unsigned char *buffer, unsigned int size) {
    // // printf("I2C_Master_Send start\r\n");
    // if(size <= 0) return;
    // // I2SR的bit7: 0表示当前I2C正在发送数据, 1表示发送完成
    // // 等待上一个数据发送完成
    // while((((i2c->I2SR) >> 7) & 0x1) == 0);
    // // 清除中断标志位
    // // 这里其实可以不清除的,但是不知道不清除是否有问题
    // // 做了清除也没关系,所以这里先清除
    // // TODO: 有机会可以修改来测试一下
    // i2c->I2SR &= ~(1 << 1);
    // // 设置为发送模式
    // // 在上面的函数中可能已经设置过了,但是为了保险在发送前再设置一次也没关系
    // // 因为在其它函数里面可能将这个位重置了
    // i2c->I2CR |= (1 << 4);
    // while(size --) {
    //     i2c->I2DR = *buffer++;
    //     // NXP官方的驱动程序在这里就是使用的bit1来判断
    //     // 这里与上面的判断发送完成不一样，这里判断的是bit1
    //     // 猜测: bit7是用来判断发送寄存器是否就绪,bit1是用来判断当前发送的数据是否完成了
    //     // bit1 表示的是一个中断是否就绪,即发送完一个字节数据会产生一个中断,使用中断可以做到非阻塞发送
    //     // 所以在上面需要清除一下bit1的中断位,因为在发送数据过程中这个位会发生变化
    //     while((((i2c->I2SR) >> 1) & 0x1) == 0);
    //     i2c->I2SR &= ~(1 << 1);
    //     // 每发送完一个字节的数据需要等待ACK
    //     // 检查I2SR中的bit0位是否收到了ACK，如果是NAK(NO ACK)则返回大于0的数据，程序退出,表示发送数据过程中有错误
    //     // 猜测: 上面的检查i2c->I2SR的bit0是在等待到ACK之后才会就绪的
    //     if(I2C_Check_And_Error_Clean(i2c, i2c->I2SR)) {
    //         break;
    //     }
    // }
    // // 发送完后清除相关的位
    // // 清除中断标志位
    // i2c->I2SR &= ~(1 << 1);
    // // 写完数据,交出主机控制权
    // I2C_Master_Stop(i2c);
    // // printf("I2C_Master_Send end\r\n");

    /* 等待传输完成 */
	while(!(i2c->I2SR & (1 << 7))); 
	
	i2c->I2SR &= ~(1 << 1); 	/* 清除标志位 */
	i2c->I2CR |= 1 << 4;		/* 发送数据 */
	
	while(size--)
	{
		i2c->I2DR = *buffer++; 	/* 将buf中的数据写入到I2DR寄存器 */
		
		while(!(i2c->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		i2c->I2SR &= ~(1 << 1);			/* 清除标志位 */

		/* 检查ACK */
		if(I2C_Check_And_Error_Clean(i2c, i2c->I2SR))
			break;
	}
	
	i2c->I2SR &= ~(1 << 1);
	I2C_Master_Stop(i2c); 	/* 发送停止信号 */
}

// 接受数据
void I2C_Master_Receive(I2C_Type *i2c, unsigned char *buffer, unsigned int size) {
    // printf("I2C_Master_Receive start\r\n");
    // volatile uint8_t dummy = 0;
	// dummy++; 	/* 防止编译报错 */
    // // I2SR的bit7: 0表示当前I2C正在发送数据, 1表示发送完成
    // // 等待上一个数据发送完成
    // while((((i2c->I2SR) >> 7) & 0x1) == 0);
    // // 清除中断标记位
    // i2c->I2SR &= ~(1 << 1);
    // // bit4 设置成0，切换到接受模式(这里已经让出主机控制权了,进入到接受数据模式)
    // i2c->I2CR &= ~(1 << 4);
    // // 设置成接受到数据后会应答ACK
    // i2c->I2CR &= ~(1 << 3);
    // /// 只需要接受一个数据,接受完成后会发送一个NO ACK
    // if(size == 1) {
    //     i2c->I2CR |= (1 << 3);
    // }
    // dummy = i2c->I2DR;// 假读
    // while (size--) {
    //     printf("I2C_Master_Receive size: %d \r\n", size);

    //     // 开始接受数据
    //     // 等待数据接受完成
    //     while((((i2c->I2SR) >> 1) & 0x1) == 0);
    //     i2c->I2SR &= ~(1 << 1);
    //     if(size == 0) {
    //         // 数据接受完成
    //         I2C_Master_Stop(i2c);
    //     }
    //     if(size == 1) {
    //         // 已经接受到倒数第二个字节数据了,将bit3设置成NO ACK，表示接受完最后一个字节后就会回复一个NO ACK，不再回复ACk
    //         i2c->I2CR |= (1 << 3);
    //     }
    //     // send ack/no ack after read I2DR register data
    //     *buffer = i2c->I2DR;
    //     buffer ++;
    // }

    volatile uint8_t dummy = 0;

	dummy++; 	/* 防止编译报错 */
	
	/* 等待传输完成 */
	while(!(i2c->I2SR & (1 << 7))); 
	
	i2c->I2SR &= ~(1 << 1); 				/* 清除中断挂起位 */
	i2c->I2CR &= ~((1 << 4) | (1 << 3));	/* 接收数据 */
	
	/* 如果只接收一个字节数据的话发送NACK信号 */
	if(size == 1)
        i2c->I2CR |= (1 << 3);

	dummy = i2c->I2DR; /* 假读 */
	
	while(size--)
	{
		while(!(i2c->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		i2c->I2SR &= ~(1 << 1);			/* 清除标志位 */

	 	if(size == 0)
        {
        	I2C_Master_Stop(i2c); 			/* 发送停止信号 */
        }

        if(size == 1)
        {
            i2c->I2CR |= (1 << 3);
        }
		*buffer++ = i2c->I2DR;
	}
    
    // printf("I2C_Master_Receive end\r\n");
    // I2C_Master_Stop(i2c);
}


/*
 * @description	: I2C数据传输，包括读和写
 * @param - i2c: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
char I2C_Master_Transfer(I2C_Type *i2c, struct i2c_transfer *xfer) {
    // printf("I2C_Master_Transfer start\r\n");
	unsigned char ret = 0;
    I2CDirection direction = xfer->direction;	
	i2c->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */
	/* 等待传输完成 */
	while(!((i2c->I2SR >> 7) & 0X1)); 
    /* 读取I2C从机设备寄存器数据 */
	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == I2CDirection_READ)) {
        direction = I2CDirection_WRITE;
    }
	ret = I2C_Master_Start(i2c, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret) {	
		return ret;
	}
	while(!(i2c->I2SR & (1 << 1)));			/* 等待传输完成 */
    ret = I2C_Check_And_Error_Clean(i2c, i2c->I2SR);	/* 检查是否出现传输错误 */
    if(ret) {
        // printf("I2C_Master_Transfer error: %d\r\n", ret);
      	I2C_Master_Stop(i2c); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址, */
    if(xfer->subaddressSize) {
        do {
            // printf("subaddressSize: %d \r\n", xfer->subaddressSize);
			i2c->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
            i2c->I2DR =  ((xfer->subaddress) >> (8 * xfer->subaddressSize)); //向I2DR寄存器写入子地址
			while(!(i2c->I2SR & (1 << 1)));  	/* 等待传输完成 */
            /* 检查是否有错误发生 */
            ret = I2C_Check_And_Error_Clean(i2c, i2c->I2SR);
            if(ret) {
                // printf("subaddressSize error: %d\r\n", ret);
             	I2C_Master_Stop(i2c); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2CStatus_OK));
        // printf("subaddressSize end \r\n");

        if(xfer->direction == I2CDirection_READ) { 		/* 读取数据 */
            // printf("ready for read ap3216c data \r\n");
            i2c->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            I2C_Restart(i2c, xfer->slaveAddress, I2CDirection_READ); /* 发送重复开始信号和从机地址 */
    		while(!(i2c->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = I2C_Check_And_Error_Clean(i2c, i2c->I2SR);
            if(ret) {
             	ret = I2CStatus_ACK;
                // printf("restart error: %d\r\n", ret);
                I2C_Master_Stop(i2c); 		/* 发送停止信号 */
                return ret;  
            }
           	          
        }
    }	
    /* 发送数据 */
    if ((xfer->direction == I2CDirection_WRITE) && (xfer->dataSize > 0)) {
    	I2C_Master_Send(i2c, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == I2CDirection_READ) && (xfer->dataSize > 0)) {
       	I2C_Master_Receive(i2c, xfer->data, xfer->dataSize);
	}
    // printf("I2C_Master_Transfer end\r\n");
	return 0;	
}
