# esp32 pcm Wifi-Raw 发送端

这里使用 esp32外接 INMP441 通过80211发送到另一个esp32进行播放。
无需配网，使用wifi 在80211层直接发送， main.cpp中 `WLAN_IEEE_HEADER_AIR2GROUND` 接受端与本端一致即可接收。
这里不存在点对点，都是往空中发，接收端可以存在多个同时接收。


### 编译说明

* 根据需要修改`i2s_config` 下的 `flags` 这里默认接线如下

  | esp32s3引脚 | INMP441 | platformio中配置项key | 描述                                                         |
    | ----------- | ------- | --------------------- | ------------------------------------------------------------ |
  | 4           | SCK     | CONF_I2S_SCK          | BCK                                                          |
  | 5           | WS      | CONF_I2S_FS           | WS                                                           |
  | 6           | SD      | CONF_I2S_SD           | data in                                                      |
  | 3.3V        | VDD     |                       |                                                              |
  | GND         | GND     |                       |                                                              |
  | GND         | L/R     | CONF_I2S_MODE         | 左右声道，拉低是左声道，代码使用左声道，可以直接接地<br>左声道设置为I2S_LEFT_JUSTIFIED_MODE |
