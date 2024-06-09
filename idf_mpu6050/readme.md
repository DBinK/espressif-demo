# IDF MPU6050例程
使用idf组件库里的MPU6050库。

## 背景
在如今版本`espressif32 @ 6.7.0` 以及 `Adafruit MPU6050 @ 2.2.6` 下获取MPU6050的相关数据，
运行一段时间后会遇到如下错误:
```text
[ 17989][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
[ 17997][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
[ 18006][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
[ 18014][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
AccelX:5.19,AccelY:0.00,AccelZ:0.00, GyroX:-13.56,GyroY:21.89,GyroZ:-14.92
[ 18032][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
[ 18040][E][Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
```
出现错误后，将没有读数。

所以这里采用了IDF组件库里面的MPU6050库，以放置在lib目录，无需额外操作。

## 编译说明
指定I2C引脚
```ini
build_flags =
    -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_INFO
    -D CONFIG_ARDUHAL_LOG_COLORS=1
    -D MPU_SDA=5
    -D MPU_SCL=4
```
 修改platformio.ini里面的 `MPU_SDA` `MPU_SCL`的值以指定i2c引脚。

## 参考

* [github: esp-bsp mpu6050](https://github.com/espressif/esp-bsp/tree/master/components/mpu6050)
* [idf 组件库 MPU6050](https://components.espressif.com/components/espressif/mpu6050/versions/1.2.0)