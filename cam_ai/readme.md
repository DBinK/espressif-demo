# ESP32-S3-WROOM CAM 人脸识别

这里使用Freenove ESP32-S3-WROOM Board 在国内叫做ESP32-S3 WROOM N16R8 CAM。
如果使用其他开发板，摄像头引脚和rgb灯不一定适用。
其摄像头引脚等同于 esp32-s3-eye
含有一个RGB灯，这里识别人脸的回调函数，仅仅改变了rgb灯的颜色和亮灭。

这里使用ST7789 LCD屏幕，分辨率为 320 * 240。如果需要使用其他屏幕，则修改platformio.ini中的
tft_config选项。

## 编译配置

使用platformio.ini 中 build_flags 往其中加入以下选项的定义则可以配置值。
例如，当我们需要将FACE_ID_SAVE_NUMBER改为7时，可以按以下方式配置:

```ini
build_flags =
    -D BOARD_HAS_PSRAM
    -D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_INFO
    -D CONFIG_ARDUHAL_LOG_COLORS=1
    -D ARDUINO_SERIAL_EVENT_TASK_RUNNING_CORE=0
    ${CameraPin.esp32s3Eye}
    ${tft_config.st7789_esp32s3}
    -D FACE_ID_SAVE_NUMBER=7
```

* FACE_ID_SAVE_NUMBER
  人脸ID存储的数量,这个会影响登记人脸时候的数量，默认为4.
* RECOGNITION_ENABLED
  人脸识别启用，这个是初始化时，是否启用人脸识别，默认启用。
  如果不启用，则仅仅运行人脸检测。
  如果初始化不启用，也可以后续通过串口启用。
* RECOGNITION_RESULT_QUEUE_MAX_DELAY
  识别结果队列的发送最大延时。默认为0，不等待消费线程取出内容。
  消费线程未及时取数，当队列满后，就会使得本次发送结果丢失。
  当然如果不需要太频繁的调用，丢失部分数据等同于消抖。 可以根据需要设置。

* RECOGNITION_RESULT_QUEUE_SIZE
  识别结果发送队列大小，默认10，如果降低延时并不能完全消抖，可以调小队列大小。
  如果希望处理更多的结果，可以加大队列并同时加大队列发送等待延时。

## 录入人脸

默认是识别模式，但没有录入的人脸，当需要检测的人脸出现的时候，会出现警告。
并回调人脸不匹配。
通过串口输入`e`或者`E`进入登记模式，
在识别模式启用情况下，登记模式启用则在登记完成存储的数量之后。便是正常的识别。

## 关闭人脸识别，仅仅检测

通过串口输入`d`或者`D`关闭识别。

## 启用人脸识别

不改编译配置默认启用。如果需要手动启用，通过串口输入`r`或者`R`。

## 识别结果回调

`void onFaceResult(face_recognize_status_t faceRecognizeStatus)`
在该函数中，接收识别结果。目前仅仅改变rgb灯的颜色，根据需要修改该函数。
`face_recognize_status_t` 说明

| 值  | 枚举          | 描述                |
|----|-------------|-------------------|
| -1 | UNAVAILABLE | 不可用，目前不会发送该值      |
| 0  | TARGET_OK   | 识别成功，与录入人脸匹配      |
| 1  | TARGET_MIS  | 识别不匹配，与录入人脸不匹配    |
| 2  | TARGET_FIND | 检测模式，检测成功         |
| 3  | NORMAL      | 没有人脸的情况，但目前不会发送该值 |