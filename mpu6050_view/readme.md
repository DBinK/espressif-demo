# mpu6050 view

esp32自身提供一个web,显示3d视图
![demo.png](demo.png)


## web说明
需要文件系统，这里已经提供data目录，需要构建和上传文件系统
* 使用platformio任务树上提供的`Build Filesystem Image`构建文件系统
* 使用platformio任务树上提供的`Upload Filesystem Image `上传文件系统
* 这里使用SPIF 文件系统,确保分区表有该分区

web修改全量编译，需要配置`nodejs` `npm` 使用platformio命令树上的`Build Web`
生成data目录,并重新生成index.h

## 编译说明
修改platformio.ini
```ini
[common]
wifi_ssid = \"test0\"
wifi_password = \"12345687\"
server_name = \"esp32mpu6050\"
# i2c引脚
sda = 5
scl = 4
```

修改wifi的ssid 和密码以及连接MPU6050的 i2c引脚
`server_name` 是esp32的mdns地址，如果你又多个esp32不要使用同一个地址，可以通过支持mdns的设备，上使用浏览器访问。

## web使用说明
经过测试在edge浏览器上无法很好发挥gpu性能会严重占用cpu，需要谷歌浏览器。

如果你的esp32的mdns地址是`esp32mpu6050` 则可以在PC浏览器(如果这台PC支持MDNS的话)
输入该地址`esp32mpu6050` 来访问web。

通过websocket推送姿态，如果连接图标为绿色，连接成功后，但如果esp32重启，不会立即感知，可以手动刷新页面。
灰色为断连，紫色是连接中。
如果松开boot按钮后出现连接图标一直是紫色情况，请按一下boot按钮。
* 使用方法
需要按住esp32的boot (io0)按钮，移动mpu6050