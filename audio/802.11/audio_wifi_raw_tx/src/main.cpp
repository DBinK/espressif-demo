#include <Arduino.h>
#include <esp_wifi.h>
#include <I2S.h>

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

wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();

uint8_t buffer[WLAN_MAX_PACKET_SIZE];

void config_wifi_raw() {
    esp_err_t ret = esp_wifi_init(&wifi_config);
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
}

void setup() {
    I2S.setAllPins(CONF_I2S_SCK, CONF_I2S_FS, CONF_I2S_SD, -1, -1);
    if (!I2S.begin(CONF_I2S_MODE, CONFIG_SAMPLE_RATE, CONFIG_SAMPLE_BITS)) {
        log_e("Failed to initialize I2S!");
        while (1);
    }
    config_wifi_raw();
}

constexpr size_t record_size = 512;
uint8_t *rec_buffer = buffer + WLAN_IEEE_HEADER_SIZE;

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
    esp_err_t result = esp_wifi_80211_tx(WIFI_IF_STA, buffer, WLAN_IEEE_HEADER_SIZE + bytes_read, false);
    if (result != ESP_OK) {
        log_e("send failed %d", result);
    }
}