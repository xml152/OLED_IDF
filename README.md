# ESP32-S3 OLED 屏幕显示

ESP32-S3 驱动 0.96 寸 OLED 显示屏（SSD1306，128x64 分辨率，I2C 接口）

## 开发环境
vscode或其他支持espidf插件的ide

## 功能特性

- **基础显示**：ASCII 字符、字符串显示
- **图形绘制**：点、线、矩形、填充矩形
- **中文显示**：支持 16x16 中文字库（UTF-8 编码）
- **图片显示**：位图、XBM 格式、全屏图片
- **图标系统**：电池、WiFi 等预定义图标
- **进度条**：支持动态进度显示

## 硬件连接

| OLED 引脚 | ESP32-S3 引脚 |
|-----------|---------------|
| VCC       | 3.3V          |
| GND       | GND           |
| SDA       | GPIO 1        |
| SCL       | GPIO 2        |

## 项目结构

```
├── CMakeLists.txt
├── components
│   └── OLED
│       ├── CMakeLists.txt
│       ├── OLED.c              # 基础显示驱动,英文和常用符号
│       ├── OLED_Chinese.c      # 中文字库支持
│       ├── OLED_Picture.c      # 图片/图标显示
│       └── include
│           ├── OLED.h
│           ├── OLED_Chinese.h
│           └── OLED_Picture.h
├── main
│   ├── CMakeLists.txt
│   └── main.c                  # 示例程序
├── sdkconfig
└── README.md
```

## 快速开始

### 1. 编译项目

```bash
source ~/esp/v5.5.3/esp-idf/export.sh
idf.py build
```

### 2. 烧录程序

```bash
idf.py flash
```

### 3. 查看输出

```bash
idf.py monitor
```

## API 使用说明

### 基础显示

```c
#include "OLED.h"

// 初始化 OLED
OLED_Init();

// 清屏
OLED_Clear();
OLED_Refresh();

// 显示字符串
OLED_DrawString(0, 0, "Hello World", true);
OLED_Refresh();
```

### 中文显示

```c
#include "OLED_Chinese.h"

// 显示中文字符串（支持中英文混合）
OLED_DrawChineseString(0, 0, "你好 World", true);
OLED_Refresh();
```

### 图标和进度条

```c
#include "OLED_Picture.h"

// 显示图标
OLED_DrawIcon(0, 0, ICON_WIFI, true);
OLED_DrawIcon(20, 0, ICON_BATTERY, true);

// 显示进度条（0-100）
OLED_DrawProgressBar(0, 20, 100, 10, 50, true);
OLED_Refresh();
```

## 添加自定义汉字说明
如图：取模说明.png

 * 取模工具：网页工具https://www.23bei.com/tool/218.html
 * 取模设置：
 *   - 输出格式：C51格式
 *   - 数据排列：从左到右，从上到下
 *   - 取模方式：横向8点左高位
 *   - 黑白取反：正常
 *   - 字体种类：任意16*16（此处使用宋体）
 *   - 强制全角：ASCII自动转全角

### 2. 添加字模数据

在 `components/OLED/OLED_Chinese.c` 中添加：

```c
static const ChineseChar chinese_font[] = {
    // 已有汉字...
    
    // 添加新汉字
    {
        "新",
        {
            0xXX, 0xXX, ... // 32 字节字模数据
        }
    },
};
```

### 3. 使用新汉字

```c
OLED_DrawChineseString(0, 0, "新汉字", true);
```

## 添加自定义图标

在 `components/OLED/OLED_Picture.c` 中添加：

```c
// 16x16 图标数据（32 字节）
const uint8_t icon_myicon_16x16[] = {
    0xXX, 0xXX, ... // 32 字节
};

// 在 OLED_DrawIcon() 函数中添加 case
void OLED_DrawIcon(uint8_t x, uint8_t y, IconType icon, bool color) {
    const uint8_t* icon_data = NULL;
    
    switch (icon) {
        // 已有图标...
        case ICON_MYICON:
            icon_data = icon_myicon_16x16;
            break;
    }
    // ...
}
```

## 显示图片

### 方法 1：位图方式

```c
// 图片数据（使用取模工具生成）
const uint8_t my_image[] = { ... };

// 显示位图
OLED_DrawBitmap(0, 0, 64, 32, my_image, true);
OLED_Refresh();
```

### 方法 2：全屏图片

```c
// 128x64 全屏图片数据
const uint8_t fullscreen_image[1024] = { ... };

// 直接填充到帧缓冲区
OLED_DrawFullScreenBitmap(fullscreen_image);
OLED_Refresh();
```

## 依赖

- ESP-IDF v5.5.3
- 目标芯片：ESP32-S3

## 注意事项

1. **I2C 地址**：默认 0x3C，如无法显示请尝试 0x3D
2. **字库大小**：中文字库占用 Flash 空间，建议只添加需要的汉字
3. **刷新频率**：频繁刷新可能影响性能，建议批量绘制后统一刷新

## 许可证

MIT License
