#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "mbed.h"
#define MMA8451_I2C_ADDRESS 0x1d<<1

#define REG_CTRL_REG_1    0x2a
#define REG_OUT_X_MSB     0x01
#define REG_OUT_Y_MSB     0x03
#define REG_OUT_Z_MSB     0x05
#define REG_OUT_X_LSB     0x02
#define REG_OUT_Y_LSB     0x04
#define REG_OUT_Z_LSB     0x06

#define UINT14_MAX        16383

class MMA8451Q
{
public:

  MMA8451Q(PinName sda, PinName scl, int addr);

  int16_t concatAxis(int16_t reg0, int16_t reg1);
  void getAccAxis(float *returnValue);
private:
  I2C acc_i2c;
  int acc_addr;


};

#endif