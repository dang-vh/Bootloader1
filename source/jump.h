/*
 * jump.h
 *
 *  Created on: 19 thg 2, 2024
 *      Author: dangv
 */
#ifndef _JUMP_H_
#define _JUMP_H_
#include "MKE16Z4.h"
#include "stdint.h"
#include "stdio.h"
#include "flash.h"

#define MY_APP 0x8000U
#define BACKUP 0x4000U

#define LIMIT_MY_APP 0xFC00U
#define LIMIT_BACKUP 0x8000U

#define MY_APP_S 0x800000U
#define BACKUP_S 0x400000U

void go2app(uint32_t app);
void DelAPP(uint32_t address);
#endif
