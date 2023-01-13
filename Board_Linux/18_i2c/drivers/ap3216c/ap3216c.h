#ifndef _AP3216C_H_
#define _AP3216C_H_

#define AP3216C_ADDR                            0x1E        // I2C Slave Address

#define AP3216C_SYSTEM_CONFIGURATION_REGISTER   0x00
#define AP3216C_INPUT_STATUS                    0x01
#define AP3216C_INT_MANAGER                     0x02
#define AP3216C_IR_DATA_LOW                     0X0A
#define AP3216C_IR_DATA_HIGH                    0X0B
#define AP3216C_ALS_DATA_LOW                    0X0C
#define AP3216C_ALS_DATA_HIGH                   0X0D
#define AP3216C_PS_DATA_LOW                     0X0E
#define AP3216C_PS_DATA_HIGH                    0X0F

void AP3216C_Init();
unsigned char AP3216C_ReadOneByte(unsigned int slaveAddress, unsigned char register);
unsigned char AP3216C_WriteOneByte(unsigned int slaveAddress, unsigned char register, unsigned char data);
// 读取传感器数据
void AP3216C_ReadData(unsigned short *ir, unsigned short *als, unsigned short *ps);
#endif