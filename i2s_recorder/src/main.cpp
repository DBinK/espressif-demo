#include <Arduino.h>

#include <cstring>
#include <driver/i2c.h>
#include <SD_MMC.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_check.h"
#include "format_wav.h"

#define SD_MOUNT_POINT      "/sdcard"
#define NUM_CHANNELS        (1) // 一个声道
// 字节速率 也就是 比特率/8 ,采样率 * 位深 * 通道数量得到比特率 比特转 byte 除8
#define BYTE_RATE           ((CONFIG_EXAMPLE_SAMPLE_RATE * (CONFIG_EXAMPLE_BIT_SAMPLE / 8)) * NUM_CHANNELS)
i2s_port_t I2S_NUM_ = static_cast<i2s_port_t>(CONFIG_EXAMPLE_I2S_CH);
#define SAMPLE_SIZE         1024

static const char err_reason[][30] = {"input param is invalid",
                                      "operation timeout"
};
static int16_t i2s_readraw_buff[SAMPLE_SIZE];

static const char *TAG = "i2s_std_recorder";

void mount_sdcard() {
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
        ESP_LOGI(TAG, "Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        ESP_LOGI(TAG, "No SD_MMC card attached");
        return;
    }

    ESP_LOGI(TAG, "SD_MMC Card Type: ");
    if (cardType == CARD_MMC) {
        ESP_LOGI(TAG, "MMC");
    } else if (cardType == CARD_SD) {
        ESP_LOGI(TAG, "SDSC");
    } else if (cardType == CARD_SDHC) {
        ESP_LOGI(TAG, "SDHC");
    } else {
        ESP_LOGI(TAG, "UNKNOWN");
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    ESP_LOGI(TAG, "SD_MMC Card Size: %lluMB", cardSize);
    ESP_LOGI(TAG, "Initializing SD card");
}

static esp_err_t i2s_driver_init() {
    i2s_config_t i2s_cfg = {
            .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = CONFIG_EXAMPLE_SAMPLE_RATE,
            .bits_per_sample = i2s_bits_per_sample_t(CONFIG_EXAMPLE_BIT_SAMPLE),
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 1024
    };


    i2s_pin_config_t i2s_pin_cfg = {
            .bck_io_num = CONFIG_EXAMPLE_I2S_BCK_GPIO,
            .ws_io_num = CONFIG_EXAMPLE_I2S_WS_GPIO,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = CONFIG_EXAMPLE_I2S_DATA_GPIO,
    };
    ESP_RETURN_ON_ERROR(i2s_driver_install(I2S_NUM_, &i2s_cfg, 0, nullptr), TAG,
                        "install i2s failed");
    ESP_RETURN_ON_ERROR(i2s_set_pin(I2S_NUM_, &i2s_pin_cfg), TAG, "set i2s pins failed");
    return ESP_OK;
}


static void record_wav(void *args) {
    ESP_LOGI(TAG, "bite rate:%d" ,BYTE_RATE * 8 );
    uint32_t flash_rec_time = BYTE_RATE * CONFIG_EXAMPLE_REC_TIME;
    const wav_header_t wav_header =
            WAV_HEADER_PCM_DEFAULT(flash_rec_time, CONFIG_EXAMPLE_BIT_SAMPLE, CONFIG_EXAMPLE_SAMPLE_RATE, NUM_CHANNELS);
    // First check if file exists before creating a new file.
    if (SD_MMC.exists(SD_MOUNT_POINT"/record.wav")) {
        // Delete it if it exists
        SD_MMC.remove(SD_MOUNT_POINT"/record.wav");
        ESP_LOGI(TAG, "remove record.wav");
    }

    // Create new WAV file
    File file = SD_MMC.open(SD_MOUNT_POINT"/record.wav", "a", true);

    // 写入 wav头信息
    file.write(reinterpret_cast<const uint8_t *>(&wav_header), sizeof(wav_header));
    esp_err_t ret = ESP_OK;
    size_t bytes_read = 0;
    ESP_LOGI(TAG, "recording start");
    size_t flash_wr_size = 0;
    while (flash_wr_size < flash_rec_time) {
        ret = i2s_read(I2S_NUM_, (uint8_t *) i2s_readraw_buff, SAMPLE_SIZE, &bytes_read, 1000);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[echo] i2s read failed, %s", err_reason[ret == ESP_ERR_TIMEOUT]);
            abort();
        }
        file.write((uint8_t *) i2s_readraw_buff, bytes_read);
        flash_wr_size += bytes_read;
    }
    file.close();
    SD_MMC.end();
    ESP_LOGI(TAG, "The recording is complete.");
    vTaskDelete(nullptr);
}


void setup() {
    pinMode(LR_PIN, OUTPUT);
    digitalWrite(LR_PIN, LOW);
    mount_sdcard();
    if (i2s_driver_init() != ESP_OK) {
        ESP_LOGE(TAG, "i2s driver init failed");
        abort();
    }
    xTaskCreate(record_wav, "i2s_std_recorder", 8192, nullptr, 5, nullptr);
    disableLoopWDT();
}

void loop() {
    delay(10 * 10000);
}