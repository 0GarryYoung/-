/************************************************************************************
 * 128x64 SSD1306 OLED driver over software I2C (sw_i2c, bus hi2c_oled = PB6/PB7).
 *
 * Ported from the original Health_watch "OLED_I2C.c": every display routine is
 * kept byte-for-byte; only the low-level WriteCmd/WriteDat were redirected from
 * the STM32 hardware I2C peripheral to the bit-bang SW_I2C primitives.
 ***********************************************************************************/

#include "OLED.h"
#include "OLED_Font.h"
#include "sw_i2c.h"
#include "delay.h"

/* One full I2C transaction: START - 0x78 - ctrl - data - STOP.
 * ctrl = 0x00 -> command, 0x40 -> display data. */
static void OLED_WriteByte(uint8_t ctrl, uint8_t data)
{
    SW_I2C_Start(&hi2c_oled);
    SW_I2C_SendByte(&hi2c_oled, OLED_ADDRESS);
    SW_I2C_WaitAck(&hi2c_oled);
    SW_I2C_SendByte(&hi2c_oled, ctrl);
    SW_I2C_WaitAck(&hi2c_oled);
    SW_I2C_SendByte(&hi2c_oled, data);
    SW_I2C_WaitAck(&hi2c_oled);
    SW_I2C_Stop(&hi2c_oled);
}

#define WriteCmd(cmd)   OLED_WriteByte(0x00, (cmd))
#define WriteDat(dat)   OLED_WriteByte(0x40, (dat))

void OLED_Init(void)
{
    SW_I2C_Init(&hi2c_oled);     /* own the OLED bus pins (PB6/PB7) */
    DelayMs(100);                /* power-up settle */

    WriteCmd(0xAE); //display off
    WriteCmd(0x20); //Set Memory Addressing Mode
    WriteCmd(0x10); //00,Horizontal;01,Vertical;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8); //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //brightness 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
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
        WriteCmd(0xb0 + m);     //page0-page7
        WriteCmd(0x00);         //low column start address
        WriteCmd(0x10);         //high column start address
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
    WriteCmd(0X8D);  //set charge pump
    WriteCmd(0X14);  //enable charge pump
    WriteCmd(0XAF);  //OLED wake up
}

void OLED_OFF(void)
{
    WriteCmd(0X8D);  //set charge pump
    WriteCmd(0X10);  //disable charge pump
    WriteCmd(0XAE);  //OLED sleep
}

/* x:0~127  y:0~7  chr:ASCII  TextSize:1(6x8)/2(8x16)  mode:1 inverse/0 normal */
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char TextSize, uint8_t mode)
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
        {
            while (ch[j] != '\0')
            {
                c = ch[j] - 32;
                if (x > 126) { x = 0; y++; }
                OLED_SetPos(x, y);
                for (i = 0; i < 6; i++)
                    WriteDat(F6x8[c][i]);
                x += 6;
                j++;
            }
        } break;
        case 2:
        {
            while (ch[j] != '\0')
            {
                c = ch[j] - 32;
                if (x > 120) { x = 0; y++; }
                OLED_SetPos(x, y);
                for (i = 0; i < 8; i++)
                    WriteDat(F8X16[c * 16 + i]);
                OLED_SetPos(x, y + 1);
                for (i = 0; i < 8; i++)
                    WriteDat(F8X16[c * 16 + i + 8]);
                x += 8;
                j++;
            }
        } break;
    }
}

/* 16x16 Chinese glyph N from F16x16[].  mode:1 inverse/0 normal */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N, uint8_t mode)
{
    unsigned char wm = 0;
    unsigned int  adder = 32 * N;
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
