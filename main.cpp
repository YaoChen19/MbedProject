#include "mbed.h"
#include "TemHumSensor.h"
#include "BrightnessControl.h"
#include "RGBSensor.h"
#include "SoilMoistureSensor.h"
#include "Accelerometer.h"
#include "CalculateEveryHour.h"
#include <cstdio>
#include <string>
#include <queue>
#include "GPS.h"

InterruptIn userButton(PB_2);
DigitalOut led1(PB_5);
DigitalOut led2(PA_5);
DigitalOut led3(PB_6);
AnalogIn lightSensor(A0);
AnalogIn SoilMoistureSensor(A2);
DigitalOut redLED(PA_4); //Luz Rojo
DigitalOut greenLED(PH_1); //Luz Verde
DigitalOut blueLED(PH_0); //Luz Azul
BufferedSerial pc(USBTX, USBRX); //Crear un objeto BufferedSerial para realizar comunicación serial a través de los pines USBTX(Enviar) y USBRX (Recibir).
// I2C接口初始化
I2C i2cTemHum(PB_14, PB_13);  // SDA, SCL
I2C i2cRGB(PB_7, PA_9);
MMA8451Q acc(PB_14, PB_13, MMA8451_I2C_ADDRESS);
GPS gps1(PA_9, NC,9600);

Timer thirtySecondTimer; 
Timer hourlyTimer;


int  length;

enum Mode {
    TEST_MODE,
    NORMAL_MODE,
    ADVANCED_MODE
};

enum AlarmType {
    HUMIDITY_ALARM,
    TEMPERATURE_ALARM,
    ACCELEROMETER_ALARM,
    COLOUR_ALARM,
    SOIL_MOISTURE_ALARM,
    BRIGHTNESS_ALARM
};
std::queue<AlarmType> alarmQueue;


std::vector<float> temperatureData;
std::vector<float> humidityData;
std::vector<float> lightData;
std::vector<float> soilMoistureData;
std::vector<const char*> domainColorData;
std::vector<float> xAccelData;
std::vector<float> yAccelData;
std::vector<float> zAccelData;

const float TEMP_MIN = -10.0; // Minimum temperature in °C
const float TEMP_MAX = 10.0;  // Maximum temperature in °C

const float HUMIDITY_MIN = 25.0; // Minimum humidity in %
const float HUMIDITY_MAX = 25.0;  // Maximum humidity in %

const float LIGHT_MIN = 0.0;   // Minimum light in %
const float LIGHT_MAX = 0.0; // Maximum light in %

const float MOISTURE_MIN = 0.0;   // Minimum soil moisture in %
const float MOISTURE_MAX = 0.0; // Maximum soil moisture in %

const float ACCELEROMETER_MIN = -19.62; 
const float ACCELEROMETER_MAX = -19.62; 

const uint16_t COLOUR_MIN = 0;   
const uint16_t COLOUR_MAX = 0; 

volatile Mode currentMode = TEST_MODE;

volatile bool modeChanged = false;

void toggleMode() {
    if (currentMode == TEST_MODE) {
        currentMode = NORMAL_MODE;
         
        led1 = 0; 
        led2 = 1;
        led3 = 0;
    } else if (currentMode == NORMAL_MODE) {
        currentMode= ADVANCED_MODE;
        
        led1 = 0;
        led2 = 0;
        led3 = 1;
    } else if (currentMode == ADVANCED_MODE) {
        currentMode = TEST_MODE;
         
        led1 = 1;
        led2 = 0;
        led3 = 0;
    }
    modeChanged = true;
     
}


