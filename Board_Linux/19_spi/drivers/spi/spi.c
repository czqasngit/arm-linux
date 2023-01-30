
#include "spi.h"
#include "imx6ul.h"
#include "MCIMX6Y2.h"
#include "stdio.h"
#include "delay.h"
#include "clk.h"

void spi_init(ECSPI_Type *base) {

    clk_spi_init(); // 初始化时时钟

    base->CONREG = 0; // 清零 
    base->CONREG |= (1 << 0); // 使能spi
    base->CONREG |= (1 << 3); // 向TXFIFO写入数据时立即开始数据对外传输
    base->CONREG |= (1 << 4); // 7-4对应ss3 - ss0四个片选,这里使用的是ss0,所以将bit[7:5]都设置为0，bit4设置为1
    base->CONREG |= (7 << 20); // 设置突发长度(一次数据传输的长度)为一个字节(8 bit)

    base->CONFIGREG = 0;

    base->PERIODREG = 0x2000;// 采样周期(时钟周期)

    /* icm20608 的最高时钟只支持8MHz, 这里设置成6MHz */
    base->CONREG &= ~(0xF << 12); // bit 15:12 清零
    base->CONREG &= ~(0xF << 8);  // bit 11:8 清零
    base->CONREG |= (9 << 12); // 一级分频设置成0,此时已经得到6MHz, 二级分频不设置

}

unsigned char spi_read(ECSPI_Type *base) {
    base->CONREG &= ~(3 << 18); // 选择ss0
    while(((base->STATREG >> 3) & 0x1) == 0);
    uint32_t value = base->RXDATA; // 取低8位
    return value;
}
void spi_write(ECSPI_Type *base, unsigned char value) {
    base->CONREG &= ~(3 << 18); // 选择ss0
    while(((base->STATREG >> 0) & 0x1) == 0); // 0: FIFO数据不为空, 1: FIFO数据为空，此时可以发送数据
    base->TXDATA = (uint32_t)value;
}

unsigned char spich0_readwrite_byte(ECSPI_Type *base, unsigned char txdata)
{ 
	uint32_t  spirxdata = 0;
	uint32_t  spitxdata = txdata;

    /* 选择通道0 */
	base->CONREG &= ~(3 << 18);
	base->CONREG |= (0 << 18);

  	while((base->STATREG & (1 << 0)) == 0){} /* 等待发送FIFO为空 */
		base->TXDATA = spitxdata;
	
	while((base->STATREG & (1 << 3)) == 0){} /* 等待接收FIFO有数据 */
		spirxdata = base->RXDATA;
	return spirxdata;
}