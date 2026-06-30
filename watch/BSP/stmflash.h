#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "bsp_types.h"

//////////////////////////////////////////////////////////////////////////////
// STM32F103C8: 64 KB flash, 1 KB page size.
#define STM32_FLASH_SIZE 64 	 		//on-chip flash size in KB
#define STM32_FLASH_WREN 1              //enable flash write (0:disable;1:enable)
//////////////////////////////////////////////////////////////////////////////

#define STM32_FLASH_BASE 0x08000000 	//STM32 flash base address

u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);
void Test_Write(u32 WriteAddr,u16 WriteData);
#endif
