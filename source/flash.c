#include "flash.h"

/*******************************************************************************
 * Function name: DelFlash
 * Function: This Function to delete 1 sector in flash memory
********************************************************************************/
void FTFA_Init()
{
	// Clock enable
	PCC->CLKCFG[PCC_FLASH_INDEX] |= PCC_CLKCFG_PCS(1U);
}
void DelFlash(uint32_t address)
{
    if(((address % 0x400) == 0) && (address != 0x400) && (address < 0xFFFF))
    {
        if((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == FTFA_FSTAT_CCIF_MASK)
        {
            uint8_t address1;
            uint8_t address2;
            uint8_t address3;
            address1 = address >> 16;
            address2 = address >> 8;
            address3 = address;
            FTFA->FCCOB3 = FTFA_FCCOB3_CCOBn(address3);
            FTFA->FCCOB2 = FTFA_FCCOB2_CCOBn(address2);
            FTFA->FCCOB1 = FTFA_FCCOB1_CCOBn(address1);
            FTFA->FCCOB0 = FTFA_FCCOB0_CCOBn(0x09);
            FTFA->FSTAT |= FTFA_FSTAT_CCIF_MASK;
            /*__asm("CPSID   I");*/
            while((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);
            /*__asm("CPSIE   I");*/
        }
    }
}
/*******************************************************************************
 * Function name: WriteFlash
 * Function: This Function to write in address which was deleted in flash
********************************************************************************/
void WriteFlash(uint32_t address, uint8_t u16data[])
{
    if(((address % 0x4) == 0) && (address < 0xFFFF) && (address >= 0x2000))
    {
        if((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == FTFA_FSTAT_CCIF_MASK)
        {
            FTFA->FCCOB3 = FTFA_FCCOB3_CCOBn(address);
            FTFA->FCCOB2 = FTFA_FCCOB2_CCOBn(address >> 8);
            FTFA->FCCOB1 = FTFA_FCCOB1_CCOBn(address >> 16);
            FTFA->FCCOB0 = FTFA_FCCOB0_CCOBn(0x06);
            FTFA->FCCOB4 = FTFA_FCCOB4_CCOBn(u16data[0]);
            FTFA->FCCOB5 = FTFA_FCCOB5_CCOBn(u16data[1]);
            FTFA->FCCOB6 = FTFA_FCCOB6_CCOBn(u16data[2]);
            FTFA->FCCOB7 = FTFA_FCCOB7_CCOBn(u16data[3]);
            FTFA->FSTAT |= FTFA_FSTAT_CCIF_MASK;
            /*__asm("CPSID   I");*/
            while((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);
            /*__asm("CPSIE   I");*/
        }
    }
}

