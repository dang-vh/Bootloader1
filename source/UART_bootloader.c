/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "UART_bootloader.h"



/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPUART          LPUART2
#define DEMO_LPUART_CLKSRC   kCLOCK_ScgSysOscClk
#define DEMO_LPUART_CLK_FREQ CLOCK_GetFreq(kCLOCK_ScgSysOscClk)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

volatile uint8_t sFlag = 0;
volatile uint8_t stypeFlag = 0;
volatile uint8_t ChecksumFlag = 0;
volatile uint8_t ERRFlag = 0;
volatile uint16_t sum = 0;
volatile uint8_t LoadFlashFlag =0;
volatile uint8_t Time_count = 0;
volatile uint32_t app1 = MY_APP;
volatile uint16_t line = 0;
typedef enum{
	SREC_DEFAULT,
	SREC_S_CHAR,
	SREC_S_TYPE,
	SREC_BYTECOUNT_1,
	SREC_BYTECOUNT_2,
	SREC_ADDRESS,
	SREC_ADDRESS_END,
	SREC_DATAWORD,
	SREC_DATAWORD_END,
	SREC_END_OF_LINE,
	SREC_END_OF_FILE,
}STEP_t;

typedef enum{
	FILE_ERR,
	FILE_OK,
}STATUS_t;

typedef struct{
	uint8_t Stype;
	uint8_t NumofAdress;
	uint8_t ByteCount;
	uint8_t NumofData;
	uint32_t Address;
	uint32_t DataWord;
	uint8_t Checksum;
} SREC_t;

SREC_t SrecData;
uint8_t DataWrite[4];
STEP_t step = SREC_DEFAULT;
STATUS_t status=FILE_OK;
uint8_t addrCount = 0;
volatile uint8_t dataCount = 0;
volatile uint8_t complete = 0;
volatile uint8_t ExitFlag = 0;
/*******************************************************************************
 * Variables
 ******************************************************************************/

//uint8_t txbuff[]   = "Lpuart polling example with seven data bits\r\nBoard will send back received characters\r\n";
//uint8_t rxbuff[20] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
volatile char ch='r';
volatile uint8_t Data;
volatile uint8_t HexData;
volatile uint8_t RxIntFlag = 0;
volatile uint16_t rxIndex = 0; /* Index of the memory to save new arrived data. */
void UART_ClockPort_Init()
{
	PCC->CLKCFG[PCC_PORTB_INDEX] |= PCC_CLKCFG_CGC(1);

	PORTB->PCR[1]&=PORT_PCR_MUX_MASK;
	PORTB->PCR[1]|=PORT_PCR_MUX(0b010);
	PORTB->PCR[1]|=PORT_PCR_PE(1);
	PORTB->PCR[1]|=PORT_PCR_PS(1);

	PORTB->PCR[0]&=PORT_PCR_MUX_MASK;
	PORTB->PCR[0]|=PORT_PCR_MUX(0b010);
	PORTB->PCR[0]|=PORT_PCR_PE(1);
	PORTB->PCR[0]|=PORT_PCR_PS(1);

	// Disable SIRC
	SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN(0);

	// Config SIRCDIV to divide by 4
	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(3);

	// Enable the UART & PORT module clock
	PCC->CLKCFG[PCC_LPUART0_INDEX] &= ~PCC_CLKCFG_PCS_MASK;
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_PCS(2U);
	PCC->CLKCFG[PCC_LPUART0_INDEX] |= PCC_CLKCFG_CGC(1);

	//
//	SCG->SIRCCFG&=~SCG_SIRCCFG_RANGE_MASK;
	// Config SIRCDIV to divide by 1
//	SCG->SIRCDIV &= ~SCG_SIRCDIV_SIRCDIV2_MASK;
//	SCG->SIRCDIV |= SCG_SIRCDIV_SIRCDIV2(1);
}
void UART_Init(void)
{
	//Asynch module clock(2M)
	UART_ClockPort_Init();
	// disable

	//Baudrate (9600)
	 	 //BAUD divider
	LPUART0->BAUD&=~(LPUART_BAUD_SBR_MASK);
	LPUART0->BAUD|=LPUART_BAUD_SBR(52);
	 	 //OSR Divide
	LPUART0->BAUD|=LPUART_BAUD_BOTHEDGE_MASK;
	LPUART0->BAUD=(LPUART0->BAUD&~LPUART_BAUD_OSR_MASK)|LPUART_BAUD_OSR(3U);
	//parity
	LPUART0->CTRL&=~(LPUART_CTRL_PE_MASK);
	//Frame
	 	 //Data length - BAUD[M10],CTRL[M7],CTRL[M]- tranfer 8 bit
	LPUART0->BAUD&=~(LPUART_BAUD_M10_MASK);
	LPUART0->CTRL&=~(LPUART_CTRL_M7_MASK);
	LPUART0->CTRL&=~(LPUART_CTRL_M_MASK);
	 	 //Number of Stop Bits
	LPUART0->BAUD&=~(LPUART_BAUD_SBNS_MASK);
	 	 // LSB first MSB first
	LPUART0->STAT&=~(LPUART_STAT_MSBF_MASK);
	// interrupt or polling
	 	 //TX interrupt: TIE/ TCIE
	LPUART0->CTRL|=LPUART_CTRL_RIE(1U);
//	LPUART0->CTRL|=LPUART_CTRL_TCIE(1U);
	 	 //NVIC
	NVIC_EnableIRQ(LPUART0_IRQn);

}

