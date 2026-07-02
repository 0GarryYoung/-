#include "bsp_key.h"
#include "bsp_delay.h"

void KEY_Init(void)
{
    /* CubeMX 已配置按键引脚，此处无需重复初始化 */
}

u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;
    if (mode) key_up = 1;
    if (key_up && (KEY0 == GPIO_PIN_RESET || KEY1 == GPIO_PIN_RESET ||
                      KEY2 == GPIO_PIN_RESET || KEY3 == GPIO_PIN_RESET))
    {
        DelayMs(10);
        key_up = 0;
        if (KEY0 == GPIO_PIN_RESET) return 1;
        else if (KEY1 == GPIO_PIN_RESET) return 2;
        else if (KEY2 == GPIO_PIN_RESET) return 3;
        else if (KEY3 == GPIO_PIN_RESET) return 4;
    }
    else if (KEY0 == GPIO_PIN_SET && KEY1 == GPIO_PIN_SET &&
             KEY2 == GPIO_PIN_SET && KEY3 == GPIO_PIN_SET)
    {
        key_up = 1;
    }
    return 0;
}
