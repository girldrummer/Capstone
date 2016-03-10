#include "alert.h"

using namespace Alert;

Warning::Warning(time_t detect_time, int angle, std::map<int, int> sensor_readings,
                 std::vector<int> triggered_sensors) : detect_time(detect_time), angle(angle*M_PI/180.0), sensor_readings(sensor_readings), triggered_sensors(triggered_sensors)
{}

void Warning::set(time_t detect_time, std::map<int, int> sensor_readings, std::vector<int> triggered_sensors)
{
    this->detect_time = detect_time;
    this->sensor_readings = sensor_readings;
    this->triggered_sensors = triggered_sensors;
}

double Warning::calculate_v_distance(int sensor)
{
    if (sensor != hip) {
        return -1;
    }
    else
    {
        int distance = sensor_readings[sensor];
        double vertical_dist = (double) (sin(angle) * distance);
        return vertical_dist;
    }
    
}

double Warning::calculate_h_distance(int sensor)
{
    int distance = sensor_readings[sensor];
    double horizontal_dist = (double) (sin(angle) * distance);
    return horizontal_dist;
}

void Warning::add_triggered_sensor(int sensor)
{
    if(std::find(triggered_sensors.begin(), triggered_sensors.end(), sensor) == triggered_sensors.end())
    {
        triggered_sensors.push_back(sensor);
    }
}

void Warning::remove_triggered_sensor(int sensor)
{
    for(std::vector<int>::iterator i = triggered_sensors.begin(); i < triggered_sensors.end(); i++)
    {
        if(*i == sensor)
        {
            triggered_sensors.erase(i);
        }
    }
}

void Warning::print_warning()
{
    std::cout << "**Warning**\n" << "Time: " << detect_time << std::endl;
    std::cout << "Sensor:        Distance:" << std::endl;
    
    for(std::map<int, int>::iterator it = sensor_readings.begin(); it != sensor_readings.end(); it++)
    {
        switch(it->first)
        {
            case 0: // head left
                std::cout << "Head Left        " << it->second << std::endl;
                break;
            case 1: // head right
                std::cout << "Head Right       " << it->second << std::endl;
                break;
            case 2: // hip
                std::cout << "Hip              " << it->second << std::endl;
                break;
            default:
                break;
                
        }
    }
    std::cout << std::endl;
}

time_t Warning::get_time()
{
    return detect_time;
}

int Warning::get_distance(int sensor)
{
    return sensor_readings[sensor];
}

std::vector<int> Warning::get_triggered_sensors()
{
    return triggered_sensors;
}