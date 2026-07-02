#ifndef __BSP_IIC_ADXL_H
#define __BSP_IIC_ADXL_H

#include "bsp_config.h"

void ADXL_SDA_OUT(void);
void ADXL_SDA_IN(void);
void IIC_init(void);
void IIC_start(void);
void IIC_stop(void);
void IIC_ack(void);
void IIC_noack(void);
u8 IIC_wait_ack(void);
void IIC_send_byte(u8 txd);
u8 IIC_read_byte(u8 ack);

#endif
