#include <stdio.h>
#include <string.h>
#include "OLED_Picture.h"
#include "OLED.h"

// 示例图片数据 - 128x64 的 ESP32 Logo 简化版
// 实际使用时，使用图片取模工具生成
const uint8_t esp32_logo_128x64[] = {
    // 这里是一个示例，实际使用时需要用取模工具生成
    // 可以使用 PCtoLCD2002、Image2Lcd 等工具
    // 格式：横向取模，字节倒序
};

// 绘制位图
// x, y: 起始坐标
// w, h: 图片宽高
// bitmap: 位图数据（每行 ceil(w/8) 字节，从上到下）
void OLED_DrawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap, bool color) {
    uint8_t bytes_per_row = (w + 7) / 8;  // 每行字节数
    
    for (uint8_t row = 0; row < h && (y + row) < OLED_HEIGHT; row++) {
        for (uint8_t col = 0; col < w && (x + col) < OLED_WIDTH; col++) {
            uint8_t byte_idx = row * bytes_per_row + col / 8;
            uint8_t bit_idx = 7 - (col % 8);  // MSB first
            bool pixel = (bitmap[byte_idx] >> bit_idx) & 0x01;
            OLED_DrawPixel(x + col, y + row, color ? pixel : !pixel);
        }
    }
}

// 绘制 XBM 格式图片
// XBM 格式：每个像素占 1 bit，MSB 在前
void OLED_DrawXBM(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* xbm_data, bool color) {
    uint8_t bytes_per_row = (w + 7) / 8;
    
    for (uint8_t row = 0; row < h && (y + row) < OLED_HEIGHT; row++) {
        for (uint8_t col = 0; col < w && (x + col) < OLED_WIDTH; col++) {
            uint8_t byte_idx = row * bytes_per_row + col / 8;
            uint8_t bit_idx = col % 8;  // XBM 是 LSB first
            bool pixel = (xbm_data[byte_idx] >> bit_idx) & 0x01;
            OLED_DrawPixel(x + col, y + row, color ? pixel : !pixel);
        }
    }
}

// 直接填充整个屏幕的位图（128x64）
// 数据格式：8 个 page，每个 page 128 字节
void OLED_DrawFullScreenBitmap(const uint8_t* bitmap) {
    extern uint8_t framebuffer[OLED_HEIGHT / 8][OLED_WIDTH];
    memcpy(framebuffer, bitmap, (OLED_HEIGHT / 8) * OLED_WIDTH);
}

/*
 * ============================================
 * 图标数据 - 16x16 像素
 * ============================================
 * 
 * 取模工具：PCtoLCD2002 / Image2Lcd / 在线取模工具
 * 取模设置：
 *   - 点阵格式：阴码（1表示点亮）
 *   - 取模方式：横向取模
 *   - 取模走向：高位在前（MSB First）
 *   - 输出格式：C51格式
 * 
 * 每个图标占 32 字节（16x16 = 256 点，每行 2 字节，共 16 行）
 * 
 * 替换步骤：
 * 1. 使用取模工具生成 16x16 图标数据
 * 2. 替换下方的数组内容
 */

// 电池图标
const uint8_t icon_battery_16x16[] = {
    
};

// WiFi 图标 
const uint8_t icon_wifi_16x16[] = {
    
};


// 绘制图标
void OLED_DrawIcon(uint8_t x, uint8_t y, IconType icon, bool color) {
    const uint8_t* icon_data = NULL;
    
    switch (icon) {
        case ICON_BATTERY:
            icon_data = icon_battery_16x16;
            break;
        case ICON_WIFI:
            icon_data = icon_wifi_16x16;
            break;
        default:
            return;
    }
    
    OLED_DrawBitmap(x, y, 16, 16, icon_data, color);
}

// 绘制进度条
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t progress, bool color) {
    // 绘制边框
    OLED_DrawRect(x, y, w, h, color);
    
    // 计算填充宽度
    uint8_t fill_width = (uint16_t)(w - 2) * progress / 100;
    
    // 填充
    if (fill_width > 0) {
        OLED_FillRect(x + 1, y + 1, fill_width, h - 2, color);
    }
    
    // 清空剩余部分
    if (fill_width < w - 2) {
        OLED_FillRect(x + 1 + fill_width, y + 1, w - 2 - fill_width, h - 2, !color);
    }
}
