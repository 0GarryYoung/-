#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#include "main.h"

/* 类型别名，兼容原工程 */
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define vu16 volatile uint16_t

/* Flash 保存地址 */
#define FLASH_SAVE_ADDR  ((u32)0x0800F000)

/* ============================================================
 * OLED (Hardware I2C1 on PB6/PB7)
 * ============================================================ */
#define OLED_ADDRESS     0x78
#define OLED_I2C_HANDLE  hi2c1

/* ============================================================
 * ADXL345 Software I2C (PA7=SCL, PB0=SDA)
 * ============================================================ */
#define ADXL_SCL_PORT    GPIOA
#define ADXL_SCL_PIN     GPIO_PIN_7
#define ADXL_SDA_PORT    GPIOB
#define ADXL_SDA_PIN     GPIO_PIN_0

#define ADXL_SCL_H()     HAL_GPIO_WritePin(ADXL_SCL_PORT, ADXL_SCL_PIN, GPIO_PIN_SET)
#define ADXL_SCL_L()     HAL_GPIO_WritePin(ADXL_SCL_PORT, ADXL_SCL_PIN, GPIO_PIN_RESET)
#define ADXL_SDA_H()     HAL_GPIO_WritePin(ADXL_SDA_PORT, ADXL_SDA_PIN, GPIO_PIN_SET)
#define ADXL_SDA_L()     HAL_GPIO_WritePin(ADXL_SDA_PORT, ADXL_SDA_PIN, GPIO_PIN_RESET)
#define ADXL_SDA_READ()  HAL_GPIO_ReadPin(ADXL_SDA_PORT, ADXL_SDA_PIN)

/* ============================================================
 * MAX30102 Software I2C (PB11=SCL, PB10=SDA)
 * ============================================================ */
#define MAX_SCL_PORT     GPIOB
#define MAX_SCL_PIN      GPIO_PIN_11
#define MAX_SDA_PORT     GPIOB
#define MAX_SDA_PIN      GPIO_PIN_10

#define MAX_SCL_H()      HAL_GPIO_WritePin(MAX_SCL_PORT, MAX_SCL_PIN, GPIO_PIN_SET)
#define MAX_SCL_L()      HAL_GPIO_WritePin(MAX_SCL_PORT, MAX_SCL_PIN, GPIO_PIN_RESET)
#define MAX_SDA_H()      HAL_GPIO_WritePin(MAX_SDA_PORT, MAX_SDA_PIN, GPIO_PIN_SET)
#define MAX_SDA_L()      HAL_GPIO_WritePin(MAX_SDA_PORT, MAX_SDA_PIN, GPIO_PIN_RESET)
#define MAX_SDA_READ()   HAL_GPIO_ReadPin(MAX_SDA_PORT, MAX_SDA_PIN)
#define MAX_SCL_READ()   HAL_GPIO_ReadPin(MAX_SCL_PORT, MAX_SCL_PIN)

/* ============================================================
 * DS18B20 1-Wire (PA11)
 * ============================================================ */
#define DS18B20_PORT     GPIOA
#define DS18B20_PIN      GPIO_PIN_11

#define DS18B20_DQ_H()   HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET)
#define DS18B20_DQ_L()   HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET)
#define DS18B20_DQ_RD()  HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)

/* ============================================================
 * DS1302 (PC13=SCLK, PC14=IO/DAT, PC15=CE/RST)
 * ============================================================ */
#define DS1302_SCK_PORT  GPIOC
#define DS1302_SCK_PIN   GPIO_PIN_13
#define DS1302_DAT_PORT  GPIOC
#define DS1302_DAT_PIN   GPIO_PIN_14
#define DS1302_RST_PORT  GPIOC
#define DS1302_RST_PIN   GPIO_PIN_15

#define SCK_SET          HAL_GPIO_WritePin(DS1302_SCK_PORT, DS1302_SCK_PIN, GPIO_PIN_SET)
#define SCK_CLR          HAL_GPIO_WritePin(DS1302_SCK_PORT, DS1302_SCK_PIN, GPIO_PIN_RESET)
#define DAT_SET          HAL_GPIO_WritePin(DS1302_DAT_PORT, DS1302_DAT_PIN, GPIO_PIN_SET)
#define DAT_CLR          HAL_GPIO_WritePin(DS1302_DAT_PORT, DS1302_DAT_PIN, GPIO_PIN_RESET)
#define DAT_READ         HAL_GPIO_ReadPin(DS1302_DAT_PORT, DS1302_DAT_PIN)
#define RST_SET          HAL_GPIO_WritePin(DS1302_RST_PORT, DS1302_RST_PIN, GPIO_PIN_SET)
#define RST_CLR          HAL_GPIO_WritePin(DS1302_RST_PORT, DS1302_RST_PIN, GPIO_PIN_RESET)

/* ============================================================
 * Keys (PB12-PB15, input pull-up)
 * ============================================================ */
#define KEY0_PORT        GPIOB
#define KEY0_PIN         GPIO_PIN_12
#define KEY1_PORT        GPIOB
#define KEY1_PIN         GPIO_PIN_13
#define KEY2_PORT        GPIOB
#define KEY2_PIN         GPIO_PIN_14
#define KEY3_PORT        GPIOB
#define KEY3_PIN         GPIO_PIN_15

#define KEY0             HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)
#define KEY1             HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)
#define KEY2             HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN)
#define KEY3             HAL_GPIO_ReadPin(KEY3_PORT, KEY3_PIN)

/* 函数声明 */
void BSP_GPIO_HighSpeed_Init(void);

#endif /* __BSP_CONFIG_H */
