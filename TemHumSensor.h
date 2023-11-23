#ifndef TEMGYMSENSOR_H
#define TEMGYMSENSOR_H


float getTemperature() ;  
    
float getHumidity() ;

void collectTemperatureData(float temperature);
void collectHumidityData(float humidity);

#endif // TEMGYMSENSOR_H