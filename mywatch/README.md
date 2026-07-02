# mywatch 下位机开发文档

> 基于 STM32F103C8T6 + HAL 库的健康监测手表/手环下位机工程。

---

## 目录

1. [项目概述](#1-项目概述)
2. [硬件平台](#2-硬件平台)
3. [功能介绍](#3-功能介绍)
4. [系统工作流程](#4-系统工作流程)
5. [引脚分配与外设配置](#5-引脚分配与外设配置)
6. [CubeMX 配置摘要](#6-cubemx-配置摘要)
7. [软件架构](#7-软件架构)
8. [各传感器/模块驱动原理](#8-各传感器模块驱动原理)
   - 8.1 [OLED 显示屏 SSD1306](#81-oled-显示屏-ssd1306)
   - 8.2 [ADXL345 三轴加速度计](#82-adxl345-三轴加速度计)
   - 8.3 [MAX30102 心率血氧传感器](#83-max30102-心率血氧传感器)
   - 8.4 [DS18B20 温度传感器](#84-ds18b20-温度传感器)
   - 8.5 [DS1302 实时时钟](#85-ds1302-实时时钟)
   - 8.6 [按键输入](#86-按键输入)
   - 8.7 [串口/WiFi 模块](#87-串口wifi-模块)
   - 8.8 [内部 Flash 存储](#88-内部-flash-存储)
9. [主循环逻辑](#9-主循环逻辑)
10. [数据发送协议](#10-数据发送协议)
11. [编译与下载](#11-编译与下载)
12. [注意事项与常见问题](#12-注意事项与常见问题)

---

## 1. 项目概述

mywatch 是一个以 STM32F103C8T6 为主控的健康监测下位机，集成 OLED 显示、实时时钟、体温/心率/血氧/步数监测，并通过串口把数据上报给上位机或 WiFi 模块。本工程使用 **STM32CubeMX + Keil5 + HAL 库** 开发。

---

## 2. 硬件平台

| 项目 | 参数 |
|------|------|
| MCU | STM32F103C8T6 |
| 内核 | ARM Cortex-M3 |
| 主频 | 72 MHz |
| Flash | 64 KB |
| RAM | 20 KB |
| 封装 | LQFP48 |
| 供电 | 3.3 V |

---

## 3. 功能介绍

| 功能 | 说明 |
|------|------|
| 时间显示 | 通过 DS1302 RTC 显示年月日、星期、时分秒 |
| 体温测量 | DS18B20 单总线数字温度传感器 |
| 心率/血氧测量 | MAX30102 光电容积脉搏波（PPG）传感器 |
| 步数统计 | ADXL345 三轴加速度计检测步态 |
| OLED 显示 | 128×64 点阵 I2C OLED（SSD1306） |
| 数据上报 | USART1 串口每 2 秒发送一次数据给 WiFi/蓝牙模块 |
| 按键设置 | 4 个按键用于调整时间和清零步数 |
| 掉电保存 | 步数存储在内部 Flash，掉电不丢失 |

---

## 4. 系统工作流程

```
上电
 │
 ├─ HAL 初始化、系统时钟 72MHz
 │
 ├─ 外设初始化
 │   ├─ GPIO（按键、软件 I2C、1-Wire、DS1302）
 │   ├─ I2C1（OLED）
 │   ├─ USART1（WiFi/串口）
 │   └─ 重新配置 GPIO 为高速模式
 │
 ├─ 应用层初始化
 │   ├─ OLED 初始化并显示欢迎界面
 │   ├─ 从内部 Flash 读取步数
 │   ├─ DS18B20 初始化
 │   ├─ DS1302 初始化并读取当前时间
 │   ├─ ADXL345 初始化
 │   └─ MAX30102 初始化，采集前 150 个样本
 │
 └─ 进入主循环
     ├─ 按键扫描与设置处理
     ├─ 显示时间和温度
     ├─ 采集步数（ADXL345）
     ├─ 采集心率血氧（MAX30102）
     └─ 每 2 秒串口发送一次数据
```

---

## 5. 引脚分配与外设配置

| 功能 | 信号 | 引脚 | CubeMX 模式 | 说明 |
|------|------|------|-------------|------|
| OLED | I2C1_SCL | PB6 | I2C1_SCL（AF_OD）| 硬件 I2C1，400kHz |
| OLED | I2C1_SDA | PB7 | I2C1_SDA（AF_OD）| 硬件 I2C1，400kHz |
| ADXL345 | SCL | PA7 | GPIO_Output OD | 软件 I2C |
| ADXL345 | SDA | PB0 | GPIO_Output OD | 软件 I2C |
| MAX30102 | SCL | PB11 | GPIO_Output OD | 软件 I2C |
| MAX30102 | SDA | PB10 | GPIO_Output OD | 软件 I2C |
| DS18B20 | DQ | PA11 | GPIO_Output OD | 1-Wire 单总线 |
| DS1302 | SCLK | PC13 | GPIO_Output PP | 3 线 RTC |
| DS1302 | IO/DAT | PC14 | GPIO_Output PP | 3 线 RTC |
| DS1302 | RST/CE | PC15 | GPIO_Output PP | 3 线 RTC |
| WiFi/串口 | TX | PA9 | USART1_TX（AF_PP）| 9600 baud |
| WiFi/串口 | RX | PA10 | USART1_RX（AF_PP）| 9600 baud |
| 按键 KEY0 | - | PB12 | GPIO_Input PU | 设置模式切换 |
| 按键 KEY1 | - | PB13 | GPIO_Input PU | 数值加 |
| 按键 KEY2 | - | PB14 | GPIO_Input PU | 数值减 |
| 按键 KEY3 | - | PB15 | GPIO_Input PU | 步数清零 |
| SWD | SWDIO | PA13 | Serial Wire | 调试用 |
| SWD | SWCLK | PA14 | Serial Wire | 调试用 |

> 注：原工程有 LED/BEEP，但与 DS1302/MAX30102 引脚冲突，且从未被初始化，本工程已省略。

---

## 6. CubeMX 配置摘要

### 6.1 时钟

- HSE：8 MHz 外部晶振
- PLL：×9 倍频
- SYSCLK：72 MHz
- AHB：72 MHz
- APB1：36 MHz
- APB2：72 MHz

### 6.2 SYS

- Debug：Serial Wire
- Timebase Source：SysTick

### 6.3 I2C1

- Mode：I2C
- Speed Mode：Fast Mode
- Clock Frequency：400000 Hz（400 kHz）
- 不使用 DMA

### 6.4 USART1

- Mode：Asynchronous
- Baud Rate：9600
- Word Length：8 Bits
- Stop Bits：1
- Parity：None
- NVIC：USART1 global interrupt 使能

### 6.5 GPIO

软件 I2C、1-Wire、DS1302 引脚配置为 **Output Open Drain** 或 **Output Push Pull**，按键配置为 **Input Pull-Up**。

---

## 7. 软件架构

```
mywatch/
├── Core/
│   ├── Inc/                 # CubeMX 生成的头文件
│   └── Src/                 # CubeMX 生成的源文件 + main.c 业务逻辑
├── Drivers/
│   ├── STM32F1xx_HAL_Driver/# HAL 库
│   └── CMSIS/               # CMSIS 核心文件
├── bsp/                     # 板级支持包（用户驱动）
│   ├── bsp_config.h         # 引脚宏、类型别名
│   ├── bsp_delay.c/h        # 延时函数
│   ├── bsp_oled.c/h         # OLED 驱动
│   ├── bsp_iic_adxl.c/h     # ADXL345 软件 I2C
│   ├── bsp_iic_max.c/h      # MAX30102 软件 I2C
│   ├── bsp_adxl345.c/h      # ADXL345 寄存器操作
│   ├── bsp_max30102.c/h     # MAX30102 寄存器操作
│   ├── bsp_algorithm.c/h    # Maxim 心率血氧算法
│   ├── bsp_ds18b20.c/h      # DS18B20 驱动
│   ├── bsp_ds1302.c/h       # DS1302 驱动
│   ├── bsp_usart1.c/h       # 串口/WiFi 驱动
│   ├── bsp_key.c/h          # 按键驱动
│   ├── bsp_flash.c/h        # 内部 Flash 读写
│   └── bsp_codetab.h        # OLED 字库
└── MDK-ARM/
    └── mywatch.uvprojx      # Keil 工程文件
```

---

## 8. 各传感器/模块驱动原理

### 8.1 OLED 显示屏 SSD1306

#### 硬件接口

- 控制器：SSD1306
- 分辨率：128 × 64
- 通信方式：硬件 I2C1（PB6/PB7）
- I2C 地址：0x78（写地址）

#### 驱动文件

- `bsp/bsp_oled.c`
- `bsp/bsp_oled.h`
- `bsp/bsp_codetab.h`（字库）

#### 写时序

SSD1306 的 I2C 通信协议：

```
START
Device Address (0x78) + W
Control Byte: 0x00 表示后续是命令；0x40 表示后续是数据
Command/Data
STOP
```

HAL 实现：

```c
HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, addr,
                  I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
```

#### 常用命令

| 命令 | 功能 |
|------|------|
| 0xAE | 关闭显示 |
| 0xAF | 打开显示 |
| 0xB0 ~ 0xB7 | 设置页地址 |
| 0x00 ~ 0x0F | 设置列低地址 |
| 0x10 ~ 0x1F | 设置列高地址 |
| 0x81, contrast | 设置对比度 |

#### 显示函数

- `OLED_ShowChar(x, y, chr, size, mode)`：显示 ASCII 字符
- `OLED_ShowCN(x, y, N, mode)`：显示中文字库中第 N 个汉字
- `OLED_ShowStr(x, y, str, size)`：显示字符串
- `OLED_DrawBMP(...)`：显示位图

---

### 8.2 ADXL345 三轴加速度计

#### 硬件接口

- 通信方式：软件 I2C
- SCL：PA7
- SDA：PB0
- I2C 写地址：0xA6
- I2C 读地址：0xA7

#### 驱动文件

- `bsp/bsp_iic_adxl.c/h`：软件 I2C 底层
- `bsp/bsp_adxl345.c/h`：ADXL345 寄存器操作

#### 软件 I2C 时序

标准 I2C 协议：

```
START:  SCL=1, SDA 从高变低
发送 8 位数据：SCL 低时改变 SDA，SCL 高时保持 SDA
ACK:    第 9 个时钟，从机拉低 SDA
STOP:   SCL=1, SDA 从低变高
```

代码中通过 `DelayUs()` 实现微秒级延时，控制 SCL 频率约几十到一百 kHz。

#### 初始化配置

```c
adxl345_write_reg(0x31, 0x0B);  // 16g 量程，13 位全分辨率
adxl345_write_reg(0x2C, 0x0B);  // 100Hz 数据输出速率
adxl345_write_reg(0x2D, 0x08);  // 测量模式
adxl345_write_reg(0x2E, 0x80);  // 使能 DATA_READY 中断
adxl345_write_reg(0x1E, 0x00);  // X 偏移
adxl345_write_reg(0x1F, 0x00);  // Y 偏移
adxl345_write_reg(0x20, 0x05);  // Z 偏移
```

#### 数据读取

加速度数据从寄存器 0x32 ~ 0x37 读取，共 6 字节：

```
DATAX0 (0x32), DATAX1 (0x33) -> X 轴低/高字节
DATAY0 (0x34), DATAY1 (0x35) -> Y 轴低/高字节
DATAZ0 (0x36), DATAZ1 (0x37) -> Z 轴低/高字节
```

每个轴为有符号 16 位数据，分辨率约 4 mg/LSB（全分辨率模式）。

#### 计步算法

程序只读取 X 轴加速度，通过阈值检测判断一步：

```
if (acc > 0 且 acc/10 > 5 且 flag==1) -> 计一步，flag=0
if (acc < 0 且 |acc|/10 >= 5) -> flag=1
```

即检测一次正向峰值和负向峰值的交替，完成一步计数。

---

### 8.3 MAX30102 心率血氧传感器

#### 硬件接口

- 通信方式：软件 I2C
- SCL：PB11
- SDA：PB10
- I2C 写地址：0xAE
- I2C 读地址：0xAF

#### 驱动文件

- `bsp/bsp_iic_max.c/h`：软件 I2C 底层
- `bsp/bsp_max30102.c/h`：MAX30102 寄存器操作
- `bsp/bsp_algorithm.c/h`：Maxim 官方心率血氧算法

#### 初始化配置

```c
maxim_max30102_write_reg(REG_INTR_ENABLE_1, 0xC0);
maxim_max30102_write_reg(REG_INTR_ENABLE_2, 0x00);
maxim_max30102_write_reg(REG_FIFO_WR_PTR, 0x00);
maxim_max30102_write_reg(REG_OVF_COUNTER, 0x00);
maxim_max30102_write_reg(REG_FIFO_RD_PTR, 0x00);
maxim_max30102_write_reg(REG_FIFO_CONFIG, 0x6F);  // 8 点平均，FIFO 不溢出
maxim_max30102_write_reg(REG_MODE_CONFIG, 0x03);  // SpO2 模式
maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x2F);  // 4096nA，400Hz，411us 脉宽
maxim_max30102_write_reg(REG_LED1_PA, 0x17);      // LED 电流
maxim_max30102_write_reg(REG_LED2_PA, 0x17);
maxim_max30102_write_reg(REG_PILOT_PA, 0x7F);
```

#### FIFO 数据读取

每次从 FIFO 读取 6 字节：

```
[RED_MSB][RED_MID][RED_LSB][IR_MSB][IR_MID][IR_LSB]
```

每个样本为 18 位数据，低 18 位有效。

#### 心率血氧算法

算法文件 `bsp/bsp_algorithm.c` 实现 Maxim 官方算法：

1. 采集 150 个样本（约 3 秒，采样率 50Hz）
2. 对 IR 信号去除直流分量，4 点移动平均
3. 用峰值检测找出脉搏波的波峰
4. 根据相邻波峰间距计算心率：
   ```
   HR = (FS * 60) / 平均波峰间隔
   ```
5. 计算 Red/IR 的交流/直流比值，查表得到血氧值

算法参数：

- `FS = 50`：采样频率 50Hz
- `BUFFER_SIZE = 150`：3 秒数据缓冲

---

### 8.4 DS18B20 温度传感器

#### 硬件接口

- 通信方式：1-Wire 单总线
- DQ：PA11
- 需要 4.7kΩ 上拉电阻（可内部上拉 + 外部上拉）

#### 驱动文件

- `bsp/bsp_ds18b20.c/h`

#### 1-Wire 时序

##### 复位脉冲

```
主机拉低 DQ 750us
主机释放 DQ 15us
等待从机回应脉冲（60~240us 低电平）
```

##### 写时隙

- 写 0：主机拉低 DQ 60us
- 写 1：主机拉低 DQ 2us，然后释放 60us

##### 读时隙

```
主机拉低 DQ 2us
释放 DQ，等待 12us 后读取 DQ 状态
整个时隙持续 60us
```

#### 温度读取流程

```
复位 -> 发送 0xCC（Skip ROM） -> 发送 0x44（启动转换） -> 延时
复位 -> 发送 0xCC -> 发送 0xBE（读暂存器） -> 读取 9 字节
```

程序中只读取前 2 字节（温度 LSB/MSB），分辨率为 0.0625°C，程序中乘以 0.625 后按 0.1°C 显示。

---

### 8.5 DS1302 实时时钟

#### 硬件接口

- 通信方式：3 线串行（类似 SPI，半双工）
- SCLK：PC13
- IO/DAT：PC14
- RST/CE：PC15

#### 驱动文件

- `bsp/bsp_ds1302.c/h`

#### 3 线通信时序

```
RST 拉低 -> SCLK 拉低 -> 延时 5us
RST 拉高 -> 延时 5us
发送 8 位命令（SCLK 上升沿采样 IO）
发送/接收 8 位数据
RST 拉低结束
```

#### 寄存器

| 寄存器 | 地址 | 说明 |
|--------|------|------|
| 秒 | 0x80 写 / 0x81 读 | CH=0 使能时钟 |
| 分 | 0x82 / 0x83 | |
| 时 | 0x84 / 0x85 | |
| 日 | 0x86 / 0x87 | |
| 月 | 0x88 / 0x89 | |
| 星期 | 0x8A / 0x8B | |
| 年 | 0x8C / 0x8D | |
| 控制 | 0x8E / 0x8F | WP=0 允许写 |

#### 时间格式

DS1302 内部以 **BCD 码**存储时间，驱动中通过宏转换：

```c
#define DataToBcd(x) ((x / 10) * 16 + (x % 10))
#define BcdToData(x) ((x / 16) * 10 + (x % 16))
```

#### 初始化时间

在 `bsp/bsp_ds1302.c` 中：

```c
DATE SysDate = {0, 0, 12, 22, 7, 4, 21};
// 表示：秒=0, 分=0, 时=12, 日=22, 月=7, 星期=4, 年=21 -> 2021-07-22 12:00:00 周四
```

修改初始时间只需改这行。

---

### 8.6 按键输入

#### 硬件接口

- KEY0：PB12
- KEY1：PB13
- KEY2：PB14
- KEY3：PB15
- 全部配置为 **输入上拉**，按下时为低电平

#### 驱动文件

- `bsp/bsp_key.c/h`

#### 按键功能

| 按键 | 引脚 | 功能 |
|------|------|------|
| KEY0 | PB12 | 进入/切换设置项（setn 1~7） |
| KEY1 | PB13 | 当前设置项 +1 |
| KEY2 | PB14 | 当前设置项 -1 |
| KEY3 | PB15 | 清零步数 |

#### 设置模式

按 KEY0 循环切换：

| setn | 设置内容 |
|------|----------|
| 1 | 年 |
| 2 | 月 |
| 3 | 日 |
| 4 | 星期 |
| 5 | 时 |
| 6 | 分 |
| 7 | 秒 |
| 0 | 正常显示模式 |

只有在 `setn == 0` 时，才刷新温度、步数、心率血氧。

---

### 8.7 串口/WiFi 模块

#### 硬件接口

- USART1：PA9（TX）、PA10（RX）
- 波特率：9600
- 可接 HC-05 蓝牙模块或 ESP8266 等 WiFi 模块

#### 驱动文件

- `bsp/bsp_usart1.c/h`

#### printf 重定向

通过 `fputc()` 把 `printf()` 输出重定向到 USART1：

```c
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
```

需要开启 **MicroLIB**。

#### 数据发送

在 [`UsartSendData()`](mywatch/Core/Src/main.c) 中，每 2 秒发送一次：

```
温度: 36.5C
心率: 75
血氧: 98
步数: 1234
```

#### 数据接收

USART1 中断把收到的字节存入 `Usart1RecBuf[48]`，当前代码只接收不解析。如需与 WiFi 模块交互，可在此缓冲区处理返回数据。

---

### 8.8 内部 Flash 存储

#### 存储地址

```c
#define FLASH_SAVE_ADDR  ((u32)0x0800F000)
```

| 偏移 | 内容 | 说明 |
|------|------|------|
| +0x10 | "FDYDZ" | 首次上电标记 |
| +0x20 | bushu | 步数值 |

#### 驱动文件

- `bsp/bsp_flash.c/h`

#### 读写机制

STM32F103C8T6 的 Flash 按 **1KB 页**擦除，按 **16 位半字**编程。

写入流程：

1. 读取整页 1KB 数据到 RAM 缓冲
2. 修改对应地址的数据
3. 擦除整页
4. 写回整页

读取流程：

```c
return *(vu16*)faddr;
```

直接读取 Flash 地址即可。

#### 清零步数

方法 1：按 KEY3

方法 2：代码中调用

```c
bushu = 0;
STMFLASH_Write(FLASH_SAVE_ADDR + 0x20, &bushu, 1);
```

方法 3：整片擦除 Flash 后重新下载程序

---

## 9. 主循环逻辑

```c
while (1)
{
    KeySettings();          // 扫描按键，处理时间设置
    DisplayTime();          // 显示日期时间
    if (setn == 0)          // 正常模式下才更新传感器
    {
        DisplayTemperature();   // 显示温度
        GetSteps();             // 读取 ADXL345 并计步
        GetHeartRateSpO2();     // 读取 MAX30102 并计算心率血氧
    }
    UsartSendData();        // 串口发送数据
}
```

---

## 10. 数据发送协议

通过 USART1 每 2 秒发送一次，格式如下：

```text
温度: 36.5C  \r\n
心率: 75  \r\n
血氧: 98  \r\n
步数: 1234  \r\n
\r\n
```

编码：ASCII
行尾：`\r\n`

如果对接的是 ESP8266 等 WiFi 模块，可以在 `UsartSendData()` 中把格式改为 JSON 或 URL 参数形式，例如：

```c
sprintf(buf, "{\"temp\":%.1f,\"hr\":%d,\"spo2\":%d,\"step\":%d}\r\n",
        temp, hrAvg, spo2Avg, bushu);
uart1_SendStr(buf);
```

---

## 11. 编译与下载

### 11.1 编译

1. 用 Keil uVision5 打开 `MDK-ARM/mywatch.uvprojx`
2. 确认以下设置：
   - `Options for Target -> C/C++`：定义 `USE_HAL_DRIVER`、`STM32F103xB`
   - `Options for Target -> C/C++`：Include Path 包含 `../bsp`
   - `Options for Target -> Target`：勾选 **Use MicroLIB**
   - `Flash Download`：选择 `STM32F10x Med-density Flash`，勾选 `Reset and Run`
3. 点击 **Rebuild** 编译

### 11.2 下载

使用 ST-Link 或 J-Link 连接 SWD（PA13/PA14），点击 Download 下载。

---

## 12. 注意事项与常见问题

### 12.1 软件 I2C 速度

CubeMX 生成的 GPIO 默认速度为 `LOW`，程序在 `BSP_GPIO_HighSpeed_Init()` 中重新配置为 `HIGH`，以保证软件 I2C 和 1-Wire 时序正确。

### 12.2 上拉电阻

- OLED I2C（PB6/PB7）：必须外接 4.7kΩ 上拉电阻
- ADXL345 软件 I2C（PA7/PB0）：必须外接上拉电阻
- MAX30102 软件 I2C（PB10/PB11）：必须外接上拉电阻
- DS18B20（PA11）：必须外接 4.7kΩ 上拉电阻

### 12.3 DS1302 备用电池

DS1302 支持外接纽扣电池，掉电后继续走时。如果没有接电池，系统掉电后时间会丢失，重新上电会使用 `SysDate` 初始值。

### 12.4 心率血氧测量

MAX30102 对环境光敏感，测量时手指应紧贴传感器，避免强光直射。心率血氧算法需要约 3 秒初始化采集，之后每 1 秒更新一次显示。

### 12.5 步数清零

步数掉电保存是设计意图。如需每次上电清零，可修改 `CHECK_NEW_MCU()`：

```c
void CHECK_NEW_MCU(void)
{
    bushu = 0;
    STMFLASH_Write(FLASH_SAVE_ADDR + 0x10, (u16*)"FDYDZ", 5);
    STMFLASH_Write(FLASH_SAVE_ADDR + 0x20, &bushu, 1);
    DelayMs(100);
}
```

### 12.6 串口无输出

检查：
- WiFi/蓝牙模块是否接对 TX/RX（STM32 TX 接模块 RX，STM32 RX 接模块 TX）
- 模块波特率是否为 9600
- Keil 中是否勾选 Use MicroLIB
- 模块供电是否正常

---

## 附录：文件索引

| 功能 | 文件 |
|------|------|
| 主程序入口 | `Core/Src/main.c` |
| 系统时钟配置 | `Core/Src/main.c` 中的 `SystemClock_Config()` |
| GPIO 初始化 | `Core/Src/gpio.c` |
| I2C1 初始化 | `Core/Src/i2c.c` |
| USART1 初始化 | `Core/Src/usart.c` |
| 中断处理 | `Core/Src/stm32f1xx_it.c` |
| 延时函数 | `bsp/bsp_delay.c` |
| OLED 驱动 | `bsp/bsp_oled.c` |
| ADXL345 软件 I2C | `bsp/bsp_iic_adxl.c` |
| MAX30102 软件 I2C | `bsp/bsp_iic_max.c` |
| ADXL345 驱动 | `bsp/bsp_adxl345.c` |
| MAX30102 驱动 | `bsp/bsp_max30102.c` |
| 心率血氧算法 | `bsp/bsp_algorithm.c` |
| DS18B20 驱动 | `bsp/bsp_ds18b20.c` |
| DS1302 驱动 | `bsp/bsp_ds1302.c` |
| 串口驱动 | `bsp/bsp_usart1.c` |
| 按键驱动 | `bsp/bsp_key.c` |
| Flash 存储 | `bsp/bsp_flash.c` |
| 引脚配置 | `bsp/bsp_config.h` |
| 字库 | `bsp/bsp_codetab.h` |

---

*文档生成日期：2026-07-02*
