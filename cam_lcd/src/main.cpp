#include <Arduino.h>

#include <TJpg_Decoder.h>

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

// This next function will be called during decoding of the jpeg file to render each
// 16x16 or 8x8 image tile (Minimum Coding Unit) to the TFT.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    // Stop further decoding as image is running off bottom of screen
    if (y >= tft.height()) return false;

#ifdef USE_DMA
    // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
    // bitmap can then be updated by the jpeg decoder while DMA is in progress
    if (dmaBufferSel) dmaBufferPtr = dmaBuffer2;
    else dmaBufferPtr = dmaBuffer1;
    dmaBufferSel = !dmaBufferSel; // Toggle buffer selection
    //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
    tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
    // The DMA transfer of image block to the TFT is now in progress...
#else
    // Non-DMA blocking alternative
  tft.pushImage(x, y, w, h, bitmap);  // Blocking, so only returns when image block is drawn
#endif
    // Return 1 to decode next block.
    return true;
}

#include "esp_camera.h"

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n Testing TJpg_Decoder library");

    // Initialise the TFT
    tft.begin();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);

#ifdef USE_DMA
    tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
#endif

    // The jpeg image can be scaled down by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);

    // The colour byte order can be swapped by the decoder
    // using TJpgDec.setSwapBytes(true); or by the TFT_eSPI library:
    TJpgDec.setSwapBytes(true);
    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);
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
}

void loop() {

    camera_fb_t *fb = NULL;
    size_t len = 0;
    log_i("do loop");
#ifdef ROTATION
    tft.setRotation(ROTATION);
#endif

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return;
        }
        len = fb->len;
        uint32_t dt = millis();

        tft.startWrite();
        TJpgDec.drawJpg(0, 0, fb->buf, len);
        tft.endWrite();

        dt = millis() - dt;
        log_i("%dms", dt);
        esp_camera_fb_return(fb);
    }

}
