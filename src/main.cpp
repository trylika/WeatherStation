#include <main.h>

void setup() {
    Wire.begin();

    DisplayHelper::initDisplay();

    DisplayHelper::loadingStart();

    initBME280();
    initCCS811();

    DisplayHelper::loadingEnd();
    delay(1000);

    if (sensorBME280connected || sensorCCS811connected) {
        DisplayHelper::clearScreen();
    }
}

void loop() {
    if (millis() - lastSensorUpdate > SENSOR_DELAY) {
        updateDataBME280();
        updateDataCCS811();
        lastSensorUpdate = millis();
    }

    updateScreen();
}

void initBME280() {
    DisplayHelper::printInformation("BME280...   ");

    sensorBME280.setI2CAddress(BME280_I2C);
    sensorBME280connected = sensorBME280.beginI2C();

    if (sensorBME280connected) {
        sensorBME280.setFilter(2); // 0 to 4 is valid. Filter coefficient. See 3.4.4
        sensorBME280.setStandbyTime(0); // 0 to 7 valid. Time between readings. See table 27.

        sensorBME280.setTempOverSample(5); // 0 .. 5 sets 0 to 16. 0 disables temp sensing. See table 24.
        sensorBME280.setPressureOverSample(5); // 0 .. 5 sets 0 to 16. 0 disables pressure sensing. See table 23.
        sensorBME280.setHumidityOverSample(5); // 0 .. 5 sets 0 to 16. 0 disables humidity sensing. See table 19.

        sensorBME280.setMode(MODE_SLEEP); // MODE_NORMAL

        int16_t backupX = DisplayHelper::display.getCursorX();
        int16_t backupY = DisplayHelper::display.getCursorY();
        for(int i=1; i<=100; i++) {
            updateDataBME280();
            if ((i % 5) == 0) {
                DisplayHelper::display.setCursor(backupX, backupY);
                DisplayHelper::display.print(i);
            }
            delay(20);
        }
        DisplayHelper::display.setCursor(backupX, backupY);
        DisplayHelper::printSuccess();
    } else {
        DisplayHelper::printFailure();
    }
}

void initCCS811() {
    DisplayHelper::printInformation("CCS811...   ");

    CCS811Core::status returnCode = sensorCCS811.begin();
    if (returnCode == CCS811Core::SENSOR_SUCCESS) {
        sensorCCS811connected = true;
    }

    if (sensorCCS811connected) {
        sensorCCS811.disableInterrupts();
        sensorCCS811.setDriveMode(1); // 1S

        DisplayHelper::printSuccess();
    } else {
        DisplayHelper::printFailure();
    }
}

void updateDataBME280() {
    if (sensorBME280connected) {
        sensorBME280.setMode(MODE_FORCED);
        while(sensorBME280.isMeasuring() == false) ; // Wait for sensor to start measurment
        while(sensorBME280.isMeasuring() == true) ; // Hang out while sensor completes the reading

        dataTemperature = sensorBME280.readTempC();
        dataDew = sensorBME280.dewPointC();
        dataHumidity = sensorBME280.readFloatHumidity();
        dataAbsoluteHumidity = WeatherCalculations::getAbsoluteHumidity(dataTemperature, dataHumidity);
        dataPressure = sensorBME280.readFloatPressure();

        sensorBME280.setMode(MODE_SLEEP);
    }
}

void updateDataCCS811() {
    if (sensorCCS811connected) {
        if (sensorCCS811.dataAvailable()) {
            sensorCCS811.readAlgorithmResults();

            dataCO2 = sensorCCS811.getCO2();
            dataTVOC = sensorCCS811.getTVOC();

            if (sensorBME280connected) {
                sensorCCS811.setEnvironmentalData(dataHumidity, dataTemperature);
            }
        }
    }
}

