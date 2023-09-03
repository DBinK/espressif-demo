# esp32 vue项目
需要

* 配置 nodejs npm

* 构建web 使用platformio 树上的 `Build vue` 会调用npm

* 使用platformio的`Build Filesystem Image`和 `Upload Filesystem Image`
  上传到esp32中。

再烧录esp32的代码。

如果使用cnpm 则修改 platformio.ini中的npm

```
# 如果使用cnpm 则修改  npm = cnpm
npm = npm
```



使用命令并上传web则是

```shell
pio run -t buildWeb
pio run -t buildfs
pio run -t uploadfs
```

