#ifndef main_h
#define main_h

#include <Arduino.h>
#include <Wire.h>
#include <WeatherCalculations.h>
#include <DisplayHelper.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>

#define SENSOR_DELAY 500
unsigned long lastSensorUpdate = 0;

#define BME280_I2C 0x76
BME280 sensorBME280;
bool sensorBME280connected = false;
float dataTemperature = 0.0F;
double dataDew = 0.0F;
float dataHumidity = 0.0F;
float dataAbsoluteHumidity = 0.0F;
float dataPressure = 0.0F;

#define CCS811_I2C 0x5A
CCS811 sensorCCS811(CCS811_I2C);
bool sensorCCS811connected = false;
uint16_t dataCO2 = 0;
uint16_t dataTVOC = 0;

void setup();
void loop();
void initBME280();
void initCCS811();
void updateDataBME280();
void updateDataCCS811();
void updateScreen();

#endif
