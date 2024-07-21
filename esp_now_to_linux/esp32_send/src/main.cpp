#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

uint8_t peer_addr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void InitESPNow() {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    if (esp_now_init() == 0) {
        Serial.println("ESPNow Init Success");
    } else {
        Serial.println("ESPNow Init Failed");
        ESP.restart();
    }
    Serial.print(" \n LOCAL MAC: ");
    Serial.println(WiFi.macAddress());
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

    Serial.printf("Sent to:%02X:%02X:%02X:%02X:%02X:%02X ", mac_addr[0], mac_addr[1], mac_addr[2],
                  mac_addr[3], mac_addr[4], mac_addr[5]);
    if (status == ESP_OK) {
        Serial.println("Success");
    } else {
        Serial.printf("failed with code:[%d]\n", status);
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    pinMode(0, INPUT_PULLUP);

    Serial.setTimeout(100);
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
        Serial.println("Pair success");
    } else {
        Serial.printf("error code is %d\n", addStatus);
    }
}

void loop() {
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    if (digitalRead(0) == LOW) {
        uint8_t hello[] = "hello";
        int i = esp_now_send(peer_addr, hello, 5);
        if (i != 0) {
            Serial.print("not ok esp now status is:");
            Serial.println(i);
        }
    }
    static uint8_t msg[16] = {0};

    size_t len = Serial.readBytesUntil('\n', msg, 16);
    if (len < 2) {
        return;
    }

    // 避免界面刷屏之后依然日志完全一致
    Serial.print("length is ");
    Serial.println(len);
#ifdef CONTROLLER_DEBUG_OUTPUT
    Serial.print("recv: ");
    for (size_t i = 0; i < len; ++i) {
        Serial.printf("%02X ", msg[i]);
    }
    Serial.println();
#endif
    int i = esp_now_send(peer_addr, msg + 1, len - 1);
    if (i != 0) {
        Serial.print("not ok esp now status is:");
        Serial.println(i);
    }
    digitalWrite(LED_BUILTIN, HIGH);

}