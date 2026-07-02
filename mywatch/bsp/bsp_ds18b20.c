#include "bsp_ds18b20.h"
#include "bsp_delay.h"

void DS18B20_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

void DS18B20_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

void DS18B20_Rst(void)
{
    DS18B20_IO_OUT();
    DS18B20_DQ_L();
    DelayUs(750);
    DS18B20_DQ_H();
    DelayUs(15);
}

u8 DS18B20_Check(void)
{
    u8 retry = 0;
    DS18B20_IO_IN();
    while (DS18B20_DQ_RD() && retry < 200)
    {
        retry++;
        DelayUs(1);
    };
    if (retry >= 200) return 1;
    else retry = 0;
    while (!DS18B20_DQ_RD() && retry < 240)
    {
        retry++;
        DelayUs(1);
    };
    if (retry >= 240) return 1;
    return 0;
}

u8 DS18B20_Read_Bit(void)
{
    u8 data;
    DS18B20_IO_OUT();
    DS18B20_DQ_L();
    DelayUs(2);
    DS18B20_DQ_H();
    DS18B20_IO_IN();
    DelayUs(12);
    if (DS18B20_DQ_RD()) data = 1;
    else data = 0;
    DelayUs(50);
    return data;
}

u8 DS18B20_Read_Byte(void)
{
    u8 i, j, dat;
    dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = DS18B20_Read_Bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

void DS18B20_Write_Byte(u8 dat)
{
    u8 j;
    u8 testb;
    DS18B20_IO_OUT();
    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        if (testb)
        {
            DS18B20_DQ_L();
            DelayUs(2);
            DS18B20_DQ_H();
            DelayUs(60);
        }
        else
        {
            DS18B20_DQ_L();
            DelayUs(60);
            DS18B20_DQ_H();
            DelayUs(2);
        }
    }
}

void DS18B20_Start(void)
{
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);
    DS18B20_Write_Byte(0x44);
}

u8 DS18B20_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    DS18B20_IO_OUT();
    DS18B20_DQ_H();

    DS18B20_Rst();
    return DS18B20_Check();
}

short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL, TH;
    short tem;
    DS18B20_Start();
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);
    DS18B20_Write_Byte(0xbe);
    TL = DS18B20_Read_Byte();
    TH = DS18B20_Read_Byte();

    if (TH > 7)
    {
        TH = ~TH;
        TL = ~TL;
        temp = 0;
    }
    else temp = 1;
    tem = TH;
    tem <<= 8;
    tem += TL;
    tem = (float)tem * 0.625;
    if (temp) return tem;
    else return -tem;
}
