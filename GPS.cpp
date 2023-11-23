#include "GPS.h"

GPS::GPS(PinName tx, PinName rx, int Baud) : _gps(tx, rx)
{
   
                time = 0.0;
                longitude = 0.0;
                latitude = 0.0;
                sats = 0;
                alt = 0.0;



}

int GPS::sample()
{
  
    int lock;

     while(1) {
       char ns, ew, unit;
        getline();

        // Check if it is a GPGGA msg 
        if(sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c,%f", &time, &latitude, &ns, &longitude, &ew, &lock, &sats, &hdop, &alt, &unit, &geoid) >= 1) {
            if(!lock) {
                time = 0.0;
                longitude = 0.0;
                latitude = 0.0;
                sats = 0;
                alt = 0.0;
                return 0;
            } else {
                time = time +10000.00;
                hour = int(time) / 10000;
                minute = (int(time) % 10000) / 100;
                seconed = int(time) % 100;
                if(ns == 'S') {    latitude  *= -1.0; }
                if(ew == 'W') {    longitude *= -1.0; }
                float degrees = trunc(latitude / 100.0f);
                float minutes = latitude - (degrees * 100.0f);
                latitude = degrees + minutes / 60.0f;    
                degrees = trunc(longitude / 100.0f * 0.01f);
                minutes = longitude - (degrees * 100.0f);
                longitude = degrees + minutes / 60.0f;
                return 1;
            }
        }
    }

}
float GPS::trunc(float v) {
    if(v < 0.0) {
        v*= -1.0;
        v = floor(v);
        v*=-1.0;
    } else {
        v = floor(v);
    }
    return v;
}


            

void GPS::getline() {
    char c;
    while (true) {
        if (_gps.read(&c, 1) == 1) {  // 读取一个字符
            if (c == '$') {
                break;  // 如果字符是 '$'，则开始新行
            }
        }
    }

    for (int i = 0; i < 256; i++) {
        if (_gps.read(&c, 1) == 1) {  // 继续读取字符
            msg[i] = c;
            if (c == '\r') {
                msg[i] = 0;  // 如果字符是回车符，则结束行
                return;
            }
        } else {
            // 读取失败
            error("Error reading from GPS");
        }
    }
    error("Overflowed message limit");  // 超出消息限制
}