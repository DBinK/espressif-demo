#include <Arduino.h>
#include "esp_camera.h"
#include "udp/UdpClient.hpp"

LightUDP streamSender;
constexpr uint16_t localPort = 2333;

const char* serverAddress = SERVER_HOST;
constexpr uint16_t serverUdpPort = 8004;

const char *wifiSsid = WIFI_SSID;
const char *wifiPasswd = WIFI_PASSWORD;

void connectWifi(const char *ssid, const char *passphrase) {
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);
    log_i("connecting to router... ");
    // 等待wifi连接成功 连不上把调试日志打开
    while (WiFiClass::status() != WL_CONNECTED) {
        log_printf(".");
        delay(500);
    }
    const IPAddress &localIp = WiFi.localIP();
    const IPAddress &gatewayIp = WiFi.gatewayIP();
    log_i("\nWiFi connected, local IP address:%u.%u.%u.%u\nWifi GWip is:%u.%u.%u.%u\n",
          localIp[0], localIp[1], localIp[2], localIp[3],
          gatewayIp[0], gatewayIp[1], gatewayIp[2], gatewayIp[3]);

}

void setup() {
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
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 3;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK) {
        log_e("Camera init failed with error 0x%x", err);
        delay(3000);
        abort();
    }
    sensor_t *s = esp_camera_sensor_get();

    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_HVGA);
    connectWifi(wifiSsid, wifiPasswd);
    streamSender.begin(WiFi.localIP(), localPort);
    streamSender.setServer(serverAddress, serverUdpPort);

    disableLoopWDT();
}

void loop() {
    camera_fb_t *fb;
    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return;
        }
        streamSender.send(fb->buf, fb->len);
        esp_camera_fb_return(fb);
    }
}