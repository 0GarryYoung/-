#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f1xx_hal.h"

/* Timing helpers used by the bit-bang drivers (software I2C, DS1302, DS18B20).
 * DelayUs is backed by the Cortex-M3 DWT cycle counter for deterministic
 * microsecond timing; DelayMs/DelayS wrap the HAL SysTick tick. */
void DelayInit(void);            /* call once at start-up, enables DWT->CYCCNT */
void DelayUs(uint32_t nus);
void DelayMs(uint32_t nms);
void DelayS(uint32_t ns);

#endif /* __DELAY_H */
