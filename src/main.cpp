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
        DisplayHelper::display.setTextSize(2);
    }
}

void loop() {
    updateDataBME280();
    updateDataCCS811();
    prepareData();
    updateScreen();
    delay(1000);
}

void initBME280() {
    DisplayHelper::display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    DisplayHelper::display.print("BME280...   ");

    sensorBME280.setI2CAddress(BME280_I2C);
    sensorBME280connected = sensorBME280.beginI2C();

    if (sensorBME280connected) {
        sensorBME280.setFilter(2); //0 to 4 is valid. Filter coefficient. See 3.4.4
        sensorBME280.setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.

        sensorBME280.setTempOverSample(5); //0 .. 5 sets 0 to 16. 0 disables temp sensing. See table 24.
        sensorBME280.setPressureOverSample(5); //0 .. 5 sets 0 to 16. 0 disables pressure sensing. See table 23.
        sensorBME280.setHumidityOverSample(5); //0 .. 5 sets 0 to 16. 0 disables humidity sensing. See table 19.

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
    DisplayHelper::display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    DisplayHelper::display.print("CCS811...   ");

    CCS811Core::status returnCode = sensorCCS811.begin();
    if (returnCode == CCS811Core::SENSOR_SUCCESS) {
        sensorCCS811connected = true;
    }

    if (sensorCCS811connected) {
        sensorCCS811.disableInterrupts();
        sensorCCS811.setDriveMode(1);

        DisplayHelper::printSuccess();
    } else {
        DisplayHelper::printFailure();
    }
}

void updateDataBME280() {
    if (sensorBME280connected) {
        sensorBME280.setMode(MODE_FORCED);
        while(sensorBME280.isMeasuring() == false) ; //Wait for sensor to start measurment
        while(sensorBME280.isMeasuring() == true) ; //Hang out while sensor completes the reading

        dataTemperature = sensorBME280.readTempC();
        dataHumidity = sensorBME280.readFloatHumidity();
        dataPressure = sensorBME280.readFloatPressure();
        dataDew = sensorBME280.dewPointC();

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

void prepareData() {
    if (sensorBME280connected) {
        dtostrf(dataTemperature, 5, 2, DisplayHelper::displayBuffer[0]);
        sprintf(DisplayHelper::displayBuffer[0], "%s%cC", DisplayHelper::displayBuffer[0], 247);
        DisplayHelper::displayBufferColor[0] = DisplayHelper::mapTemperatureColor(dataTemperature);

        float heatIndex = WeatherCalculations::getHeatIndex(dataTemperature, dataHumidity);
        dtostrf(heatIndex, 5, 2, DisplayHelper::displayBuffer[1]);
        sprintf(DisplayHelper::displayBuffer[1], "%s%cC HI", DisplayHelper::displayBuffer[1], 247);
        DisplayHelper::displayBufferColor[1] = DisplayHelper::mapTemperatureColor(heatIndex);

        dtostrf(dataDew, 5, 2, DisplayHelper::displayBuffer[2]);
        sprintf(DisplayHelper::displayBuffer[2], "%s%cC DEW", DisplayHelper::displayBuffer[2], 247);
        DisplayHelper::displayBufferColor[2] = DisplayHelper::mapDewPointColor(dataDew);

        char tempString[3];
        float absoluteHumidity = WeatherCalculations::getAbsoluteHumidity(dataTemperature, dataHumidity);
        dtostrf(dataHumidity, 5, 2, DisplayHelper::displayBuffer[3]);
        dtostrf(absoluteHumidity, 2, 0, tempString);
        sprintf(DisplayHelper::displayBuffer[3], "%s%% %s%% H", DisplayHelper::displayBuffer[3], tempString);
        DisplayHelper::displayBufferColor[3] = ST7735_GREEN;

        dtostrf(dataPressure / 100.0F, 7, 2, DisplayHelper::displayBuffer[4]);
        sprintf(DisplayHelper::displayBuffer[4], "%s hPa", DisplayHelper::displayBuffer[4]);
        DisplayHelper::displayBufferColor[4] = ST7735_GREEN;
    }

    if (sensorCCS811connected) {
        sprintf(DisplayHelper::displayBuffer[5], "%d ppm CO2", dataCO2);
        DisplayHelper::displayBufferColor[5] = ST7735_GREEN;

        sprintf(DisplayHelper::displayBuffer[6], "%d ppb TVOC", dataTVOC);
        DisplayHelper::displayBufferColor[6] = ST7735_GREEN;
    }
}

void updateScreen() {
    if (!sensorBME280connected and !sensorCCS811connected) {
        return;
    }

    if (millis() - DisplayHelper::lastScreenUpdate < DISPLAY_DELAY) {
        return;
    }

    for (int i = 0; i < DISPLAY_LINES; i++) {
        DisplayHelper::display.setCursor(DISPLAY_OFFSET_X, (DISPLAY_OFFSET_Y * (i + 1)) + (i * 15));
        DisplayHelper::display.setTextColor(DisplayHelper::displayBufferColor[i], ST7735_BLACK);
        DisplayHelper::display.print(DisplayHelper::displayBuffer[i]);
    }

    DisplayHelper::lastScreenUpdate = millis();
}
