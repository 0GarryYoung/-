# health_watch2 HAL 版 — STM32CubeMX 配置说明

本 README 用于指导如何在 STM32CubeMX 中初始化 health_watch2 的 HAL 工程。原工程是基于 STM32F103C8T6 的标准库实现，本配置把外设迁移到 HAL 库，并用 Keil5 编译下载。

---

## 1. 芯片选择

1. 打开 STM32CubeMX。
2. `File -> New Project`，选择 `Access to MCU Selector`。
3. 在 **Commercial Part Number** 中输入：`STM32F103C8Tx`。
4. 选择 **STM32F103C8Tx**（LQFP48 封装），点击 `Start Project`。

---

## 2. SYS 配置

在 `Pinout & Configuration` 左侧选择 **SYS**：

| 参数 | 设置 | 说明 |
|------|------|------|
| Debug | `Serial Wire` | 启用 SWD：PA13/SWDIO、PA14/SWCLK |
| Timebase Source | `SysTick` | 保持默认 |

---

## 3. RCC / 时钟配置

### 3.1 RCC 设置
在 `Pinout & Configuration` 左侧选择 **RCC**：

| 参数 | 设置 |
|------|------|
| High Speed Clock (HSE) | `Crystal/Ceramic Resonator` |
| Low Speed Clock (LSE) | `Disable`（未使用外部 32.768kHz 晶振） |

### 3.2 Clock Configuration
切换到 **Clock Configuration** 标签页：

| 参数 | 设置 |
|------|------|
| Input frequency | `8.0000 MHz` |
| PLL Source Mux | `HSE` |
| System Clock Mux | `PLLCLK` |
| PLL Mul | `x9` |
| AHB Prescaler | `/1` |
| APB1 Prescaler | `/2` |
| APB2 Prescaler | `/1` |

确认最终时钟：
- HCLK = 72 MHz
- PCLK1 = 36 MHz
- PCLK2 = 72 MHz

如有警告，点击 `Resolve Clock Issues`。

---

## 4. I2C1 配置（OLED SSD1306）

OLED 使用硬件 I2C1：

1. 在引脚视图中：
   - 点击 **PB6**，选择 `I2C1_SCL`
   - 点击 **PB7**，选择 `I2C1_SDA`
2. 在左侧 `Connectivity` 中选择 **I2C1**：
   - I2C Speed Mode：`Fast Mode`
   - I2C Clock Frequency (Hz)：`400000`（400 kHz）
   - 其余保持默认
3. 不使用 DMA

---

## 5. USART1 配置（HC-05 蓝牙模块）

蓝牙串口使用 USART1，波特率 9600，开启 RX 中断：

1. 在引脚视图中：
   - 点击 **PA9**，选择 `USART1_TX`
   - 点击 **PA10**，选择 `USART1_RX`
2. 在左侧 `Connectivity` 中选择 **USART1**：
   - Mode：`Asynchronous`
   - Baud Rate：`9600 Bits/s`
   - Word Length：`8 Bits (including Parity)`
   - Stop Bits：`1`
   - Parity：`None`
   - Mode：`TX and RX`
3. NVIC 中勾选 **USART1 global interrupt**

---

## 6. GPIO 配置

以下引脚需要手动配置为 GPIO。在引脚视图中逐个点击并设置模式，然后在 `GPIO` 配置页面统一调整参数。

### 6.1 ADXL345 软件 I2C（PA7 / PB0）

| 引脚 | 模式 | GPIO output level | GPIO mode | Pull-up/Pull-down | Max output speed | User Label |
|------|------|-------------------|-----------|-------------------|------------------|------------|
| PA7 | GPIO_Output | `High` | `Output Open Drain` | `No pull-up and no pull-down` | `High` | `ADXL_SCL` |
| PB0 | GPIO_Output | `High` | `Output Open Drain` | `No pull-up and no pull-down` | `High` | `ADXL_SDA` |

### 6.2 MAX30102 软件 I2C（PB10 / PB11）

| 引脚 | 模式 | GPIO output level | GPIO mode | Pull-up/Pull-down | Max output speed | User Label |
|------|------|-------------------|-----------|-------------------|------------------|------------|
| PB10 | GPIO_Output | `High` | `Output Open Drain` | `No pull-up and no pull-down` | `High` | `MAX_SDA` |
| PB11 | GPIO_Output | `High` | `Output Open Drain` | `No pull-up and no pull-down` | `High` | `MAX_SCL` |

### 6.3 DS18B20 单总线（PA11）

| 引脚 | 模式 | GPIO output level | GPIO mode | Pull-up/Pull-down | Max output speed | User Label |
|------|------|-------------------|-----------|-------------------|------------------|------------|
| PA11 | GPIO_Output | `High` | `Output Open Drain` | `Pull-up` | `High` | `DS18B20_DQ` |

> 建议外部再并一个 4.7kΩ 上拉电阻。

### 6.4 DS1302 三线 RTC（PC13 / PC14 / PC15）

| 引脚 | 模式 | GPIO output level | GPIO mode | Pull-up/Pull-down | Max output speed | User Label |
|------|------|-------------------|-----------|-------------------|------------------|------------|
| PC13 | GPIO_Output | `Low` | `Output Push Pull` | `No pull-up and no pull-down` | `High` | `DS1302_SCLK` |
| PC14 | GPIO_Output | `Low` | `Output Push Pull` | `No pull-up and no pull-down` | `High` | `DS1302_DAT` |
| PC15 | GPIO_Output | `Low` | `Output Push Pull` | `No pull-up and no pull-down` | `High` | `DS1302_RST` |

### 6.5 按键输入（PB12 ~ PB15）

