# udp-> ffmpeg
```mermaid
graph LR
esp32[esp32]
ffmpeg[ffmpeg]
vlc[vlc]
esp32 --mjpeg(over udp)--> ffmpeg --rtp(h.264)--> vlc
```

目前的esp32无视频编码能力，可以将jpeg发给ffmpeg以h.264通过rtp推送，可以一定程度减少带宽占用。

这里使用vlc播放ffmpeg推送的rtp视频流。

### 编译说明

* 修改platformio里 common下的wifi的ssid和密码

* 修改main.cpp中 服务端的地址，也就是运行ffmpeg的pc的局域网ip。

### 安装ffmpeg

在widows下可以从该地址下载安装[Builds - CODEX FFMPEG @ gyan.dev](https://www.gyan.dev/ffmpeg/builds/)

这里我下载的是最新的release版本。

linux下可以使用对应具体系统的包管理安装如apt yum。

### 烧录程序后检测esp32到ffmpeg链路

在目标PC上安装ffmpeg后执行

```shell
ffmpeg -i udp://0.0.0.0:8004 -vframes 1 test.jpg
```
可以保存一帧图片，用于检测链路是否正常

### 使用ffmpeg 进行rtp推流

以下将mjpeg转h.264使用rtp推流

```shell
ffmpeg -re -f mjpeg -i udp://0.0.0.0:8004 -c:v libx264 -preset veryfast -tune zerolatency -f rtp rtp://127.0.0.1:8554/mystream
```

### 使用vlc播放

widows下可以从这里下载vlc[Download official VLC media player for Windows - VideoLAN](https://www.videolan.org/vlc/download-windows.html)

如果是64位可以下拉选择64位的vlc。



上一步在127.0.0.1的8554端口监听,将以下内容复杂新建一个后缀为sdp的文件如`play.sdp`。

```ini
m=video 8554 RTP/AVP 96
a=rtpmap:96 H264
a=framerate:25
c=IN IP4 127.0.0.1
```

然后使用vlc打开该sdp文件，即可播放。

当然sdp文件的内容，也可以直接使用ffmpeg在收到esp32推流之后打印在控制台的内容中`SDP:`的后续几行。

![ffmpeg out](./img/sdp.png)

以这些内容新建一个sdp文件。



如果需要走局域网，则需要改rtp的ip为局域网ip。
