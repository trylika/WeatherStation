#ifndef main_h
#define main_h

#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <WeatherCalculations.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>

#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

BME280 sensorBME280;
bool sensorBME280connected = false;

float dataTemperature = 0.0F;
float dataHumidity = 0.0F;
float dataPressure = 0.0F;

void setup();
void loop();
void initBME280();
void readData();
void printData();

#endif
