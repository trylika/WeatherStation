#include "DisplayHelper.h"

namespace DisplayHelper {
    Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);
    unsigned long lastScreenUpdate = 0;

    void initDisplay() {
        display.initR(INITR_BLACKTAB);
        display.setRotation(3);
        clearScreen();
    }

    void loadingStart() {
        display.setCursor(DISPLAY_INITIAL_OFFSET_X, DISPLAY_INITIAL_OFFSET_Y);
        display.setTextColor(ST7735_WHITE, ST7735_BLACK);
        display.println("Loading");
    }

    void loadingEnd() {
        display.setCursor(DISPLAY_INITIAL_OFFSET_X, display.getCursorY());
        display.setTextColor(ST7735_WHITE, ST7735_BLACK);
        display.println("Done");
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

    void printInformation(const char str[]) {
        display.setCursor(DISPLAY_INITIAL_OFFSET_X, display.getCursorY());
        display.setTextColor(ST7735_WHITE, ST7735_BLACK);
        display.print(str);
    }

    uint16_t mapTemperatureColor(float temperature) {
        if (temperature<14.0F) {
            return ST7735_BLUE;
        }

        if (temperature<18.0F) {
            return ST7735_CYAN;
        }

        if (temperature<24.0F) {
            return ST7735_GREEN;
        }

        // temperature>24.0F
        return ST7735_RED;
    }

    uint16_t mapDewPointColor(double temperature) {
        if (temperature<10.0F) { // A bit dry
            return ST7735_BLUE;
        }

        if (temperature<12.0F) { // Very comfortabile
            return ST7735_CYAN;
        }

        if (temperature<16.0F) { // Comfortabile
            return ST7735_GREEN;
        }

        if (temperature<18.0F) { // OKish
            return ST7735_YELLOW;
        }

        if (temperature<21.0F) { // Not ok
            return ST7735_MAGENTA;
        }

        if (temperature<24.0F) { // Very not ok
            return ST7735_RED;
        }

        if (temperature<26.0F) { // x(
            return ST7735_RED;
        }

        // temperature>26.0F
        return ST7735_RED;
    }

    uint16_t mapHumidityColor(double humidity) {
        if (humidity>45 && humidity<55) {
            return ST7735_GREEN;
        }

        if (humidity>30 && humidity<60) {
            return ST7735_YELLOW;
        }

        // humidity>60% humidity<25%
        return ST7735_RED;
    }

    uint16_t mapCO2Color(uint16_t co2ppm) {
        if (co2ppm < 800) {
            return ST7735_GREEN;
        }

        if (co2ppm < 2000) {
            return ST7735_YELLOW;
        }

        // co2ppm>5000
        return ST7735_RED;
    }

    uint16_t mapTVOCColor(uint16_t tvocppb) {
        if (tvocppb < 150) {
            return ST7735_GREEN;
        }

        if (tvocppb < 500) {
            return ST7735_CYAN;
        }

        if (tvocppb < 1500) {
            return ST7735_YELLOW;
        }

        if (tvocppb < 5000) {
            return ST7735_MAGENTA;
        }

        // tvocppb>5000
        return ST7735_RED;
    }
}
