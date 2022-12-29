#ifndef CONST_KEY0_H
#define CONST_KEY0_H
void Key0_Interrupt_Irq_Handler(unsigned int gicciar, void *context);
void Key0_Init();
void Key0_Filter_Init();
void Key0_Epit1_Interrupt_Irq_Handler(unsigned int gicciar, void *context);
void Key0_Filter_Start();
void Key0_Filter_Stop();
#endif