#include "lpuart.h"
void LPUART0_IRQHandler(void)
{
	char Data = LPUART0->DATA;
	if (Data == 'D')
	{
		RxIntFlag = 1;
		UART_SendQueueData(&testQueue);
	}
}

void UART_ClockPort_Init()
{
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1);

	PORTB->PCR[1] &= PORT_PCR_MUX_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(0b010);
	PORTB->PCR[1] |= PORT_PCR_PE(1);
	PORTB->PCR[1] |= PORT_PCR_PS(1);

	PORTB->PCR[0] &= PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(0b010);
	PORTB->PCR[0] |= PORT_PCR_PE(1);
	PORTB->PCR[0] |= PORT_PCR_PS(1);

	// Disable SIRC
	SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN(0);

	// Config SIRCDIV to divide by 4
	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(3);

	// Enable the UART & PORT module clock
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(2U);
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC(1);
}

void UART_Init(void)
{
	// Asynch module clock(2M)
	UART_ClockPort_Init();
	// disable

	// Baudrate (9600)
	// BAUD divider
	LPUART0->BAUD &= ~(LPUART_BAUD_SBR_MASK);
	LPUART0->BAUD |= LPUART_BAUD_SBR(52);
	// OSR Divide
	LPUART0->BAUD |= LPUART_BAUD_BOTHEDGE_MASK;
	LPUART0->BAUD = (LPUART0->BAUD & ~LPUART_BAUD_OSR_MASK) | LPUART_BAUD_OSR(3U);
	// parity
	LPUART0->CTRL &= ~(LPUART_CTRL_PE_MASK);
	// Frame
	// Data length - BAUD[M10],CTRL[M7],CTRL[M]- tranfer 8 bit
	LPUART0->BAUD &= ~(LPUART_BAUD_M10_MASK);
	LPUART0->CTRL &= ~(LPUART_CTRL_M7_MASK);
	LPUART0->CTRL &= ~(LPUART_CTRL_M_MASK);
	// Number of Stop Bits
	LPUART0->BAUD &= ~(LPUART_BAUD_SBNS_MASK);
	// LSB first MSB first
	LPUART0->STAT &= ~(LPUART_STAT_MSBF_MASK);
	// interrupt or polling
	// TX interrupt: TIE/ TCIE
	LPUART0->CTRL |= LPUART_CTRL_RIE(1U);
	//	LPUART0->CTRL|=LPUART_CTRL_TCIE(1U);
	// NVIC
	NVIC_EnableIRQ(LPUART0_IRQn);
}

void UART_SendString(uint8_t *str, uint8_t Len)
{
	uint8_t index;
	// enable tranfer
	LPUART0->CTRL |= LPUART_CTRL_TE(1U);
	for (index = 0; index < Len; index++)
	{
		// Wait ultil buffer is empty
		while (0U == (LPUART0->STAT & LPUART_STAT_TDRE_MASK));
		// Send Data
		LPUART0->DATA = str[index];
	}

	// Wait until sending complete
	while (0U == (LPUART0->STAT & LPUART_STAT_TC_MASK));
	// disable tranfer
	LPUART0->CTRL &= ~(LPUART_CTRL_TE_MASK);
}

void UART_SendQueueData(Queue_Types *Queue)
{
	int index = Queue->Front;

	if (Queue->Size == 0)
	{
		// Queue is empty
		return;
	}

	do
	{
		// Data to string
		char dataStr[12]; //
		snprintf(dataStr, sizeof(dataStr), "%lu  ", Queue->QueueArr[index]);
		UART_SendString((uint8_t *)dataStr, strlen(dataStr));

		// Update index
		index = (index + 1) % QUEUE_MAX_SIZE;
	} while (index != (Queue->Rear + 1) % QUEUE_MAX_SIZE); // Stop after last data
}

void StartReceiveInt(void)
{
	LPUART0->CTRL |= LPUART_CTRL_RE(1);
}
