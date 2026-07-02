#ifndef __BSP_DS1302_H
#define __BSP_DS1302_H

#include "bsp_config.h"

#define DataToBcd(x) ((x / 10) * 16 + (x % 10))
#define BcdToData(x) ((x / 16) * 10 + (x % 16))

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
} DATE;

void DS1302_GPIO_Init(void);
void DAT_MODE_OUT(void);
void DAT_MODE_IN(void);
void DS1302_WriteOneByte(uint8_t data);
void DS1302_WriteByte(const uint8_t addr, const uint8_t data);
uint8_t DS1302_ReadByte(const uint8_t addr);
int DS1302_DateSet(const DATE * const date);
int DS1302_DateRead(DATE * const date);
int DS1302_Init(const DATE * const date);

extern DATE SysDate;

#endif
