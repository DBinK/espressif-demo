# espressif-demo

#### 介绍
esp32 esp8266 示例代码



### 项目

* [cam-udp-s3-tft](./cam-udp-s3-tft)  esp32-esp32 无线视频监控

  使用esp32cam采集图片发送给esp32s3通过tft显示

* [TestSPIFFS](./TestSPIFFS) SPIFFS读取入门，使用platformio一键将文件写入flash后从中读取。

  platformio esp32 项目data目录使用。 

* [web](./web)  esp32 使用静态html文件的web服务。

* [esp32Vue](./esp32Vue) platformio 集成esp32 整合 vue 项目示例。

  需要nodejs npm环境。

  将一个vue 3.0 hello world项目 集成，可以通过platformio 一键构建 vue项目，并将生成文件写入esp32 flash。

  esp32作为服务端正确发送打包好的项目给浏览器。

  并含platformio 自定义脚本 自定义task 示例。 用于拓展ci。
  

* [esp32WebSystemInfo](./esp32WebSystemInfo) platformio 集成esp32 整合 vue 项目示例。通过web获取esp32信息，和控制esp32 板载led示例。

  在一个项目基础上整合 UI框架，[**arco design**]([Arco Design Vue](https://arco.design/vue/docs/start))

