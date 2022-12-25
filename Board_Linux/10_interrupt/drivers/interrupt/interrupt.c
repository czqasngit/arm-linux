#include "interrupt.h"
void Init_Interrupt()
{
    GIC_Init();
    __set_VBAR(0x87800000);
}
void system_irq_handler(int value)
{
}