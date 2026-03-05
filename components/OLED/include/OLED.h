#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include <stdbool.h>

// I2C Configuration
#define OLED_I2C_PORT       I2C_NUM_0
#define OLED_I2C_SDA_PIN    GPIO_NUM_1
#define OLED_I2C_SCL_PIN    GPIO_NUM_2
#define OLED_I2C_FREQ_HZ    400000  // 400kHz

// OLED I2C Address
#define OLED_I2C_ADDR       0x3C    // 0x3C or 0x3D

// OLED Display Dimensions
#define OLED_WIDTH          128
#define OLED_HEIGHT         64

// Function Prototypes
bool OLED_Init(void);
void OLED_Clear(void);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_SetContrast(uint8_t contrast);
void OLED_DrawPixel(uint8_t x, uint8_t y, bool color);
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool color);
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color);
void OLED_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color);
void OLED_DrawChar(uint8_t x, uint8_t y, char c, bool color);
void OLED_DrawString(uint8_t x, uint8_t y, const char *str, bool color);
void OLED_Refresh(void);

#endif // OLED_H
