# esp32 pcm Wifi-Raw 接收端

这里使用 esp32 通过80211 从接收PCM数据 外接 MAX98357 播放。
无需配网，使用wifi 在80211层直接接收 main.cpp中 `WLAN_IEEE_HEADER_AIR2GROUND` 一致即可接收。
这里不存在点对点，都是发送端往空中发，接收端可以存在多个同时接收。

### 编译说明

* 根据需要修改`i2s_config` 下的 `flags` 这里默认接线如下

| esp32s3引脚 | MAX98357 | platformio中配置项key | 描述        |
|-----------|----------|-------------------|-----------|
| 4         | LRC      | CONF_I2S_LRC      | 左右声道控制 WS |
| 5         | BCLK     | CONF_I2S_BCLK     | 时钟        |
| 6         | DIN      | CONF_I2S_DOUT     | 数据输出      |
| 5V        | VIN      |                   |           |
| GND       | GND      |                   |           |