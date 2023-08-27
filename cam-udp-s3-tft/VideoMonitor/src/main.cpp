#include <Arduino.h>
#include <TJpg_Decoder.h>
#include "SPI.h"
#include "UdpClient.hpp"

#include <TFT_eSPI.h>              // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
LightUDP jpegReceiver;
IPAddress multicastAddr = IPAddress(224, 0, 0, 5);
uint16_t multicastPort = 2334;
uint16_t localPort = 2333;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height()) {
        return false;
    }
    tft.pushImage(x, y, w, h, bitmap);  // Blocking, so only returns when image block is drawn
    return true;
}

void connectWifi(char *ssid, char *passphrase) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);

    Serial.println("connecting to router... ");
    //等待wifi连接成功
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("\nWiFi connected, local IP address:");
    Serial.println(WiFi.localIP());
}

#define topic_length 11
#define topic_ack_length 15
uint8_t light_video_topic[15] = {'l', 'i', 'g', 'h', 't',
                                 '-', 'v', 'i', 'd', 'e', 'o',
                                 '-', 'a', 'c', 'k'};

void notifyOnline() {
    jpegReceiver.begin(WiFi.localIP(), localPort);
    jpegReceiver.setServer(multicastAddr, multicastPort);
    uint8_t buffer[32];
    while (true) {
        jpegReceiver.send(light_video_topic, topic_length);
        delay(600);
        int len = jpegReceiver.recv(buffer, 32);
        if (len < topic_ack_length) {
            continue;
        }
        if (memcmp(light_video_topic, buffer, topic_ack_length) == 0) {
#ifdef STATUS_LED_PIN
            digitalWrite(STATUS_LED_PIN, HIGH);
#endif
            break;
        }
    }
}

void setup() {

    Serial.begin(115200);
    Serial.println("\n\n Testing TJpg_Decoder library");
#ifdef STATUS_LED_PIN
    pinMode(STATUS_LED_PIN, OUTPUT);
#endif
    // Initialise the TFT
    tft.begin();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
#ifdef ROTATION
    tft.setRotation(ROTATION);
#endif
    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);

    // The byte order can be swapped (set true for TFT_eSPI)
    TJpgDec.setSwapBytes(true);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);
    connectWifi(WIFI_SSID, WIFI_PASSWD);
    notifyOnline();

    disableLoopWDT(); // 关闭loop函数的看门狗
}


static uint8_t buffer[16 * 1024];

static int currentIndex = 0;

void loop() {
    tft.fillScreen(TFT_RED);


    while (true) {
        int len = jpegReceiver.recv(&buffer[currentIndex], 1024);
        if (len <= 0) {
            continue;
        }
        // 帧头出现时，应该开始一张新的照片，如果当前索引不在0，则说明需要把帧头的数据移动到头部
        if (currentIndex != 0 && buffer[currentIndex] == 0xFF && buffer[currentIndex + 1] == 0xD8) {
            currentIndex = len;
            memcpy(buffer, &buffer[currentIndex], len);
            continue;
        }
        currentIndex += len;
        // 帧尾出现时，刷新屏幕
        if (buffer[currentIndex - 1] == 0xD9 && buffer[currentIndex - 2] == 0xFF) {
            tft.startWrite();
            // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
            TJpgDec.drawJpg(0, 0, buffer, currentIndex);
            log_i("1 frame");
            tft.endWrite();
            currentIndex = 0;
        }
    }


}
