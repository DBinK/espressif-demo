# websocket server
移植于esp-idf ws_echo_server。本身已经包含http server无需再使用额外的http服务。

客户端的mdns如局域网的PC(大部分情况下手机平板并不支持mdns)
会从`ws://esp32:80/ws`这个地址接收消息，并发回

如果mdns不生效可以从日志打印中查看ip
```text
[   226][I][main.cpp:186] example_connect(): [ws_echo_server] Waiting for WiFi to connect...
[   727][I][main.cpp:192] example_connect(): [ws_echo_server] connected, localIP: 192.168.0.173
```

将mdns地址`esp32`替换为 ip
