/*
 * jump.c
 *
 *  Created on: 19 thg 2, 2024
 *      Author: dangv
 */
#include "jump.h"

typedef void (*functionPtr)(void);

void go2app(uint32_t app)
{
	uint32_t JumpAddress;
	functionPtr Jump_To_Application;

	// Reset all peripheral & De-init the low level hardware
	if (CONTROL_nPRIV_Msk & __get_CONTROL())
	{
		__set_CONTROL(__get_CONTROL() & ~CONTROL_nPRIV_Msk);
	}

	NVIC->ICER[0] = 0xFFFFFFFF;
	NVIC->ICER[1] = 0xFFFFFFFF;
	NVIC->ICER[2] = 0xFFFFFFFF;
	NVIC->ICER[3] = 0xFFFFFFFF;
	NVIC->ICER[4] = 0xFFFFFFFF;
	NVIC->ICER[5] = 0xFFFFFFFF;
	NVIC->ICER[6] = 0xFFFFFFFF;
	NVIC->ICER[7] = 0xFFFFFFFF;

	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICPR[1] = 0xFFFFFFFF;
	NVIC->ICPR[2] = 0xFFFFFFFF;
	NVIC->ICPR[3] = 0xFFFFFFFF;
	NVIC->ICPR[4] = 0xFFFFFFFF;
	NVIC->ICPR[5] = 0xFFFFFFFF;
	NVIC->ICPR[6] = 0xFFFFFFFF;
	NVIC->ICPR[7] = 0xFFFFFFFF;

	SysTick->CTRL = 0;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	SCB->CCR &= ~(SCB_CCR_UNALIGN_TRP_Msk |
				  SCB_CCR_STKALIGN_Msk);

	if (CONTROL_SPSEL_Msk & __get_CONTROL())
	{
		__set_MSP(__get_PSP());
		__set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
	}

	// set vector table to app address ensuring this instruction is completed
	__DMB();
	SCB->VTOR = (uint32_t)app;
	__DSB();

	// set MSP
	__set_MSP(*(uint32_t *)app);

	// jump to reset handler
	JumpAddress = *((volatile uint32_t *)(app + 4));
	Jump_To_Application = (functionPtr)(JumpAddress);

	// reset MCU
	Jump_To_Application();
}

void DelAPP(uint32_t address)
{
	uint32_t index = address;
	if (address == MY_APP)
	{
		for (index = address; index < LIMIT_MY_APP; index += 0x400)
		{
			if (*(uint32_t *)index != 0xFFFFFFFF)
			{
				DelFlash(index); // delete app
			}
		}
	}
	if (address == BACKUP)
	{
		for (index = address; index < LIMIT_BACKUP; index += 0x400)
		{
			if (*(uint32_t *)index != 0xFFFFFFFF)
			{
				DelFlash(index); // delete backup
			}
		}
	}
}
