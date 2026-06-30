#include "sw_i2c.h"
#include "delay.h"

/*
 * Bus instances. SCL/SDA pins match the original Health_watch wiring:
 *   OLED      -> PB6 (SCL) / PB7 (SDA)   (reuses the board's existing I2C pins)
 *   ADXL345   -> PA6 (SCL) / PA7 (SDA)
 *   MAX30102  -> PB11 (SCL) / PB10 (SDA)
 */
SW_I2C_Handle_t hi2c_oled     = { GPIOB, GPIO_PIN_6,  GPIOB, GPIO_PIN_7  };
SW_I2C_Handle_t hi2c_adxl     = { GPIOA, GPIO_PIN_6,  GPIOA, GPIO_PIN_7  };
SW_I2C_Handle_t hi2c_max30102 = { GPIOB, GPIO_PIN_11, GPIOB, GPIO_PIN_10 };

/* Per-phase bit delay. ~4 us per quarter-bit gives a bus rate near 100 kHz,
 * comfortable for every slave on this board. Raise it if a device misbehaves. */
#define SW_I2C_DELAY()   DelayUs(4)

#define SCL_HIGH(h)   ((h)->scl_port->BSRR = (h)->scl_pin)
#define SCL_LOW(h)    ((h)->scl_port->BRR  = (h)->scl_pin)
#define SDA_HIGH(h)   ((h)->sda_port->BSRR = (h)->sda_pin)
#define SDA_LOW(h)    ((h)->sda_port->BRR  = (h)->sda_pin)
#define SDA_READ(h)   (HAL_GPIO_ReadPin((h)->sda_port, (h)->sda_pin) == GPIO_PIN_SET)

static void sw_i2c_enable_clock(GPIO_TypeDef *port)
{
    if (port == GPIOA)      __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
#ifdef GPIOE
    else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
#endif
}

void SW_I2C_Init(SW_I2C_Handle_t *h)
{
    GPIO_InitTypeDef gi = {0};

    sw_i2c_enable_clock(h->scl_port);
    sw_i2c_enable_clock(h->sda_port);

    gi.Mode  = GPIO_MODE_OUTPUT_OD;     /* open-drain; relies on external pull-ups */
    gi.Pull  = GPIO_NOPULL;
    gi.Speed = GPIO_SPEED_FREQ_HIGH;

    gi.Pin = h->scl_pin;
    HAL_GPIO_Init(h->scl_port, &gi);
    gi.Pin = h->sda_pin;
    HAL_GPIO_Init(h->sda_port, &gi);

    SDA_HIGH(h);
    SCL_HIGH(h);
}

void SW_I2C_Start(SW_I2C_Handle_t *h)
{
    /* SDA falls while SCL is high */
    SDA_HIGH(h);
    SCL_HIGH(h);
    SW_I2C_DELAY();
    SDA_LOW(h);
    SW_I2C_DELAY();
    SCL_LOW(h);
    SW_I2C_DELAY();
}

void SW_I2C_Stop(SW_I2C_Handle_t *h)
{
    /* SDA rises while SCL is high */
    SDA_LOW(h);
    SCL_HIGH(h);
    SW_I2C_DELAY();
    SDA_HIGH(h);
    SW_I2C_DELAY();
}

void SW_I2C_SendByte(SW_I2C_Handle_t *h, uint8_t byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80) SDA_HIGH(h);
        else             SDA_LOW(h);
        SW_I2C_DELAY();
        SCL_HIGH(h);
        SW_I2C_DELAY();
        SCL_LOW(h);
        if (i == 7) SDA_HIGH(h);        /* release SDA after the last bit */
        byte <<= 1;
        SW_I2C_DELAY();
    }
}

uint8_t SW_I2C_ReceiveByte(SW_I2C_Handle_t *h)
{
    uint8_t i, value = 0;
    SDA_HIGH(h);                        /* release SDA so the slave can drive it */
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        SCL_HIGH(h);
        SW_I2C_DELAY();
        if (SDA_READ(h)) value++;
        SCL_LOW(h);
        SW_I2C_DELAY();
    }
    return value;
}

uint8_t SW_I2C_WaitAck(SW_I2C_Handle_t *h)
{
    uint8_t ack;
    SDA_HIGH(h);                        /* release SDA, let slave pull it low */
    SW_I2C_DELAY();
    SCL_HIGH(h);
    SW_I2C_DELAY();
    ack = SDA_READ(h) ? 1 : 0;          /* low = ACK */
    SCL_LOW(h);
    SW_I2C_DELAY();
    return ack;
}

void SW_I2C_SendAck(SW_I2C_Handle_t *h)
{
    SDA_LOW(h);
    SW_I2C_DELAY();
    SCL_HIGH(h);
    SW_I2C_DELAY();
    SCL_LOW(h);
    SW_I2C_DELAY();
    SDA_HIGH(h);
}

void SW_I2C_SendNotAck(SW_I2C_Handle_t *h)
{
    SDA_HIGH(h);
    SW_I2C_DELAY();
    SCL_HIGH(h);
    SW_I2C_DELAY();
    SCL_LOW(h);
    SW_I2C_DELAY();
}
