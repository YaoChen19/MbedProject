#include "Accelerometer.h"
#include <cstdint>



MMA8451Q::MMA8451Q(PinName sda, PinName scl, int addr) : acc_i2c(sda, scl), acc_addr(addr) {
    // activate the peripheral
    uint8_t data[2];
    data[0] = REG_CTRL_REG_1;
    data[1] = 0x01;
    acc_i2c.write(acc_addr,(char *)data, 2);
}



int16_t MMA8451Q::concatAxis(int16_t reg0, int16_t reg1) {
    int16_t value = (reg0 << 6) | (reg1 >> 2);
    //printf("Value: %x\n",value);
    if (value > (UINT14_MAX / 2)){
        value -= UINT14_MAX;}
    return value;
}
void MMA8451Q::getAccAxis(float *returnValue) {
    int16_t acc;
    uint8_t res[6];
    
    
    char t[1] = {REG_OUT_X_MSB};
    acc_i2c.write(acc_addr, t, 1, true);
    acc_i2c.read(acc_addr, (char *)res, 6);

    returnValue[0] = float(concatAxis(res[0], res[1])) / 4096.0*9.81;
    returnValue[1] = float(concatAxis(res[2], res[3])) / 4096.0 *9.81;
    returnValue[2] = float(concatAxis(res[4], res[5])) / 4069.0 *9.81;
     acc = (res[0] << 6) | (res[1] >> 2);
     if (acc > UINT14_MAX/2)
         acc -= UINT14_MAX;
        
     acc = float(acc)/4096.0;
     
}




