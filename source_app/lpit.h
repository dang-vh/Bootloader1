#ifndef _LPIT_H_
#define _LPIT_H_
#include "MKE16Z4.h"
#include "adc.h"

void LPIT0_IRQHandler();
void LPIT_Init();
void LPIT_StartTimer();
void Toggle_led(void);
void Red_led_Init(void);

















#endif /* _LPIT_H_ */
