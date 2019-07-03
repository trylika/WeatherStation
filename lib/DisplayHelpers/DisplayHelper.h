#ifndef DisplayHelper_H
#define DisplayHelper_H

#include "Adafruit_ST7735.h"

#define DISPLAY_CS 10
#define DISPLAY_RST 8
#define DISPLAY_DC 9
#define DISPLAY_LINES 7
#define DISPLAY_OFFSET_X 2
#define DISPLAY_OFFSET_Y 2
#define DISPLAY_DELAY 1000

namespace DisplayHelper {
    extern Adafruit_ST7735 display;
    extern char displayBuffer[DISPLAY_LINES][16];
    extern uint16_t displayBufferColor[DISPLAY_LINES];
    extern unsigned long lastScreenUpdate;

    void initDisplay();
    void loadingStart();
    void loadingEnd();
    void printSuccess();
    void printFailure();
    void clearScreen();
    uint16_t mapDewPointColor(double temperature);
    uint16_t mapTemperatureColor(float temperature);
}

#endif
