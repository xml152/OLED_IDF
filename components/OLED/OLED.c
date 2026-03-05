#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "OLED.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "OLED"

// SSD1306 Commands
#define SSD1306_CMD_DISPLAY_OFF         0xAE
#define SSD1306_CMD_DISPLAY_ON          0xAF
#define SSD1306_CMD_SET_DISPLAY_CLOCK   0xD5
#define SSD1306_CMD_SET_MULTIPLEX       0xA8
#define SSD1306_CMD_SET_DISPLAY_OFFSET  0xD3
#define SSD1306_CMD_SET_START_LINE      0x40
#define SSD1306_CMD_CHARGE_PUMP         0x8D
#define SSD1306_CMD_MEMORY_ADDR_MODE    0x20
#define SSD1306_CMD_SET_SEGMENT_REMAP   0xA1
#define SSD1306_CMD_COM_SCAN_DIR_DEC    0xC8
#define SSD1306_CMD_SET_COM_PINS        0xDA
#define SSD1306_CMD_SET_CONTRAST        0x81
#define SSD1306_CMD_SET_PRECHARGE       0xD9
#define SSD1306_CMD_SET_VCOM_DETECT     0xDB
#define SSD1306_CMD_DISPLAY_ALL_ON_RESUME 0xA4
#define SSD1306_CMD_NORMAL_DISPLAY      0xA6
#define SSD1306_CMD_COLUMN_ADDR         0x21
#define SSD1306_CMD_PAGE_ADDR           0x22

// Framebuffer
static uint8_t framebuffer[OLED_HEIGHT / 8][OLED_WIDTH];

// 5x8 Font (ASCII 32-126)
static const uint8_t font5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // "\"
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x08, 0x08, 0x2A, 0x1C, 0x08}, // ->
    {0x08, 0x1C, 0x2A, 0x08, 0x08}, // <-
};

static esp_err_t OLED_WriteCommand(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};  // 控制字节 + 命令
    return i2c_master_write_to_device(OLED_I2C_PORT, OLED_I2C_ADDR, data, 2, pdMS_TO_TICKS(100));
}

static esp_err_t OLED_WriteData(uint8_t *data, size_t len) {
    uint8_t *buffer = malloc(len + 1);
    if (buffer == NULL) return ESP_ERR_NO_MEM;
    
    buffer[0] = 0x40;  // 数据控制字节
    memcpy(buffer + 1, data, len);
    
    esp_err_t ret = i2c_master_write_to_device(OLED_I2C_PORT, OLED_I2C_ADDR, buffer, len + 1, pdMS_TO_TICKS(100));
    free(buffer);
    return ret;
}

bool OLED_Init(void) { // 初始化OLED
    esp_err_t ret;
    
    // Configure I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = OLED_I2C_SDA_PIN,
        .scl_io_num = OLED_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = OLED_I2C_FREQ_HZ,
    };
    
    ret = i2c_param_config(OLED_I2C_PORT, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed");
        return false;
    }
    
    ret = i2c_driver_install(OLED_I2C_PORT, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed");
        return false;
    }
    
    // Wait for OLED to power up
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Initialize SSD1306
    OLED_WriteCommand(SSD1306_CMD_DISPLAY_OFF);
    OLED_WriteCommand(SSD1306_CMD_SET_DISPLAY_CLOCK);
    OLED_WriteCommand(0x80);  // 设置显示时钟分频器/振荡器频率
    OLED_WriteCommand(SSD1306_CMD_SET_MULTIPLEX);
    OLED_WriteCommand(0x3F);  // 1/64 duty
    OLED_WriteCommand(SSD1306_CMD_SET_DISPLAY_OFFSET);
    OLED_WriteCommand(0x00);  // No offset
    OLED_WriteCommand(SSD1306_CMD_SET_START_LINE | 0x00);
    OLED_WriteCommand(SSD1306_CMD_CHARGE_PUMP);
    OLED_WriteCommand(0x14);  // Enable charge pump
    OLED_WriteCommand(SSD1306_CMD_MEMORY_ADDR_MODE);
    OLED_WriteCommand(0x00);  // Horizontal addressing mode
    OLED_WriteCommand(SSD1306_CMD_SET_SEGMENT_REMAP);
    OLED_WriteCommand(SSD1306_CMD_COM_SCAN_DIR_DEC);
    OLED_WriteCommand(SSD1306_CMD_SET_COM_PINS);
    OLED_WriteCommand(0x12);  // Alternative COM pin config
    OLED_WriteCommand(SSD1306_CMD_SET_CONTRAST);
    OLED_WriteCommand(0xCF);  // Contrast
    OLED_WriteCommand(SSD1306_CMD_SET_PRECHARGE);
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(SSD1306_CMD_SET_VCOM_DETECT);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(SSD1306_CMD_DISPLAY_ALL_ON_RESUME);
    OLED_WriteCommand(SSD1306_CMD_NORMAL_DISPLAY);
    OLED_WriteCommand(SSD1306_CMD_DISPLAY_ON);
    
    OLED_Clear();
    OLED_Refresh();
    
    ESP_LOGI(TAG, "OLED initialized successfully");
    return true;
}

