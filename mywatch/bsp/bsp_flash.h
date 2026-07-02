#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

#include "bsp_config.h"

#define STM32_FLASH_SIZE    64
#define STM32_FLASH_WREN    1
#define STM32_FLASH_BASE    0x08000000

u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);
void Test_Write(u32 WriteAddr, u16 WriteData);

#endif
