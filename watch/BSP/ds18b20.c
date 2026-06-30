#include "ds18b20.h"
#include "delay.h"

//reset DS18B20
void DS18B20_Rst(void)
{
    DS18B20_IO_OUT();   //PA11 output
    DS18B20_DQ_OUT(0);  //pull DQ low
    DelayUs(750);       //>480us
    DS18B20_DQ_OUT(1);  //release
    DelayUs(15);
}
//wait for presence pulse. 1: not detected, 0: ok
u8 DS18B20_Check(void)
{
    u8 retry=0;
    DS18B20_IO_IN();    //PA11 input
    while (DS18B20_DQ_IN()&&retry<200)
    {
        retry++;
        DelayUs(1);
    };
    if(retry>=200)return 1;
    else retry=0;
    while (!DS18B20_DQ_IN()&&retry<240)
    {
        retry++;
        DelayUs(1);
    };
    if(retry>=240)return 1;
    return 0;
}
//read one bit
u8 DS18B20_Read_Bit(void)
{
    u8 data;
    DS18B20_IO_OUT();
    DS18B20_DQ_OUT(0);
    DelayUs(2);
    DS18B20_DQ_OUT(1);
    DS18B20_IO_IN();
    DelayUs(12);
    if(DS18B20_DQ_IN())data=1;
    else data=0;
    DelayUs(50);
    return data;
}
//read one byte
u8 DS18B20_Read_Byte(void)
{
    u8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++)
    {
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }
    return dat;
}
//write one byte
void DS18B20_Write_Byte(u8 dat)
{
    u8 j;
    u8 testb;
    DS18B20_IO_OUT();
    for (j=1;j<=8;j++)
    {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb)
        {
            DS18B20_DQ_OUT(0);// Write 1
            DelayUs(2);
            DS18B20_DQ_OUT(1);
            DelayUs(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);// Write 0
            DelayUs(60);
            DS18B20_DQ_OUT(1);
            DelayUs(2);
        }
    }
}
//start temperature conversion
void DS18B20_Start(void)
{
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}
//init DS18B20 IO (PA11), return 1 if not present, 0 if ok
u8 DS18B20_Init(void)
{
    GPIO_InitTypeDef gi = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    gi.Pin   = GPIO_PIN_11;
    gi.Mode  = GPIO_MODE_OUTPUT_PP;
    gi.Pull  = GPIO_NOPULL;
    gi.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gi);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

    DS18B20_Rst();
    return DS18B20_Check();
}
//get temperature, unit 0.1C, range -550~1250
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
    short tem;
    DS18B20_Start();
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// read scratchpad
    TL=DS18B20_Read_Byte(); // LSB
    TH=DS18B20_Read_Byte(); // MSB

    if(TH>7)
    {
        TH=~TH;
        TL=~TL;
        temp=0;//negative
    }else temp=1;//positive
    tem=TH;
    tem<<=8;
    tem+=TL;
    tem=(float)tem*0.625;//convert
    if(temp)return tem;
    else return -tem;
}
