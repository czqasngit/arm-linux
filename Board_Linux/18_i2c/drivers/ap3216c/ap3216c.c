#include "ap3216c.h"
#include "imx6ul.h"
#include "MCIMX6Y2.h"
#include "i2c.h"

void AP3216C_Init() {
    // 引脚复用
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1)
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70b0);

    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70b0);

    // 初始化I2C主机
    I2C_Init(I2C1);
}

/// 从人机读取一个字节的数据
unsigned char AP3216C_ReadOneByte(unsigned int slaveAddress, unsigned char register) {
    struct i2c_transfer xfer;
    xfer.slaveAddress = AP3216C_ADDR;
    xfer.direction = I2CDirection_READ;
    I2C_Master_Transfer(I2C1, &xfer);

}