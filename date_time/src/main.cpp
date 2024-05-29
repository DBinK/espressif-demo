#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const long gmtOffset_sec = 28800; // 中国时区偏移量，UTC+8，即28800秒
const int daylightOffset_sec = 0; // 夏令时偏移，中国不使用夏令时，设为0
void setClock() {
    configTime(gmtOffset_sec, daylightOffset_sec, "ntp.aliyun.com");
    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2) {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }
    Serial.println();
    struct tm timeinfo{};
    gmtime_r(&nowSecs, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    // wait for WiFi connection
    Serial.println("Waiting for WiFi to connect...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");

    setClock();
}

void loop() {
    time_t now;
    struct tm timeinfo{};
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[20];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println(strftime_buf);
    delay(999);
}
