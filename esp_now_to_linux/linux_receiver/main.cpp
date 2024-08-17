/*
Etienne Arlaud
*/

#define OPI_ZERO_SERVO

#include <stdint.h>

#include <assert.h>
#include <unistd.h>

#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include "ESPNOW_manager.h"
#include "ESPNOW_types.h"
#ifdef OPI_ZERO_SERVO
#include "Servo.h"
#endif

using namespace std;

static uint8_t my_mac[6] = {0x90, 0xDE, 0x80, 0x34, 0x74, 0x81};
static uint8_t dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t ESP_mac[6] = {0xEC, 0xDA, 0x3B, 0x67, 0x73, 0xEC};

ESPNOW_manager *handler;

uint8_t payload[127];

namespace fs = std::filesystem;

void writeBrightness(int value) {
    fs::path path("/sys/devices/platform/leds/leds/green_led/brightness");
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open brightness file." << std::endl;
        return;
    }
    file << value;
    file.close();
}

void callback(uint8_t src_mac[6], uint8_t *data, int len) {
    std::string str(reinterpret_cast<const char *>(data), len);
    std::stringstream ss;

    ss << "rev from : ";
    for (int i = 0; i < 6; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(src_mac[i]);
        if (i < 5) {
            ss << ":";
        }
    }
    ss << " msg: " << str;
    std::cout << ss.str() << std::endl;
    static char brightness = 1;
    writeBrightness(brightness);
    brightness = 1 - brightness;
#ifdef OPI_ZERO_SERVO
    rotateServoToAngle(brightness ? 45 : 135);
#endif
}

int main(int argc, char **argv) {
    assert(argc > 1);

    nice(-20);

    handler = new ESPNOW_manager(argv[1], DATARATE_24Mbps, CHANNEL_freq_1, my_mac, dest_mac, false);

    //handler->set_filter(ESP_mac, dest_mac);

    handler->set_recv_callback(&callback);

    handler->start();
#ifdef OPI_ZERO_SERVO
    initializeServo();
#endif
    while (1) {
        std::this_thread::yield();
    }

    handler->end();
}
