#ifndef OLED_CHINESE_H
#define OLED_CHINESE_H

#include <stdint.h>
#include <stdbool.h>

// 绘制 16x16 汉字
// str: UTF-8 编码的汉字字符串（3字节）
void OLED_DrawChinese16x16(uint8_t x, uint8_t y, const char* str, bool color);

// 绘制混合字符串（中文 + ASCII）
// 支持 UTF-8 编码的中文字符串
void OLED_DrawChineseString(uint8_t x, uint8_t y, const char* str, bool color);

#endif // OLED_CHINESE_H
