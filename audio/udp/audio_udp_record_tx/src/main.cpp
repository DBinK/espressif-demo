#include <Arduino.h>
#include <I2S.h>
#include "udp/UdpClient.hpp"

LightUDP audioSender;
constexpr uint16_t localPort = 2233;

const char* serverAddress = SERVER_HOST;
constexpr uint16_t serverUdpPort = 8005;

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
    I2S.setAllPins(CONF_I2S_SCK, CONF_I2S_FS, CONF_I2S_SD, -1, -1);
    if (!I2S.begin(CONF_I2S_MODE, CONFIG_SAMPLE_RATE, CONFIG_SAMPLE_BITS)) {
        log_e("Failed to initialize I2S!");
        while (1);
    }
    connectWifi(wifiSsid, wifiPasswd);
    audioSender.begin(WiFi.localIP(), localPort);
    audioSender.setServer(serverAddress, serverUdpPort);
}

constexpr size_t record_size = 512;
uint8_t rec_buffer[record_size];

size_t bytes_read = 0;

void loop() {
    esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, rec_buffer, record_size, &bytes_read, portMAX_DELAY);

    if (bytes_read < 1) {
        log_e("Failed read!");
        return;
    }
    //  放大音量
    for (uint32_t i = 0; i < bytes_read; i += CONFIG_SAMPLE_BITS / 8) {
        (*(uint16_t *) (rec_buffer + i)) <<= CONFIG_VOLUME_GAIN;
    }
    audioSender.send(rec_buffer, bytes_read);
}