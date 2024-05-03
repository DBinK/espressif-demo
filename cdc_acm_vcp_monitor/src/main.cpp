#include <Arduino.h>

#include "cdc_acm_vcp.h"
//
#include "U8x8lib.h"
#include "U8g2lib.h"

#ifndef U8G2_SDA
#define U8G2_SDA SDA
#endif
#ifndef U8G2_SCL
#define U8G2_SCL SCL
#endif
#ifndef U8LOG_WIDTH
#define U8LOG_WIDTH 20
#endif
#ifndef U8LOG_HEIGHT
#define U8LOG_HEIGHT 6
#endif

// 这里使用硬件i2c
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ U8G2_SCL, /* data=*/ U8G2_SDA);

U8G2LOG u8g2log;

uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

void init2cScreen() {
    u8g2.begin();
    u8g2.setBusClock(400 * 1000);
    u8g2.setFont(u8g2_font_t0_12_tf);    // set the font for the terminal window
    u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8g2log.setLineHeightOffset(0);    // set extra space between lines in pixel, this can be negative
    u8g2log.setRedrawMode(0);        // 0: Update screen with newline, 1: Update screen for every char
}

CdcAcmVcp cdcAcmVcp;

/**
 * @brief Data received callback
 *
 * Just pass received data to stdout
 *
 * @param[in] data     Pointer to received data
 * @param[in] data_len Length of received data in bytes
 * @param[in] arg      Argument we passed to the device open function
 * @return
 *   true:  We have processed the received data
 *   false: We expect more data
 */
static bool handle_rx(const uint8_t *data, size_t data_len, void *arg) {
    u8g2log.write(data, data_len);
    return true;
}

void initCdcAcmVcp() {
    cdcAcmVcp.start(
            {
                    . dwDTERate = VCP_BAUDRATE,
                    .bCharFormat = VCP_STOP_BITS,
                    .bParityType = VCP_PARITY,
                    .bDataBits = VCP_DATA_BITS,
            }, handle_rx);
}

void setup() {
    init2cScreen();
    initCdcAcmVcp();
}


void loop() {
    delay(10 * 10000);
}