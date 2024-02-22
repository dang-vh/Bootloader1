#include "lpit.h"

void LPIT0_IRQHandler()
{
	// Clear interrupt flag
	LPIT0->MSR |= LPIT_MSR_TIF0(1U);
	// ADC interrupt?
	ADC0->SC1[0] |= ADC_SC1_AIEN(1);
	Toggle_led();
	// start ADC
	StartADC_INT();
}

void LPIT_Init()
{
	// Disable FIRC
	SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN(0);

	// Config FIRCDIV to 1.5MHz
	SCG->FIRCDIV &= ~SCG_FIRCDIV_FIRCDIV2_MASK;
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(6);

	// Enable asyn clock, use FIRC
	PCC->CLKCFG[PCC_LPIT0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_PCS(3);

	// Enable the LPIT module clock
	PCC->CLKCFG[PCC_LPIT0_INDEX] |= PCC_CLKCFG_CGC(1);

	// Reset the timer channels and registers
	LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
	LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

	// Setup timer operation in debug and doze modes and enable the module
	LPIT0->MCR |= (LPIT_MCR_DBG_EN(1) | LPIT_MCR_DOZE_EN(1) | LPIT_MCR_M_CEN_MASK);

	/* Setup the channel counters operation mode to "32-bit Periodic Counter",and keep
default values for the trigger source */

	LPIT0->CHANNEL[0].TCTRL &= ~LPIT_TCTRL_MODE_MASK;
	LPIT0->CHANNEL[0].TCTRL |= LPIT_TCTRL_MODE(0U);

	// Set timer period for channel 0 as 1 second
	//LPIT0->CHANNEL[0].TVAL = 1500000;

	// Set timer period for channel 0 as 2 second
	LPIT0->CHANNEL[0].TVAL = 3000000;

	// Enable channel0 interrupt
	LPIT0->MIER |= LPIT_MIER_TIE0(1U);

	NVIC_EnableIRQ(LPIT0_IRQn);
}

void LPIT_StartTimer()
{
	// Starts the timer counting afer all configuration
	LPIT0->SETTEN = LPIT_SETTEN_SET_T_EN_0(1U);
}

void Red_led_Init(void)
{
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1U);
	PORTB->PCR[5] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[5] |= PORT_PCR_MUX(1U);
	GPIOB->PDDR |= GPIO_PDDR_PDD(32U);
}

void Toggle_led(void)
{
	GPIOB->PTOR = 32;
}