| 引脚 | 模式 | GPIO mode | Pull-up/Pull-down | User Label |
|------|------|-----------|-------------------|------------|
| PB12 | GPIO_Input | `Input mode` | `Pull-up` | `KEY0` |
| PB13 | GPIO_Input | `Input mode` | `Pull-up` | `KEY1` |
| PB14 | GPIO_Input | `Input mode` | `Pull-up` | `KEY2` |
| PB15 | GPIO_Input | `Input mode` | `Pull-up` | `KEY3` |

---

## 7. NVIC 配置

在 `System Core -> NVIC` 中：

| 中断 | 状态 | Preemption Priority | Sub Priority |
|------|------|---------------------|--------------|
| USART1 global interrupt | Enabled | `0` | `0` |
| SysTick | Enabled | `1` | `0` |

确保 USART1 优先级高于 SysTick。

---

## 8. Project Manager / 生成代码

### 8.1 Project
切换到 **Project Manager -> Project**：

| 参数 | 建议值 |
|------|--------|
| Project Name | `HealthWatch_HAL` |
| Project Location | 你的工作目录，例如 `D:\\_res\\dlut\\生产实习\\` |
| Application Structure | `Basic` |
| Toolchain / IDE | `MDK-ARM V5` |

### 8.2 Code Generator
切换到 **Project Manager -> Code Generator**：

| 参数 | 设置 |
|------|------|
| Copy only the necessary library files | ✅ 勾选 |
| Generate peripheral initialization as a pair of .c/.h files per peripheral | ✅ 勾选 |
| Keep User Code when re-generating | ✅ 勾选 |
| Set all free pins as analog to optimize power consumption | ❌ 取消勾选 |

点击 **GENERATE CODE**。

---

## 9. Keil5 工程补充设置

1. 用 Keil uVision5 打开生成的 `HealthWatch_HAL.uvprojx`。
2. 进入 `Project -> Options for Target -> C/C++`：
   - **Define** 中确认有：`USE_HAL_DRIVER`、`STM32F103xB`
   - 删除旧定义：`STM32F10X_MD`、`USE_STDPERIPH_DRIVER`
3. 进入 `Target` 标签页：
   - 勾选 **Use MicroLIB**（printf 重定向到串口需要）
4. 进入 `Flash -> Configure Flash Tools -> Debug -> Settings -> Flash Download`：
   - 勾选 **Reset and Run**
   - Flash 算法选择 `STM32F10x Med-density Flash`

---

## 10. 引脚冲突说明

原工程中：
- `LED` 占用 PC13，但 PC13 同时被 DS1302 的 SCLK 使用
- `BEEP` 占用 PB11，但 PB11 同时被 MAX30102 的 SCL 使用

由于 `LED` 和 `BEEP` 的初始化函数在原 `main()` 中从未被调用，本次 HAL 移植 **不配置 LED/BEEP**，保持功能与原工程一致。如果后续需要这两个功能，需要重新选择不冲突的引脚并修改硬件连接。

---

## 11. 配置完成后的引脚一览

| 功能 | 信号 | 引脚 | CubeMX 模式 |
|------|------|------|-------------|
| OLED SCL | I2C1_SCL | PB6 | I2C1_SCL（AF_OD） |
| OLED SDA | I2C1_SDA | PB7 | I2C1_SDA（AF_OD） |
| ADXL345 SCL | GPIO_Output | PA7 | Output Open Drain |
| ADXL345 SDA | GPIO_Output | PB0 | Output Open Drain |
| MAX30102 SDA | GPIO_Output | PB10 | Output Open Drain |
| MAX30102 SCL | GPIO_Output | PB11 | Output Open Drain |
| DS18B20 DQ | GPIO_Output | PA11 | Output Open Drain |
| DS1302 SCLK | GPIO_Output | PC13 | Output Push Pull |
| DS1302 DAT | GPIO_Output | PC14 | Output Push Pull |
| DS1302 RST | GPIO_Output | PC15 | Output Push Pull |
| 蓝牙 TX | USART1_TX | PA9 | USART1_TX（AF_PP） |
| 蓝牙 RX | USART1_RX | PA10 | USART1_RX（AF_PP） |
| KEY0 | GPIO_Input | PB12 | Input with pull-up |
| KEY1 | GPIO_Input | PB13 | Input with pull-up |
| KEY2 | GPIO_Input | PB14 | Input with pull-up |
| KEY3 | GPIO_Input | PB15 | Input with pull-up |
| SWDIO | - | PA13 | 由 SYS 自动生成 |
| SWCLK | - | PA14 | 由 SYS 自动生成 |

---

## 12. 下一步

CubeMX 生成代码后，在生成的 `Core/Src/main.c` 基础上，把原 `health_watch2/User` 中的驱动逐步移植为 HAL 版本。移植顺序建议：

1. `delay.c/h` → 新建 `delay_hal.c/h`（`DelayMs` 用 `HAL_Delay`，`DelayUs` 用 DWT/循环）
2. `usart1.c/h` → printf 重定向到 `HAL_UART_Transmit`，RX 中断处理
3. `key.c/h` → 使用 `HAL_GPIO_ReadPin`
4. `OLED_I2C.c/h` → 使用 `HAL_I2C_Mem_Write`
5. `iic.c/h` 和 `myiic.c/h` → 使用 `HAL_GPIO_WritePin`/`HAL_GPIO_ReadPin`
6. `ds18b20.c/h` 和 `ds1302.c/h` → 用 HAL GPIO 替换寄存器操作
7. `stmflash.c/h` → 使用 `HAL_FLASH` / `HAL_FLASHEx`
8. `main.c` → 整合初始化与业务逻辑

原 `algorithm.c/h`、`codetab.h` 无硬件依赖，可直接复制使用。
