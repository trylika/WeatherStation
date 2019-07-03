#include "DisplayHelper.h"

namespace DisplayHelper {
    Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);
    char displayBuffer[DISPLAY_LINES][16];
    uint16_t displayBufferColor[DISPLAY_LINES];
    unsigned long lastScreenUpdate = 0;

    void initDisplay() {
        display.initR(INITR_BLACKTAB);
        display.setRotation(3);
        clearScreen();
    }

    void loadingStart() {
        display.setCursor(0, 0);
        display.setTextColor(ST7735_WHITE, ST7735_BLACK);
        display.println();
        display.println("Loading");
        display.println();
    }

    void loadingEnd() {
        display.setTextColor(ST7735_WHITE, ST7735_BLACK);
        display.println();
        display.println("Done");
        display.println();
    }

    void printSuccess() {
        display.setTextColor(ST7735_GREEN, ST7735_BLACK);
        display.println("[OK]");
    }

    void printFailure() {
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.println("[Error]");
    }

    void clearScreen() {
        display.fillScreen(ST7735_BLACK);
    }

    uint16_t mapDewPointColor(double temperature) {
        if (temperature<10.0F) { // A bit dry
            return ST7735_BLUE;
        }

        if (10.0F<temperature && temperature<12.0F) { // Very comfortabile
            return ST7735_CYAN;
        }

        if (12.0F<temperature && temperature<16.0F) { // Comfortabile
            return ST7735_GREEN;
        }

        if (16.0F<temperature && temperature<18.0F) { // OKish
            return ST7735_YELLOW;
        }

        if (18.0F<temperature && temperature<21.0F) { // Not ok
            return ST7735_MAGENTA;
        }

        if (21.0F<temperature && temperature<24.0F) { // Very not ok
            return ST7735_RED;
        }

        if (24.0F<temperature && temperature<26.0F) { // x(
            return ST7735_RED;
        }

        if (26.0F<temperature) { // x(
            return ST7735_RED;
        }

        return ST7735_RED;
    }

    uint16_t mapTemperatureColor(float temperature) {
        if (temperature<14.0F) {
            return ST7735_BLUE;
        }

        if (14.0F<temperature && temperature<18.0F) {
            return ST7735_CYAN;
        }

        if (18.0F<temperature && temperature<24.0F) {
            return ST7735_GREEN;
        }

        if (24.0F<temperature) {
            return ST7735_RED;
        }

        return ST7735_RED;
    }
}
