# esp32静态web项目



基于platformio开发 ，源码中含有静态网页需要传到文件系统。才能正确读到文件。

不懂得如何使用platformio上传文件到esp32可以参考上一个示例的操作。其实也就是点击两个按钮的事情。

 [TestSPIFFS · 云逸之/espressif-demo - 码云 - 开源中国 (gitee.com)](https://gitee.com/yunyizhi/espressif-demo/tree/master/TestSPIFFS)

修改main.cpp中的wifi ssid和密码 或者你可以开一个名为 `test0` 密码为 `12345687`的2.4G 热点

##### 运行日志如下

```tex
ets Jul 29 2019 12:21:46

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13192
load:0x40080400,len:3028
entry 0x400805e4

..[  1298][W][WiFiGeneric.cpp:1057] _eventCallback(): Reason: 2 - AUTH_EXPIRE
.
Connected to test0
MDNS responder started
HTTP server started
http://192.168.137.139
[  6418][E][WebServer.cpp:649] _handleRequest(): request handler not found

```

HTTP server started 后一句就是esp32web的地址。

在浏览器中打开