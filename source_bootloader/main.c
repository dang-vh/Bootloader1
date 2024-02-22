/*
 * main.c
 *
 *  Created on: 19 thg 2, 2024
 *      Author: dangv
 */
#include "PORT_Timer.h"
#include "flash.h"
#include "jump.h"
#include "UART_bootloader.h"

#define CURRENT_APP *(uint32_t *)0xFC00U

/* Define port config____________________________________________________________________________________*/

const port_config_t SW_PORT_config = {
	.mux = PORT_MUX_GPIO,
	.pull = PORT_PULL_UP,
	.interrupt = PORT_INTERRUPT_DISABLE,
};

const gpio_pin_config_t sw_config = {
	kGPIO_DigitalInput,
	0,
};

uint32_t *Button = (uint32_t *)(0x400FF0D0U);
uint32_t app = MY_APP;
int main(void)
{
	PORT_Init(PORTD, 2, &SW_PORT_config);
	GPIO_PinInit(GPIOD, 2, &sw_config);

	UART_Init();

	SEND_message("HELLO\n");

	if (0 == CheckSW(GPIOD, 2))
	{
		bootloader();
	}

	SEND_message("APP_MODE\n");
	if (*(uint32_t *)ADDRESS_APP == 0xFFFFFFFF)
	{
		SEND_message("not available, return to boot mode to backup or load the app\n");
	}
	else
	{
		if (*(uint32_t *)(CURRENT_APP) == 0xFFFFFFFF)
		{
			SEND_message("not available, return to boot mode to backup or load the app\n");
		}
		else
		{
			go2app(CURRENT_APP);
		}
	}
}
