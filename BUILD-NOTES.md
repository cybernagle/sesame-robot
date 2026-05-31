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
I2C 地址: 0x3C

### 舵机信号线
```
代码名   物理位置(背向视角)   GPIO
R1       左前髋               18
R2       左前膝               33
L1       右前髋                9
L2       右前膝                7
R4       左后髋                5
R3       左后膝                3
L3       右后膝               35
L4       右后髋               37
```

### 电源
```
电池(11.5V) → 开关 → 稳压器 IN
稳压器 OUT(5V) → 洞洞板汇流排
注意: 洞洞板红线=GND, 黑线=VCC（焊接时颜色反了）
```

## 对称推导规则

三对对称关系 → 只需 2 个自由变量 (Ah=髋, Ak=膝)
- 左右镜像: R + L = 180
- 前后镜像: 前右值 = 后左值, 前左值 = 后右值

推导表:
```
              右侧           左侧
前髋(Ah)      180-Ah          Ah
前膝(Ak)      180-Ak          Ak
后髋          Ah              180-Ah
后膝          Ak              180-Ak
```

## Stand 角度 (Ah=52, Ak=70)

舵盘已调整，代码角度 = 物理角度（无反转层）。

```
代码名   物理位置       Stand角度
R1       左前髋          52
R2       左前膝          70
L1       右前髋         128
L2       右前膝         110
R4       左后髋         128
R3       左后膝         110
L3       右后膝          70
L4       右后髋          52
```

### 膝关节物理范围（实测）
```
         stand   下限   上限   可用范围
R2(左前膝)   70     10    180   170°
L3(右后膝)   70     10    180   170°
L2(右前膝)  110      0    170   170°
R3(左后膝)  110      0    170   170°
```

### 髋关节物理范围（实测）
```
         stand   下限   上限   可用范围
R1(左前髋)   52      0    ~90    90°
L4(右后髋)   52      0    ~90    90°
L1(右前髋)  128    ~100    180   80°
R4(左后髋)  128    ~100    180   80°
```

## 语义层方向表

语义约定: +offset = 头侧(forward)/外展(extend), -offset = 尾侧(backward)/收起(retract)

```
              stand  direction  +offset含义        安全偏移范围
R1(左前髋)      52      -1      尾侧(backward)     -52 ~ +38
R4(左后髋)     128      -1      尾侧(backward)     -28 ~ +52
L1(右前髋)     128      +1      头侧(forward)      -28 ~ +52
L4(右后髋)      52      +1      头侧(forward)      -52 ~ +38

R2(左前膝)      70      +1      外展/抬起(extend)   -60 ~ +110
L2(右前膝)     110      -1      内收/收起(retract)  -110 ~ +60
R3(左后膝)     110      -1      内收/收起(retract)  -110 ~ +60
L3(右后膝)      70      +1      外展/抬起(extend)   -60 ~ +110
```

注意: 髋关节 +offset 在 direction=-1 时实际是尾侧，语义层内部处理方向翻转。

## WiFi 配置

```
AP 模式: Sesame-Controller / 12345678 (192.168.4.1)
家庭 WiFi: CU_Z7xh / 8pj6dxwq (192.168.1.11)
mDNS: http://sesame-robot.local
```

## 已知问题

- 洞洞板 VCC/GND 线颜色反了（红线=GND, 黑线=VCC）
- iPhone USB-C 线不能传数据，需要支持数据传输的 USB-C 线
- curl 需要 --noproxy '*' 绕过代理

## 校准进度

- [x] 确认物理映射（背向视角，逐个电机测试）
- [x] 去掉反转层（setServoAngle 不再做 180-angle）
- [x] 实测物理范围并推导最佳 stand 角度
- [x] 调整舵盘到目标 stand 角度
- [x] 膝关节物理范围实测
- [x] 髋关节物理范围实测
- [x] 语义层方向表确认
- [ ] 实现语义层固件
- [ ] 姿势校准（wave, walk, dance 等）

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
