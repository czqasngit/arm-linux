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

/// LCD显存起始地址
#define LCD_FRAMEBUFFER_ADDR    (0x89000000)

// 屏幕参数
typedef struct _LCD_Device_Info {
    unsigned short width;       // 屏幕宽
    unsigned short height;      // 屏幕高
    unsigned char byte_per_pixel;    // 每个像素所占用的字节数
    unsigned short vspw;
    unsigned short vbp;
    unsigned short vfp;
    unsigned short hspw;
    unsigned short hbp;
    unsigned short hfp;
    unsigned int frameBuffer;       // 显存起始地址
    unsigned int foreColor;         // 前景色
    unsigned int backColor;         // 背景色
} LCD_Device_Info;

void LCD_Init();
void LCD_Init_Device_Info();
int LCD_Device_Id();
void LCD_IO_Init();
void LCD_Open_Background_Light();
void LCD_Soft_Reset();
void LCD_Soft_Reset_Complete();
void LCD_Enable();
void LCD_Disable();
void LCD_Register_Config();
void LCD_DrawPoint(unsigned short x, unsigned short y, unsigned int color);
unsigned int LCD_GetPointColor(unsigned short x, unsigned short y);
void LCD_Clean_Screen(unsigned int color);
#endif