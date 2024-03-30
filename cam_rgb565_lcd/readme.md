# CAM_RGB565_LCD

ESP32-S3 WROOM N16R8 CAM连接ST7789屏幕显示画面。

build时会动态下载库。



无需按照arduino开发的方式修改库文件的配置。

本例程已经将配置集成在platformio.ini中。

可以在[tft_config] 中修改对应的配置。默认不用修改。

引脚配置

| TFT配置 | 屏幕引脚 | ESP32-S3 WROOM N16R8 CAM |
| ------- | -------- | ------------------------ |
| MOSI    | SDA      | 20                       |
| SCLK    | SCL      | 19                       |
| CS      | CS       | 45                       |
| DC      | 2        | 21                       |
| RST     | RES      | EN                       |
| -       | GND      | GND                      |
| -       | VCC      | 3.3V                     |
| -       | BLK      | 默认开启背光可以不连     |

屏幕BLK未连接默认开启背光，不连接可以亮屏，可以根据需要通过io进行控制

esp32-s3-devkitc-1 Freenove ESP32-S3-WROOM Board 在国内叫做ESP32-S3 WROOM N16R8 CAM 摄像头引脚同于esp32 s3 eyes



