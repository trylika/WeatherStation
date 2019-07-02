#include <main.h>

void setup() {
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.println();
  tft.println("Loading");
  tft.println();

  initBME280();

  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.println();  
  tft.println("Done");
  tft.println();
  
  delay(1000);
  if (sensorBME280connected) {
    tft.fillScreen(ST7735_BLACK);  
  }
}

void loop() {
  readData();
  printData();
  delay(1000);
}

void initBME280() {
  sensorBME280.setI2CAddress(0x76);
  sensorBME280connected = sensorBME280.beginI2C();

  tft.print("BME280...   ");

  if (sensorBME280connected) {
    sensorBME280.setFilter(2); //0 to 4 is valid. Filter coefficient. See 3.4.4
    sensorBME280.setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.

    sensorBME280.setTempOverSample(5); //0 .. 5 sets 0 to 16. 0 disables temp sensing. See table 24.
    sensorBME280.setPressureOverSample(5); //0 .. 5 sets 0 to 16. 0 disables pressure sensing. See table 23.
    sensorBME280.setHumidityOverSample(5); //0 .. 5 sets 0 to 16. 0 disables humidity sensing. See table 19.    
   
    sensorBME280.setMode(MODE_SLEEP); // MODE_NORMAL

    int16_t backupX = tft.getCursorX();
    int16_t backupY = tft.getCursorY();
    for(int i=1; i<=100; i++){
      readData();
      if ((i % 5) == 0) {
          tft.setCursor(backupX, backupY);
          tft.print(i);
      }
      delay(20);
    }
    tft.setCursor(backupX, backupY);
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.println("[OK]");    
  } else {
    tft.setTextColor(ST7735_RED, ST7735_BLACK);
    tft.println("[Error]");
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
  tft.setTextSize(2);

  

  dtostrf(dataTemperature, 5, 2, tempString);
  sprintf(dataBuf, "%s%cC", tempString, 247);
  
  tft.setCursor(4, 8);
  tft.setTextColor(WeatherCalculations::mapTemperatureColor(dataTemperature), ST7735_BLACK);
  tft.print(dataBuf);



  float heatIndex = WeatherCalculations::getHeatIndex(dataTemperature, dataHumidity);
  dtostrf(heatIndex, 5, 2, tempString);
  sprintf(dataBuf, "%s%cC    HI", tempString, 247);
  
  tft.setCursor(4, 32);
  tft.setTextColor(WeatherCalculations::mapTemperatureColor(heatIndex), ST7735_BLACK);
  tft.print(dataBuf);



  float dewPoint = WeatherCalculations::getDewPoint(dataTemperature, dataHumidity);
  dtostrf(dewPoint, 5, 2, tempString);
  sprintf(dataBuf, "%s%cC   DEW", tempString, 247);
  
  tft.setCursor(4, 56);
  tft.setTextColor(WeatherCalculations::mapDewPointColor(dewPoint), ST7735_BLACK);
  tft.print(dataBuf); 

  

  float absoluteHumidity = WeatherCalculations::getAbsoluteHumidity(dataTemperature, dataHumidity);
  dtostrf(dataHumidity, 5, 2, tempString);
  dtostrf(absoluteHumidity, 2, 0, tempStringTwo);
  sprintf(dataBuf, "%s%% %s%%  H", tempString, tempStringTwo);
  
  tft.setCursor(4, 80);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.print(dataBuf);



  dtostrf(dataPressure / 100.0F, 7, 2, tempString);
  sprintf(dataBuf, "%s   hPa", tempString);  
  
  tft.setCursor(4, 104);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.print(dataBuf);
}
