#include <Arduino.h>
#include <driver/i2s.h>
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

wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();

static QueueHandle_t pcm_rec_group = nullptr;

constexpr uint16_t buffer_queue_size = 4; // 这里使用& 取模应该保证 buffer_queue_size是2的n次方
constexpr uint16_t segment_size = 512;

struct PcmPack {
    uint8_t data[segment_size];
    uint16_t len;
};

PcmPack rxBufferMsg[buffer_queue_size] = {};

int rx_queue_index = 0;

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

    uint16_t packSize = std::min(len, segment_size);
    PcmPack *pPack = rxBufferMsg + rx_queue_index;
    memcpy(pPack->data, data, packSize);
    pPack->len = packSize;

    xQueueSend(pcm_rec_group, pPack, 0);
    rx_queue_index = (rx_queue_index + 1) & (buffer_queue_size - 1);
}

void config_wifi_raw() {
    esp_err_t err = esp_wifi_init(&wifi_config);
    if (err != ESP_OK) {
        log_e("WiFi initialization failed");
        delay(3000);
        ESP.restart();
        return;
    }
    // 设置混杂模式接收回调函数
    esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_rx_cb);
    esp_wifi_set_promiscuous(true);
}

void config_speaker() {
    i2s_config_t i2s_config = {
            .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = CONFIG_SAMPLE_RATE,
            .bits_per_sample = static_cast<i2s_bits_per_sample_t>(CONFIG_SAMPLE_BITS),
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_I2S),
            .intr_alloc_flags = 0,
            .dma_buf_count = 16,
            .dma_buf_len = 60
    };

    i2s_pin_config_t pin_config = {
            .bck_io_num = CONF_I2S_BCLK,
            .ws_io_num = CONF_I2S_LRC,
            .data_out_num = CONF_I2S_DOUT,
            .data_in_num = -1   //Not used
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, CONFIG_SAMPLE_RATE, (i2s_bits_per_sample_t) CONFIG_SAMPLE_BITS, I2S_CHANNEL_MONO);
}

void setup() {
    pcm_rec_group = xQueueCreate(buffer_queue_size + 1, sizeof(PcmPack));
    log_i("config wifi_raw");
    config_wifi_raw();
    log_i("config speaker");
    config_speaker();
    disableLoopWDT();
    log_i("all ready");
}

void loop() {
    PcmPack receivedPack{};
    size_t bytes_written = 0;
    for (;;) {
        if (xQueueReceive(pcm_rec_group, &receivedPack, portMAX_DELAY) == pdTRUE) {
            i2s_write(I2S_NUM_0, receivedPack.data, receivedPack.len, &bytes_written, portMAX_DELAY);
            if (bytes_written < receivedPack.len) {
                log_w("not play all");
            }
        }
    }
}