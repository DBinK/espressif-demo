#include <Arduino.h>
#include "esp_wifi.h"
#include "esp_system.h"

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

void wifi_init() {

    esp_err_t err = esp_wifi_init(&config);
    if (err != ESP_OK) {
        Serial.println("WiFi initialization failed");
        return;
    }
}

void wifi_promiscuous_rx_cb(void *buffer, wifi_promiscuous_pkt_type_t type) {
    switch (type) {
        case WIFI_PKT_MGMT:
        case WIFI_PKT_CTRL:
        case WIFI_PKT_MISC:
            return;
        default:
            break;
    }
    auto *pkt = (wifi_promiscuous_pkt_t *) buffer;
    uint16_t len = pkt->rx_ctrl.sig_len;
    uint8_t *data = pkt->payload;
    if (memcmp(data + 10, WLAN_IEEE_HEADER_AIR2GROUND + 10, 6) != 0) {
        return;
    }

    data += WLAN_IEEE_HEADER_SIZE;
    len -= WLAN_IEEE_HEADER_SIZE; //skip the 802.11 header

    len -= 4; //the received length has 4 more bytes at the end for some reason.
    Serial.print("rec:");
    Serial.println(String(data, len));
}

void setup() {
    Serial.begin(115200);
    wifi_init();

    // 设置混杂模式接收回调函数
    esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_rx_cb);
    esp_wifi_set_promiscuous(true);
    Serial.println("Promiscuous mode enabled");
}

void loop() {
    delay(1000);
}