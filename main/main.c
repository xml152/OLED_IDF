#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "OLED.h"
#include "OLED_Chinese.h"
#include "OLED_Picture.h"

void app_main(void) {
    // Initialize OLED
    if (!OLED_Init()) {
        printf("OLED initialization failed!\n");
        return;
    }

    for(;;){
        // 显示图标和进度条
        OLED_Clear();
        OLED_DrawString(0, 0, "Loading", true);
        OLED_DrawString(0, 16, "ESP32 is Ready", true);
        
        // 绘制进度条
        OLED_DrawProgressBar(0, 32, 100, 10, 0, true);
        OLED_DrawString(105, 32, "0%", true);
        OLED_Refresh();
        
        // 进度条动画
        for (int i = 0; i <= 100; i += 10) {
            vTaskDelay(200 / portTICK_PERIOD_MS);
            OLED_DrawProgressBar(0, 32, 100, 10, i, true);
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", i);
            OLED_FillRect(105, 32, 20, 8, false);
            OLED_DrawString(105, 32, buf, true);
            OLED_Refresh();
        }

        OLED_Clear();
        //中文测试
        OLED_DrawString(0, 0, "Chinese Test:", true);
        OLED_DrawChineseString(0, 16, "中文测试", true);
        OLED_DrawChineseString(0, 32, "你好世界", true);
        OLED_Refresh();

        vTaskDelay(2000 / portTICK_PERIOD_MS);

    }
}
