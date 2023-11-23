#include "mbed.h"
#include "SoilMoistureSensor.h"
#include "CalculateEveryHour.h"

extern AnalogIn SoilMoistureSensor;
extern std::vector<float> soilMoistureData;
float getSoilMoisture() {
    
    return SoilMoistureSensor.read() *100; 
}

void collectMoistureData(float soilMoisture) {
    soilMoistureData.push_back(soilMoisture);
}