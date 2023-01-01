#ifndef _H_UART_
#define _H_UART_

void UART1_Init();
void UART1_IO_Init();
void UART1_Disable();
void UART1_Enable();
/// Reset the transmit and receive state machines, all FIFOs and register USR1, USR2, UBIR, UBMR, UBRC , URXD, UTXD and UTS[6-3].
void UART1_Soft_Reset();
void UART1_Config();
void Uart1_SendChar(char c);
void Uart1_SendString(char *s);
char Uart1_ReadChar();
#endif