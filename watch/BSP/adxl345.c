/***********************
 ADXL345 accelerometer (step counting) - software I2C on bus hi2c_adxl (PA6/PA7).
 Ported from the original Health_watch adxl345.c: the iic.c primitives have been
 replaced one-for-one by the handle-based sw_i2c calls.
************************/
#include "adxl345.h"
#include "sw_i2c.h"
#include "delay.h"

#define BUS  (&hi2c_adxl)

//init
void adxl345_init()
{
    adxl345_write_reg(0X31,0X0B);   //13-bit full resolution, right justify, 16g
    adxl345_write_reg(0x2C,0x0B);   //output data rate 100Hz
    adxl345_write_reg(0x2D,0x08);   //measure mode
    adxl345_write_reg(0X2E,0x80);
    adxl345_write_reg(0X1E,0x00);
    adxl345_write_reg(0X1F,0x00);
    adxl345_write_reg(0X20,0x05);
}
//write register
void adxl345_write_reg(u8 addr,u8 val)
{
    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, slaveaddress);     //write command
    SW_I2C_WaitAck(BUS);
    SW_I2C_SendByte(BUS, addr);             //register address
    SW_I2C_WaitAck(BUS);
    SW_I2C_SendByte(BUS, val);              //value
    SW_I2C_WaitAck(BUS);
    SW_I2C_Stop(BUS);
}
//read register
u8 adxl345_read_reg(u8 addr)
{
    u8 temp=0;
    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, slaveaddress);     //write command
    temp=SW_I2C_WaitAck(BUS);
    SW_I2C_SendByte(BUS, addr);             //register address
    temp=SW_I2C_WaitAck(BUS);
    SW_I2C_Start(BUS);                      //restart
    SW_I2C_SendByte(BUS, regaddress);       //read command
    temp=SW_I2C_WaitAck(BUS);
    temp=SW_I2C_ReceiveByte(BUS);           //read one byte, then NACK
    SW_I2C_SendNotAck(BUS);
    SW_I2C_Stop(BUS);
    return temp;
}
//read x/y/z data
void adxl345_read_data(short *x,short *y,short *z)
{
    u8 buf[6];
    u8 i;
    SW_I2C_Start(BUS);
    SW_I2C_SendByte(BUS, slaveaddress);     //write command
    SW_I2C_WaitAck(BUS);
    SW_I2C_SendByte(BUS, 0x32);             //data start register 0x32
    SW_I2C_WaitAck(BUS);

    SW_I2C_Start(BUS);                      //restart
    SW_I2C_SendByte(BUS, regaddress);       //read command
    SW_I2C_WaitAck(BUS);
    for(i=0;i<6;i++)
    {
        buf[i]=SW_I2C_ReceiveByte(BUS);
        if(i==5) SW_I2C_SendNotAck(BUS);    //last byte -> NACK
        else     SW_I2C_SendAck(BUS);       //continue -> ACK
    }
    SW_I2C_Stop(BUS);
    *x=(short)(((u16)buf[1]<<8)+buf[0]);
    *y=(short)(((u16)buf[3]<<8)+buf[2]);
    *z=(short)(((u16)buf[5]<<8)+buf[4]);
}
//averaged read over `times` samples
void adxl345_read_average(float *x,float *y,float *z,u8 times)
{
    u8 i;
    short tx,ty,tz;
    *x=0;
    *y=0;
    *z=0;
    if(times)
    {
        for(i=0;i<times;i++)
        {
            adxl345_read_data(&tx,&ty,&tz);
            *x+=tx;
            *y+=ty;
            *z+=tz;
            DelayMs(5);
        }
        *x/=times;
        *y/=times;
        *z/=times;
    }
}
