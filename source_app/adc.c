#include "adc.h"

void ADC0_IRQHandler(void)
{

	setChannel(ADC12_TEMPERATURE_CHN);
	/* Get current ADC value */
	sAdcValue = ReadADC();
	uint16_t temp = GetCurrentTempValue(sAdcValue);
	//Push data into Queue
	Queue_PushData(&testQueue, temp);
	//Limit 10 data
	if (count == 10)
	{
		count = 0;
		ADC0->SC1[0] |= ADC_SC1_ADCH(DISABLE_ADC); // Disable module
		// Disable interrupt?
		ADC0->SC1[0] &= ~ADC_SC1_AIEN_MASK;
	}
	else
	{
		count++;
	}
}

void StartADC_INT()
{
	// Start ADC
	ADC0->SC1[0] = ((ADC0->SC1[0]) & ~ADC_SC1_ADCH_MASK) | ADC12_TEMPERATURE_CHN;
}

void ADC_Init()
{
	// Init Clock - Async Clock
	// Enable Clock for LPIT0
	// init Clock - Async Peripheral clock
	SCG->FIRCDIV &= ~SCG_FIRCDIV_FIRCDIV2_MASK;
	SCG->FIRCDIV |= SCG_FIRCDIV_FIRCDIV2(6);

	PCC->CLKCFG[PCC_ADC0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
	PCC->CLKCFG[PCC_ADC0_INDEX] |= PCC_CLKCFG_PCS(3); // Fast IRC Clock

	// Bus Clock
	PCC->CLKCFG[PCC_ADC0_INDEX] |= PCC_CLKCFG_CGC(1U);

	// CLock source /  Clock Divide
	ADC0->CFG1 &= ~(ADC_CFG1_ADICLK_MASK);
	ADC0->CFG1 |= ADC_CFG1_ADIV(0);

	// Resolution 12 bit
	ADC0->CFG1 &= ~(ADC_CFG1_MODE_MASK);
	ADC0->CFG1 |= ADC_CFG1_MODE(1U);

	// Sample Time Clock
	ADC0->CFG2 = 200;

	// Voltage Reference
	ADC0->SC2 &= ~(ADC_SC2_REFSEL_MASK);

	//  single Mode
	ADC0->SC3 &= ~ADC_SC3_ADCO_MASK;
	//  Continous Mode
	// ADC0->SC3 |= ADC_SC3_ADCO(1U);

	/*
		// Hardware average
		ADC0->SC3 |= ADC_SC3_AVGE(1U);
		//16 sample averages
		ADC0->SC3 &= ~ADC_SC3_AVGS_MASK;
		ADC0->SC3 |= ADC_SC3_AVGS(2U);
	*/

	// interrupt?
	// ADC0->SC1[0] |= ADC_SC1_AIEN(1);

	// Trigger Software
	ADC0->SC2 &= ~(ADC_SC2_ADTRG_MASK);

	NVIC_EnableIRQ(ADC0_IRQn);
}

uint16_t ReadADC(void)
{
	// Conversion in progress
	// while(!(ADC0->SC2 & ADC_SC2_ADACT_MASK));
	// wait COCO
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	// Read Data
	return (uint16_t)(ADC0->R[0]);
}

void setChannel(uint8_t channel)
{
	ADC0->SC1[0] = ((ADC0->SC1[0]) & ~ADC_SC1_ADCH_MASK) | channel;
	while (!(ADC0->SC2 & ADC_SC2_ADACT_MASK));
}
uint32_t GetCurrentTempValue(uint32_t sAdcValue)
{
	uint32_t currentTemperature = 0;
	uint32_t bandgapValue = 0; /*! ADC value of BANDGAP */
	uint32_t vdd = 0;

	// Get current ADC BANDGAP value
	setChannel(ADC12_BANDGAP_CHN);
	bandgapValue = ReadADC();
	// Get VDD value measured in mV: VDD = (ADCR_VDD x V_BG) / ADCR_BG
	vdd = ADCR_VDD * V_BG / bandgapValue;
	// Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD
	sAdcrTemp25 = ADCR_VDD * V_TEMP25 / (vdd * 10);
	// ADCR_100M = ADCR_VDD x M x 100 / VDD
	sAdcr100m = (ADCR_VDD * M) / (vdd * 10);

	currentTemperature = (uint32_t)(STANDARD_TEMP - ((int32_t)sAdcValue - (int32_t)sAdcrTemp25) * 100 / (int32_t)sAdcr100m);
	return currentTemperature;
}
