# esp32 读取文件系统示例

本项目基于platformio开发

这里以nodemcu-32s为例,会出现几个esp32特有的platformio target。

##### 执行 platformio.exe run --list-targets的结果

```tex
Environment    Group     Name         Title                        Description
-------------  --------  -----------  ---------------------------  ----------------------
nodemcu-32s    Platform  buildfs      Build Filesystem Image
nodemcu-32s    Platform  erase        Erase Flash
nodemcu-32s    Platform  size         Program Size                 Calculate program size
nodemcu-32s    Platform  upload       Upload
nodemcu-32s    Platform  uploadfs     Upload Filesystem Image
nodemcu-32s    Platform  uploadfsota  Upload Filesystem Image OTA


```

我们这里需要用到 `Build Filesystem Image` 构建文件系统镜像 `Upload Filesystem Image` 上传文件系统镜像

1. 在platformio 项目的data文件夹，放置文件。

2. 然后点击vscode或者clion任务树上的`Build Filesystem Image `和`Upload Filesystem Image` 则可以将其中文件烧录到esp32开发 板中。

   当然你也可以使用命令，如clion 232版本前的任务树上并无这个两个命令可以手动执行， 在 -t 参数后面加上target名称则行。 

   `pio run -t buildfs`

   `pio run -t uploadfs`



* 注意根据需要更换platformio.ini开发板名称。如果不知道开发板名称，就到platformio home里面搜索。

  

##### 运行结果

```tex
--- Terminal on COM4 | 115200 8-N-1
--- Available filters and text transformations: colorize, debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
ets Jul 29 2019 12:21:46

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13192
load:0x40080400,len:3028
entry 0x400805e4
1234567890

```