void updateScreen() {
    if (!sensorBME280connected and !sensorCCS811connected) {
        return;
    }

    if (millis() - DisplayHelper::lastScreenUpdate < DISPLAY_DELAY) {
        return;
    }
    char charBuffer[16];

    // Temperature
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 0, DISPLAY_INITIAL_OFFSET_Y + 0);
    DisplayHelper::display.setTextColor(DisplayHelper::mapTemperatureColor(dataTemperature), ST7735_BLACK);
    DisplayHelper::display.setTextSize(3);
    dtostrf(dataTemperature, 5, 2, charBuffer);
    DisplayHelper::display.print(charBuffer);
    DisplayHelper::display.print((char)247);

    // Temperature Dew
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 120, DISPLAY_INITIAL_OFFSET_Y + 2);
    DisplayHelper::display.setTextColor(DisplayHelper::mapDewPointColor(dataDew), ST7735_BLACK);
    DisplayHelper::display.setTextSize(1);
    dtostrf(dataDew, 5, 2, charBuffer);
    DisplayHelper::display.print(charBuffer);
    DisplayHelper::display.print((char)247);

    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 136, DISPLAY_INITIAL_OFFSET_Y + 12);
    DisplayHelper::display.print("DEW");

    // Humidity
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 0, DISPLAY_INITIAL_OFFSET_Y + 26);
    DisplayHelper::display.setTextColor(DisplayHelper::mapHumidityColor(dataHumidity), ST7735_BLACK);
    DisplayHelper::display.setTextSize(3);
    dtostrf(dataHumidity, 5, 2, charBuffer);
    DisplayHelper::display.print(charBuffer);
    DisplayHelper::display.print("%");

    // Absolute Humidity
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 120, DISPLAY_INITIAL_OFFSET_Y + 28);
    DisplayHelper::display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    DisplayHelper::display.setTextSize(1);
    dtostrf(dataAbsoluteHumidity, 5, 2, charBuffer);
    DisplayHelper::display.print(charBuffer);
    DisplayHelper::display.print("%");

    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 136, DISPLAY_INITIAL_OFFSET_Y + 38);
    DisplayHelper::display.print("AH");

    // Presure
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 0, DISPLAY_INITIAL_OFFSET_Y + 52);
    DisplayHelper::display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    DisplayHelper::display.setTextSize(3);
    dtostrf(dataPressure / 100.0F, 6, 1, charBuffer);
    DisplayHelper::display.print(charBuffer);

    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 110, DISPLAY_INITIAL_OFFSET_Y + 59);
    DisplayHelper::display.setTextSize(2);
    DisplayHelper::display.print("hPa");

    // CO2 ppm
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 0, DISPLAY_INITIAL_OFFSET_Y + 78);
    DisplayHelper::display.setTextColor(DisplayHelper::mapCO2Color(dataCO2), ST7735_BLACK);
    DisplayHelper::display.setTextSize(3);
    dtostrf(dataCO2, 5, 0, charBuffer);
    DisplayHelper::display.print(charBuffer);

    DisplayHelper::display.setTextSize(1);
    DisplayHelper::display.print("ppm");

    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 110, DISPLAY_INITIAL_OFFSET_Y + 85);
    DisplayHelper::display.setTextSize(2);
    DisplayHelper::display.print("CO2");

    // TVOC ppb
    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 0, DISPLAY_INITIAL_OFFSET_Y + 104);
    DisplayHelper::display.setTextColor(DisplayHelper::mapTVOCColor(dataTVOC), ST7735_BLACK);
    DisplayHelper::display.setTextSize(3);
    dtostrf(dataTVOC, 5, 0, charBuffer);
    DisplayHelper::display.print(charBuffer);

    DisplayHelper::display.setTextSize(1);
    DisplayHelper::display.print("ppb");

    DisplayHelper::display.setCursor(DISPLAY_INITIAL_OFFSET_X + 110, DISPLAY_INITIAL_OFFSET_Y + 111);
    DisplayHelper::display.setTextSize(2);
    DisplayHelper::display.print("TVOC");

    DisplayHelper::lastScreenUpdate = millis();
}
