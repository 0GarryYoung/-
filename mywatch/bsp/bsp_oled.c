#include "bsp_oled.h"
#include "bsp_delay.h"
#include "bsp_codetab.h"

extern I2C_HandleTypeDef hi2c1;

void I2C_WriteByte(uint8_t addr, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, addr,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

void WriteCmd(unsigned char I2C_Command)
{
    I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(unsigned char I2C_Data)
{
    I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
    DelayMs(100);

    WriteCmd(0xAE);
    WriteCmd(0x20);
    WriteCmd(0x10);
    WriteCmd(0xb0);
    WriteCmd(0xc8);
    WriteCmd(0x00);
    WriteCmd(0x10);
    WriteCmd(0x40);
    WriteCmd(0x81);
    WriteCmd(0xff);
    WriteCmd(0xa1);
    WriteCmd(0xa6);
    WriteCmd(0xa8);
    WriteCmd(0x3F);
    WriteCmd(0xa4);
    WriteCmd(0xd3);
    WriteCmd(0x00);
    WriteCmd(0xd5);
    WriteCmd(0xf0);
    WriteCmd(0xd9);
    WriteCmd(0x22);
    WriteCmd(0xda);
    WriteCmd(0x12);
    WriteCmd(0xdb);
    WriteCmd(0x20);
    WriteCmd(0x8d);
    WriteCmd(0x14);
    WriteCmd(0xaf);
}

void OLED_SetPos(unsigned char x, unsigned char y)
{
    WriteCmd(0xb0 + y);
    WriteCmd(((x & 0xf0) >> 4) | 0x10);
    WriteCmd((x & 0x0f) | 0x01);
}

void OLED_Fill(unsigned char fill_Data)
{
    unsigned char m, n;
    for (m = 0; m < 8; m++)
    {
        WriteCmd(0xb0 + m);
        WriteCmd(0x00);
        WriteCmd(0x10);
        for (n = 0; n < 128; n++)
        {
            WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)
{
    OLED_Fill(0x00);
}

void OLED_ON(void)
{
    WriteCmd(0X8D);
    WriteCmd(0X14);
    WriteCmd(0XAF);
}

void OLED_OFF(void)
{
    WriteCmd(0X8D);
    WriteCmd(0X10);
    WriteCmd(0XAE);
}

void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char TextSize, u8 mode)
{
    unsigned char c = 0, i = 0;

    c = chr - ' ';
    if (TextSize == 2)
    {
        if (x > 120) { x = 0; y++; }
        OLED_SetPos(x, y);
        for (i = 0; i < 8; i++)
            if (mode == 1) WriteDat(~(F8X16[c * 16 + i])); else WriteDat(F8X16[c * 16 + i]);
        OLED_SetPos(x, y + 1);
        for (i = 0; i < 8; i++)
            if (mode == 1) WriteDat(~(F8X16[c * 16 + i + 8])); else WriteDat(F8X16[c * 16 + i + 8]);
    }
    else
    {
        if (x > 126) { x = 0; y++; }
        OLED_SetPos(x, y);
        for (i = 0; i < 6; i++)
            if (mode == 1) WriteDat(~(F6x8[c][i])); else WriteDat(F6x8[c][i]);
    }
}

void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0, i = 0, j = 0;
    switch (TextSize)
    {
    case 1:
        while (ch[j] != '\0')
        {
            c = ch[j] - 32;
            if (x > 126)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x, y);
            for (i = 0; i < 6; i++)
                WriteDat(F6x8[c][i]);
            x += 6;
            j++;
        }
        break;
    case 2:
        while (ch[j] != '\0')
        {
            c = ch[j] - 32;
            if (x > 120)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x, y);
            for (i = 0; i < 8; i++)
                WriteDat(F8X16[c * 16 + i]);
            OLED_SetPos(x, y + 1);
            for (i = 0; i < 8; i++)
                WriteDat(F8X16[c * 16 + i + 8]);
            x += 8;
            j++;
        }
        break;
    }
}

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N, u8 mode)
{
    unsigned char wm = 0;
    unsigned int adder = 32 * N;
    OLED_SetPos(x, y);
    for (wm = 0; wm < 16; wm++)
    {
        if (mode == 1) WriteDat(~(F16x16[adder])); else WriteDat(F16x16[adder]);
        adder += 1;
    }
    OLED_SetPos(x, y + 1);
    for (wm = 0; wm < 16; wm++)
    {
        if (mode == 1) WriteDat(~(F16x16[adder])); else WriteDat(F16x16[adder]);
        adder += 1;
    }
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        OLED_SetPos(x0, y);
        for (x = x0; x < x1; x++)
        {
            WriteDat(BMP[j++]);
        }
    }
}
