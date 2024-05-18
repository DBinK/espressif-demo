#include <Arduino.h>
#include <mbedtls/sha256.h>
#include <sha256_alt.h>


void setup() {
    Serial.begin(115200);
    while (!Serial) {};
    mbedtls_sha256_context ctx{};
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, false); // false is224 表示sha256而非sha224

    mbedtls_sha256_update(&ctx, reinterpret_cast<const unsigned char *>("hello12345"), 10);
    uint8_t md[32]; //  256位 256/8 32个U8
    mbedtls_sha256_finish(&ctx, md);
    Serial.println("sha256 for hello12345:");
    for (unsigned char i: md) {
        Serial.printf("%02x", i);
    }
    Serial.println();
}

void loop() {
    delay(10 * 1000);
}