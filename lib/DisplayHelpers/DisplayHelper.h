#ifndef DisplayHelper_H
#define DisplayHelper_H

#include "Arduino.h"
#include "Adafruit_ST7735.h"

#define DISPLAY_CS 10
#define DISPLAY_RST 8
#define DISPLAY_DC 9

namespace DisplayHelper {
    extern Adafruit_ST7735 display;

    void initDisplay();
    void loadingStart();
    void loadingEnd();
    void printSuccess();
    void printFailure();
    void clearScreen();
}

#endif
