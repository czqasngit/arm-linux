#include "icm20608.h"
#include "spi.h"
#include "imx6ul.h"
#include "MCIMX6Y2.h"
#include "stdio.h"
#include "delay.h"
#include "gpio.h"

void icm20608_init() {
    icm20608_iomux_init();
    spi_init(ECSPI3);
    icm20608_test();
    // 初始化icm20608的传感器相关参数
    icm20608_reg_init();
}

// 引脚复用
void icm20608_iomux_init() {
    // 引脚复用 CLK, MOSI, MISO
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0x10B0);

    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0x10B0);

    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0x10B0);

    // 片选引用,当前底板将SPI外设的CS引脚接到了GPIO1_IO20上(ss0接到GPIO1_IO20)
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0x10b0);
    GPIO_Init(GPIO1, 20, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 0, GPIO_INTERRUPT_MODE_NO_INTERRUPT});
}

u8 icm20608_read(u8 reg) {
    // 输出0，表示当前片选拉低，选中当前SPI外设
    ICM_CS_SELECTED(0);
    reg |= (1 << 7); // bit7 0: 写数据， 1: 读数据
    spi_write(ECSPI3, reg);
    // spi_write(ECSPI3, 0xF);// icm20608 需要至少16个时钟周期才可以完成一次读写,这里写一次无效数据,
    u8 value = spi_read(ECSPI3);
    // 拉高,停止访问
    ICM_CS_SELECTED(1); 
    return value; 
}

void icm20608_write(u8 reg, u8 data) {
    ICM_CS_SELECTED(0);
    reg &= ~(1 << 7);
    spi_write(ECSPI3, reg);
    spi_write(ECSPI3, data);
    ICM_CS_SELECTED(1);
}

void icm20608_reg_init() {
    icm20608_write(ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write(ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write(ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write(ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write(ICM20_ACCEL_CONFIG2, 0x04); 	/* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write(ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write(ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write(ICM20_FIFO_EN, 0x00);		/* 关闭FIFO						*/
}

void icm20608_test() {
    u8 value = icm20608_read(ICM20_WHO_AM_I);
    printf("ICM20608 ID VALUE : %#x \r\n", value);
}