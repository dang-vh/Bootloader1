#ifndef _LPUART_H_
#define _LPUART_H_
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "queue.h"
#include "MKE16Z4.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern volatile uint8_t RxIntFlag;
extern Queue_Types testQueue;

void LPUART0_IRQHandler(void);
void UART_ClockPort_Init();
void UART_Init(void);
void UART_SendString(uint8_t* str, uint8_t Len);
void UART_SendQueueData(Queue_Types *Queue);
void StartReceiveInt(void);
















#endif /* _LPUART_H_ */
