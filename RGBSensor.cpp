#include "RGBSensor.h"
#include "CalculateEveryHour.h"

extern DigitalOut redLED;
extern DigitalOut greenLED;
extern DigitalOut blueLED;

extern std::vector<const char*> domainColorData;

// 初始化TCS34725传感器
void initializeTCS34725() {
    char config[2];
    // 启用RGBC和振荡器
    config[0] = TCS34725_COMMAND_BIT | TCS34725_ENABLE;
    config[1] = 0x03; // 设置ENABLE寄存器，启动RGBC和振荡器
    i2cRGB.write(TCS34725_ADDRESS << 1, config, 2);
    ThisThread::sleep_for(3ms); // 在初始化后等待传感器稳定
}

// 读取RGB值
void readRGB(uint16_t &clear, uint16_t &red, uint16_t &green, uint16_t &blue) {
    char cmd = TCS34725_COMMAND_BIT | TCS34725_CDATAL;
    char data[8]; // 用于存储读取的所有数据值


    // 发送读取命令
    i2cRGB.write(TCS34725_ADDRESS << 1, &cmd, 1);
    ThisThread::sleep_for(20ms); 
    // 读取数据
    i2cRGB.read(TCS34725_ADDRESS << 1, data, 8);

    // 转换数据从8位到16位
    clear = (data[1] << 8) | data[0];
    red   = (data[3] << 8) | data[2];
    green = (data[5] << 8) | data[4];
    blue  = (data[7] << 8) | data[6];

}

const char* getDomainColor( uint16_t &red, uint16_t &green, uint16_t &blue){
    
        if ( red > green && red > blue) {
                return "Red";
            } else if (green > red && green > blue) {
                return "Green";
            } else {
                return "Blue";
            }

}

void collectColorData(const char* domainColor) {
   domainColorData.push_back(domainColor); //放在Vector里
}
