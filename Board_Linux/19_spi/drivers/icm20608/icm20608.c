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

void icm20608_test() {
    u8 value = icm20608_read(ICM20_WHO_AM_I);
    printf("ICM20608 who am i : %#x \r\n", value);
}