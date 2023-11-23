#include "mbed.h"
#include "BrightnessControl.h"
#include "CalculateEveryHour.h"

extern AnalogIn lightSensor;
extern std::vector<float> lightData;

float getBrightness() {
    
    return lightSensor.read() * 100 ; 
}

void collectLightData(float light) {
    lightData.push_back(light);
}