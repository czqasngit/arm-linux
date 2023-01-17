#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "interrupt.h"
#include "key0.h"
#include "epit.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"
#include "lcdapi.h"
#include "rtc.h"
#include "ap3216c.h"
#include "icm20608.h"

 void imx6ul_hardfpu_enable(void)
{
	uint32_t cpacr;
	uint32_t fpexc;

	/* 使能NEON和FPU */
	cpacr = __get_CPACR();
	cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk))
		   |  (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);
	__set_CPACR(cpacr);
	fpexc = __get_FPEXC();
	fpexc |= 0x40000000UL;	
	__set_FPEXC(fpexc);
}


int main(void)
{
    printf("main function start \r\n");
    imx6ul_hardfpu_enable();
    /// 初始化中断服务函数
    Init_Interrupt();
    Clk_Freq_Init();
    Clk_Init();
    UART1_Init();
    Led_Init();
    Beep_Init();
    // Key0_Init();
    // Epit_Init(500);
    Delay_Init();
    LCD_Init();
    RTC_Init();
    // AP3216C_Init();
    icm20608_init();
    lcd_device_info.foreColor = 0x00FF0000;
    lcd_device_info.backColor = 0x00FFFFFF;
    
    lcd_show_string(50, 50, 260, 32, 32, "Hello I.MX6ULL");
    lcd_show_string(50, 90, 260, 24, 24, "ARM ON Linux");
    lcd_show_string(50, 130, 260, 24, 24, "Write Using C Language");

    while (1) {
        Led_On();
        HightPrecisionDelayMS(500);
        Led_Off();
        HightPrecisionDelayMS(500);
        
    }




    return 0;
}