# IEEE802.11 原始包收发例程

类似于espnow无需建立sta ap 连接场景， 直接对空中发包和接受空中wifi帧来通讯的方式。

但相比于espnow可以有更大的报文长度和更高的带宽。espnow本质也是一种特定的80211中的帧。

### esp32 混杂模式

esp32可以设置混杂模式可以转储802.11相关帧，参考[Wi-Fi 驱动程序 - ESP32 - — ESP-IDF 编程指南 v5.1 文档 (espressif.com)](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.1/esp32/api-guides/wifi.html?highlight=sniffer#wi-fi-sniffer)。

在linux系统下类似功能可以加个wifi网卡设置为monitor模式，便可获取802.11帧,直接应用就是用于wireshark空口抓包。

甚至可以在linux上接收espnow报文。

esp32混杂模式也具有类似功能。

**无需目的地址为当前mac地址都会收到。**

### 发送80211帧

参考[Wi-Fi 驱动程序 - ESP32 - — ESP-IDF 编程指南 v5.1 文档 (espressif.com)](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.1/esp32/api-guides/wifi.html?highlight=sniffer#wi-fi-80211)

**无需发送MAC地址为当前mac地址都可以发送出去。**



### 代码

* [air_scan](./air_scan) 混杂模式接受空中wifi帧，这里设置了过滤条件为air_send例程的报文格式。

  地址为 接受的报文的发送的者mac为 `11:22:33:44:55:66`

* [air_send](./air_send) 发送80211帧，构造了一个发送者为`11:22:33:44:55:66`目标地址也是`11:22:33:44:55:66`的报文，并加入了一句`hello00000`。

  

  可以自行修改mac地址，可以使用真实mac或者使用这种连号避免与其他设备冲突。

  

