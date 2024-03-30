#include <Arduino.h>
#include "face_recognize.h"
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
#include <driver/uart.h>
#include "esp_camera.h"

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library


void onFaceResult(face_recognize_status_t faceRecognizeStatus);

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n Testing TJpg_Decoder library");

    // Initialise the TFT
    tft.begin();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLUE);

#ifdef USE_DMA
    tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
#endif
#ifdef ROTATION
    tft.setRotation(ROTATION);
#endif
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_RGB565;


    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 10;
        config.fb_count = 3;
    } else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        log_i("Camera init failed with error 0x%x", err);
        return;
    }
    log_i("get sensor ");
    sensor_t *s = esp_camera_sensor_get();
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);

    initFaceRecognize(onFaceResult);
    Serial.onReceive(serialReceiveTask);
}

void loop() {
    camera_fb_t *fb = nullptr;
    log_i("do loop");
    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return;
        }
        loop(tft, fb);
    }
}

/**
 * 当有识别结果的时候 回调此函数
 * */
void onFaceResult(face_recognize_status_t faceRecognizeStatus) {
    log_i("%d", faceRecognizeStatus);
    switch (faceRecognizeStatus) {
        case TARGET_FIND:
            neopixelWrite(RGB_BUILTIN, 255, 255, 0);
            delay(300);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
            break;
        case TARGET_OK:
            neopixelWrite(RGB_BUILTIN, 0, 223, 0);
            delay(300);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
            break;
        case TARGET_MIS:
            neopixelWrite(RGB_BUILTIN, 255, 0, 0);
            delay(300);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
            break;
        case NORMAL:
            neopixelWrite(RGB_BUILTIN, 60, 60, 0);
            break;
        default:
            break;
    }
}