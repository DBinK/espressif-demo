# esp32录音推送给ffplay播放

这里使用seeed xiao esp32s3 自带的麦克风录音采集PCM数据，并通过UDP发送到ffplay进行播放。



### 编译说明

* 修改platformio里 connection下的wifi的ssid和密码;以及服务端的地址`host`，也就是运行ffplay的pc的局域网ip。

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



目前ffplay每次启动时候延时比较低，随着运行延时累加，重新运行ffplay延时又会降低。