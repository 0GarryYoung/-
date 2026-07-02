#include "bsp_ds1302.h"
#include "bsp_delay.h"

DATE SysDate = {0, 0, 12, 22, 7, 4, 21};

void DAT_MODE_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS1302_DAT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_DAT_PORT, &GPIO_InitStruct);
}

void DAT_MODE_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS1302_DAT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS1302_DAT_PORT, &GPIO_InitStruct);
}

void DS1302_GPIO_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS1302_SCK_PIN | DS1302_DAT_PIN | DS1302_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, DS1302_SCK_PIN | DS1302_DAT_PIN | DS1302_RST_PIN, GPIO_PIN_RESET);
}

void DS1302_WriteOneByte(uint8_t data)
{
    uint8_t index = 0;
    DAT_MODE_OUT();
    for (index = 0; index < 8; index++)
    {
        SCK_CLR;

        if (data & 0x01)
        {
            DAT_SET;
        }
        else
        {
            DAT_CLR;
        }
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

    for (index = 0; index < 8; index++)
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
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x00);

    DS1302_WriteByte(WRITE_SEC_REG, DataToBcd(date->sec));
    DS1302_WriteByte(WRITE_MIN_REG, DataToBcd(date->min));
    DS1302_WriteByte(WRITE_HOUR_REG, DataToBcd(date->hour));
    DS1302_WriteByte(WRITE_DAY_REG, DataToBcd(date->day));
    DS1302_WriteByte(WRITE_MON_REG, DataToBcd(date->mon));
    DS1302_WriteByte(WRITE_WEEK_REG, DataToBcd(date->week));
    DS1302_WriteByte(WRITE_YEAR_REG, DataToBcd(date->year));
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x80);
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
    if (BcdToData(DS1302_ReadByte(READ_RAM_REG)) == 0x01)
    {
        return 0;
    }
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x00);
    DS1302_WriteByte(WRITE_RAM_REG, DataToBcd(0x01));
    DS1302_WriteByte(WRITE_CONTROL_REG, 0x80);
    DS1302_DateSet(date);
    return 0;
}
