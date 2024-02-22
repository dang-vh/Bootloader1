/*
 * UART_bootloader.h
 *
 *  Created on: 19 thg 2, 2024
 *      Author: dangv
 */

#ifndef _UART_H_
#define _UART_H_
#include "MKE16Z4.h"

#include "flash.h"
#include "jump.h"
#include "PORT_Timer.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define ADDRESS_APP 0xFC00U
#define CURRENT_APP *(uint32_t*)0xFC00U

void bootloader(void);
void UART_Init(void);
void SEND_message(uint8_t *message);
#endif

