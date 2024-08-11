#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

uint8_t peer_addr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
SemaphoreHandle_t buttonSemaphore = nullptr;

void InitESPNow() {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    if (esp_now_init() == 0) {
        log_i("ESPNow Init Success");
    } else {
        log_i("ESPNow Init Failed");
        ESP.restart();
    }
    log_i(" \n LOCAL MAC: %s", WiFi.macAddress().c_str());
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

    log_i("Sent to:%02X:%02X:%02X:%02X:%02X:%02X ", mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
    if (status == ESP_OK) {
        log_i("Success");
    } else {
        log_i("failed with code:[%d]\n", status);
    }
}

void ARDUINO_ISR_ATTR buttonUp() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);
}

void setup() {
    pinMode(BOOT_PIN, INPUT_PULLUP);
    attachInterrupt(BOOT_PIN, buttonUp, FALLING);
    buttonSemaphore = xSemaphoreCreateBinary();
    // Init ESPNow with a fallback logic
    InitESPNow();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    peerInfo.channel = CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    memcpy(peerInfo.peer_addr, peer_addr, 6);
    esp_err_t addStatus = esp_now_add_peer(&peerInfo);

    if (addStatus == ESP_OK) {
        // Pair success
        log_i("Pair success");
    } else {
        log_i("error code is %d\n", addStatus);
    }
    disableLoopWDT();
}


void loop() {
    for (;;) {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
            uint8_t hello[] = "hello";
            int i = esp_now_send(peer_addr, hello, 5);
            if (i != 0) {
                log_i("not ok esp now status is:%d", i);
            }
        }
    }
}