void readAndSendData() {
  
    float temperature = getTemperature();
    float humidity = getHumidity();
    collectTemperatureData(temperature);
    collectHumidityData(humidity);
    float brightness = getBrightness();
    collectLightData(brightness);
    float moisture = getSoilMoisture();
    collectMoistureData(moisture);
    uint16_t clear, red, green, blue;
    readRGB(clear, red, green, blue);
    float values[3];
    acc.getAccAxis(values);
    xAccelData.push_back(values[0]);
    yAccelData.push_back(values[1]);
    zAccelData.push_back(values[2]);
    const char* domainColor = getDomainColor(red, green, blue);
    collectColorData(domainColor);
    
    gps1.sample();
 
    float altitude = gps1.alt;
    float longitude = gps1.longitude;
    float latitude = gps1.latitude;
    char latDirection = gps1.ns;
    char lonDirection = gps1.ew;
    char altUnit = gps1.unit;
    int hour = gps1.hour;
    int min = gps1.minute;
    int seg = gps1.seconed;
    int sats = gps1.sats;
    
   
        if (currentMode == TEST_MODE) {
            if(strcmp(domainColor, "Red") == 0){
                redLED = 0; greenLED = 1; blueLED = 1;  
            }else if(strcmp(domainColor, "Blue") ==0){               
                 redLED = 1; greenLED = 1; blueLED = 0; 
            }else{
                redLED = 1; greenLED = 0; blueLED = 1; 
            }
                  char buffer[500];
    length = snprintf(buffer, sizeof(buffer), 
                         "\n----------TEST MODE-----------\n"
                          "Sensor Data:\nTemperature: %.1f C, Humidity: %.1f %%\n"
                          "Light: %.1f %%\n"
                          "Clear: %u, Red: %u, Green: %u, Blue: %u, DomainColor: %s\n"
                          "Soil Moisture: %.1f %%\n"
                          "Accelerometers: X_axis: %.2f m/s², Y_axis: %.2f m/s², Z_axis: %.2f m/s²\n"
                          "GPS Data: Sat Number:%d, Lat(UTC): %.6f %c, Long(UTC): %.6f %c, Altitude: %.2f %c, Current Time: %02d:%02d:%02d\n\n",
                          temperature, humidity,
                        brightness,
                         clear, red, green, blue, domainColor,
                         moisture,
                         values[0], values[1],values[2],
                         sats,latitude,latDirection,longitude,lonDirection,altitude,altUnit,hour, min, seg);



if (length > 0 && length < sizeof(buffer)) {
    pc.write(buffer, length); // 发送 buffer 到 PC
} 
        }
      if (currentMode == NORMAL_MODE) {
        // 检查温度是否超出限制
            if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
            // 为温度设置RGB LED颜色
            redLED = 0; greenLED = 1; blueLED = 1; // 示例：红色表示温度超限
               printf("\nWarning: Temperature out of bounds!\n");
        } 
        // 检查湿度是否超出限制
            else if (humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) {
            // 为湿度设置RGB LED颜色
            redLED = 0; greenLED = 0; blueLED = 1; // 黄色
               printf("\nWarning: Humidity out of bounds!\n");
        } 
            else if (brightness < LIGHT_MIN || brightness > LIGHT_MAX){
            redLED = 1; greenLED = 1; blueLED = 0; // 示例：蓝色
              printf("\nWarning: Light intensity out of bounds!\n");
            }else if (moisture < MOISTURE_MIN || moisture > MOISTURE_MAX) {
            redLED = 1; greenLED = 0; blueLED = 0; // 示例：Cian
             printf("\nWarning: Soil moisture out of bounds!\n");
             }else if(clear < COLOUR_MIN || clear > COLOUR_MAX || red < COLOUR_MIN || red > COLOUR_MAX ||
                      blue < COLOUR_MIN || blue > COLOUR_MAX || green < COLOUR_MIN || green > COLOUR_MAX){
               redLED = 1; greenLED = 0; blueLED = 1; // 示例：绿    
             printf("\nWarning: Colour out of bounds!\n");   
             }else if( values[0]< ACCELEROMETER_MIN ||values[1] < ACCELEROMETER_MIN || values[2] < ACCELEROMETER_MIN ||
                      values[0] > ACCELEROMETER_MAX || values[1] > ACCELEROMETER_MAX || values[2]> ACCELEROMETER_MAX){
                         redLED = 0; greenLED = 1; blueLED = 0; //紫色
                   printf("\nWarning: Acceleration out of bounds!\n");          
                 }else{
             redLED = 1; greenLED = 1; blueLED = 1; //不亮
             printf("\nAll parameters within normal ranges.\n");
     }
           char buffer[500];
    length = snprintf(buffer, sizeof(buffer), 
                        "\n----------NORMAL MODE-----------\n"
                          "Sensor Data:\nTemperature: %.1f C, Humidity: %.1f %%\n"
                          "Light: %.1f %%\n"
                          "Clear: %u, Red: %u, Green: %u, Blue: %u, DomainColor: %s\n"
                          "Soil Moisture: %.1f %%\n"
                          "Accelerometers: X_axis: %.2f m/s², Y_axis: %.2f m/s², Z_axis: %.2f m/s²\n"
                          "GPS Data: Sat Number:%d, Lat(UTC): %.6f %c, Long(UTC): %.6f %c, Altitude: %.2f %c, Current Time: %02d:%02d:%02d\n\n",
                          temperature, humidity,
                        brightness,
                         clear, red, green, blue, domainColor,
                         moisture,
                         values[0], values[1],values[2],
                         sats,latitude,latDirection,longitude,lonDirection,altitude,altUnit,hour, min, seg);



if (length > 0 && length < sizeof(buffer)) {
    pc.write(buffer, length); // 发送 buffer 到 PC
} 
    }
    if (currentMode == ADVANCED_MODE) {
        
        if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
            alarmQueue.push(TEMPERATURE_ALARM);
    }   
        if (humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) {
        alarmQueue.push(HUMIDITY_ALARM);
    }
         if (brightness < LIGHT_MIN || brightness > LIGHT_MAX) {
        alarmQueue.push(BRIGHTNESS_ALARM);
    }   
        if (moisture < MOISTURE_MIN || moisture > MOISTURE_MAX) {
        alarmQueue.push(SOIL_MOISTURE_ALARM);
    }   
         if (clear < COLOUR_MIN || clear > COLOUR_MAX ||
        red < COLOUR_MIN || red > COLOUR_MAX ||
        green < COLOUR_MIN || green > COLOUR_MAX ||
        blue < COLOUR_MIN || blue > COLOUR_MAX) {
        alarmQueue.push(COLOUR_ALARM);
    }
         if (values[0] < ACCELEROMETER_MIN || values[0] > ACCELEROMETER_MAX ||
        values[1] < ACCELEROMETER_MIN || values[1] > ACCELEROMETER_MAX ||
        values[2]< ACCELEROMETER_MIN || values[2] > ACCELEROMETER_MAX) {
        alarmQueue.push(ACCELEROMETER_ALARM);
    }
     static Timer alarmDisplayTimer;
     static int alarmDisplayIndex = 0;

      if (alarmQueue.empty()) {
        // 没有报警，保持RGB LED关闭
        redLED = 1;
        greenLED = 1;
        blueLED = 1;
         alarmDisplayTimer.reset();  // 重置计时器
         alarmDisplayIndex = 0;     // 重置状态
    } else {
        // 如果计时器还没开始，就启动它
        if ( alarmDisplayIndex == 0) {
            alarmDisplayTimer.start();
        }
        // 计算已过时间和当前应显示的报警索引
         int timeElapsed = alarmDisplayTimer.elapsed_time().count();
        

        if (timeElapsed / 500  > alarmDisplayIndex) {
            if (alarmDisplayIndex < alarmQueue.size() * 2) { 
            // 显示下一个报警颜色
            switch (alarmQueue.front()) {
               case HUMIDITY_ALARM:
        redLED = 0; greenLED = 0; blueLED = 1; // 黄色
               break;
                  case TEMPERATURE_ALARM:
         redLED = 0; greenLED = 1; blueLED = 1; 
                 break;
                case ACCELEROMETER_ALARM:
                redLED = 0; greenLED = 1; blueLED = 0;
                  break;
                 case COLOUR_ALARM:
         redLED = 1; greenLED = 0; blueLED = 1; // 示例：绿 
                  break;
              case SOIL_MOISTURE_ALARM:
         redLED = 1; greenLED = 0; blueLED = 0; // 示例：Cian
                 break;
             case BRIGHTNESS_ALARM:
            redLED = 1; greenLED = 1; blueLED = 0; // 示例：蓝色
                  break;

            }

        alarmQueue.push(alarmQueue.front()); // 把当前报警移动到队列末尾
        alarmQueue.pop(); // 移除当前显示的报警
        } else {
                // 在剩余的时间里保持LED关闭
                redLED = 1;
                greenLED = 1;
                blueLED = 1;
            } alarmDisplayIndex++;
    }   
    if (timeElapsed >= 3000) {
            alarmDisplayTimer.reset();  // 重置计时器
            alarmDisplayIndex = 0;      // 重置索引
            redLED = 1;
            greenLED = 1;
            blueLED = 1;
        }
    }
    }



    

}

