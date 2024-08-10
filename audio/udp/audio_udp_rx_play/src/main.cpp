#include <Arduino.h>
#include <driver/i2s.h>
#include "udp/UdpClient.hpp"

LightUDP audioSender;
constexpr uint16_t localPort = 8005;

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
    i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = CONFIG_SAMPLE_RATE,
            .bits_per_sample = static_cast<i2s_bits_per_sample_t>(CONFIG_SAMPLE_BITS),
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
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
    i2s_set_clk(I2S_NUM_0, CONFIG_SAMPLE_RATE, (i2s_bits_per_sample_t)CONFIG_SAMPLE_BITS, I2S_CHANNEL_MONO);
    connectWifi(wifiSsid, wifiPasswd);
    audioSender.begin(WiFi.localIP(), localPort);
}

// 这里与发送端一致就好
constexpr size_t rx_buffer_size = 512;
uint8_t rec_buffer[rx_buffer_size];

size_t bytes_read = 0;
size_t bytes_written = 0;

void loop() {
    bytes_read = audioSender.recv(rec_buffer, rx_buffer_size);
    if (bytes_read < 1) {
        return;
    }
    i2s_write(I2S_NUM_0, rec_buffer, bytes_read, &bytes_written, portMAX_DELAY);
    if (bytes_written < bytes_read) {
        log_w("not play all");
    }
}