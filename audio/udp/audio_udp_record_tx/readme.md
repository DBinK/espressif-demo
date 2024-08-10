# esp32录音推送给ffplay播放

这里使用 esp32外接 INMP441 发送到另一个esp32进行播放。

### 编译说明

* 修改platformio里 `connection` 下的wifi的ssid和密码;以及服务端的地址`host`，也就是另一个esp32的ip这里可以使用类似mdns的机制，
或者自定义实现组播发现。

* 根据修改`i2s_config` 下的 `flags` 这里默认接线如下

  | esp32s3引脚 | INMP441 | platformio中配置项key | 描述                                                         |
    | ----------- | ------- | --------------------- | ------------------------------------------------------------ |
  | 4           | SCK     | CONF_I2S_SCK          | BCK                                                          |
  | 5           | WS      | CONF_I2S_FS           | WS                                                           |
  | 6           | SD      | CONF_I2S_SD           | data in                                                      |
  | 3.3V        | VDD     |                       |                                                              |
  | GND         | GND     |                       |                                                              |
  | GND         | L/R     | CONF_I2S_MODE         | 左右声道，拉低是左声道，代码使用左声道，可以直接接地<br>左声道设置为I2S_LEFT_JUSTIFIED_MODE |
