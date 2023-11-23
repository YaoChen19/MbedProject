
#include "mbed.h"

#ifndef MBED_GPS_H
#define MBED_GPS_H

class GPS {
public:

 
    GPS(PinName tx, PinName rx, int Baud);
    
    
    int sample();
    

    int hour;
    int minute;
    int seconed;
    
    float longitude;

    char ns, ew, unit;

    float latitude;

    float time;
    
    int sats;
    
  
    float hdop;
    

    
    float alt;
    

    float  geoid;
    
    /* The NMEA sentence */
    char msg[256];
    
    
private:
    float trunc(float v);
    void getline();
    
    //create a serial object 
    BufferedSerial _gps;
};

#endif