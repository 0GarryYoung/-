#ifndef __BSP_USART1_H
#define __BSP_USART1_H

#include "bsp_config.h"

#define USART1_RXBUFF_SIZE  48

extern uint32_t RxCounter;
extern char Usart1RecBuf[USART1_RXBUFF_SIZE];

void uart1_Init(uint32_t bound);
void uart1_SendStr(char* SendBuf);
void uart1_send(uint8_t *bufs, uint8_t len);

#endif
