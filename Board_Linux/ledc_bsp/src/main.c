#include <stdio.h>
#include "delay.h"
#include "clk.h"
#include "led.h"

int main(void)
{
    Clk_Init();
    Led_Init();
    while (1)
    {
        Delay(500);
        Led_On();
        Delay(500);
        Led_Off();
    }

    return 0;
}