void OLED_Clear(void) { // 清屏
    memset(framebuffer, 0x00, sizeof(framebuffer));
}

void OLED_DisplayOn(void) { // 显示
    OLED_WriteCommand(SSD1306_CMD_DISPLAY_ON);
}

void OLED_DisplayOff(void) { // 关闭显示
    OLED_WriteCommand(SSD1306_CMD_DISPLAY_OFF);
}

void OLED_SetContrast(uint8_t contrast) { // 设置对比度
    OLED_WriteCommand(SSD1306_CMD_SET_CONTRAST);
    OLED_WriteCommand(contrast);
}

void OLED_DrawPixel(uint8_t x, uint8_t y, bool color) { // 绘制像素
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    if (color) {
        framebuffer[y / 8][x] |= (1 << (y % 8));
    } else {
        framebuffer[y / 8][x] &= ~(1 << (y % 8));
    }
}

// 绘制线
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool color) {
    int dx = abs((int)x1 - (int)x0);
    int dy = abs((int)y1 - (int)y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        OLED_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// 绘制矩形
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color) {
    OLED_DrawLine(x, y, x + w - 1, y, color);
    OLED_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    OLED_DrawLine(x + w - 1, y + h - 1, x, y + h - 1, color);
    OLED_DrawLine(x, y + h - 1, x, y, color);
}

// 填充矩形
void OLED_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color) {
    for (uint8_t i = x; i < x + w && i < OLED_WIDTH; i++) {
        for (uint8_t j = y; j < y + h && j < OLED_HEIGHT; j++) {
            OLED_DrawPixel(i, j, color);
        }
    }
}

// 绘制字符
void OLED_DrawChar(uint8_t x, uint8_t y, char c, bool color) {
    if (c < 32 || c > 126) c = '?';
    
    const uint8_t *char_data = font5x8[c - 32];
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = char_data[col];
        for (uint8_t row = 0; row < 8; row++) {
            bool pixel = (line >> row) & 0x01;
            OLED_DrawPixel(x + col, y + row, color ? pixel : !pixel);
        }
    }
}

// 绘制字符串
void OLED_DrawString(uint8_t x, uint8_t y, const char *str, bool color) {
    while (*str) {
        OLED_DrawChar(x, y, *str++, color);
        x += 6;  // 5 pixels + 1 spacing
        if (x + 5 >= OLED_WIDTH) break;
    }
}

void OLED_Refresh(void) { // 刷新显示
    // 设置列地址
    OLED_WriteCommand(SSD1306_CMD_COLUMN_ADDR);
    OLED_WriteCommand(0);
    OLED_WriteCommand(OLED_WIDTH - 1);
    
    // 设置页地址
    OLED_WriteCommand(SSD1306_CMD_PAGE_ADDR);
    OLED_WriteCommand(0);
    OLED_WriteCommand((OLED_HEIGHT / 8) - 1);
    
    // 发送帧缓存
    for (uint8_t page = 0; page < OLED_HEIGHT / 8; page++) {
        OLED_WriteData(framebuffer[page], OLED_WIDTH);
    }
}
