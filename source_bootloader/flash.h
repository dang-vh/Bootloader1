
#ifndef _FLASH_H_
#define _FLASH_H_
#include "MKE16Z4.h"
#include "stdint.h"
#include "stdio.h"
/***************************************************************************
*prototypes
***************************************************************************/
void DelFlash(uint32_t address);
void WriteFlash(uint32_t address, uint8_t u16data[]);
void FTFA_Init();

#endif
