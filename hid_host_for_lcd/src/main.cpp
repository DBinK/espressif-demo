#include <Arduino.h>
#include <TFT_eSPI.h>
#include <list>
#include "hid_host_example.h"

TFT_eSPI tft = TFT_eSPI();
const uint32_t backGroundColor = TFT_BLUE;

void setup() {
    // Initialise the TFT
    tft.begin();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(backGroundColor);
    tft.setTextSize(5);

#ifdef USE_DMA
    tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
#endif
#ifdef ROTATION
    tft.setRotation(ROTATION);
#endif
    start();

}

struct ScreenChar {
    int16_t cursor_x;
    int16_t cursor_y;
    char ch;
};
std::list<ScreenChar> printQueue;

void key_board_input(unsigned char ch) {
    if (ch == '\b') {
        if (printQueue.empty()) {
            return;
        }
        const ScreenChar &current = printQueue.back();
        printQueue.pop_back();
        int16_t charWidth = tft.textWidth(&current.ch);
        if (printQueue.empty()) {
            tft.setCursor(0, 0);
            tft.fillRect(0, 0, charWidth, tft.fontHeight(), backGroundColor);
            return;
        }
        // 移动到上一个点
        const ScreenChar &last = printQueue.back();
        tft.setCursor(last.cursor_x, last.cursor_y);
        if (current.cursor_y != last.cursor_y) { // 换行的情况则是当前点删除当前宽度的字符
            tft.fillRect(current.cursor_x - charWidth, current.cursor_y, charWidth, tft.fontHeight(), backGroundColor);
            return;
        }
        // 正常从上一个点覆盖当前字符大小 则退格
        tft.fillRect(last.cursor_x, last.cursor_y, charWidth, tft.fontHeight(), backGroundColor);
        return;
    }
    char ch1 = static_cast<char >(ch);
    tft.print(ch1);
    printQueue.push_back({tft.getCursorX(), tft.getCursorY(), ch1});
}

void loop() {
    delay(100000);
}