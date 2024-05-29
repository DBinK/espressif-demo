# 日期和时间
esp32和esp8266通过ntp同步时间后，并从本地获取时间并格式化打印。
这里使用阿里云的ntp`ntp.aliyun.com` 和中国时区。
本例程只在启动的时候进行一个ntp对时，时间久了可能累积偏差，可以周期性同步时间。

## 编译选项
目前只添加了两个开发板 esp32s3 dev kit c和esp8266 nodemcu 12
但代码通用。

使用目标开发板只需要修改wifi的ssid密码即可。
```ini
[common]
wifi_ssid = \"test0\"
wifi_password = \"12345687\"
```

使用非目标的芯片类型，可以添加相关env配置。

如添加经典款esp32芯片的开发板
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
upload_speed = 921600
monitor_speed = 115200
monitor_filters =
    direct
    esp32_exception_decoder
build_flags =
    -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_INFO
    -D CONFIG_ARDUHAL_LOG_COLORS=1
    -D WIFI_SSID=${common.wifi_ssid}
    -D WIFI_PASSWORD=${common.wifi_password}
```

