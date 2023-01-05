#include "lcd.h"
#include "gpio.h"
#include "const.h"
#include "stdio.h"
#include "delay.h"
#include "clk.h"

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
    /// 配置LCD所需要的时钟频率,通过频率参数推断出来的满足刷新刷新60PFS的最小频率参数
    Clk_LCD_Init(42, 4, 8);
    /// 配置LCD相关的寄存器参数
    LCD_Register_Config();
    LCD_Enable();
    HightPrecisionDelayMS(20);
    LCD_Clean_Screen(0x00FFFFFF);
}
/* 根据device id(正点原子开发板特有的数据),其它开发板直接根据屏幕给定的参数来初始化LCD即可 */
void LCD_Init_Device_Info() {
     int lcdDevicdID = LCD_Device_Id();
    printf("LCD ID: %#x \r\n", lcdDevicdID);
    /*if(lcdDevicdID == ATK4342) {
        lcd_device_info.width = 480;
        lcd_device_info.height = 272;
        lcd_device_info.hspw = 1;
        lcd_device_info.hbp = 40;
        lcd_device_info.hfp = 5;
        lcd_device_info.vspw = 1;
        lcd_device_info.vbp = 8;
        lcd_device_info.vfp = 8;
    } else*/ if(lcdDevicdID == ATK4384) {
        lcd_device_info.width = 800;
        lcd_device_info.height = 480;
        lcd_device_info.hspw = 48;
        lcd_device_info.hbp = 88;
        lcd_device_info.hfp = 40;
        lcd_device_info.vspw = 3;
        lcd_device_info.vbp = 32;
        lcd_device_info.vfp = 13;
        /// (VSPW+VBP+LINE+VFP) * (HSPW + HBP + HOZVAL + HFP)
        Clk_LCD_Init(42, 4, 8);
    }/* else if(lcdDevicdID == ATK7084) {
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
    }*/
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
void LCD_Enable() {
    LCDIF->CTRL |= (1 << 0);
}
void LCD_Disable() {
    LCDIF->CTRL |= (0 << 0);
}
/// 寄存器配置
void LCD_Register_Config() {

    /// 首先将寄存器清0
    LCDIF->CTRL = 0;
    /*
        bit:1, 这里设置成0,寄存器清空不作处理
        Used only when WORD_LENGTH = 3, i.e. 24-bit. Note that this applies to both packed and unpacked 24- bit data.
        0x0 ALL_24_BITS_VALID — Data input to the block is in 24 bpp format, such that all RGB 888 data is contained in 24 bits.
        0x1 DROP_UPPER_2_BITS_PER_BYTE — Data input to the block is actually RGB 18 bpp, but there is 1 color per byte, hence the upper 2 bits in each byte do not contain any useful data, and should be dropped.

        bit:5, 设置工作在主机模式
        Set this bit to make the eLCDIF act as a bus master.

        bit 9:8 设置RGB数据输入数据格式,这里设置成24bit数据(RGB888)

        bit 11:10 设置LCD数据总线位宽, 24bit

        bit 13:12 数据大小端在传输前的交换模式,这里不需要交互,由于上面清0了这里不用再次设置

        bit 17 设置LCD模式为DOTCLK

        bit 19,DOTCLK与DVI模式必须设置成1, VSYNC与MPU设置为0
    */
    LCDIF->CTRL |= (1 << 5);
    LCDIF->CTRL |= (3 << 8);
    LCDIF->CTRL |= (3 << 10);
    LCDIF->CTRL |= (1 << 17);
    LCDIF->CTRL |= (1 << 19);

    /*
        bit 19:16 设置4个字节中的哪几个字节是有效的传输位,这里使用的是ARGB一个32位(4字节来表示一个颜色)，A不传输,RGB表示低3位0111，所以值设置成7
    */
    LCDIF->CTRL1 &= ~(7 << 16);

    /*
        bit 15:0    屏幕宽
        bit 31:16   屏幕高
    */
    LCDIF->TRANSFER_COUNT = 0;
    LCDIF->TRANSFER_COUNT |= (lcd_device_info.width << 0);
    LCDIF->TRANSFER_COUNT |= (lcd_device_info.height << 16);

    /*
        bit 17:0 VSYNC_ PULSE_ WIDTH
        bit 20 V-Sync 信号宽度单位 设置成0 表示按时钟周期来显示VSYNC_PULSE_WIDTH，设置成1则以完整的水平线计数
        bit 21 DOTCLK模式设置为1，以完整的水平线计数
        bit 24 高电平触发数据传输还是低电平触发,当前的LCD RGB屏幕是高电平所以设置成1
        bit 25 DOTCLK_POL极性,0表示低电平是有效期, 1表示高电平是有效其，当前LCD RGB是低电平有效，所以设置成0
        bit 26 HSYNC_POL极性, 低电平有效
        bit 27 VSYNC_POL极性, 低电平活跃
        bit 28 设置成1，表示在DOTCLK模式下使能,从而与其它VSYNC,HSYNC,DOTCLK信号一起组成RGB接口信号
    */
    LCDIF->VDCTRL0 = 0;
    LCDIF->VDCTRL0 |= (lcd_device_info.vspw << 0);
    LCDIF->VDCTRL0 |= (1 << 20);
    LCDIF->VDCTRL0 |= (1 << 21);
    LCDIF->VDCTRL0 |= (1 << 24); 

    /*
        VSYNC的总信号,在前面的V-Sync里面已经将单位设置成了完整的一行水平宽为一个单位
        所以这里只需要设置一个VSYNC的垂直一行的数量 VSPW + VBP + LINES(HEIGHT) + VFP
    */
    LCDIF->VDCTRL1 = 0;
    LCDIF->VDCTRL1 = lcd_device_info.vspw + lcd_device_info.vbp + lcd_device_info.height + lcd_device_info.vfp; 

    /*
        HSYNC 一个信号的周期时钟
        bit 31: 18 hspw
        bit 17: 0  hsync 一个周期的完整时钟

    */
    LCDIF->VDCTRL2 = 0;
    LCDIF->VDCTRL2 |= lcd_device_info.hspw << 18;
    LCDIF->VDCTRL2 |= lcd_device_info.hspw + lcd_device_info.hbp + lcd_device_info.width + lcd_device_info.hfp;

    /*
        bit 15: 0 垂直方向上(从上到下)需要等待的时钟周期数(单位是: VSYNC_PERIOD_UNIT,一个单位则是一个完整的水平周期)
        bit 27: 16 水平方向上(从左到右)需要等待的时钟周期数量(单位是是一个clk)
    */
   LCDIF->VDCTRL3 |= (lcd_device_info.vspw + lcd_device_info.vbp);
   LCDIF->VDCTRL3 |= (lcd_device_info.hspw + lcd_device_info.hbp) << 16;

   /*
        bit 17: 0 屏幕有效的显示宽
        bit 18  启用信号位
   */
    LCDIF->VDCTRL4 |= (lcd_device_info.width << 0);
    LCDIF->VDCTRL4 |= (1 << 18);

    /* 当前显存起始地址*/
    LCDIF->CUR_BUF = lcd_device_info.frameBuffer;
    /* 下一帧显存起始地址 */
    LCDIF->NEXT_BUF = lcd_device_info.frameBuffer;
}


void LCD_DrawPoint(unsigned short x, unsigned short y, unsigned int color) {
    // 显存存起始地址
    unsigned int lcdFrameBufferAddr = lcd_device_info.frameBuffer;
    // 显存绘制点偏移地址
    unsigned int offset = (lcd_device_info.width * y + x) * lcd_device_info.byte_per_pixel;
    // 需要绘制的点的偏移地址
    unsigned int drawPointAddr = lcdFrameBufferAddr + offset;
    // 直接将这个地址赋值给一个指针,使用指针即可访问到对应的内存地址
    unsigned int *drawPointPointer = (unsigned int *)drawPointAddr;
    // 给指针指向的内存写入一个颜色
    *drawPointPointer = color;
}

unsigned int LCD_GetPointColor(unsigned short x, unsigned short y) {
    // 显存存起始地址
    unsigned int lcdFrameBufferAddr = lcd_device_info.frameBuffer;
    // 显存绘制点偏移地址
    unsigned int offset = (lcd_device_info.width * y + x) * lcd_device_info.byte_per_pixel;
    // 需要绘制的点的偏移地址
    unsigned int drawPointAddr = lcdFrameBufferAddr + offset;
    // 直接将这个地址赋值给一个指针,使用指针即可访问到对应的内存地址
    unsigned int *drawPointPointer = (unsigned int *)drawPointAddr;
    return *drawPointPointer;
}
// 清屏幕
void LCD_Clean_Screen(unsigned int color) {
    // 使用unsigned int *类型去读取显存
    unsigned int *p_start = (unsigned int *)lcd_device_info.frameBuffer;
    // 得到总共有多少像素点
    unsigned int size = lcd_device_info.width * lcd_device_info.height;
    for(unsigned int i = 0; i < size; i ++) {
        // 给每个像素点都写入color
        p_start[i] = color;
    }
}