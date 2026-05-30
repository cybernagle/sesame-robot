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

### 舵机信号线（已测试确认）
```
Motor 0 (代码R1 右前髋) → GPIO 18
Motor 1 (代码R2 右前膝) → GPIO 33
Motor 2 (代码L1 左前髋) → GPIO 9
Motor 3 (代码L2 左前膝) → GPIO 7
Motor 4 (代码R4 右后髋) → GPIO 5
Motor 5 (代码R3 右后膝) → GPIO 3
Motor 6 (代码L3 左后膝) → GPIO 35
Motor 7 (代码L4 左后髋) → GPIO 37
```

### 舵机物理位置映射（实测）
```
Motor 1 (API) = L1 (左前髋)
Motor 2 (API) = L4 (左后髋)
Motor 3 (API) = R1 (右前髋)
Motor 4 (API) = R4 (右后髋)
Motor 5 (API) = L3 (左后膝)
Motor 6 (API) = L2 (左前膝)
Motor 7 (API) = R2 (右前膝)
Motor 8 (API) = R3 (右后膝)
```

### 电源
```
电池(11.5V) → 开关 → 稳压器 IN
稳压器 OUT(5V) → 洞洞板汇流排

洞洞板接线注意: 红线实际是 GND，黑线实际是 VCC（焊接时颜色搞反了）
```

## 已知问题

### 1. PSRAM 引脚可用（已验证）
GPIO 33/35/37 虽然被 PSRAM 使用，但在当前固件版本中可以正常控制舵机。8 个舵机全部工作正常。

### 2. 洞洞板 VCC/GND 线颜色反了
- 红线 = 实际 GND
- 黑线 = 实际 VCC
接线时需要反过来接，容易搞混。建议后续重新焊正确。

### 3. USB-C 数据线
iPhone 自带的 USB-C 线无法传输数据，需要使用支持数据传输的 USB-C 线。

### 4. 舵机校准方式
- R1, R2, L1：代码中自动反转（180 - angle）
- L2, R4, R3, L3, L4：每个动作需要单独校准角度
- 部分舵机可能需要重新定位舵盘（如 L3）

### 5. L3 左后膝舵盘需要重新定位
当前 L3 在 0-180° 范围内无法实现向上挥手。需要拆下舵盘，将 90° 对准水平方向，这样 0°=站立，180°=举高。

## 固件修改

### 文件: firmware/sesame-firmware-main.ino

**I2C 引脚** (第 39-40 行):
```cpp
#define I2C_SDA 16  // 原始: 33
#define I2C_SCL 11  // 原始: 35
```

**舵机引脚** (第 93-94 行):
```cpp
const int servoPins[8] = {9, 35, 18, 3, 7, 37, 5, 33};
// 映射: R1→9, R2→35, L1→18, L2→3, R4→7, R3→37, L3→5, L4→33
```

**舵机反转** (setServoAngle 函数):
- R1, R2, L1 自动反转: `angle = 180 - angle`
- 其他舵机需要逐个姿势校准

### Stand 姿态校准值
```
舵机    原版    校准后
R1 右前髋  135  →  45   (自动反转)
R2 右前膝   45  → 135  (自动反转)
L1 左前髋   45  → 135  (自动反转)
L2 左前膝  135  →   0   (手动校准)
R4 右后髋    0  → 100  (手动校准)
R3 右后膝  180  → 130  (手动校准)
L3 左后膝    0  → 140  (手动校准)
L4 左后髋  180  →  60  (手动校准)
```

### WiFi 配置
```
AP 模式: Sesame-Controller / 12345678 (192.168.4.1)
家庭 WiFi: CU_Z7xh / 8pj6dxwq (192.168.1.11)
mDNS: http://sesame-robot.local
```

### 校准进度
- [x] Stand 姿态校准完成
- [x] Wave 姿态测试（L3 方向需要调整，通过重新定位舵盘解决）
- [ ] 其他动作校准（dance, walk, swim 等）
- [ ] L3 舵盘重新定位（90°=水平，0°=站立，180°=举高）

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
