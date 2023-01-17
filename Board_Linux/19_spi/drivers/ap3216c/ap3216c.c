#include "ap3216c.h"
#include "imx6ul.h"
#include "MCIMX6Y2.h"
#include "i2c.h"
#include "delay.h"
#include "stdio.h"
#include "stdio.h"

void AP3216C_Init() {
    // 引脚复用
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);

    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0x70B0);

    // // 初始化I2C主机
    I2C_Init(I2C1);

    // /// 复位
    AP3216C_WriteOneByte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIGURATION_REGISTER, 0x4);
    HightPrecisionDelayMS(50);
    AP3216C_WriteOneByte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIGURATION_REGISTER, 0x3);
    unsigned char val = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIGURATION_REGISTER);
    printf("AP3216C_SYSTEM_CONFIGURATION_REGISTER: %#x \r\n", val);
}

/// 从人机读取一个字节的数据
unsigned char AP3216C_ReadOneByte(unsigned int slaveAddress, unsigned char reg) {
    unsigned char value = 0;
    struct i2c_transfer xfer;
    xfer.slaveAddress = slaveAddress;
    xfer.direction = I2CDirection_READ;
    xfer.subaddress = reg;
    xfer.subaddressSize = 1;
    xfer.data = &value;
    xfer.dataSize = 1;
    I2C_Master_Transfer(I2C1, &xfer);
    return value;
}
unsigned char AP3216C_WriteOneByte(unsigned int slaveAddress, unsigned char reg, unsigned char data) {
    unsigned char value = data;
    struct i2c_transfer xfer;
    xfer.slaveAddress = slaveAddress;
    xfer.direction = I2CDirection_WRITE;
    xfer.subaddress = reg;
    xfer.subaddressSize = 1;
    xfer.data = &value;
    xfer.dataSize = 1;
    return I2C_Master_Transfer(I2C1, &xfer);
}

void AP3216C_ReadData(unsigned short *ir, unsigned short *als, unsigned short *ps) {

    // unsigned char irDataLow = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_IR_DATA_LOW);
    // unsigned char irDataHigh = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_IR_DATA_HIGH);

    // unsigned char alsDataLow = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_ALS_DATA_LOW);
    // unsigned char alsDataHigh = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_ALS_DATA_HIGH);

    // unsigned char psDataLow = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_PS_DATA_LOW);
    // unsigned char psDataHigh = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_PS_DATA_HIGH);

    // // 判断ir与ps是否有效
    // char ir_ps_valid = (irDataLow >> 8) & 0x01;
    // if(ir_ps_valid) {
    //     printf("IR and PS unvalid \r\n");
    // } else {
    //     *ir = (((unsigned short)irDataHigh) << 2) | (irDataLow & 0x3);
    //     *ps = (((unsigned short)psDataHigh & 0x3F)) << 6 | (psDataLow & 0xF);
    // }
    // *als = (((unsigned short)alsDataHigh) << 8) | alsDataLow;
    unsigned char buf[6];
    unsigned char i;

	/* 循环读取所有传感器数据 */
    for(i = 0; i < 6; i++)	
    {
        buf[i] = AP3216C_ReadOneByte(AP3216C_ADDR, AP3216C_PS_DATA_LOW + i);	
    }
	
    if(buf[0] & 0X80) 	/* IR_OF位为1,则数据无效 */
		*ir = 0;					
	else 				/* 读取IR传感器的数据   		*/
		*ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 			
	
	*als = ((unsigned short)buf[3] << 8) | buf[2];	/* 读取ALS传感器的数据 			 */  
	
    if(buf[4] & 0x40)	/* IR_OF位为1,则数据无效 			*/
		*ps = 0;    													
	else 				/* 读取PS传感器的数据    */
		*ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 	
}