# esp32推音视频

esp32同时推音视频。
> 这里使用均是esp32s3 esp32s3有lcd_cam外设，而esp32和esp32s2的摄像头驱动会用到I2S相关,
> 未测试I2S0和摄像头能否同时使用。

### 编译说明

含有两个环境

#### esp32-s3-devkitc-1

使用Freenove ESP32-S3-WROOM Board 某宝叫做 ESP32-S3 WROOM N16R8 CAM

[Freenove/Freenove_ESP32_S3_WROOM_Board: Apply to FNK0085 (github.com)](https://gitee.com/link?target=https%3A%2F%2Fgithub.com%2FFreenove%2FFreenove_ESP32_S3_WROOM_Board)

这里外接i2s麦克风`INMP441`
##### i2s引脚

| esp32s3引脚 | INMP441 | platformio中配置项key | 描述                                                          |
      |-----------|---------|-------------------|-------------------------------------------------------------|
| 19        | SCK     | CONF_I2S_SCK      | BCK                                                         |
| 20        | WS      | CONF_I2S_FS       | WS                                                          |
| 21        | SD      | CONF_I2S_SD       | data in                                                     |
| 3.3V      | VDD     |                   |                                                             |
| GND       | GND     |                   |                                                             |
| GND       | L/R     | CONF_I2S_MODE     | 左右声道，拉低是左声道，代码使用左声道，可以直接接地<br>左声道设置为I2S_LEFT_JUSTIFIED_MODE |

#### seeed_xiao_esp32s3

[Seeed Studio XIAO ESP32S3 (Sense) 开发板 | Seeed Studio Wiki](https://wiki.seeedstudio.com/cn/xiao_esp32s3_getting_started/)
这里无需修改引脚

#### 连接信息修改
修改platformio里 connection下的wifi的ssid和密码;以及服务端的地址`host`，也就是运行ffplay的pc的局域网ip。

### 安装ffmpeg

ffplay属于ffmpeg bin目录的一个可执行文件。

在widows下可以从该地址下载安装[Builds - CODEX FFMPEG @ gyan.dev](https://www.gyan.dev/ffmpeg/builds/)

这里我下载的是最新的release版本。

linux下可以使用对应具体系统的包管理安装如apt yum。

### 使用ffplay播放
正确做法应该是把音视频使用ffmpeg接受合成一个视频推送。

#### 播放音频
以下命令将使用扬声器播放收到的PCM数据。
```shell
ffplay -f s16le -ar 16000 -i udp://0.0.0.0:8005 -fflags nobuffer -flags low_delay -af "aresample=min_comp=0.001:min_hard_comp=0.100" -sync audio -analyzeduration 0 -probesize 32
```
#### 播放视频
```shell
ffplay -f mjpeg -i "udp://0.0.0.0:8004" -bufsize 1024 -fflags +genpts -analyzeduration 0 -probesize 32  -noautoexit
```
目前seeed_xiao_esp32s3 推ffplay每次启动时候延时比较低，随着运行延时累加，重新运行ffplay延时又会降低。

而外接INMP441目前正常。

