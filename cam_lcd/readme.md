# CAM_LCD

esp32cam 或者esp32_wrover_cam 连接ST7789屏幕显示画面。

build时会动态下载库。



无需按照arduino开发的方式修改库文件的配置。

本例程已经将配置集成在platformio.ini中。

可以在[tft_config] 中修改对应的配置。默认不用修改。

引脚配置

| TFT配置 | 屏幕引脚 | esp32cam               | esp32_wrover_cam     |
| ------- | -------- | ---------------------- | -------------------- |
| MOSI    | SDA      | 13                     | 13                   |
| SCLK    | SCL      | 14                     | 14                   |
| CS      | CS       | 15                     | 15                   |
| DC      | 2        | 2                      | 2                    |
| RST     | RES      | 12                     | EN                   |
| -       | GND      | GND                    | GND                  |
| -       | VCC      | 3.3V                   | 3.3V                 |
| -       | BLK      | 默认开启背光可以不连。 | 默认开启背光可以不连 |

屏幕BLK未连接默认开启背光，不连接可以亮屏，可以根据需要通过io进行控制