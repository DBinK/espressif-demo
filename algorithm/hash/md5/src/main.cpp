#include <Arduino.h>
#include <MD5Builder.h>

void setup() {
    Serial.begin(115200);
    while (!Serial) {};

    uint8_t md[ESP_ROM_MD5_DIGEST_LEN];

    MD5Builder md5Builder{};
    md5Builder.begin();
    md5Builder.add("hello");
    md5Builder.add("12345");
    md5Builder.calculate();

    md5Builder.getBytes(md);
    Serial.println("md5 for [\"hello12345\"]:");
    for (unsigned char i: md) {
        Serial.printf("%02x", i);
    }
    Serial.println();
    // 计算字节数据的md5
    md5Builder.begin();
    uint8_t data[] = {1, 2, 3, 4, 5};
    md5Builder.add(data, 5);

    md5Builder.calculate();
    md5Builder.getBytes(md);
    Serial.println("md5 for {1, 2, 3, 4, 5}:");
    for (unsigned char i: md) {
        Serial.printf("%02x", i);
    }
    Serial.println();

    // 较大字节数组可以分段读入buffer后分段add参考ESP.getSketchMD5()的实现
    Serial.println("SketchMD5:" + ESP.getSketchMD5());

}

void loop() {
    delay(10 * 1000);
}