# esp32 通过espnow遥控 linux设备



ESP-NOW 是一种由乐鑫公司定义的无连接 Wi-Fi 通信协议。在 ESP-NOW 中，应用程序数据被封装在各个供应商的动作帧中，然后在无连接的情况下，从一个 Wi-Fi 设备传输到另一个 Wi-Fi 设备。见[ESP-NOW - ESP32 - — ESP-IDF 编程指南 latest 文档 (espressif.com)](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/network/esp_now.html)



在linux设备上，可以将无线网卡切换到monitor模式，进行wifi的空口抓包，用于获取原始的802.11数据帧。

同理espnow 是一种特定的802.11数据帧，我们可以在monitor模式下的无线网卡抓到espnow的报文，因而可以使用linux特定过滤espnow报文，实现espnow接收端。从而可以使用esp设备控制linux设备。



### 接收端

接收端的代码来自[Linux-ESPNOW/ESPNOW_lib at master · thomasfla/Linux-ESPNOW · GitHub](https://github.com/thomasfla/Linux-ESPNOW/tree/master/ESPNOW_lib)

进行了简单的修改，添加了CMakeLists.txt ，并修改了main.cpp，将mac地址过滤去掉了。并在callback中打印接收的报文并控制led灯的亮灭。

这里使用香橙派zero2,通过对 `/sys/devices/platform/leds/leds/green_led/brightness` 写入1或者0控制绿色led灯亮灭。

根据需要修改callback中的内容。

同时默认添加了香橙派zero2使用PWM1控制舵机的操作，如果需要使用**请根据香橙派zero2用户手册开启PWM1**。

如果无线使用去掉main.cpp头部的`#define OPI_ZERO_SERVO`。



由于代码很少，直接使用香橙派zero2编译问题不大，这里使用cmake项目，本人是使用clion远程到香橙派上的toolchain，加clion部署的同步，把代码映射到香橙派路径上。

需要安装gcc g++ cmake 构建系统可以使用makefile或者 ninja。

手动cmake:本人使用clion使用构建生成的命令如下`/usr/bin/cmake --build /home/orangepi/esp_now_lib/cmake-build-debug --target esp_now_linux -- -j 14`根据需要修改。

#### USB-wifi网卡

香橙派zero2自带的wifi网卡不支持monitor模式，可能是驱动或者硬件原因的限制，这里本人经过尝试不同的usb网卡

找到一款支持空口抓包的无线网卡,名为:`英伟达Jetson Nano 免驱无线网卡 Ubuntu deepin uos WiFi接收器`

将其切换至monitor模式，root用户下执行(以下网卡名称替换为真实的网卡名)

```shell
ifconfig wlx90de80347481 down
iwconfig wlx90de80347481 mode monitor
ifconfig wlx90de80347481 up
iwconfig wlx90de80347481 channel 1 
```

如果发现网卡正常UP同时还有IP说明它自动切回manager模式了。

我们需要使用iwconfig检查。

```tex
root@orangepizero2:~# iwconfig wlx90de80347481
wlx90de80347481  IEEE 802.11  Mode:Monitor  Frequency:2.412 GHz  Tx-Power=20 dBm
          Retry short limit:7   RTS thr:off   Fragment thr:off
          Power Management:off

```

可使用`orangepi-config`将USB-WIFI的连接断开，再次切换monitor模式。



#### 运行

以root用户运行，在cmake输出目录会生成执行文件(网卡名称替换为真实的网卡名)  `./esp_now_linux wlx90de80347481`



### esp32发送端

或者按下boot发送 hello。