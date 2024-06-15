#include <Arduino.h>

void setup() {
    log_i("flash size:%u", ESP.getFlashChipSize());
    log_i("Chip Model:%s", ESP.getChipModel());
}

void loop() {
    log_i("hello world");
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 255, 165, 0);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 255, 255, 0);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 0, 127, 255);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);
    delay(500);
    neopixelWrite(RGB_BUILTIN, 139, 0, 255);
    delay(500);
}
