#ifndef __DS18B20_H
#define __DS18B20_H
#include "bsp_types.h"

/* DS18B20 1-Wire temperature sensor on PA11.
 * Direction is switched via direct CRH register writes (CMSIS, works under HAL);
 * the DQ level is driven / read through HAL GPIO. */
#define DS18B20_IO_IN()   {GPIOA->CRH&=0xFFFF0FFF;GPIOA->CRH|=0x00008000;}  //PA11 input
#define DS18B20_IO_OUT()  {GPIOA->CRH&=0xFFFF0FFF;GPIOA->CRH|=0x00003000;}  //PA11 output PP

#define DS18B20_DQ_OUT(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define DS18B20_DQ_IN()   (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET)

u8 DS18B20_Init(void);
short DS18B20_Get_Temp(void);
void DS18B20_Start(void);
void DS18B20_Write_Byte(u8 dat);
u8 DS18B20_Read_Byte(void);
u8 DS18B20_Read_Bit(void);
u8 DS18B20_Check(void);
void DS18B20_Rst(void);
#endif
