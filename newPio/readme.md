# 在PIO使用新版的arduino平台
由于一些原因platformio并未支持[Arduino ESP32](https://github.com/espressif/arduino-esp32)到 3.0以上
>详情见:
>https://github.com/platformio/platform-espressif32/issues/1225

于是我们可以手动引包，并指定相关开发板配置的来支持Arduino ESP32 3.0开发。

## 注意
* 这里依然不能新建项目，需要复制platformio.ini到新的文件夹。
接下来打开项目
    1.使用`platformio.exe init --ide vscode` 初始化项目，然后使用vscode打开。

    2.或者使用clion打开项目时候自动同步platformio.ini。

**由于一些openocd 配置问题暂未解决，暂时无法使用该方法debug**