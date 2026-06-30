/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "sw_i2c.h"
#include "OLED.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "stmflash.h"
#include "key.h"
#include "usart1.h"
#include "adxl345.h"
#include "algorithm.h"
#include "max30102.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_SAVE_ADDR  ((u32)0x0800F000)   /* step counter / first-boot marker */
#define MAX_BRIGHTNESS   255
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char setn = 0;            /* setting mode index, 0 = run */
unsigned char p_r  = 0;            /* 0 = common year, 1 = leap year */
float adx, ady, adz;
float acc;
u8  flag = 0;
u16 bushu = 0;                     /* step count */
short temperature = 0;
char display[16];

uint32_t aun_ir_buffer[150];       /* infrared LED sensor data */
uint32_t aun_red_buffer[150];      /* red LED sensor data */
int32_t n_ir_buffer_length;        /* data length */
int32_t n_spo2;                    /* SPO2 value */
int8_t  ch_spo2_valid;             /* SPO2 valid flag */
int32_t n_heart_rate;              /* heart rate value */
int8_t  ch_hr_valid;               /* heart rate valid flag */
uint8_t uch_dummy;

int32_t hr_buf[16];
int32_t hrSum;
int32_t hrAvg;                     /* averaged heart rate */
int32_t spo2_buf[16];
int32_t spo2Sum;
int32_t spo2Avg;                   /* averaged SpO2 */
int32_t spo2BuffFilled;
int32_t hrBuffFilled;
int32_t hrValidCnt = 0;
int32_t spo2ValidCnt = 0;
int32_t hrThrowOutSamp = 0;
int32_t spo2ThrowOutSamp = 0;
int32_t spo2Timeout = 0;
int32_t hrTimeout = 0;
uint32_t un_min, un_max, un_prev_data;
uint32_t un_brightness;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Init_MAX30102(void);
void GetHeartRateSpO2(void);
void DisplayTime(void);
void DisplayTemperature(void);
void CHECK_NEW_MCU(void);
void GetSteps(void);
void KeySettings(void);
void UsartSendData(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  DelayInit();                 /* DWT microsecond timer for the bit-bang drivers */

  OLED_Init();                 /* OLED + its software I2C bus (PB6/PB7) */
  KEY_Init();
  DelayMs(200);
  CHECK_NEW_MCU();             /* first-boot check + restore saved step count */
  OLED_CLS();
  DS18B20_Init();
  DS1302_Init(&SysDate);
  DelayMs(100);
  DS1302_DateRead(&SysDate);
  {
    char i;
    /* welcome line: huan ying shi yong shou biao xin lv (glyphs 8..15) */
    for (i = 0; i < 8; i++) OLED_ShowCN(i * 16, 2, i + 8, 0);
  }
  DelayMs(1000); DelayMs(1000);
  OLED_CLS();
  {
    char i;
    for (i = 0; i < 2; i++) OLED_ShowCN(i * 16,      4, i + 16, 0); /* bu shu   (steps) */
    for (i = 0; i < 2; i++) OLED_ShowCN(i * 16 + 48, 4, i + 18, 0); /* xue yang (SpO2)  */
    for (i = 0; i < 2; i++) OLED_ShowCN(i * 16 + 95, 4, i + 20, 0); /* wen du   (temp)  */
  }

  SW_I2C_Init(&hi2c_adxl);     /* ADXL345 bus (PA6/PA7) */
  adxl345_init();
  Init_MAX30102();             /* inits the MAX30102 bus (PB11/PB10) internally */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    KeySettings();
    DisplayTime();
    if (setn == 0)               /* in run mode, read all sensors */
    {
      DisplayTemperature();
      GetSteps();
      GetHeartRateSpO2();
    }
    UsartSendData();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Init_MAX30102(void)
{
    int32_t i;

    un_brightness = 0;
    un_min = 0x3FFFF;
    un_max = 0;

    SW_I2C_Init(&hi2c_max30102);            /* MAX30102 bus (PB11/PB10) */
    maxim_max30102_reset();
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy);
    maxim_max30102_init();

    n_ir_buffer_length = 150;               /* 150 samples = 3 s at 50 sps */

    /* read the first 150 samples and find the signal range */
    for (i = 0; i < n_ir_buffer_length; i++)
    {
        maxim_max30102_read_fifo((aun_ir_buffer + i), (aun_red_buffer + i));
        if (un_min > aun_red_buffer[i]) un_min = aun_red_buffer[i];
        if (un_max < aun_red_buffer[i]) un_max = aun_red_buffer[i];
    }
    un_prev_data = aun_red_buffer[i - 1];
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

void GetHeartRateSpO2(void)
{
    int32_t i;
    float f_temp;
    static u8 COUNT = 8;
    unsigned char x = 0;

    i = 0;
    un_min = 0x3FFFF;
    un_max = 0;

    /* drop the first 50 samples and shift the last 100 to the top */
    for (i = 50; i < 150; i++)
    {
        aun_red_buffer[i - 50] = aun_red_buffer[i];
        aun_ir_buffer[i - 50] = aun_ir_buffer[i];
        if (un_min > aun_red_buffer[i]) un_min = aun_red_buffer[i];
        if (un_max < aun_red_buffer[i]) un_max = aun_red_buffer[i];
    }

    /* take 50 new samples before recalculating the heart rate */
    for (i = 100; i < 150; i++)
    {
        un_prev_data = aun_red_buffer[i - 1];
        maxim_max30102_read_fifo((aun_ir_buffer + i), (aun_red_buffer + i));

        if (aun_red_buffer[i] > un_prev_data)
        {
            f_temp = aun_red_buffer[i] - un_prev_data;
            f_temp /= (un_max - un_min);
            f_temp *= MAX_BRIGHTNESS;
            f_temp = un_brightness - f_temp;
            if (f_temp < 0)
                un_brightness = 0;
            else
                un_brightness = (int)f_temp;
        }
        else
        {
            f_temp = un_prev_data - aun_red_buffer[i];
            f_temp /= (un_max - un_min);
            f_temp *= MAX_BRIGHTNESS;
            un_brightness += (int)f_temp;
            if (un_brightness > MAX_BRIGHTNESS)
                un_brightness = MAX_BRIGHTNESS;
        }
    }
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
    if (COUNT++ > 8)
    {
        COUNT = 0;
        temperature = DS18B20_Get_Temp();
        if ((ch_hr_valid == 1) && (n_heart_rate < 150) && (n_heart_rate > 60))
        {
            hrTimeout = 0;
            if (hrValidCnt == 4)
            {
                hrThrowOutSamp = 1;
                hrValidCnt = 0;
                for (i = 12; i < 16; i++)
                {
                    if (n_heart_rate < hr_buf[i] + 10)
                    {
                        hrThrowOutSamp = 0;
                        hrValidCnt   = 4;
                    }
                }
            }
            else
            {
                hrValidCnt = hrValidCnt + 1;
            }

            if (hrThrowOutSamp == 0)
            {
                for (i = 0; i < 15; i++) hr_buf[i] = hr_buf[i + 1];
                hr_buf[15] = n_heart_rate;
                if (hrBuffFilled < 16) hrBuffFilled = hrBuffFilled + 1;

                hrSum = 0;
                if (hrBuffFilled < 2)
                {
                    hrAvg = 0;
                }
                else if (hrBuffFilled < 4)
                {
                    for (i = 14; i < 16; i++) hrSum = hrSum + hr_buf[i];
                    hrAvg = hrSum >> 1;
                }
                else if (hrBuffFilled < 8)
                {
                    for (i = 12; i < 16; i++) hrSum = hrSum + hr_buf[i];
                    hrAvg = hrSum >> 2;
                }
                else if (hrBuffFilled < 16)
                {
                    for (i = 8; i < 16; i++) hrSum = hrSum + hr_buf[i];
                    hrAvg = hrSum >> 3;
                }
                else
                {
                    for (i = 0; i < 16; i++) hrSum = hrSum + hr_buf[i];
                    hrAvg = hrSum >> 4;
                }
            }
            hrThrowOutSamp = 0;
        }
        else
        {
            hrValidCnt = 0;
            if (hrTimeout == 4)
            {
                hrAvg = 0;
                hrBuffFilled = 0;
            }
            else
            {
                hrTimeout++;
            }
        }

        if ((ch_spo2_valid == 1) && (n_spo2 > 80))
        {
            spo2Timeout = 0;
            if (spo2ValidCnt == 4)
            {
                spo2ThrowOutSamp = 1;
                spo2ValidCnt = 0;
                for (i = 12; i < 16; i++)
                {
                    if (n_spo2 > spo2_buf[i] - 10)
                    {
                        spo2ThrowOutSamp = 0;
                        spo2ValidCnt   = 4;
                    }
                }
            }
            else
            {
                spo2ValidCnt = spo2ValidCnt + 1;
            }

            if (spo2ThrowOutSamp == 0)
            {
                for (i = 0; i < 15; i++) spo2_buf[i] = spo2_buf[i + 1];
                spo2_buf[15] = n_spo2;
                if (spo2BuffFilled < 16) spo2BuffFilled = spo2BuffFilled + 1;

                spo2Sum = 0;
                if (spo2BuffFilled < 2)
                {
                    spo2Avg = 0;
                }
                else if (spo2BuffFilled < 4)
                {
                    for (i = 14; i < 16; i++) spo2Sum = spo2Sum + spo2_buf[i];
                    spo2Avg = spo2Sum >> 1;
                }
                else if (spo2BuffFilled < 8)
                {
                    for (i = 12; i < 16; i++) spo2Sum = spo2Sum + spo2_buf[i];
                    spo2Avg = spo2Sum >> 2;
                }
                else if (spo2BuffFilled < 16)
                {
                    for (i = 8; i < 16; i++) spo2Sum = spo2Sum + spo2_buf[i];
                    spo2Avg = spo2Sum >> 3;
                }
                else
                {
                    for (i = 0; i < 16; i++) spo2Sum = spo2Sum + spo2_buf[i];
                    spo2Avg = spo2Sum >> 4;
                }
            }
            spo2ThrowOutSamp = 0;
        }
        else
        {
            spo2ValidCnt = 0;
            if (spo2Timeout == 4)
            {
                spo2Avg = 0;
                spo2BuffFilled = 0;
            }
            else
            {
                spo2Timeout++;
            }
        }
        /* show heart rate (cols 0..2) and SpO2 (cols 6..8) on page 6 */
        OLED_ShowChar((x++) * 8, 6, hrAvg % 1000 / 100 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, hrAvg % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, hrAvg % 10 + '0', 2, 0);
        x = 6;
        OLED_ShowChar((x++) * 8, 6, spo2Avg % 1000 / 100 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, spo2Avg % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, spo2Avg % 10 + '0', 2, 0);
    }
}

void DisplayTime(void)
{
    unsigned char i = 0, x = 0;
    u16 nian_temp;

    if (setn == 0) DS1302_DateRead(&SysDate);
    nian_temp = 2000 + SysDate.year;
    if ((nian_temp % 400 == 0) || ((nian_temp % 100 != 0) && (nian_temp % 4 == 0)))
        p_r = 1;
    else
        p_r = 0;

    OLED_ShowChar((x++) * 8, 0, '2', 2, setn + 1 - 1);
    OLED_ShowChar((x++) * 8, 0, '0', 2, setn + 1 - 1);
    OLED_ShowChar((x++) * 8, 0, SysDate.year / 10 + '0', 2, setn + 1 - 1);
    OLED_ShowChar((x++) * 8, 0, SysDate.year % 10 + '0', 2, setn + 1 - 1);
    OLED_ShowChar((x++) * 8, 0, '-', 2, 0);
    OLED_ShowChar((x++) * 8, 0, SysDate.mon / 10 + '0', 2, setn + 1 - 2);
    OLED_ShowChar((x++) * 8, 0, SysDate.mon % 10 + '0', 2, setn + 1 - 2);
    OLED_ShowChar((x++) * 8, 0, '-', 2, 0);
    OLED_ShowChar((x++) * 8, 0, SysDate.day / 10 + '0', 2, setn + 1 - 3);
    OLED_ShowChar((x++) * 8, 0, SysDate.day % 10 + '0', 2, setn + 1 - 3);

    OLED_ShowCN(i * 16 + 88, 0, 0, setn + 1 - 4);   /* "nian" glyph */
    switch (SysDate.week)
    {
    case 1: OLED_ShowCN(i * 16 + 104, 0, 1, setn + 1 - 4); break;
    case 2: OLED_ShowCN(i * 16 + 104, 0, 2, setn + 1 - 4); break;
    case 3: OLED_ShowCN(i * 16 + 104, 0, 3, setn + 1 - 4); break;
    case 4: OLED_ShowCN(i * 16 + 104, 0, 4, setn + 1 - 4); break;
    case 5: OLED_ShowCN(i * 16 + 104, 0, i + 5, setn + 1 - 4); break;
    case 6: OLED_ShowCN(i * 16 + 104, 0, 6, setn + 1 - 4); break;
    case 7: OLED_ShowCN(i * 16 + 104, 0, 7, setn + 1 - 4); break;
    }
    x = 0;
    OLED_ShowChar((x++) * 8, 2, SysDate.hour / 10 + '0', 2, setn + 1 - 5);
    OLED_ShowChar((x++) * 8, 2, SysDate.hour % 10 + '0', 2, setn + 1 - 5);
    OLED_ShowChar((x++) * 8, 2, ':', 2, 0);
    OLED_ShowChar((x++) * 8, 2, SysDate.min / 10 + '0', 2, setn + 1 - 6);
    OLED_ShowChar((x++) * 8, 2, SysDate.min % 10 + '0', 2, setn + 1 - 6);
    OLED_ShowChar((x++) * 8, 2, ':', 2, 0);
    OLED_ShowChar((x++) * 8, 2, SysDate.sec / 10 + '0', 2, setn + 1 - 7);
    OLED_ShowChar((x++) * 8, 2, SysDate.sec % 10 + '0', 2, setn + 1 - 7);
}

void DisplayTemperature(void)
{
    unsigned char x = 10;

    temperature = DS18B20_Get_Temp();
    OLED_ShowChar((x++) * 8, 2, temperature / 100 + '0', 2, 0);
    OLED_ShowChar((x++) * 8, 2, temperature % 100 / 10 + '0', 2, 0);
    OLED_ShowChar((x++) * 8, 2, '.', 2, 0);
    OLED_ShowChar((x++) * 8, 2, temperature % 10 + '0', 2, 0);
    OLED_ShowChar((x++) * 8, 2, 'C', 2, 0);
}

void CHECK_NEW_MCU(void)
{
    u16 comper_buf[6];                       /* 12 bytes, 2-byte aligned */
    char *comper_str = (char *)comper_buf;

    STMFLASH_Read(FLASH_SAVE_ADDR + 0x10, comper_buf, 5);   /* read 5 half-words */
    comper_str[10] = '\0';
    if (strstr(comper_str, "FDYDZ") == NULL)                /* fresh MCU */
    {
        STMFLASH_Write(FLASH_SAVE_ADDR + 0x10, (u16 *)"FDYDZ\0\0\0\0\0", 5);
        STMFLASH_Write(FLASH_SAVE_ADDR + 0x20, &bushu, 1);
    }
    DelayMs(100);
    STMFLASH_Read(FLASH_SAVE_ADDR + 0x20, &bushu, 1);       /* restore step count */
    if (bushu > 60000) bushu = 0;
}

void GetSteps(void)
{
    static u16 temp = 0;
    u16 x = 11;

    adxl345_read_average(&adx, &ady, &adz, 10);
    acc = adx;
    if (acc > 0)
    {
        if (acc / 10 > 5 && flag == 1)       /* peak detected -> one step */
        {
            flag = 0;
            if (bushu < 60000) bushu++;
            if (temp != bushu)
            {
                temp = bushu;
                STMFLASH_Write(FLASH_SAVE_ADDR + 0x20, &bushu, 1);
            }
        }
    }
    if (acc < 0)
    {
        acc = -acc;
        if (acc / 10 >= 5) flag = 1;
    }
    if (bushu > 9999)
    {
        OLED_ShowChar((x++) * 8, 6, bushu / 10000 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10000 / 1000 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 1000 / 100 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10 + '0', 2, 0);
    }
    else if (bushu > 999)
    {
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10000 / 1000 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 1000 / 100 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10 + '0', 2, 0);
    }
    else if (bushu > 99)
    {
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 1000 / 100 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10 + '0', 2, 0);
    }
    else if (bushu > 9)
    {
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 100 / 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
    }
    else
    {
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, bushu % 10 + '0', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
        OLED_ShowChar((x++) * 8, 6, ' ', 2, 0);
    }
}

