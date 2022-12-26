#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include "imx6ul.h"

typedef void (*Interrupt_Irq_Handler)(unsigned int gicciar, void *context);

typedef struct _Interrupt_Irq_Data
{
    Interrupt_Irq_Handler handler;
    void *context;
} Interrupt_Irq_Data;

void Init_Interrupt();
void Interrupt_Default_Irq_Handler(unsigned int gicciar, void *context);
void Interrupt_Irq_Handler_Register(IRQn_Type type, Interrupt_Irq_Handler handler, void *context);
void system_irq_handler(unsigned int gicciar);

#endif