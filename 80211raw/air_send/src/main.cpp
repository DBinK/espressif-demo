#include <Arduino.h>
#include <esp_wifi.h>
#include <WiFi.h>

constexpr uint8_t WLAN_IEEE_HEADER_AIR2GROUND[] =
        {
                0x08, 0x01, 0x00, 0x00,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                0x10, 0x86
        };

constexpr size_t WLAN_IEEE_HEADER_SIZE = sizeof(WLAN_IEEE_HEADER_AIR2GROUND);
constexpr size_t WLAN_MAX_PACKET_SIZE = 1500;
constexpr size_t WLAN_MAX_PAYLOAD_SIZE = WLAN_MAX_PACKET_SIZE - WLAN_IEEE_HEADER_SIZE;

wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
uint8_t buffer[WLAN_MAX_PACKET_SIZE];

void setup() {
    esp_err_t ret = esp_wifi_init(&config);
    if (ret != ESP_OK) {
        log_e("esp_wifi_init failed : %d", ret);
        delay(1000);
        ESP.restart();
    }
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_err_t err = esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_18M);
    if (err != ESP_OK) {
        log_e("esp_wifi_init failed : %d", ret);
        delay(1000);
        ESP.restart();
    }
    memcpy(buffer, WLAN_IEEE_HEADER_AIR2GROUND, WLAN_IEEE_HEADER_SIZE);
    memcpy(buffer + WLAN_IEEE_HEADER_SIZE, "hello00000", 10);
}

void loop() {
    esp_err_t result = esp_wifi_80211_tx(WIFI_IF_STA, buffer, WLAN_IEEE_HEADER_SIZE + 10, false);
    if (result != ESP_OK) {
        log_e("send failed %d", result);
    }
    delay(1000);
}