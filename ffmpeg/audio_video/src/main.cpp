#include <Arduino.h>
#include <I2S.h>
#include <WiFi.h>
#include <esp_camera.h>
#include <esp_task_wdt.h>
#include "udp/UdpClient.hpp"

LightUDP audioSender;
LightUDP streamSender;

constexpr uint16_t audioLocalPort = 2233;
constexpr uint16_t videoLocalPort = 2333;

const char *serverAddress = SERVER_HOST;
constexpr uint16_t videoServerPort = 8004;
constexpr uint16_t audioServerPort = 8005;

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

[[noreturn]] void audioSendTask(void *);

void setup() {
    // 初始化麦克风
    I2S.setAllPins(CONF_I2S_SCK, CONF_I2S_FS, CONF_I2S_SD, -1, -1);
    if (!I2S.begin(CONF_I2S_MODE, CONFIG_SAMPLE_RATE, CONFIG_SAMPLE_BITS)) {
        log_e("Failed to initialize I2S!");
        while (1);
    }
    // 初始化摄像头
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

    s->set_framesize(s, FRAMESIZE_HVGA);

    connectWifi(wifiSsid, wifiPasswd);

    audioSender.begin(WiFi.localIP(), audioLocalPort);
    audioSender.setServer(serverAddress, audioServerPort);

    streamSender.begin(WiFi.localIP(), videoLocalPort);
    streamSender.setServer(serverAddress, videoServerPort);

    xTaskCreatePinnedToCore(audioSendTask, "audioSendTask", 1024 * 8,
                            nullptr, 1, nullptr, 0);
}

void loop() {
    camera_fb_t *fb;
    for (;;) {
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return;
        }
        streamSender.send(fb->buf, fb->len);
        esp_camera_fb_return(fb);
    }
}

constexpr size_t record_size = 1024;
uint8_t rec_buffer[record_size];

size_t bytes_read = 0;

[[noreturn]] void audioSendTask(void *) {
    for (;;) {
        // 如果是esp32或者esp32s2 在使用摄像头的时候 esp_i2s::I2S_NUM_0 可能被使用
        esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, rec_buffer, record_size, &bytes_read, portMAX_DELAY);

        if (bytes_read < 1) {
            log_e("Failed read!");
            continue;
        }
        //  放大音量
        for (uint32_t i = 0; i < bytes_read; i += CONFIG_SAMPLE_BITS / 8) {
            (*(uint16_t *) (rec_buffer + i)) <<= CONFIG_VOLUME_GAIN;
        }
        audioSender.send(rec_buffer, bytes_read);
        esp_task_wdt_reset();
    }
}