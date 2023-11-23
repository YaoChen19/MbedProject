#ifndef CALCULATE_HOUR_H
#define CALCULATE_HOUR_H

#include <vector>


struct Stats {
    float average;
    float max;
    float min;
};


Stats calculateStats(const std::vector<float>& data);
const char* CalculateColorHour(const std::vector<const char*>& data) ;

#endif