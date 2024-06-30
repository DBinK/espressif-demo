# esp32s3使用键盘输入显示到lcd
简单实现了输入 退格 目前无长按连续重复和无enter键换行功能。
屏幕使用ST7789 2.0 240 * 320,如果符合，无需编辑tft配置。

引脚连接见platformio.ini
```ini
    -D TFT_MISO=-1
    -D TFT_MOSI=5
    -D TFT_SCLK=4
    -D TFT_CS=7
    -D TFT_DC=6
    -D TFT_RST=-1
```
 MOSI  对应 SDA
 SCLK 对应 SCL
 RST 接 EN