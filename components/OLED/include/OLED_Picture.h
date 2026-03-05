#ifndef OLED_PICTURE_H
#define OLED_PICTURE_H

#include <stdint.h>
#include <stdbool.h>

// 图标类型
typedef enum {
    ICON_BATTERY,
    ICON_WIFI,
    ICON_BLUETOOTH,
    ICON_TEMPERATURE,
    ICON_HUMIDITY,
    // 添加更多图标...
} IconType;

// 绘制位图
// x, y: 起始坐标
// w, h: 图片宽高
// bitmap: 位图数据（每行 ceil(w/8) 字节，MSB 在前）
void OLED_DrawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* bitmap, bool color);

// 绘制 XBM 格式图片（XBM 是 LSB 在前）
void OLED_DrawXBM(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* xbm_data, bool color);

// 直接填充整个屏幕的位图（128x64）
// 数据格式：8 个 page，每个 page 128 字节
void OLED_DrawFullScreenBitmap(const uint8_t* bitmap);

// 绘制预定义图标
void OLED_DrawIcon(uint8_t x, uint8_t y, IconType icon, bool color);

// 绘制进度条
// progress: 0-100
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t progress, bool color);

// 外部图片数据声明（用户自行添加）
// extern const uint8_t my_image_128x64[];

#endif // OLED_PICTURE_H