/*
 * PORT.h
 *
 *
 *  Created on: 20 thg 2, 2024
 *      Author: dangv
 */
#ifndef _PORT_TIMER_H_
#define _PORT_TIMER_H_

#include "MKE16Z4.h"
#include "stdint.h"
#include "stdio.h"

typedef void (*PortCallback_t)(uint8_t);
typedef void (*SysTickCallback_t)(void);

typedef enum
{
	PORT_MUX_ANALOG = 0U,
	PORT_MUX_GPIO = 1U,
	PORT_MUX_ALT2 = 2U,
	PORT_MUX_ALT3 = 3U,
	PORT_MUX_ALT4 = 4U,
	PORT_MUX_ALT5 = 5U,
	PORT_MUX_ALT6 = 6U,
	PORT_MUX_ALT7 = 7U
} PortMUX_t;

typedef enum
{
	PORT_INTERRUPT_DISABLE = 0U,
	PORT_INTERRUPT_LOW_LEVEL = 8U,
	PORT_INTERRUPT_RISING_EDGE = 9U,
	PORT_INTERRUPT_FALLING_EDGE = 10U,
	PORT_INTERRUPT_FBOTH_EDGE = 10U,
	PORT_INTERRUPT_HIGHT_LEVEL = 12U
} PortINTType;

/*! PS - Pull Select
 *  0b0..Internal pulldown resistor is enabled on the corresponding pin, if the corresponding PE field is set.
 *  0b1..Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE field is set.
 */
typedef enum
{
	PORT_PULL_DISABLE = 0U,
	PORT_PULL_DOWN = 2U,
	PORT_PULL_UP = 3U
} PortPull_t;

/*! @brief GPIO direction definition */
typedef enum _gpio_pin_direction
{
	kGPIO_DigitalInput = 0U,  /*!< Set current pin as digital input*/
	kGPIO_DigitalOutput = 1U, /*!< Set current pin as digital output*/
} gpio_pin_direction_t;

typedef enum _Systick_interrupt_direction
{
	Systick_Interrupt_disable = 0U, /*!< Set current pin as digital input*/
	Systick_Interrupt_enable = 1U,	/*!< Set current pin as digital output*/
} SystickINT_t;
typedef struct _gpio_pin_config
{
	gpio_pin_direction_t pinDirection; /*!< GPIO direction, input or output */
	/* Output configurations; ignore if configured as an input pin */
	uint8_t outputLogic; /*!< Set a default output logic, which has no use in input */
} gpio_pin_config_t;

typedef struct
{
	PortMUX_t mux;
	PortPull_t pull;
	PortINTType interrupt;
	PortCallback_t callback;
} port_config_t;

typedef struct
{
	SystickINT_t INTERRUPT;
	SysTickCallback_t callback;
	uint32_t reload;
} systick_config_t;

/* Define port config____________________________________________________________________________________*/

// const port_config_t LED_PORT_config = {
//	.mux = PORT_MUX_GPIO};
//
// const port_config_t SW_PORT_config = {
//	.mux = PORT_MUX_GPIO,
//	.pull = PORT_PULL_UP,
//	.interrupt = PORT_INTERRUPT_DISABLE,
// };
//
// const gpio_pin_config_t sw_config = {
//	kGPIO_DigitalInput,
//	0,
// };

uint8_t CheckSW(GPIO_Type *base, uint32_t pin);
void PORT_Init(PORT_Type *PORTx, uint8_t pin, const port_config_t *UserConfig);
void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config);
void Systick_init(systick_config_t *UserConfig);
#endif
