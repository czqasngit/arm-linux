#ifndef _H_LCD_
#define _H_LCD_

#include "imx6ul.h"
#include "MCIMX6Y2.h"

/* 屏幕ID宏定义 */
#define ATK4342 0x4342  /// 4.3     480x272
#define ATK4384 0x4384  /// 4.3     800x480
#define ATK7084 0x7084  /// 7.0     800x480
#define ATK7016 0x7016  /// 4.0     1024x600
#define ATK1018 0x1018  /// 10.1    1280x800

int LCD_Device_Id();
void LCD_Init();
void LCD_IO_Init();
void LCD_Open_Background_Light();
#endif