void UART_SendByte(char ch)
{
	//enable tranfer
	LPUART0->CTRL|=LPUART_CTRL_TE(1U);
    // Chờ đến khi bộ đệm trống
    while (0U==(LPUART0->STAT & LPUART_STAT_TDRE_MASK));

    // Gửi dữ liệu - TX bufer = ch
    LPUART0->DATA = ch;

    // Chờ đến khi dữ liệu được gửi hoàn thành
    while (0U==(LPUART0->STAT & LPUART_STAT_TC_MASK));
    //disable tranfer
	LPUART0->CTRL&=~(LPUART_CTRL_TE_MASK);
}

void UART_SendString(uint8_t* str,uint8_t Len)
{
	uint8_t index;
	//enable tranfer
		LPUART0->CTRL|=LPUART_CTRL_TE(1U);
	    for(index=0;index<Len;index++)
	    {
	    	// Chờ đến khi bộ đệm trống
	    		    while (0U==(LPUART0->STAT & LPUART_STAT_TDRE_MASK));
	    	// Gửi dữ liệu - TX bufer = ch
	    		    LPUART0->DATA = str[index];
	    }

	    // Chờ đến khi dữ liệu được gửi hoàn thành
	    while (0U==(LPUART0->STAT & LPUART_STAT_TC_MASK));
	    //disable tranfer
		LPUART0->CTRL&=~(LPUART_CTRL_TE_MASK);
}
uint8_t CountChar(char *str) {
    uint8_t count = 0;
    // Lặp qua từng ký tự trong chuỗi cho đến khi gặp ký tự kết thúc chuỗi '\0'
    while (*str != '\0') {
    	count++;
        str++;
    }
    return count;
}
void SEND_message(uint8_t *message)
{
	UART_SendString((uint8_t *)message, CountChar(message));
}
char UART_GetByte(void)
{
	char data;
	//enable reciver
	LPUART0->CTRL|=LPUART_CTRL_RE(1U);
    // Chờ đến khi bộ đệm trống
    //disable tranfer
	LPUART0->CTRL&=~(LPUART_CTRL_RE_MASK);
	return data;
}


uint32_t dataring=0;
void StartReceiveInt(void)
{
	LPUART0->CTRL |= LPUART_CTRL_RE(1);
}
uint8_t ChartoHex(uint8_t character)
{
	uint8_t result;
	if(character<='9'&&character>='0')
	{
		result=character-'0';
	}
	else if (character<='F'&&character>='A')
	{
		result=character-'A'+ 0xA;
	}
	else
	{
		result=0xFF;
	}
	return result;
}
volatile uint8_t Data ;

