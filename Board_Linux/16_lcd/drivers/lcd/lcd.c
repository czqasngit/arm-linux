#include "lcd.h"
#include "gpio.h"
#include "const.h"
#include "stdio.h"
#include "delay.h"

LCD_Device_Info lcd_device_info;

void LCD_Init() {
    /* 正点原子的开发板使用的RGB屏幕,首先需要读取一下LCD ID,目的是适配不同的屏幕 */
    LCD_Init_Device_Info();
    LCD_IO_Init();
    LCD_Open_Background_Light();
    // LCD软复位 10ms
    LCD_Soft_Reset();
    HightPrecisionDelayMS(10);
    LCD_Soft_Reset_Complete();
}
/* 根据device id(正点原子开发板特有的数据),其它开发板直接根据屏幕给定的参数来初始化LCD即可 */
void LCD_Init_Device_Info() {
     int lcdDevicdID = LCD_Device_Id();
    printf("LCD ID: %#x \r\n", lcdDevicdID);
    if(lcdDevicdID == ATK4342) {
        lcd_device_info.width = 480;
        lcd_device_info.height = 272;
        lcd_device_info.hspw = 1;
        lcd_device_info.hbp = 40;
        lcd_device_info.hfp = 5;
        lcd_device_info.vspw = 1;
        lcd_device_info.vbp = 8;
        lcd_device_info.vfp = 8;
    } else if(lcdDevicdID == ATK4384) {
        lcd_device_info.width = 800;
        lcd_device_info.height = 480;
        lcd_device_info.hspw = 48;
        lcd_device_info.hbp = 88;
        lcd_device_info.hfp = 40;
        lcd_device_info.vspw = 3;
        lcd_device_info.vbp = 32;
        lcd_device_info.vfp = 13;
    } else if(lcdDevicdID == ATK7084) {
        lcd_device_info.width = 800;
        lcd_device_info.height = 480;
        lcd_device_info.hspw = 1;
        lcd_device_info.hbp = 46;
        lcd_device_info.hfp = 210;
        lcd_device_info.vspw = 1;
        lcd_device_info.vbp = 23;
        lcd_device_info.vfp = 22;
    } else if(lcdDevicdID == ATK7016) {
        lcd_device_info.width = 1024;
        lcd_device_info.height = 600;
        lcd_device_info.hspw = 20;
        lcd_device_info.hbp = 140;
        lcd_device_info.hfp = 160;
        lcd_device_info.vspw = 3;
        lcd_device_info.vbp = 20;
        lcd_device_info.vfp = 12;
    }
    lcd_device_info.byte_per_pixel = 4;     // 每个像素4个字节
    // 显存起始地址,在内存里面开辟一块存储作为显存
    // 当前开发板的DDR内存起始地址是0x80000000
    // DDR总共是512MB,结束地址是: 0x80000000 + 0x20000000 = 0xA00000000
    // 显存起始地址是0x89000000,到结束还有368MB
    lcd_device_info.frameBuffer = LCD_FRAMEBUFFER_ADDR;     
    lcd_device_info.foreColor = 0x00FFFFFF; // 画笔白色
    lcd_device_info.backColor = 0x00000000; // 背景黑色
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
    else if(id == 2) return ATK7016;
    else if(id == 5) return ATK1018;
    else if(id == 4) return ATK4384;
    else return ATK4384;
}

// 屏幕IO初始化
void LCD_IO_Init() {
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA00_LCDIF_DATA00, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA01_LCDIF_DATA01, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA02_LCDIF_DATA02, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA03_LCDIF_DATA03, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA04_LCDIF_DATA04, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA05_LCDIF_DATA05, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA06_LCDIF_DATA06, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_LCDIF_DATA07, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA08_LCDIF_DATA08, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA09_LCDIF_DATA09, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA10_LCDIF_DATA10, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA11_LCDIF_DATA11, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA12_LCDIF_DATA12, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA13_LCDIF_DATA13, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA14_LCDIF_DATA14, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_LCDIF_DATA15, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA16_LCDIF_DATA16, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA17_LCDIF_DATA17, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA18_LCDIF_DATA18, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA19_LCDIF_DATA19, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA20_LCDIF_DATA20, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA21_LCDIF_DATA21, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA22_LCDIF_DATA22, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_LCDIF_DATA23, 0);

    IOMUXC_SetPinMux(IOMUXC_LCD_CLK_LCDIF_CLK, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_ENABLE_LCDIF_ENABLE, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_HSYNC_LCDIF_HSYNC, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_LCDIF_VSYNC, 0);

    // IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00, 0xB9);

    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA01_LCDIF_DATA01, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA02_LCDIF_DATA02, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA03_LCDIF_DATA03, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA04_LCDIF_DATA04, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA05_LCDIF_DATA05, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA06_LCDIF_DATA06, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_LCDIF_DATA07, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA08_LCDIF_DATA08, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA09_LCDIF_DATA09, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA10_LCDIF_DATA10, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA11_LCDIF_DATA11, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA12_LCDIF_DATA12, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA13_LCDIF_DATA13, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA14_LCDIF_DATA14, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_LCDIF_DATA15, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA16_LCDIF_DATA16, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA17_LCDIF_DATA17, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA18_LCDIF_DATA18, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA19_LCDIF_DATA19, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA20_LCDIF_DATA20, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA21_LCDIF_DATA21, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA22_LCDIF_DATA22, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_LCDIF_DATA23, 0xB9);

    IOMUXC_SetPinConfig(IOMUXC_LCD_CLK_LCDIF_CLK, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_ENABLE_LCDIF_ENABLE, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_HSYNC_LCDIF_HSYNC, 0xB9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_LCDIF_VSYNC, 0xB9);

   

}

void LCD_Open_Background_Light() {
     /* 背光,如果背光不点亮屏幕就是黑的 */
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_GPIO1_IO08, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_GPIO1_IO08, 0x01b0);
    GPIO_Init(GPIO1, 8, (GPIO_CONFIG){GPIO_DIRECTION_OUTPUT, 1, GPIO_INTERRUPT_MODE_NO_INTERRUPT});
    printf("open lcd background light \r\n");
}

void LCD_Soft_Reset() {
    LCDIF->CTRL |= 1 << 31;
}
void LCD_Soft_Reset_Complete() {
    LCDIF->CTRL &= ~(1 << 31);
}