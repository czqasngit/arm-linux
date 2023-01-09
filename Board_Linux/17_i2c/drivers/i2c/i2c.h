#ifndef _I2C_H
#define _I2C_H


enum I2CStatus {
    OK = 0, 
    BUSY = 1, 
    IDLE = 2, 
    NAK = 3, 
    ARBI = 4/*仲裁丢失*/, 
    TIMEOUT = 5
}
enum I2CDirection {
    WRITE = 0,
    READ = 1
}
#include "imx6ul.h"
#include "MCIMX6Y2.h"

#endif