#include "WeatherCalculations.h"

namespace WeatherCalculations {
    float getHeatIndex(float temperature, float humidity) {
        // aka FeelsLike
        //FYI: https://ehp.niehs.nih.gov/1206273/ in detail this flow graph:
        // https://ehp.niehs.nih.gov/wp-content/uploads/2013/10/ehp.1206273.g003.png
        float heatIndex(NAN);

        if (isnan(temperature) || isnan(humidity)) {
            return heatIndex;
        }

        temperature = (temperature * (9.0 / 5.0) + 32.0); /*conversion to [°F]*/

        // Using both Rothfusz and Steadman's equations
        // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
        if (temperature <= 40) {
            heatIndex = temperature;  //first red block
        } else {
            //calculate A -- from the official site, not the flow graph
            heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (humidity * 0.094));

            if (heatIndex >= 79) {
                /*
                * calculate B
                * the following calculation is optimized. Simply spoken, reduzed cpu-operations to minimize used ram and runtime.
                * Check the correctness with the following link:
                * http://www.wolframalpha.com/input/?source=nav&i=b%3D+x1+%2B+x2*T+%2B+x3*H+%2B+x4*T*H+%2B+x5*T*T+%2B+x6*H*H+%2B+x7*T*T*H+%2B+x8*T*H*H+%2B+x9*T*T*H*H
                */
                heatIndex = hi_coeff1
                    + (hi_coeff2 + hi_coeff4 * humidity + temperature * (hi_coeff5 + hi_coeff7 * humidity)) * temperature
                    + (hi_coeff3 + humidity * (hi_coeff6 + temperature * (hi_coeff8 + hi_coeff9 * temperature))) * humidity;
                //third red block
                if ((humidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
                {
                    heatIndex -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);
                } //fourth red block
                else if ((humidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
                {
                    heatIndex += (0.02 * (humidity - 85.0) * (87.0 - temperature));
                }
            }
        }

        /*conversion back to [°C]*/
        return (heatIndex - 32.0) * (5.0 / 9.0);
    }

    float getAbsoluteHumidity(float temperature, float humidity) {
        //taken from https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
        //precision is about 0.1°C in range -30 to 35°C
        //August-Roche-Magnus   6.1094 exp(17.625 x T)/(T + 243.04)
        //Buck (1981)     6.1121 exp(17.502 x T)/(T + 240.97)
        //reference https://www.eas.ualberta.ca/jdwilson/EAS372_13/Vomel_CIRES_satvpformulae.html
        float temp = NAN;
        const float mw = 18.01534;  // molar mass of water g/mol
        const float r = 8.31447215;   // Universal gas constant J/mol/K

        if (isnan(temperature) || isnan(humidity)) {
            return NAN;
        }

        temp = pow(2.718281828, (17.67 * temperature) / (temperature + 243.5));

        return (6.112 * temp * humidity * mw) / ((273.15 + temperature) * r); //long version
    }
}
