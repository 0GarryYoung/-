#include "stmflash.h"
#include "delay.h"

/*
 * Internal flash read/erase/program. Ported from the original Health_watch
 * stmflash.c: the SPL FLASH_Unlock/ErasePage/ProgramHalfWord/Lock calls are
 * replaced by the HAL flash API. Used to persist the step counter at
 * 0x0800F000 (page 60 of the 64 KB device).
 */

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}

#if STM32_FLASH_WREN
//write without erase/check
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
    u16 i;
    for(i=0;i<NumToWrite;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]);
        WriteAddr+=2;
    }
}

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //bytes
#else
#define STM_SECTOR_SIZE	2048
#endif
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//up to 2K bytes
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u32 offaddr;
    FLASH_EraseInitTypeDef EraseInit;
    u32 PageError = 0;

    if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//illegal address
    HAL_FLASH_Unlock();
    offaddr=WriteAddr-STM32_FLASH_BASE;
    secpos=offaddr/STM_SECTOR_SIZE;
    secoff=(offaddr%STM_SECTOR_SIZE)/2;
    secremain=STM_SECTOR_SIZE/2-secoff;
    if(NumToWrite<=secremain)secremain=NumToWrite;
    while(1)
    {
        STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//read whole sector
        for(i=0;i<secremain;i++)//check if erase needed
        {
            if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;
        }
        if(i<secremain)//erase needed
        {
            EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
            EraseInit.PageAddress = secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE;
            EraseInit.NbPages     = 1;
            HAL_FLASHEx_Erase(&EraseInit, &PageError);//erase this page
            for(i=0;i<secremain;i++)//merge
            {
                STMFLASH_BUF[i+secoff]=pBuffer[i];
            }
            STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//write back
        }else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//already erased, write directly
        if(NumToWrite==secremain)break;//done
        else//more to write
        {
            secpos++;
            secoff=0;
            pBuffer+=secremain;
            WriteAddr+=secremain;
            NumToWrite-=secremain;
            if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//next sector still won't fit
            else secremain=NumToWrite;//next sector fits
        }
    };
    HAL_FLASH_Lock();
}
#endif

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
    u16 i;
    for(i=0;i<NumToRead;i++)
    {
        pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);
        ReadAddr+=2;
    }
}

void Test_Write(u32 WriteAddr,u16 WriteData)
{
    STMFLASH_Write(WriteAddr,&WriteData,1);
}
