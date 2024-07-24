#include <Arduino.h>
#include "usb_camera.h"
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
    connectWifi(wifiSsid, wifiPasswd);
    streamSender.begin(WiFi.localIP(), localPort);
    streamSender.setServer(serverAddress, serverUdpPort);
    start();
}

void loop() {
    delay(10 * 10000);
}

void camera_frame_cb(uvc_frame_t *frame, void *ptr)
{
    streamSender.send(static_cast<uint8_t *>(frame->data), frame->data_bytes);
}