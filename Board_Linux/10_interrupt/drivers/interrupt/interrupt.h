#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include "imx6ul.h"

void Init_Interrupt();
void system_irq_handler(int value);
#endif