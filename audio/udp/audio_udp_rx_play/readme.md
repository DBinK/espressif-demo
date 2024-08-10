# esp32录音推送给ffplay播放

这里使用 esp32 通过udp接受PCM数据 外接 MAX98357 播放。

### 编译说明

* 修改platformio里 `connection` 下的wifi的ssid和密码

* 根据修改`i2s_config` 下的 `flags` 这里默认接线如下

| esp32s3引脚 | MAX98357 | platformio中配置项key | 描述        |
|-----------|----------|-------------------|-----------|
| 4         | LRC      | CONF_I2S_LRC      | 左右声道控制 WS |
| 5         | BCLK     | CONF_I2S_BCLK     | 时钟        |
| 6         | DIN      | CONF_I2S_DOUT     | 数据输出      |
| 5V        | VIN      |                   |           |
| GND       | GND      |                   |           |