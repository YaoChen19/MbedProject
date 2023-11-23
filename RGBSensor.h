#ifndef RGBSENSOR_H
#define RGBSENSOR_H

#include "mbed.h"

extern I2C i2cRGB;

// TCS34725 I2C地址和寄存器定义
#define TCS34725_ADDRESS 0x29 // TCS34725的I2C地址
#define TCS34725_COMMAND_BIT 0x80 // 命令位，用于指示后续字节是寄存器地址

// TCS34725寄存器地址
#define TCS34725_ENABLE 0x00
#define TCS34725_ATIME 0x01
#define TCS34725_WTIME 0x03
#define TCS34725_CDATAL 0x14 // 清除通道数据低字节
#define TCS34725_CDATAH 0x15 // 清除通道数据高字节
#define TCS34725_RDATAL 0x16 // 红色通道数据低字节
#define TCS34725_RDATAH 0x17 // 红色通道数据高字节
#define TCS34725_GDATAL 0x18 // 绿色通道数据低字节
#define TCS34725_GDATAH 0x19 // 绿色通道数据高字节
#define TCS34725_BDATAL 0x1A // 蓝色通道数据低字节
#define TCS34725_BDATAH 0x1B // 蓝色通道数据高字节

// 初始化和读取函数
void initializeTCS34725();
void readRGB(uint16_t &clear, uint16_t &red, uint16_t &green, uint16_t &blue);
const char* getDomainColor( uint16_t &red, uint16_t &green, uint16_t &blue);
void collectColorData(const char* domainColor);
#endif