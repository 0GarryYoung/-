#ifndef _DS_1302_H
#define _DS_1302_H

#include "bsp_types.h"

/* DS1302 3-wire RTC on PC13 (SCLK), PC14 (IO/DAT), PC15 (RST/CE).
 * GPIO access ported from SPL GPIO_SetBits/ResetBits/ReadInputDataBit to HAL. */
#define RST_CLR  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET) //CE low
#define RST_SET  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET)   //CE high

#define DAT_CLR  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET) //IO low
#define DAT_SET  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET)   //IO high
#define DAT_READ HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14)                  //read IO

#define SCK_CLR  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET) //SCLK low
#define SCK_SET  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)   //SCLK high

#define DataToBcd(x) ((x/10)*16+(x%10))
#define BcdToData(x) ((x/16)*10+(x%16))

#define WRITE_SEC_REG	        0x80
#define WRITE_MIN_REG           0x82
#define WRITE_HOUR_REG          0x84
#define WRITE_DAY_REG           0x86
#define WRITE_MON_REG           0x88
#define WRITE_WEEK_REG          0x8a
#define WRITE_YEAR_REG          0x8c
#define READ_SEC_REG            0x81
#define READ_MIN_REG            0x83
#define READ_HOUR_REG           0x85
#define READ_DAY_REG            0x87
#define READ_MON_REG            0x89
#define READ_WEEK_REG           0x8b
#define READ_YEAR_REG           0x8d
#define WRITE_CONTROL_REG       0x8e
#define READ_CONTROL_REG        0x8f
#define WRITE_CHARGE_REG        0x90
#define READ_CHARGE_REG         0x91
#define WRITE_RAM_REG           0xc0
#define READ_RAM_REG            0xc1

typedef struct
{
    volatile uint8_t sec;
    volatile uint8_t min;
    volatile uint8_t hour;
    volatile uint8_t day;
    volatile uint8_t mon;
    volatile uint8_t week;
    volatile uint16_t year;
}DATE;


int DS1302_Init(const DATE * const date);
int DS1302_DateSet(const DATE * const date);
int DS1302_DateRead(DATE * const date);

extern DATE SysDate;

#endif
