#include "mbed.h"
#include "TemHumSensor.h"
#include "CalculateEveryHour.h"



#define SI7021_ADDR          0x80  
#define MEASURE_HUMIDITY     0xF5  
#define MEASURE_TEMPERATURE  0xF3  

extern I2C i2cTemHum;
extern std::vector<float> temperatureData;
extern std::vector<float> humidityData;



float getTemperature() {
    char data[2];
    char cmd[1];
    cmd[0]= MEASURE_TEMPERATURE; 
    i2cTemHum.write(SI7021_ADDR, cmd, 1);  
    ThisThread::sleep_for(20ms);  
    i2cTemHum.read(SI7021_ADDR, data, 2);  
    
    
    int temp = (data[0] << 8) | data[1];
    return ((175.72 * temp) / 65536.0) - 46.85; //datashert page 22
}

float getHumidity() {
    char data[2];
    char cmd[1];
    cmd[0]= MEASURE_HUMIDITY; 
    i2cTemHum.write(SI7021_ADDR, cmd, 1);  
    ThisThread::sleep_for(20ms);  
    i2cTemHum.read(SI7021_ADDR, data, 2);  
    
    
    int humid = (data[0] << 8) | data[1];
    return ((125.0 * humid) / 65536.0) - 6.0; //datashell page 21
}

void collectTemperatureData(float temperature) {
    temperatureData.push_back(temperature); //放在Vector里
}

void collectHumidityData(float humidity) {
    humidityData.push_back(humidity);
}



