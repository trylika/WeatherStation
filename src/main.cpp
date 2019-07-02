#include <main.h>

void setup() {
    Wire.begin();

    display.initR(INITR_BLACKTAB);
    display.setRotation(3);
    display.fillScreen(ST7735_BLACK);

    display.setCursor(0, 0);
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.println();
    display.println("Loading");
    display.println();

    initBME280();
    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    initCCS811();

    display.setTextColor(ST7735_WHITE, ST7735_BLACK);
    display.println();
    display.println("Done");
    display.println();

    delay(1000);
    if (sensorBME280connected || sensorCCS811connected) {
        display.fillScreen(ST7735_BLACK);
    }
}

void loop() {
    readData();
    printData();
    delay(1000);
}

void initBME280() {
    sensorBME280.setI2CAddress(BME280_I2C);
    sensorBME280connected = sensorBME280.beginI2C();

    display.print("BME280...   ");

    if (sensorBME280connected) {
        sensorBME280.setFilter(2); //0 to 4 is valid. Filter coefficient. See 3.4.4
        sensorBME280.setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.

        sensorBME280.setTempOverSample(5); //0 .. 5 sets 0 to 16. 0 disables temp sensing. See table 24.
        sensorBME280.setPressureOverSample(5); //0 .. 5 sets 0 to 16. 0 disables pressure sensing. See table 23.
        sensorBME280.setHumidityOverSample(5); //0 .. 5 sets 0 to 16. 0 disables humidity sensing. See table 19.

        sensorBME280.setMode(MODE_SLEEP); // MODE_NORMAL

        int16_t backupX = display.getCursorX();
        int16_t backupY = display.getCursorY();
        for(int i=1; i<=100; i++) {
            readData();
            if ((i % 5) == 0) {
                display.setCursor(backupX, backupY);
                display.print(i);
            }
            delay(20);
        }
        display.setCursor(backupX, backupY);
        display.setTextColor(ST7735_GREEN, ST7735_BLACK);
        display.println("[OK]");
    } else {
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.println("[Error]");
    }
}

void initCCS811() {
    CCS811Core::status returnCode = sensorCCS811.begin();
    if (returnCode == CCS811Core::SENSOR_SUCCESS) {
        sensorCCS811connected = true;
    }

    display.print("CCS811...   ");

    if (sensorCCS811connected) {
        display.setTextColor(ST7735_GREEN, ST7735_BLACK);
        display.println("[OK]");
    } else {
        display.setTextColor(ST7735_RED, ST7735_BLACK);
        display.println("[Error]");
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

        sensorBME280.setMode(MODE_SLEEP);
    }
}

void printData() {
    if (!sensorBME280connected) {
        return;
    }

    char dataBuf[16];
    char tempString[10];
    char tempStringTwo[10];
    display.setTextSize(2);



    dtostrf(dataTemperature, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC", tempString, 247);

    display.setCursor(4, 8);
    display.setTextColor(WeatherCalculations::mapTemperatureColor(dataTemperature), ST7735_BLACK);
    display.print(dataBuf);



    float heatIndex = WeatherCalculations::getHeatIndex(dataTemperature, dataHumidity);
    dtostrf(heatIndex, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC    HI", tempString, 247);

    display.setCursor(4, 32);
    display.setTextColor(WeatherCalculations::mapTemperatureColor(heatIndex), ST7735_BLACK);
    display.print(dataBuf);



    float dewPoint = WeatherCalculations::getDewPoint(dataTemperature, dataHumidity);
    dtostrf(dewPoint, 5, 2, tempString);
    sprintf(dataBuf, "%s%cC   DEW", tempString, 247);

    display.setCursor(4, 56);
    display.setTextColor(WeatherCalculations::mapDewPointColor(dewPoint), ST7735_BLACK);
    display.print(dataBuf);



    float absoluteHumidity = WeatherCalculations::getAbsoluteHumidity(dataTemperature, dataHumidity);
    dtostrf(dataHumidity, 5, 2, tempString);
    dtostrf(absoluteHumidity, 2, 0, tempStringTwo);
    sprintf(dataBuf, "%s%% %s%%  H", tempString, tempStringTwo);

    display.setCursor(4, 80);
    display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    display.print(dataBuf);



    dtostrf(dataPressure / 100.0F, 7, 2, tempString);
    sprintf(dataBuf, "%s   hPa", tempString);

    display.setCursor(4, 104);
    display.setTextColor(ST7735_GREEN, ST7735_BLACK);
    display.print(dataBuf);
}
