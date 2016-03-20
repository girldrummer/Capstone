#include "alert_simple.h"

using namespace Alert;

Warning::Warning() : loops_since_detection(0), updates(0), angle(0), sensor_readings(NULL), triggered_sensors(NULL)
{}

Warning::Warning(int angle, int* sensor_readings,
                 bool* triggered_sensors, bool drop) : loops_since_detection(0), updates(0), angle(angle*M_PI/180.0), sensor_readings(sensor_readings), triggered_sensors(triggered_sensors)
{
    // set critical levels and determine warning type
    
    if (drop) {
        warning_type = drop_off;
    }
    
    double h_hip_dist = calculate_h_distance(hip);
    
    if(sensor_readings[0] < 18 || sensor_readings[1] < 18 || sensor_readings[2] < 18)
    {
        critical_level = high;
    }
    else if(sensor_readings[0] < 24 || sensor_readings[1] < 24 || sensor_readings[2] < 24)
    {
        critical_level = medium;
    }
    else{
        critical_level = low;
    }
    
    if (!drop) {
        if (h_hip_dist < sensor_readings[head_left] && h_hip_dist < sensor_readings[head_right]) {
            
            warning_type = low_obstacle;
            
            if (abs(h_hip_dist - sensor_readings[head_left]) < 5 || abs(h_hip_dist - sensor_readings[head_right]) < 5) {
                warning_type = wall;
            }
        }
        else
        {
            if (abs(h_hip_dist - sensor_readings[head_left]) < 5 || abs(h_hip_dist - sensor_readings[head_right]) < 5) {
                warning_type = wall;
            }
            else{
                warning_type = high_obstacle;
            }
        }
    }
    
    if (warning_type == drop_off) {
        direction = forward;
    }
    else if (abs(sensor_readings[head_left] - sensor_readings[head_right]) > 8) {
        if (sensor_readings[head_left] > sensor_readings[head_right]) {
            direction = right;
        }
        else {
            direction = left;
        }
    }
    else
        direction = forward;

    
}

void Warning::set(int* sensor_readings, bool* triggered_sensors, bool drop)
{
    this->sensor_readings = sensor_readings;
    this->triggered_sensors = triggered_sensors;
    
    // redetermine critical level and warning type
    
    if (drop) {
        warning_type = drop_off;
    }
    
    double h_hip_dist = calculate_h_distance(hip);
    
    if(sensor_readings[0] < 18 || sensor_readings[1] < 18 || sensor_readings[2] < 18)
    {
        critical_level = high;
    }
    else if(sensor_readings[0] < 24 || sensor_readings[1] < 24 || sensor_readings[2] < 24)
    {
        critical_level = medium;
    }
    else{
        critical_level = low;
    }
    
    if (!drop) {
        if (h_hip_dist < sensor_readings[head_left] && h_hip_dist < sensor_readings[head_right]) {
            
            warning_type = low_obstacle;
            
            if (abs(h_hip_dist - sensor_readings[head_left]) < 5 || abs(h_hip_dist - sensor_readings[head_right]) < 5) {
                warning_type = wall;
            }
        }
        else
        {
            if (abs(h_hip_dist - sensor_readings[head_left]) < 5 || abs(h_hip_dist - sensor_readings[head_right]) < 5) {
                warning_type = wall;
            }
            else{
                warning_type = high_obstacle;
            }
        }
    }
    
    if (warning_type == drop_off) {
        direction = forward;
    }
    else if (abs(sensor_readings[head_left] - sensor_readings[head_right]) > 8) {
        if (sensor_readings[head_left] > sensor_readings[head_right]) {
            direction = right;
        }
        else {
            direction = left;
        }
    }
    else
        direction = forward;
    
    updates++;
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

void Warning::print_warning()
{
    if(sensor_readings != NULL)
    {
        Serial.println("**Warning**");
        Serial.print("Loops since detection: ");
        Serial.println((int)loops_since_detection);
        Serial.print("Updates: ");
        Serial.println(updates);
        Serial.print("Critical Level: ");
        
        switch (critical_level) {
            case low:
                Serial.println("low");
                break;
            case medium:
                Serial.println("medium");
                break;
            case high:
                Serial.println("high");
                break;
            default:
                break;
        }
        
        Serial.println("Sensor:        Distance:");
        Serial.println("Head Left        " + (String)sensor_readings[head_left]);
        Serial.println("Head Right       " + (String)sensor_readings[head_right]);
        Serial.println("Hip              " + (String)sensor_readings[hip]);
        Serial.print("Warning Type: ");
        
        switch (warning_type) {
            case high_obstacle:
                Serial.println("High Obstacle");
                break;
            case low_obstacle:
                Serial.println("Low Obstacle");
                break;
            case wall:
                Serial.println("Wall");
                break;
            case drop_off:
                Serial.println("Drop off");
                break;
            default:
                break;
        }
        
        Serial.print("Direction: ");
        
        switch (direction) {
            case left:
                Serial.println("Left");
                break;
            case right:
                Serial.println("Right");
                break;
            case forward:
                Serial.println("Forward");
                break;
            default:
                break;
        }
     
        Serial.println();
    }
    
}

int Warning::get_distance(int sensor)
{
    return sensor_readings[sensor];
}

bool* Warning::get_triggered_sensors()
{
    return triggered_sensors;
}

int* Warning::get_sensor_readings()
{
    return sensor_readings;
}

unsigned long long Warning::get_loops()
{
    return loops_since_detection;
}

void Warning::update_loops()
{
    if (sensor_readings != NULL) {
        loops_since_detection++;
    }
}

void Warning::increment_updates()
{
    updates++;
}

int Warning::get_updates()
{
    return updates;
}