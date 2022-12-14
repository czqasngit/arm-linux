#include <stdio.h>
#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"

int main(void)
{
    Clk_Freq_Init();
    Clk_Init();
    Led_Init();
    Beep_Init();
    while (1)
    {
        Delay(500);
        Led_On();
        Beep_On();
        Delay(500);
        Led_Off();
        Beep_Off();
    }

    return 0;
}