# Sesame Robot 构建记录

## 硬件

- 主板: Lolin S2 Mini (ESP32-S2FN4R2, 4MB Flash + 2MB PSRAM)
- 电池: GNB 450 3S LiPo (11.5V)
- 舵机: 8x MG90S
- 显示屏: SSD1306 128x64 I2C OLED
- 稳压器: 降压模块 (输入 11.5V, 输出 5V)
- 开关: KCD1 摇臂开关 (3 pin, 用中间+边上任意一个)

## 接线

### I2C (OLED)
```
OLED VCC → ESP32 3V3
OLED GND → ESP32 GND
OLED SDA → ESP32 GPIO 16
OLED SCL → ESP32 GPIO 11
```
- OLED 板子无引脚标注，通过 I2C 扫描确认 SDA=GPIO 16, SCL=GPIO 11
- I2C 地址: 0x3C

### 舵机信号线
```
M0 (R1) → GPIO 18
M1 (R2) → GPIO 33 ⚠️ 被 PSRAM 占用，需要改焊
M2 (L1) → GPIO 9
M3 (L2) → GPIO 7
M4 (R4) → GPIO 5
M5 (R3) → GPIO 3
M6 (L3) → GPIO 35 ⚠️ 被 PSRAM 占用，需要改焊
M7 (L4) → GPIO 37 ⚠️ 被 PSRAM 占用，需要改焊
```

### 电源
```
电池(11.5V) → 开关 → 稳压器 IN
稳压器 OUT(5V) → 洞洞板汇流排

洞洞板接线注意: 红线实际是 GND，黑线实际是 VCC（焊接时颜色搞反了）
```

## 已知问题

### 1. PSRAM 占用 GPIO 33/35/37
Lolin S2 Mini 的 PSRAM 使用 GPIO 33-37 进行通信，这些引脚无法用于舵机 PWM 输出。当前固件已将 M1、M6、M7 设为 -1 禁用。

**解决方案**: 将 M1、M6、M7 的信号线改焊到空闲引脚:
```
M1: GPIO 33 → GPIO 1 (或其他空闲引脚)
M6: GPIO 35 → GPIO 2
M7: GPIO 37 → GPIO 4
```

可用空闲引脚: 1, 2, 4, 6, 8, 10, 12, 13, 14, 15, 17, 21

### 2. 洞洞板 VCC/GND 线颜色反了
- 红线 = 实际 GND
- 黑线 = 实际 VCC
接线时需要反过来接，容易搞混。建议后续重新焊正确。

### 3. USB-C 数据线
iPhone 自带的 USB-C 线无法传输数据，需要使用支持数据传输的 USB-C 线。

### 4. OLED 和舵机定时器冲突
当同时使用 ESP32Servo 和 Wire(I2C) 时，OLED 可能不亮。原因是 ESP32-S2 只有 4 个 LEDC 定时器，被舵机全部占用后影响 I2C。实际测试中有时能同时工作，有时不行。

## 固件修改

### 文件: firmware/sesame-firmware-main.ino

**I2C 引脚** (第 39-40 行):
```cpp
#define I2C_SDA 16  // 原始: 33
#define I2C_SCL 11  // 原始: 35
```

**舵机引脚** (第 93-94 行):
```cpp
const int servoPins[8] = {18, -1, 9, 7, 5, 3, -1, -1};
// 原始: {1, 2, 4, 6, 8, 10, 13, 14}
```

## 工具链

- arduino-cli 1.5.0 (通过 Homebrew 安装)
- ESP32 core 3.3.8
- ESP32Servo 3.0.9
- Adafruit SSD1306 2.5.16
- Adafruit GFX Library 1.12.6
- 编译命令: `arduino-cli compile --fqbn esp32:esp32:lolin_s2_mini <sketch>`
- 烧录命令: `arduino-cli upload -p /dev/cu.usbmodem01 --fqbn esp32:esp32:lolin_s2_mini <sketch>`

## Git

- Fork: https://github.com/cybernagle/sesame-robot.git
- Remote `myfork` 已添加
- 待 push: 固件配置修改 + S2 Mini 引脚图 (s2-mini-pinout.svg)