int main() {
     userButton.fall(&toggleMode);
    initializeTCS34725();

        // 设置初始 LED 状态
    led1 = 1; // 默认亮起 led1
    led2 = 0;
    led3 = 0;
   
    while (true) {
     
    
         if (modeChanged) {
            readAndSendData();
            thirtySecondTimer.reset();
            modeChanged = false;
        }

        if(currentMode == TEST_MODE){
            
            readAndSendData();
            ThisThread::sleep_for(2s); }

        if (currentMode == NORMAL_MODE) {
           
            thirtySecondTimer.start();
            hourlyTimer.start();
            if (thirtySecondTimer.elapsed_time() >= 30s) {
             readAndSendData();
             thirtySecondTimer.reset();
       
             }
            if (hourlyTimer.elapsed_time() >= 90s) {
                // 每小时进行统计计算
                Stats temperatureStats = calculateStats(temperatureData);
                Stats humidityStats = calculateStats(humidityData);
                Stats lightStats =calculateStats(lightData);
                Stats soilMoistureStats = calculateStats(soilMoistureData);
                const char* domainColorHour = CalculateColorHour(domainColorData);
                Stats xStats = calculateStats(xAccelData);
                Stats yStats = calculateStats(yAccelData);
                Stats zStats = calculateStats(zAccelData);
                // 发送统计数据
                char statsBuffer[500];
                int statsLength = snprintf(statsBuffer, sizeof(statsBuffer),
                                           "\nHourly Stats:\nTemperature - Avg: %.2f, Max: %.2f, Min: %.2f\n"
                                           "Humidity - Avg: %.2f, Max: %.2f, Min: %.2f\n"
                                           "Ambiente light - Avg: %.1f %%, Max: %.1f %%, Min: %.1f %%\n"
                                           "Soil Moisture - Avg: %.1f %%, Max: %.1f %%, Min: %.1f %%\n"
                                           "Colour that has appeared as dominant more times during the last hour: %s\n"
                                           "Accelerometer -\n"
                                           "X - Max: %.2f m/s², Min: %.2f m/s²\n"
                                           "Y - Max: %.2f m/s², Min: %.2f m/s²\n"
                                           "Z - Max: %.2f m/s², Min: %.2f m/s²\n",
                                           
                                           temperatureStats.average, temperatureStats.max, temperatureStats.min,
                                           humidityStats.average, humidityStats.max, humidityStats.min,
                                           lightStats.average, lightStats.max, lightStats.min,
                                           soilMoistureStats.average, soilMoistureStats.max, soilMoistureStats.min, domainColorHour,
                                           xStats.max, xStats.min,
                                           yStats.max, yStats.min,
                                           zStats.max, zStats.min);

                if (statsLength > 0 && statsLength < sizeof(statsBuffer)) {
                    pc.write(statsBuffer, statsLength);
                }

                // 重置小时定时器和数据向量
                hourlyTimer.reset();
                temperatureData.clear();
                humidityData.clear();
                lightData.clear();
                soilMoistureData.clear();
                domainColorData.clear();
                xAccelData.clear();
                yAccelData.clear();
                zAccelData.clear();
            }

}
    if(currentMode==ADVANCED_MODE){
        
        readAndSendData();
    

    }
      
    }

    }