void KeySettings(void)
{
    unsigned char keynum = 0;

    keynum = KEY_Scan(0);
    if (keynum == 1)                         /* enter / advance setting field */
    {
        setn++;
        if (setn > 7) setn = 0;
    }
    if (keynum == 2)                         /* increment */
    {
        if (setn == 1)
        {
            SysDate.year++;
            if (SysDate.year == 100) SysDate.year = 0;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 2)
        {
            SysDate.mon++;
            if (SysDate.mon == 13) SysDate.mon = 1;
            if ((SysDate.mon == 4) || (SysDate.mon == 6) || (SysDate.mon == 9) || (SysDate.mon == 11))
            {
                if (SysDate.day > 30) SysDate.day = 1;
            }
            else
            {
                if (SysDate.mon == 2)
                {
                    if (p_r == 1) { if (SysDate.day > 29) SysDate.day = 1; }
                    else          { if (SysDate.day > 28) SysDate.day = 1; }
                }
            }
            DS1302_DateSet(&SysDate);
        }
        if (setn == 3)
        {
            SysDate.day++;
            if ((SysDate.mon == 1) || (SysDate.mon == 3) || (SysDate.mon == 5) || (SysDate.mon == 7) || (SysDate.mon == 8) || (SysDate.mon == 10) || (SysDate.mon == 12))
            {
                if (SysDate.day == 32) SysDate.day = 1;
            }
            else
            {
                if (SysDate.mon == 2)
                {
                    if (p_r == 1) { if (SysDate.day == 30) SysDate.day = 1; }
                    else          { if (SysDate.day == 29) SysDate.day = 1; }
                }
                else
                {
                    if (SysDate.day == 31) SysDate.day = 1;
                }
            }
            DS1302_DateSet(&SysDate);
        }
        if (setn == 4)
        {
            SysDate.week++;
            if (SysDate.week == 8) SysDate.week = 1;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 5)
        {
            SysDate.hour++;
            if (SysDate.hour == 24) SysDate.hour = 0;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 6)
        {
            SysDate.min++;
            if (SysDate.min == 60) SysDate.min = 0;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 7)
        {
            SysDate.sec++;
            if (SysDate.sec == 60) SysDate.sec = 0;
            DS1302_DateSet(&SysDate);
        }
    }
    if (keynum == 3)                         /* decrement */
    {
        if (setn == 1)
        {
            if (SysDate.year == 0) SysDate.year = 100;
            SysDate.year--;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 2)
        {
            if (SysDate.mon == 1) SysDate.mon = 13;
            SysDate.mon--;
            if ((SysDate.mon == 4) || (SysDate.mon == 6) || (SysDate.mon == 9) || (SysDate.mon == 11))
            {
                if (SysDate.day > 30) SysDate.day = 1;
            }
            else
            {
                if (SysDate.mon == 2)
                {
                    if (p_r == 1) { if (SysDate.day > 29) SysDate.day = 1; }
                    else          { if (SysDate.day > 28) SysDate.day = 1; }
                }
            }
            DS1302_DateSet(&SysDate);
        }
        if (setn == 3)
        {
            SysDate.day--;
            if ((SysDate.mon == 1) || (SysDate.mon == 3) || (SysDate.mon == 5) || (SysDate.mon == 7) || (SysDate.mon == 8) || (SysDate.mon == 10) || (SysDate.mon == 12))
            {
                if (SysDate.day == 0) SysDate.day = 31;
            }
            else
            {
                if (SysDate.mon == 2)
                {
                    if (p_r == 1) { if (SysDate.day == 0) SysDate.day = 29; }
                    else          { if (SysDate.day == 0) SysDate.day = 28; }
                }
                else
                {
                    if (SysDate.day == 0) SysDate.day = 30;
                }
            }
            DS1302_DateSet(&SysDate);
        }
        if (setn == 4)
        {
            if (SysDate.week == 1) SysDate.week = 8;
            SysDate.week--;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 5)
        {
            if (SysDate.hour == 0) SysDate.hour = 24;
            SysDate.hour--;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 6)
        {
            if (SysDate.min == 0) SysDate.min = 60;
            SysDate.min--;
            DS1302_DateSet(&SysDate);
        }
        if (setn == 7)
        {
            if (SysDate.sec == 0) SysDate.sec = 60;
            SysDate.sec--;
            DS1302_DateSet(&SysDate);
        }
    }
    if (keynum == 4)                         /* reset step count */
    {
        bushu = 0;
        STMFLASH_Write(FLASH_SAVE_ADDR + 0x20, &bushu, 1);
        DelayMs(50);
    }
}

void UsartSendData(void)
{
    static u8 flag1 = 0, flag2 = 1;
    float temp = 0.0;

    if (flag1 != SysDate.sec)
    {
        flag1 = SysDate.sec;
        flag2 = !flag2;
        if (flag2 == 1)                      /* report every 2 seconds */
        {
            temp = (float)temperature / 10;
            printf("Temp:%4.1fC  \r\n", temp);
            DelayMs(10);
            printf("HR:%d  \r\n", hrAvg);
            DelayMs(10);
            printf("SpO2:%d  \r\n", spo2Avg);
            DelayMs(10);
            printf("Step:%d  \r\n", bushu);
            DelayMs(10);
            printf("\r\n");
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
