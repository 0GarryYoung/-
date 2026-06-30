#ifndef __BSP_TYPES_H
#define __BSP_TYPES_H

#include "stm32f1xx_hal.h"

/*
 * Short integer aliases used by the ported StdPeriph-era drivers (ADXL345,
 * DS18B20, stmflash, key...). The original code relied on these coming from
 * the SPL "sys.h"; under the HAL project we provide just the type aliases and
 * drop the SPL bit-band macros (replaced by HAL GPIO calls in each driver).
 */
typedef uint8_t           u8;
typedef uint16_t          u16;
typedef uint32_t          u32;
typedef volatile uint8_t  vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

#endif /* __BSP_TYPES_H */
