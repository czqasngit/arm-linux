#ifndef _BEEP_H_
#define _BEEP_H_

#include "imx6ul.h"

void Beep_Init();
void Beep_On();
void Beep_Toggle();
void Beep_Off();
void Beep_Set(char value);
#endif