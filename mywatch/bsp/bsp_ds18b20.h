#ifndef __BSP_DS18B20_H
#define __BSP_DS18B20_H

#include "bsp_config.h"

void DS18B20_IO_OUT(void);
void DS18B20_IO_IN(void);
u8 DS18B20_Init(void);
short DS18B20_Get_Temp(void);
void DS18B20_Start(void);
void DS18B20_Write_Byte(u8 dat);
u8 DS18B20_Read_Byte(void);
u8 DS18B20_Read_Bit(void);
u8 DS18B20_Check(void);
void DS18B20_Rst(void);

#endif
