#include "bsp_flash.h"

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}

#if STM32_FLASH_WREN
#define STM_SECTOR_SIZE 1024
u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];

void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u16 i;
    HAL_FLASH_Unlock();
    for (i = 0; i < NumToWrite; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]);
        WriteAddr += 2;
    }
    HAL_FLASH_Lock();
}

void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u32 offaddr;

    if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))) return;

    HAL_FLASH_Unlock();
    offaddr = WriteAddr - STM32_FLASH_BASE;
    secpos = offaddr / STM_SECTOR_SIZE;
    secoff = (offaddr % STM_SECTOR_SIZE) / 2;
    secremain = STM_SECTOR_SIZE / 2 - secoff;

    if (NumToWrite <= secremain) secremain = NumToWrite;

    while (1)
    {
        STMFLASH_Read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
        for (i = 0; i < secremain; i++)
        {
            if (STMFLASH_BUF[secoff + i] != 0XFFFF) break;
        }
        if (i < secremain)
        {
            FLASH_EraseInitTypeDef EraseInitStruct = {0};
            uint32_t SectorError = 0;
            EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
            EraseInitStruct.PageAddress = secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE;
            EraseInitStruct.NbPages = 1;
            HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

            for (i = 0; i < secremain; i++)
            {
                STMFLASH_BUF[i + secoff] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
        }
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);
        }

        if (NumToWrite == secremain) break;
        else
        {
            secpos++;
            secoff = 0;
            pBuffer += secremain;
            WriteAddr += secremain;
            NumToWrite -= secremain;
            if (NumToWrite > (STM_SECTOR_SIZE / 2))
                secremain = STM_SECTOR_SIZE / 2;
            else
                secremain = NumToWrite;
        }
    }
    HAL_FLASH_Lock();
}
#endif

void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
    u16 i;
    for (i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);
        ReadAddr += 2;
    }
}

void Test_Write(u32 WriteAddr, u16 WriteData)
{
    STMFLASH_Write(WriteAddr, &WriteData, 1);
}
