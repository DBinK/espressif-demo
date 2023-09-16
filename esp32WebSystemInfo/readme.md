# esp32 vue项目

esp32作为服务端含一个获取系统信息功能，和通过web控制板载LED的功能。

需要

* 配置 nodejs npm

* 构建web 使用platformio 树上的 `Build vue` 会调用npm

* 使用platformio的`Build Filesystem Image`和 `Upload Filesystem Image`
  上传到esp32中。

再烧录esp32的代码。

##### 修改wifi ssid和密码

在platformio.ini 的common中修改

```ini
[common]
wifi_ssid = \"test0\"
password = \"12345687\"
```

##### 如果使用cnpm 则修改 platformio.ini中的npm

```
# 如果使用cnpm 则修改  npm = cnpm
npm = npm
```

##### 通过宏修改esp32域名和开发板名称

BOARD_ID

```ini
[env:nodemcu-32s]
# 省略
build_flags =
	#省略
	-D BOARD_ID=\"nodemcu-32s\"
    -D ESP32_SEVER_HOST_NAME=\"esp32\"
```

访问web方式如域名为esp32则 http://esp32  为esp32c3则 http://esp32c3  pc可能不支持mdns，那就使用串口打印的基于ip的url访问,就不用纠结域名。



如果需要命令并上传web则是以下命令

```shell
pio run -t buildWeb
pio run -t buildfs
pio run -t uploadfs
```

