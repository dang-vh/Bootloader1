#ifndef _ADC_H_
#define _ADC_H_

#include "MKE16Z4.h"
#include "queue.h"
#include <stdint.h>

#define ADCR_VDD                (4095U)    /*! Maximum value when use 16b resolution */
#define V_BG                    (1000U)     /*! BANDGAP voltage in mV (trim to 1.0V) */
#define V_TEMP25                (7405U)      /*! Typical VTEMP25 in (mV x10) */
#define M                       (1564U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)
#define ADC12_TEMPERATURE_CHN         	(26U) /* Temperature Sensor Channel.*/
#define ADC12_BANDGAP_CHN             	(27U) /* ADC channel of BANDGAP.*/
#define DISABLE_ADC  					(31U) /* Writing 0x1F (31) to ADCH register, disable ADC */

static volatile uint32_t count = 0;
volatile static uint32_t sAdcValue = 0;               /*! ADC value */
static uint32_t sAdcrTemp25 = 0;             /*! Calibrated ADCR_TEMP25 */
static uint32_t sAdcr100m = 0;

extern Queue_Types testQueue;

void ADC0_IRQHandler(void);
void StartADC_INT();
void ADC_Init();
uint16_t ReadADC(void);
void setChannel(uint8_t channel);
uint32_t GetCurrentTempValue(uint32_t sAdcValue);
















#endif /* _ADC_H_ */
