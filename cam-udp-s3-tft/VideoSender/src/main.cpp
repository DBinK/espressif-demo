#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "UdpClient.hpp"

#ifndef OVER_LOAD_CAM_CONF
#define CAMERA_MODEL_AI_THINKER // arduino ide 编译时释放该项选择esp32cam摄像头引脚定义
#define FRAMESIZE FRAMESIZE_HVGA
//#define HE_ZHOU_S3 // arduino ide 编译时释放该项选择合宙S3摄像头引脚定义
#endif

#include "cam_pins.h"

const char *ssid = "test0";
const char *passwd = "12345687";
const IPAddress multicastAddr_ = IPAddress(224, 0, 0, 5);
const uint16_t multicastPort = 2334;

LightUDP streamSender;

void connectWifi(const char *ssid, const char *passphrase) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);

    Serial.println("connecting to router... ");
    //等待wifi连接成功
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("\nWiFi connected, local IP address:");
    Serial.println(WiFi.localIP());
}
#define topic_length 11
#define topic_ack_length 15
uint8_t light_video_topic[15] = {'l', 'i', 'g', 'h', 't',
                                 '-', 'v', 'i', 'd', 'e', 'o',
                                 '-','a' ,'c','k'};


void discovery() {
    streamSender.beginMulticast(multicastAddr_, multicastPort);
    uint8_t buffer[32];

    while (true) {
        int len = streamSender.recv(buffer, 32);
        if (len < 11) {
            continue;
        }
        if (memcmp(light_video_topic, buffer, topic_length) == 0) {

            IPAddress address = streamSender.getRecRemoteIp();
            streamSender.setServer(address, streamSender.getRecRemotePort());
            streamSender.endMulticast();
#ifdef STATUS_LED_PIN
            digitalWrite(STATUS_LED_PIN, HIGH);
#endif
            streamSender.send(light_video_topic,topic_ack_length);
            break;
        }

    }

}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    while (!Serial) {}
#ifdef STATUS_LED_PIN
    pinMode(STATUS_LED_PIN, OUTPUT);
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
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_HD;
        config.jpeg_quality = 10;
        config.fb_count = 3;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
        // 使用直接内存申请
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        log_i("Camera init failed with error 0x%x", err);
        return;
    }
    log_i("get sensor ");
    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_HVGA);

    connectWifi(ssid, passwd);
    discovery();
    disableLoopWDT();

}

void loop() {
    Serial.println("do loop");
    camera_fb_t *fb = NULL;
    size_t len;
    log_i("send image");

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            break;
        }
        len = fb->len;
        size_t count = len >> 10;
        size_t lastSub = len & 0x3ff;
        for (int i = 0; i < count; ++i) {
            streamSender.send(&fb->buf[i << 10], 1024);
        }
        if (lastSub) {
            streamSender.send(&fb->buf[count << 10], lastSub);
        }

        esp_camera_fb_return(fb);
    }
}