#include <Arduino.h>
#include "SPIFFS.h"

void setup() {
    Serial.begin(115200);
    while (!Serial) {};
    SPIFFS.begin();
    File file = SPIFFS.open("/test.txt", FILE_READ);
    if (!file) {
        log_i("There was an error opening the file for writing");
        return;
    }
    uint8_t buff[10];
    file.read(buff, 10);
    Serial.println(String(buff, 10));
    file.close();
}

void loop() {
// write your code here
}