#include "ds1302.h"
#include "delay.h"

/* RTC default value used only on the very first power-up (see DS1302_Init). */
DATE SysDate = {00, 00, 12, 22, 7, 4, 21};  //2021.7.22 12:00:00 Week 4

static void DAT_MODE_OUT(void)   //IO -> push-pull output
{
    GPIO_InitTypeDef gi = {0};
    gi.Pin   = GPIO_PIN_14;
    gi.Mode  = GPIO_MODE_OUTPUT_PP;
    gi.Pull  = GPIO_NOPULL;
    gi.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &gi);
}

static void DAT_MODE_IN(void)    //IO -> input with pull-up
{
    GPIO_InitTypeDef gi = {0};
    gi.Pin  = GPIO_PIN_14;
    gi.Mode = GPIO_MODE_INPUT;
    gi.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &gi);
}

void DS1302_GPIO_Init(void)
{
    GPIO_InitTypeDef gi = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gi.Pin   = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gi.Mode  = GPIO_MODE_OUTPUT_PP;
    gi.Pull  = GPIO_NOPULL;
    gi.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &gi);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
}

void DS1302_WriteOneByte(uint8_t data)
{
    uint8_t index = 0;
    DAT_MODE_OUT();
    for(index=0; index<8; index++)
    {
        SCK_CLR;
        if (data & 0x01)
            DAT_SET;
        else
            DAT_CLR;
        DelayUs(2);
        SCK_SET;
        DelayUs(2);
        data >>= 1;
    }
}

void DS1302_WriteByte(const uint8_t addr, const uint8_t data)
{
    RST_CLR;
    SCK_CLR;
    DelayUs(5);
    RST_SET;
    DelayUs(5);
    DS1302_WriteOneByte(addr);
    DS1302_WriteOneByte(data);
    RST_CLR;
    SCK_CLR;
}

uint8_t DS1302_ReadByte(const uint8_t addr)
{
    uint8_t index = 0, data = 0;

    RST_CLR;
    SCK_CLR;
    DelayUs(5);
    RST_SET;
    DelayUs(5);
    DS1302_WriteOneByte(addr);
    DAT_MODE_IN();

    for(index=0; index<8; index++)
    {
        SCK_CLR;
        DelayUs(2);
        data >>= 1;
        if (DAT_READ == 1)
        {
            data |= 0x80;
        }
        SCK_SET;
        DelayUs(2);
    }
    SCK_CLR;
    RST_CLR;
    return data;
}

int DS1302_DateSet(const DATE * const date)
{
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x00); /* clear write protect */

    DS1302_WriteByte(WRITE_SEC_REG,  DataToBcd(date->sec));
    DS1302_WriteByte(WRITE_MIN_REG,  DataToBcd(date->min));
    DS1302_WriteByte(WRITE_HOUR_REG, DataToBcd(date->hour));
    DS1302_WriteByte(WRITE_DAY_REG,  DataToBcd(date->day));
    DS1302_WriteByte(WRITE_MON_REG,  DataToBcd(date->mon));
    DS1302_WriteByte(WRITE_WEEK_REG, DataToBcd(date->week));
    DS1302_WriteByte(WRITE_YEAR_REG, DataToBcd(date->year));
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x80); /* set write protect */
    return 0;
}

int DS1302_DateRead(DATE * const date)
{
    date->sec  = BcdToData(DS1302_ReadByte(READ_SEC_REG));
    date->min  = BcdToData(DS1302_ReadByte(READ_MIN_REG));
    date->hour = BcdToData(DS1302_ReadByte(READ_HOUR_REG));
    date->day  = BcdToData(DS1302_ReadByte(READ_DAY_REG));
    date->mon  = BcdToData(DS1302_ReadByte(READ_MON_REG));
    date->week = BcdToData(DS1302_ReadByte(READ_WEEK_REG));
    date->year = BcdToData(DS1302_ReadByte(READ_YEAR_REG));
    return 0;
}

int DS1302_Init(const DATE * const date)
{
    DS1302_GPIO_Init();
    if (BcdToData(DS1302_ReadByte(READ_RAM_REG)) == 0x01) /* not first power-up */
    {
        return 0;
    }
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x00); /* clear write protect */
    DS1302_WriteByte(WRITE_RAM_REG, DataToBcd(0x01)); /* mark as initialised */
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x80); /* set write protect */
    DS1302_DateSet(date);
    return 0;
}
