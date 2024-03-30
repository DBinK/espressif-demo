#include <Arduino.h>

#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
#include <driver/uart.h>
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
#include "esp_camera.h"

//uint8_t *out_buf;

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n Testing camera RGB565");

    // Initialise the TFT
    tft.begin();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
//    tft.setSwapBytes(true);
#ifdef USE_DMA
    tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
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
}

void loop() {

    camera_fb_t *fb = NULL;
    size_t len = 0;
    log_i("do loop");
#ifdef ROTATION
    tft.setRotation(ROTATION);
#endif

    while (true) {
        uint32_t dt = millis();
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return;
        }
        tft.startWrite();
#ifdef USE_DMA
        tft.pushImageDMA(0, 0, TFT_HEIGHT, TFT_WIDTH, (uint16_t *) fb->buf);
#else
        tft.pushImage(0, 0, TFT_HEIGHT, TFT_WIDTH, (uint16_t *) fb->buf);
#endif
        tft.endWrite();
        dt = millis() - dt;
        esp_camera_fb_return(fb);
        log_i("%ums", dt);
    }

}