void LPUART0_IRQHandler(void)
{
	Time_count=0;
	Data = LPUART0->DATA; //uint8_t)(LPUART0->DATA);
	HexData = ChartoHex(Data);
	if(Data=='S')
	{
		ChecksumFlag=1;
		line++;
	}
	if(ChecksumFlag==1)
	{
	    rxIndex++;
	}
	if (rxIndex > 2 && rxIndex % 2 != 0 && HexData != 255)
	{
		SrecData.Checksum += HexData << 4;
	}
	else if (rxIndex > 2 && rxIndex % 2 == 0 && HexData != 255)
	{
		SrecData.Checksum += HexData;
	}


	if( Data == '\n' )
	{
		step = SREC_END_OF_LINE;
	}
	else if( Data == 'Z' )
	{
		complete = 1;
		step = SREC_END_OF_FILE;
	}


	if(LoadFlashFlag==1||LoadFlashFlag==2)
	{
			switch(step)
			{
				case SREC_DEFAULT:
				{
					if ('S' == Data)
					{
						step = SREC_S_CHAR;
					}
					break;
				}
				case SREC_S_CHAR:
				{
					SrecData.Stype = HexData ;
					SrecData.NumofAdress = 2 * (SrecData.Stype + 1);
					step = SREC_S_TYPE;
					break;
				}
				case SREC_S_TYPE:
				{
					SrecData.ByteCount += HexData * 16;
					step = SREC_BYTECOUNT_1;
					break;
				}
				case SREC_BYTECOUNT_1:
				{
					SrecData.ByteCount += HexData ;
					SrecData.NumofData = 2 * SrecData.ByteCount - SrecData.NumofAdress - 2 ;
					step = SREC_BYTECOUNT_2;
					break;
				}
				case SREC_BYTECOUNT_2:
				case SREC_ADDRESS:
				{
					addrCount++;
					SrecData.Address += HexData * pow(16, SrecData.NumofAdress - addrCount);

					if (addrCount == SrecData.NumofAdress)
					{
						step = SREC_ADDRESS_END;
					}
					else
					{
						step = SREC_ADDRESS;
					}
					break;
				}
				case SREC_ADDRESS_END:
				case SREC_DATAWORD:
				{
					dataCount++;
					SrecData.DataWord = (SrecData.DataWord<<4)+ HexData;

					if ( dataCount % 8 == 0 )
					{

						*(uint32_t*)DataWrite = SrecData.DataWord;
						WriteFlash(SrecData.Address, DataWrite);
						SrecData.DataWord=0;
						SrecData.Address+=4;
						dataCount = 0;
					}

					if ( dataCount == SrecData.NumofData )
					{
		//				WriteFlash(SrecData.Address, SrecData.DataWord);
						SrecData.DataWord=0;
						SrecData.Address=0;
					}
					else
					{
						step = SREC_DATAWORD;
					}
					break;
				}
//				case SREC_DATAWORD_END:
//				{
//
//						if (0xFFU != (SrecData.Checksum & 0xFFU))
//						{
//							status = ERR_CHECKSUM;
//						}
//						break;
//				}
				case SREC_END_OF_FILE:
				case SREC_END_OF_LINE:
				{
					if (0xFFU != SrecData.Checksum)
					{
						status = FILE_ERR;
						SEND_message("FILE_ERR\n");
						if(LoadFlashFlag=1)
						{
							DelAPP(BACKUP);
						}
						else if(LoadFlashFlag=1)
						{
							DelAPP(MY_APP);
						}
						LoadFlashFlag=0;

					}
					memset(&SrecData, 0, sizeof(SrecData));
					memset(DataWrite, 0, 4);
					step = SREC_DEFAULT;;
					ChecksumFlag=0;
					rxIndex=0;
					dataCount=0;
					addrCount=0;
					break;
				}
		}
	}
};

//void CountTime(void)
//{
////	Request = LED_Active;
//	Time_count++;
//
//}
void SysTick_Handler()
{
	Time_count++;
	// clear flag
	SysTick->VAL |= 0U;
}
const systick_config_t Systick_config_count = {
	.INTERRUPT = Systick_Interrupt_enable,
//	.callback = &CountTime,
	.reload = 300*48000,
};
const gpio_pin_config_t led_config = {
	kGPIO_DigitalOutput,
	0,
};
const port_config_t LED_PORT_config = {
	.mux = PORT_MUX_GPIO};
