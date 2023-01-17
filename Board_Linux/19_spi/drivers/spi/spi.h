#ifndef _SPI_H_
#define _SPI_H_

#include "imx6ul.h"
#include "MCIMX6Y2.h"
#include "stdio.h"
#include "delay.h"

void spi_init(ECSPI_Type *base);
unsigned char spi_read(ECSPI_Type *base);
void spi_write(ECSPI_Type *base, unsigned char value);

#endif