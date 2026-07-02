#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "bsp_config.h"

void DelayInit(void);
void DelayMs(uint32_t nms);
void DelayUs(uint32_t nus);
void DelayS(uint32_t ns);

#endif