void OffLed(GPIO_Type *base, uint32_t pin)
{
	base->PSOR |= 1U << pin;
}
void OnLed(GPIO_Type *base, uint32_t pin)
{
	base->PCOR |= 1U << pin;
}
void SEND_GUIDE(void)
{
	SEND_message("-----BOOT GUIDE-----\n");
	if (*(uint32_t *)BACKUP == 0xFFFFFFFF )
			{
				SEND_message("BACKUP_INVALID----");
			}
		else
			{
				SEND_message("BACKUP_VALID-----");
			}

		if (*(uint32_t *)MY_APP == 0xFFFFFFFF )
			{
				SEND_message("APP_INVALID\n");
			}
		else
			{
				SEND_message("APP_VALID\n");
			}
		SEND_message("Send 'R' to enter the backup\n");
		SEND_message("Send 'M' to enter the application\n");
		SEND_message("Send 'U' to remove the backup\n");
		SEND_message("Send 'K' to remove the application\n");
		SEND_message("Send 'Y' to update the new backup\n");
		SEND_message("Send 'I' to update the new application\n");
		SEND_message("Send 'X' to exit boot mode\n");
	SEND_message("-----------------\n");
}
void bootloader(void)
{
	PORT_Init(PORTB, 5, &LED_PORT_config);
	GPIO_PinInit(GPIOB, 5, &led_config);
	Systick_init(&Systick_config_count);

	OnLed( GPIOB, 5 );

	SEND_message("BOOT_MODE\n");
	complete=0;

	SEND_GUIDE();

	FTFA_Init();


	StartReceiveInt();
	while (ExitFlag==0&&Time_count<=100)
	{
		switch(Data)
		{
			case 'R':
			{
				DelFlash(ADDRESS_APP);
				*(uint32_t*)DataWrite= BACKUP_S;
				WriteFlash(ADDRESS_APP, DataWrite); // go backup
				ExitFlag=1;
				Data=0;
				break;
			}
			case 'M':
			{
				DelFlash(ADDRESS_APP);
				*(uint32_t*)DataWrite= MY_APP_S;
				WriteFlash(ADDRESS_APP, DataWrite); // go app
				ExitFlag=1;
				Data=0;
				break;
			}
			case 'U':
			{
				if (*(uint32_t *)BACKUP != 0xFFFFFFFF )
					{
					DelAPP(BACKUP); // delete backup
					}
				SEND_message("Delete_complete\n");
				Data=0;
				SEND_GUIDE();
				break;
			}
			case 'K':
			{
				if (*(uint32_t *)MY_APP != 0xFFFFFFFF )
					{
						DelAPP(MY_APP);// delete app
					}
				SEND_message("Delete_complete\n");
				Data=0;
				SEND_GUIDE();
				break;
			}
			case 'Y':
			{
				if (*(uint32_t *)BACKUP != 0xFFFFFFFF )
					{
					DelAPP(BACKUP); // delete backup
					}
				LoadFlashFlag=1;
				SEND_message("Please_transmit\n");
				Data=0;
				break;
			}
			case 'I':
			{
				if (*(uint32_t *)MY_APP != 0xFFFFFFFF )
				{
					DelAPP(MY_APP);// delete app
				}
				LoadFlashFlag=2;
				SEND_message("Please_transmit\n");
				Data=0;
				break;
			}
			case 'Z':
			{
				SEND_message("File loading completed\n");
				Data=0;
				break;
			}
			case 'X':
			{
				SEND_message("Goodbye!\n");
				ExitFlag=1;
				break;
			}
		}
		if(complete==1)
		{
			SEND_GUIDE();
			LoadFlashFlag=0;
			complete=0;
			Data=0;
		}
	}

	if(Time_count>100)
	{
		SEND_message("Time out\n");
	}
	ExitFlag=0;
	SEND_message("Finish 'boot mode', switch to 'app mode'\n");
	OffLed( GPIOB, 5 );
}
