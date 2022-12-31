#ifndef _EPIT_H
#define _EPIT_H
void Epit_Init();
void Epit1_Interrupt_Irq_Handler(unsigned int gicciar, void *context);
#endif