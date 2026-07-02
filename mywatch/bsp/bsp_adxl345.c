#include "bsp_adxl345.h"
#include "bsp_iic_adxl.h"
#include "bsp_delay.h"

void adxl345_init()
{
    adxl345_write_reg(0X31, 0X0B);
    adxl345_write_reg(0x2C, 0x0B);
    adxl345_write_reg(0x2D, 0x08);
    adxl345_write_reg(0X2E, 0x80);
    adxl345_write_reg(0X1E, 0x00);
    adxl345_write_reg(0X1F, 0x00);
    adxl345_write_reg(0X20, 0x05);
}

void adxl345_write_reg(u8 addr, u8 val)
{
    IIC_start();
    IIC_send_byte(slaveaddress);
    IIC_wait_ack();
    IIC_send_byte(addr);
    IIC_wait_ack();
    IIC_send_byte(val);
    IIC_wait_ack();
    IIC_stop();
}

u8 adxl345_read_reg(u8 addr)
{
    u8 temp = 0;
    IIC_start();
    IIC_send_byte(slaveaddress);
    temp = IIC_wait_ack();
    IIC_send_byte(addr);
    temp = IIC_wait_ack();
    IIC_start();
    IIC_send_byte(regaddress);
    temp = IIC_wait_ack();
    temp = IIC_read_byte(0);
    IIC_stop();
    return temp;
}

void adxl345_read_data(short *x, short *y, short *z)
{
    u8 buf[6];
    u8 i;
    IIC_start();
    IIC_send_byte(slaveaddress);
    IIC_wait_ack();
    IIC_send_byte(0x32);
    IIC_wait_ack();

    IIC_start();
    IIC_send_byte(regaddress);
    IIC_wait_ack();
    for (i = 0; i < 6; i++)
    {
        if (i == 5) buf[i] = IIC_read_byte(0);
        else buf[i] = IIC_read_byte(1);
    }
    IIC_stop();
    *x = (short)(((u16)buf[1] << 8) + buf[0]);
    *y = (short)(((u16)buf[3] << 8) + buf[2]);
    *z = (short)(((u16)buf[5] << 8) + buf[4]);
}

void adxl345_read_average(float *x, float *y, float *z, u8 times)
{
    u8 i;
    short tx, ty, tz;
    *x = 0;
    *y = 0;
    *z = 0;
    if (times)
    {
        for (i = 0; i < times; i++)
        {
            adxl345_read_data(&tx, &ty, &tz);
            *x += tx;
            *y += ty;
            *z += tz;
            DelayMs(5);
        }
        *x /= times;
        *y /= times;
        *z /= times;
    }
}

void ADXL345_Start(void) {}
void ADXL345_Stop(void) {}
void get_angle(float x_angle, float y_angle, float z_angle) {}
