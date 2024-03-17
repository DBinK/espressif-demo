#include <Arduino.h>
#include "face_recognize.h"

#ifdef USE_DMA
uint16_t dmaBuffer1[512]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t dmaBuffer2[512]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t *dmaBufferPtr = dmaBuffer1;
bool dmaBufferSel = false;
#endif

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
#include <driver/uart.h>

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

#include "esp_camera.h"

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
    tft.setSwapBytes(true);
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
    config.pixel_format = PIXFORMAT_JPEG;


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
    initFaceRecognize();
    Serial.onReceive(serialReceiveTask);
}

void loop() {
    camera_fb_t *fb = NULL;
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
