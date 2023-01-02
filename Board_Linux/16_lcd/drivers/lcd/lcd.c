#include "lcd.h"
#include "gpio.h"
#include "const.h"
#include "stdio.h"


void LCD_Init() {
    /* 正点原子的开发板使用的RGB屏幕,首先需要读取一下LCD ID,目的是适配不同的屏幕 */
    int lcdDevicdID = LCD_Device_Id();
    printf("LCD ID: %#x \n", lcdDevicdID);
}
/*
    使用了三个IO来做这个ID值,通过三个开关来复用这三个IO,
    这三个IO在启动的时候i.MX6ULL也在使用
    LCD_DATA23: M0, LCD_DATA15: M1, LCD_DATA07: M2
    M2: M1: M0
    000: id-0x4342  4.3     480x272
    001: id-0x7084  7       800x480
    010: id-0x7016  7       1024x600
    101: id-0x1018  10.1    1280x800
    100: id-0x4384  4.3     800x480
*/
int LCD_Device_Id() {
    /* 打开模拟开关,读取M2, M1, M0 */
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_GPIO3_IO03, CONST_GPIO_OUTPUT_PAD_VALUE);
    GPIO_Init(GPIO3, 3, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});

    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_GPIO3_IO12, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_GPIO3_IO12, CONST_GPIO_INPUT_PAD_VALUE);
    GPIO_Init(GPIO3, 12, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});

    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_GPIO3_IO20, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_GPIO3_IO20, CONST_GPIO_INPUT_PAD_VALUE);
    GPIO_Init(GPIO3, 20, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});


    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_GPIO3_IO28, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_GPIO3_IO28, CONST_GPIO_INPUT_PAD_VALUE);
    GPIO_Init(GPIO3, 28, (GPIO_CONFIG){GPIO_DIRECTION_INPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});

    char m2 = GPIO_ReadValue(GPIO3, 12);
    char m1 = GPIO_ReadValue(GPIO3, 20);
    char m0 = GPIO_ReadValue(GPIO3, 28);

    int id = (m0 << 0) | (m1 << 1) | (m2 << 2);
    if(id == 0) return ATK4342;
    else if(id == 1) return ATK7084;
    else if(id = 2) return ATK7016;
    else if(id == 5) return ATK1018;
    else if(id == 4) return ATK4384;
    else return ATK4384;
}