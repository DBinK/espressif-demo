# esp32录音推送给ffplay播放

这里使用 esp32外接 INMP441 或者seeed xiao esp32s3 自带的麦克风录音采集PCM数据，并通过UDP发送到ffplay进行播放。



### 编译说明

含有两个环境`esp32s3_INMP441`和`seeed_xiao_esp32s3`



* 修改platformio里 connection下的wifi的ssid和密码;以及服务端的地址`host`，也就是运行ffplay的pc的局域网ip。

* 如果是外接INMP441 情况需要修改`i2s_config` 下的 `default_pins` 这里默认接线如下

  | esp32s3引脚 | INMP441 | platformio中配置项key | 描述                                                         |
  | ----------- | ------- | --------------------- | ------------------------------------------------------------ |
  | 4           | SCK     | CONF_I2S_SCK          | BCK                                                          |
  | 5           | WS      | CONF_I2S_FS           | WS                                                           |
  | 6           | SD      | CONF_I2S_SD           | data in                                                      |
  | 3.3V        | VDD     |                       |                                                              |
  | GND         | GND     |                       |                                                              |
  | GND         | L/R     | CONF_I2S_MODE         | 左右声道，拉低是左声道，代码使用左声道，可以直接接地<br>左声道设置为I2S_LEFT_JUSTIFIED_MODE |

* 使用seeed xiao esp32s3选择环境 `seeed_xiao_esp32s3`无需修改引脚。

### 安装ffmpeg

ffplay属于ffmpeg bin目录的一个可执行文件。

在widows下可以从该地址下载安装[Builds - CODEX FFMPEG @ gyan.dev](https://www.gyan.dev/ffmpeg/builds/)

这里我下载的是最新的release版本。

linux下可以使用对应具体系统的包管理安装如apt yum。



### 使用ffplay播放

以下命令将使用扬声器播放收到的PCM数据。

```shell
ffplay -f s16le -ar 16000 -i udp://0.0.0.0:8005 -fflags nobuffer -flags low_delay -af "aresample=min_comp=0.001:min_hard_comp=0.100" -sync audio -analyzeduration 0 -probesize 32
```



目前seeed_xiao_esp32s3 推ffplay每次启动时候延时比较低，随着运行延时累加，重新运行ffplay延时又会降低。

而外接INMP441目前正常。

