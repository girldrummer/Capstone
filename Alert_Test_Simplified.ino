#include <alert_simple.h>

using namespace Alert;

const int anPin1 = 0; // head left
const int anPin2 = 1; // head right
const int anPin3 = 2; // hip

double calc_v_distance(int distance, int angle);        // calculates vertical distance given total distance and angle
double calc_h_distance(int distance, int angle);        // calculates horizontal distance given total distance and angle
double calc_running_avg(int* readings);                 // calculates the average value of the past 10 floor readings
void read_sensors();                                    // updates sensor readings
void send_warning();                                    // sends warning at front of queue
void increment_loops();                                 // updates loop counter in all warnings

int distance_hl = 0;                                    // left head sensor distance
int distance_hr = 0;                                    // right head sensor distance
int distance_hp = 0;                                    // hip sensor distance
int current_readings_index = 0;
int warnings_index = 0;

const int threshold = 60;                               // distances below threshold will trigger a warning
const int floor_change_threshold = 3;                   // floor height changes bigger than threshold will trigger a warning
const int angle = 45;                                   // angle below horizontal of hip sensor
const int min_updates = 10;                              // specifies the number of times a warning must be updated before it can be output
double avg_floor_height = 0;
double original_avg_floor = 0;

int current_readings[10];                               // keeps track of past 10 readings
Warning generated_warnings[10];                         // list of current warnings in system

void setup() {

    Serial.begin(9600);
    Serial.println("Reading average floor height: ");

    for(int i = 0; i < 200; i++)
    {
        read_sensors();
        
        if(current_readings_index < 10)
        {
            current_readings[current_readings_index] = distance_hp;
            current_readings_index++;
        }
        else{
            current_readings_index = 0;
            current_readings[current_readings_index] = distance_hp;
        } 
    }
    
    // calculate avg floor height (leave as diagonal distance)
    avg_floor_height = calc_running_avg(current_readings);
    original_avg_floor = avg_floor_height;

    Serial.println("Average Floor Height: " + (String)calc_v_distance(avg_floor_height, angle));
    
    Serial.println("Starting Sensors:");
}

void loop() {

  read_sensors();
  
  if(current_readings_index < 10)
  {
      current_readings[current_readings_index] = distance_hp;
      current_readings_index++;
  }
  else{
      current_readings_index = 0;
      current_readings[current_readings_index] = distance_hp;
  }
  
  avg_floor_height = calc_running_avg(current_readings);
  
  bool is_drop_off = false;
  if( distance_hp > original_avg_floor && abs(calc_v_distance(distance_hp, angle) - calc_v_distance(original_avg_floor, angle)) > floor_change_threshold)
  {
    is_drop_off = true;
  }
        
  // if any (horizontal) sensor reading is less than the threshold or if the
  // difference between current floor height and running average floor height
  // is greater than the accepted threshold, create a warning
  if(distance_hl < threshold || distance_hr < threshold || calc_h_distance(distance_hp, angle) < threshold
     || is_drop_off)
  {
      // map the sensors to their current reading
      int m[3] = {distance_hl, distance_hr, distance_hp};
      
      // check which sensors are detecting object and add to vector
      bool triggered_sensors[3] = {false, false, false};
      
      if(distance_hl < threshold)
      {
          triggered_sensors[head_left] = true;
      }
      if(distance_hr < threshold)
      {
          triggered_sensors[head_right] = true;
      }
      if(calc_h_distance(distance_hp, angle) < threshold)
      {
          triggered_sensors[hip] = true;
      }
      
      // check if a warning with the same triggered sensors already exists
      // and if it was created less than X seconds ago, and if it was,
      // then update that warning's timestamp and readings, otherwise
      // create a new warning
      bool warning_exists = false;
      for (int i = 0; i < 10; i++)
      {
          if(generated_warnings[i].get_triggered_sensors() != NULL)
          {
              bool* ts = generated_warnings[i].get_triggered_sensors();
              unsigned long long loops_since_detection = generated_warnings[i].get_loops();
              
              if (ts[0] == triggered_sensors[0] && ts[1] == triggered_sensors[1] && ts[2] == triggered_sensors[2] && loops_since_detection < 50)
              {
                  warning_exists = true;
                  generated_warnings[i].set(m, triggered_sensors, is_drop_off);
                  break;
              }
          }
      }
      
      if(!warning_exists)
      {
          if(warnings_index < 10)
          {
              generated_warnings[warnings_index] = Warning(angle, m, triggered_sensors, is_drop_off);
              warnings_index++;
          }
          else{
              warnings_index = 0;
              generated_warnings[warnings_index] = Warning(angle, m, triggered_sensors, is_drop_off);
          }
      }     
  }
  
  // output the first warning that has been updated enough times
  send_warning();
  increment_loops();
}

double calc_v_distance(int distance, int angle){
    double angle_rad = PI*angle/180.0;
    double vertical_dist = (double) (sin(angle_rad) * distance);
    return vertical_dist;
}

double calc_h_distance(int distance, int angle){
    double angle_rad = PI*angle/180.0;
    double horizontal_dist = (double) (sin(angle_rad) * distance);
    return horizontal_dist;
}

double calc_running_avg(int* readings)
{
    int t = 0;
    
    for (int i = 0; i < 10; i++)
    {
        t += current_readings[i];
    }
    
    t /= 10;
    
    return t;
}

void read_sensors()
{
    distance_hl = analogRead(anPin1)/2;
    distance_hr = analogRead(anPin2)/2;
    distance_hp = analogRead(anPin3)/2;
}

void send_warning()
{
    // check that a warning has been around for X number of loops
    // start at oldest entered warning and loop through until a warning is found that has been
    // updated the necessary number of times
    
    int index = -1;
    unsigned long long max_loops = 0;
    
    for(int i = 0; i < 10; i++)
    {
        if (generated_warnings[i].get_sensor_readings() != NULL) {
            if(generated_warnings[i].get_updates() > min_updates && generated_warnings[i].get_loops() >= max_loops)
            {
                index = i;
                max_loops = generated_warnings[i].get_loops();
            }
        }
    }
    
    if(index != -1)
    {
        generated_warnings[index].print_warning();
        generated_warnings[index] = Warning(); // overwrite with null data
    }
    
}

void increment_loops()
{
    for(int i = 0; i < 10; i++)
    {
        generated_warnings[i].update_loops();
    }
}
