#include "mbed.h"
#include "CalculateEveryHour.h"

Stats calculateStats(const std::vector<float>& data) {
    Stats stats;
    if (data.empty()) {
        return stats; // 返回空的 Stats 结构体
    }

    float sum = 0;
    stats.max = data[0];
    stats.min = data[0];

    for (float value : data) {
        sum += value;
        if (value > stats.max) {
            stats.max = value;
        }
        if (value < stats.min) {
            stats.min = value;
        }
    }

    stats.average = sum / data.size();
    return stats;
}

const char* CalculateColorHour(const std::vector<const char*>& data) {
    if (data.empty()) {
        return "No data"; // 没有数据时的返回值
    }

    int countBlue = 0;
    int countGreen = 0;
    int countRed = 0;

    for (const char* value : data) {
        if (strcmp(value, "Blue") == 0) {
            countBlue++;
        } else if (strcmp(value, "Red") == 0) {
            countRed++;
        } else if (strcmp(value, "Green") == 0) {
            countGreen++;
        }
    }
        
    // 确定哪种颜色出现次数最多
    if (countBlue >= countRed && countBlue >= countGreen) {
        return "Blue";
    } else if (countRed >= countBlue && countRed >= countGreen) {
        return "Red";
    } else {
        return "Green";
    }
}