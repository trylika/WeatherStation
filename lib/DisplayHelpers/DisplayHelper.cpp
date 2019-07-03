#include "DisplayHelper.h"

namespace DisplayHelper {
    Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);

    void initDisplay() {
        display.initR(INITR_BLACKTAB);
        display.setRotation(3);
        display.fillScreen(ST7735_BLACK);
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
}
