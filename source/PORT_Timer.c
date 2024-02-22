/*
 * PORT.c
 *
 *  Created on: 20 thg 2, 2024
 *      Author: dangv
 */


#include "PORT_Timer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PORT_PCR_PE_PS_SHIFT (0U)
#define LED_RED_PIN (5U)
#define LED_GREEN_PIN (4U)
#define LED_BLUE_PIN (4U)

PortCallback_t s_callback = NULL;
SysTickCallback_t s_SYSTcallback = NULL;


/* Function Prototypes */

void PORT_Init(PORT_Type *PORTx, uint8_t pin, const port_config_t *UserConfig)
{
	// Turn on clock for PORTD
	PCC->CLKCFG[PCC_PORTD_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
	PCC->CLKCFG[PCC_PORTD_INDEX] |= PCC_CLKCFG_CGC(1);

	// MUX
	PORTx->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	PORTx->PCR[pin] |= PORT_PCR_MUX(UserConfig->mux);

	// PULL
	PORTx->PCR[pin] &= ~PORT_PCR_PE_MASK;
	PORTx->PCR[pin] &= ~PORT_PCR_PS_MASK;
	PORTx->PCR[pin] |= (UserConfig->pull << PORT_PCR_PE_PS_SHIFT);

	// Interrupt
	if (PORT_INTERRUPT_DISABLE != UserConfig->interrupt)
	{
		s_callback = UserConfig->callback;
		PORTx->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
		PORTx->PCR[pin] |= PORT_PCR_IRQC(UserConfig->interrupt);
	}
}

void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config )
{

	if (config->pinDirection == kGPIO_DigitalInput)
	{
		base->PDDR |= (uint32_t)((0U << pin));
	}
	else
	{
		//        GPIO_PinWrite(base, pin, config->outputLogic);
		base->PDDR |= (uint32_t)((1U << pin));
	}
}

uint8_t CheckSW( GPIO_Type *base, uint32_t pin )
{
	uint8_t temp = ((base->PDIR)&(1<<pin));
	return temp;
}

static inline uint8_t PORT_CheckInterruptFlag(PORT_Type *PORTx, uint8_t pin)
{
	return (uint8_t)((PORTx->ISFR) & (1U << pin));
}
static inline void PORT_ClearInterruptFlag(PORT_Type *PORTx, uint8_t pin) { PORTD->ISFR |= (1U << pin); }
void PORTBCD_IRQHandler(void)
{
	uint8_t pin;
	for (pin = 0; pin < 32; pin++)
	{
		if (0U != PORT_CheckInterruptFlag(PORTD, pin))
		{
			PORT_ClearInterruptFlag(PORTD, pin);
			if (NULL != s_callback)
			{
				(*s_callback)(pin);
			}
		}
	}
	NVIC_ClearPendingIRQ(PORTBCD_IRQn);
}
void Systick_init(systick_config_t *UserConfig)
{
	// Clock configuration - clock core
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	// Enable interrupt, call back
	if (0 != UserConfig->INTERRUPT)
	{
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
		s_SYSTcallback = UserConfig->callback;
	}
	// Reload value, clear counter
	SysTick->LOAD = UserConfig->reload;
	// Clear counter
	SysTick->VAL |= 0U;
	// Enable timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



void LPIT0_Init()
{
	// Disable FIRC
	SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(0);

	// Config FIRCDIV to divide by 8
	SCG->FIRCDIV &= ~SCG_FIRCDIV_FIRCDIV2_MASK;
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(4);

	// Enable the LPIT module clock
	PCC->CLKCFG[PCC_LPIT0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_PCS(3U); // Fast IRC Clock.

	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_CGC(1);

	// Reset the timer channels and registers
	LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
	LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

	// Setup timer operation in debug and doze modes and enable the module;
	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
	/* Setup the channel counters operation mode to "32-bit Periodic Counter",and keep
		default values for the trigger source;*/
	LPIT0->MCR |= (LPIT_MCR_DBG_EN(1) | LPIT_MCR_DOZE_EN(1));

	// Set timer period for channel 0 as 1 second;
	LPIT0->CHANNEL[0].TCTRL |= LPIT_TCTRL_MODE(0);
	LPIT0->CHANNEL[0].TVAL = 100 * 48000;

	// Enable channel0 interrupt;
	LPIT0->MIER |= LPIT_MIER_TIE0(1U);

	NVIC_EnableIRQ(LPIT0_IRQn);
}

void LPIT0_StarTimer()
{
	LPIT0->SETTEN = LPIT_SETTEN_SET_T_EN_0(1U);
}



