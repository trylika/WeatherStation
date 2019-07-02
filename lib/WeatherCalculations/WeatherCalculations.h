#ifndef WeatherCalculations_h
#define WeatherCalculations_h

#include "Arduino.h"
#include "math.h"
#include "Adafruit_ST7735.h"

#define hi_coeff1 -42.379
#define hi_coeff2   2.04901523
#define hi_coeff3  10.14333127
#define hi_coeff4  -0.22475541
#define hi_coeff5  -0.00683783
#define hi_coeff6  -0.05481717
#define hi_coeff7   0.00122874
#define hi_coeff8   0.00085282
#define hi_coeff9  -0.00000199

namespace WeatherCalculations {
    float getHeatIndex(float temperature, float humidity);
    float getAbsoluteHumidity(float temperature, float humidity);
    float getDewPoint(float temperature, float humidity);
    uint16_t mapDewPointColor(float temperature);
    uint16_t mapTemperatureColor(float temperature);
}

#endif
