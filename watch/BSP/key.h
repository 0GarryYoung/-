#ifndef __KEY_H
#define __KEY_H
#include "bsp_types.h"

/* 4 keys on PB12..PB15 (active low, internal pull-ups).
 * Ported from the original Health_watch key.c so the app's KeySettings() gets
 * the familiar KEY_Scan() returning 1..4 (KEY0..KEY3). Replaces the CubeMX
 * starter key.c that used a different ('s'/'l') state machine. */
#define KEY0  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)
#define KEY1  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)
#define KEY2  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)
#define KEY3  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)

void KEY_Init(void);     //key IO init
u8 KEY_Scan(u8 mode);    //0:single trigger  1:continuous
#endif
