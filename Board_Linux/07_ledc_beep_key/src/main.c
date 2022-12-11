#include <stdio.h>
#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "key0.h"

int main(void)
{
    Clk_Init();
    Led_Init();
    Beep_Init();
    Key0_Init();
    
    int pressed = 0;
    while (1)
    {
        Delay(10);

        if (Key0_GetValue())
        {

            Led_On();
            Beep_On();
        }
        else
        {
            Led_Off();
            Beep_Off();
        }
    }

    return 0;
}