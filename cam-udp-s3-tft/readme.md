# esp32-esp32 无线视频监控

采用esp32cam拍照发送给esp32s3通过ST7796显示。

目前480*320 目前 比较卡最多5帧。

基于platformio，建议使用platformio编译，配置由platformio.ini 无需额外配置tft库文件。

```ini
    -D TFT_MOSI=11
    -D TFT_SCLK=12
    -D TFT_CS=10
    -D TFT_DC=13
    -D TFT_RST=2
#触控的片选，这里暂不使用
```

接线参考platformio.ini  中的宏定义。背光和VCC用3v3引脚。