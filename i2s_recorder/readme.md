# I2S 录音

这里使用Freenove ESP32-S3-WROOM Board 在国内叫做ESP32-S3 WROOM N16R8 CAM。

此代码基于 arduino-esp32 v2.0.14 基于 ESP-IDF v4.4.6。由于并未i2s相关并未在arduino api层体现，使用的依然是idf api。在后续idf 5.x版本会发生变化。

## SD卡 

Freenove ESP32-S3-WROOM Board带一个SD卡槽。

引脚配置如下:

```ini
-D SD_MMC_CMD=38
-D SD_MMC_CLK=39
-D SD_MMC_D0=40
```

这里我将SD格式化为 FAT32

## 麦克风

麦克风使用 INMP441

代码对应的接线，这里使用了idf例程默认引脚，与camera引脚会冲突。

| esp32引脚 | INMP441 | 描述                                                 |
| --------- | ------- | ---------------------------------------------------- |
| 4         | SCK     | BCK                                                  |
| 5         | WS      | WS                                                   |
| 18        | SD      | data in                                              |
| 3.3V      | VDD     |                                                      |
| GND       | GND     |                                                      |
| 47        | L/R     | 左右声道，拉低是左声道，代码使用左声道，可以直接接地 |

