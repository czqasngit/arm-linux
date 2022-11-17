
#include "imx6ul.h"

void Clk_Init()
{
    CCM->CCGR1 = 0xFFFFFFFF;
}