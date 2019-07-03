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
    readData();
    printData();
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
            readData();
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

void readData() {
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

void printData() {
    if (!sensorBME280connected and !sensorCCS811connected) {
        return;
    }

    char dataBuf[16];
    char tempString[10];
    char tempStringTwo[10];
    DisplayHelper::display.setTextSize(2);



    dtostrf(dataTemperature, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC", tempString, 247);

    DisplayHelper::display.setCursor(4, 8);
    DisplayHelper::display.setTextColor(DisplayHelper::mapTemperatureColor(dataTemperature), ST7735_BLACK);
    DisplayHelper::display.print(dataBuf);



    float heatIndex = WeatherCalculations::getHeatIndex(dataTemperature, dataHumidity);
    dtostrf(heatIndex, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC    HI", tempString, 247);

    DisplayHelper::display.setCursor(4, 32);
    DisplayHelper::display.setTextColor(DisplayHelper::mapTemperatureColor(heatIndex), ST7735_BLACK);
    DisplayHelper::display.print(dataBuf);



    dtostrf(dataDew, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC   DEW", tempString, 247);

    DisplayHelper::display.setCursor(4, 56);
    DisplayHelper::display.setTextColor(DisplayHelper::mapDewPointColor(dataDew), ST7735_BLACK);
    DisplayHelper::display.print(dataBuf);



    float absoluteHumidity = WeatherCalculations::getAbsoluteHumidity(dataTemperature, dataHumidity);
    dtostrf(dataHumidity, 5, 2, tempString);
    dtostrf(absoluteHumidity, 2, 0, tempStringTwo);
    sprintf(dataBuf, "%s%% %s%%  H", tempString, tempStringTwo);

    DisplayHelper::display.setCursor(4, 80);
    DisplayHelper::display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    DisplayHelper::display.print(dataBuf);


  //getCO2() gets the previously read data from the library
//   Serial.println("CCS811 data:");
//   Serial.print(" CO2 concentration : ");
//   Serial.print(myCCS811.getCO2());
//   Serial.println(" ppm");

//   //getTVOC() gets the previously read data from the library
//   Serial.print(" TVOC concentration : ");
//   Serial.print(myCCS811.getTVOC());
// Serial.println(" ppb");


//   Serial.print((myBME280.readFloatPressure() * 0.0002953), 2);
// Serial.println(" InHg");


    dtostrf(dataPressure / 100.0F, 7, 2, tempString);
    sprintf(dataBuf, "%s   hPa", tempString);

    DisplayHelper::display.setCursor(4, 104);
    DisplayHelper::display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    DisplayHelper::display.print(dataBuf);
}
