#ifndef alert_h
#define alert_h

//#include <Arduino.h>
#include <vector>
#include <time.h>
#include <cmath>
#include <iostream>
#include <map>

namespace Alert {
    
    enum Sensor {head_left, head_right, hip};
    enum Critical {low, medium, high};
    enum Warning_Type {low_obstacle, high_obstacle, drop_off, wall};
    
    class Warning{
        
    private:
        time_t detect_time;
        double angle;
        std::vector<int> triggered_sensors;
        std::map<int, int> sensor_readings;
        int critical_level;
        int warning_type;
        
    public:
        //Warning(time_t detect_time, int angle, std::map<int, int> sensor_readings);
        Warning(time_t detect_time, int angle, std::map<int, int> sensor_readings, std::vector<int> triggered_sensors);
        void set (time_t detect_time, std::map<int, int> sensor_readings, std::vector<int> triggered_sensors);
        void set (time_t detect_time, int angle, std::map<int, int> sensor_readings, std::vector<int> triggered_sensors);
        double calculate_v_distance(int sensor); // calculates vertical distance of a sensor in the warning
        double calculate_h_distance(int sensor); // calculates horizontal distance of a sensor in the warning
        void add_triggered_sensor(int sensor);
        void remove_triggered_sensor(int sensor);
        void print_warning();
        time_t get_time();
        int get_distance(int sensor);
        std::vector<int> get_triggered_sensors();
        std::map<int, int> get_sensor_readings();
    };
    
}


#endif