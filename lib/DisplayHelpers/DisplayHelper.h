#ifndef DisplayHelper_H
#define DisplayHelper_H

#include "Adafruit_ST7735.h"

#define DISPLAY_CS 10
#define DISPLAY_RST 8
#define DISPLAY_DC 9
#define DISPLAY_INITIAL_OFFSET_X 2
#define DISPLAY_INITIAL_OFFSET_Y 1
#define DISPLAY_DELAY 1000

namespace DisplayHelper {
    extern Adafruit_ST7735 display;
    extern unsigned long lastScreenUpdate;

    void initDisplay();
    void loadingStart();
    void loadingEnd();
    void printSuccess();
    void printFailure();
    void printInformation(const char str[]);
    void clearScreen();
    uint16_t mapTemperatureColor(float temperature);
    uint16_t mapDewPointColor(double temperature);
    uint16_t mapHumidityColor(double humidity);
    uint16_t mapCO2Color(uint16_t co2ppm);
    uint16_t mapTVOCColor(uint16_t tvocppb);
}

#endif
