#include <iostream>
#include <queue>
#include "alert.h"

using namespace Alert;
using namespace std;

double calc_v_distance(int distance, int angle);        // calculates vertical distance given total distance and angle
double calc_h_distance(int distance, int angle);        // calculates horizontal distance given total distance and angle
double calc_running_avg(deque<int> readings);           // calculates the average value of the past 10 floor readings
void read_sensors();                                    // updates sensor readings
void send_warning(deque<Warning>* warnings);            // sends warning at front of queue

int distance_hl = 0; // left head sensor distance
int distance_hr = 0; // right head sensor distance
int distance_hp = 0; // hip sensor distance

int main(int argc, const char * argv[]) {
    
    int threshold = 10;                 // distances below threshold will trigger a warning
    int floor_change_threshold = 3;     // floor height changes bigger than threshold will trigger a warning
    deque<int> current_readings;        // keeps track of past 10 readings
    deque<Warning> generated_warnings;  // list of current warnings in system
    int angle = 45;                     // angle below horizontal of hip sensor
    double avg_floor_height = 0;
    
    long elapsed_time = 0;              // used to get an average floor height over 10 seconds
    time_t current_time;
    time_t start = time(NULL);
    
    // populate deque with floor readings for ten seconds
    // and limit deque size to 10
    cout << "Reading average floor height: " << endl;
    do{
        read_sensors();
        
        if(current_readings.size() < 10)
        {
            current_readings.push_back(distance_hp);
        }
        else{
            current_readings.pop_front();
            current_readings.push_back(distance_hp);
        }
        
        current_time = time(NULL);
        elapsed_time = current_time - start;
        
    }while(elapsed_time < 10);
    
    // output queue size for checking
    // cout << "\nQueue size check: " << current_readings.size() << endl;
    
    // calculate avg floor height (leave as diagonal distance)
    avg_floor_height = calc_running_avg(current_readings);
    
    cout << "Average Floor Height: " << calc_v_distance(avg_floor_height, angle) << endl;
    
    cout << "Starting Sensors:" << endl;
    cout << "(Enter 1 to simulate Arduino loop or enter q to quit)" << endl;
    
    // start loop
    while(true)
    {
        char x;
        std::cin >> x;
        
        if(x == 'q')
            break;
        
        else{
            if(x == '1')
            {
                read_sensors();
                
                if(current_readings.size() < 10)
                {
                    current_readings.push_back(distance_hp);
                }
                else{
                    current_readings.pop_front();
                    current_readings.push_back(distance_hp);
                }
                
                avg_floor_height = calc_running_avg(current_readings);
                
                
                // if any (horizontal) sensor reading is less than the threshold or if the
                // difference between current floor height and running average floor height
                // is greater than the accepted threshold, create a warning
                if(distance_hl < threshold || distance_hr < threshold || calc_h_distance(distance_hp, angle) < threshold
                   || abs(calc_v_distance(distance_hp, angle) - calc_v_distance(avg_floor_height, angle)) > floor_change_threshold)
                {
                    // map the sensors to their current reading
                    map<int, int> m = {{head_left, distance_hl},{head_right, distance_hr},{hip, distance_hp}};

                    // check which sensors are detecting object and add to vector
                    vector<int> triggered_sensors;
                    
                    if(distance_hl < threshold)
                    {
                        triggered_sensors.push_back(head_left);
                    }
                    if(distance_hr < threshold)
                    {
                        triggered_sensors.push_back(head_right);
                    }
                    if(calc_h_distance(distance_hp, angle) < threshold)
                    {
                        triggered_sensors.push_back(hip);
                    }
                    
                    // check if a warning with the same triggered sensors already exists
                    // and if it was created less than X seconds ago, and if it was,
                    // then update that warning's timestamp and readings, otherwise
                    // create a new warning
                    bool warning_exists = false;
                    for (deque<Warning>::iterator it = generated_warnings.begin(); it != generated_warnings.end(); it++)
                    {
                        vector<int> ts = it->get_triggered_sensors();
                        if (ts == triggered_sensors && (time(NULL) - it->get_time()) < 5)
                        {
                            warning_exists = true;
                            it->set(time(NULL), m, triggered_sensors);
                            break;
                        }
                    }
                    if(!warning_exists)
                    {
                        generated_warnings.push_back(Warning(time(NULL), angle, m, triggered_sensors));
                    }
                    
                }
                
                // output the warning at the front of the queue
                send_warning(&generated_warnings);
                cout << "Current avg floor height: " << avg_floor_height << endl;
            }
        }
        
    }

    // output all warnings in the queue
//    for (deque<Warning>::iterator it = generated_warnings.begin(); it < generated_warnings.end(); it++) {
//        it->print_warning();
//    }
    
    return 0;
}

double calc_v_distance(int distance, int angle){
    double angle_rad = M_PI*angle/180.0;
    double vertical_dist = (double) (sin(angle_rad) * distance);
    return vertical_dist;
}

double calc_h_distance(int distance, int angle){
    double angle_rad = M_PI*angle/180.0;
    double horizontal_dist = (double) (sin(angle_rad) * distance);
    return horizontal_dist;
}

double calc_running_avg(deque<int> readings)
{
    int t = 0;
    
    for (deque<int>::iterator it = readings.begin(); it < readings.end(); it++) {
        t += *it;
    }
    
    t /= readings.size();

    return t;
}

void read_sensors()
{
    distance_hl = rand()%100;
    distance_hr = rand()%100;
    distance_hp = rand()%100;
}

void send_warning(deque<Warning>* warnings)
{
    if(warnings->size() > 0)
    {
        warnings->front().print_warning();
        warnings->pop_front();
    }
}