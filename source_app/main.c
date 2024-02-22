
#include "MKE16Z4.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "lpuart.h"
#include "lpit.h"
#include "adc.h"

/***********************
#include "Port.h"
#include "GPIO.h"
#include "LPIT.h"
*/

volatile uint8_t RxIntFlag = 0;

// Array and Queue
uint32_t Queue_Arr[QUEUE_MAX_SIZE] = {0};
Queue_Types testQueue = {
	0, -1, QUEUE_MAX_SIZE,
	&Queue_Arr};



/*!
 * @brief Main function
 */
int main(void)
{
	Red_led_Init();
	ADC_Init();
	UART_Init();
	LPIT_Init();

	__NVIC_SetPriority(LPUART0_IRQn, 0);
	__NVIC_SetPriority(ADC0_IRQn, 1);
	__NVIC_SetPriority(LPIT0_IRQn, 1);

	LPIT_StartTimer();

	while (1)
	{
		StartReceiveInt();
		if (1U == RxIntFlag)
		{
			RxIntFlag = 0;
		}
	}
}
