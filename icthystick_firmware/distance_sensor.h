
#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

typedef enum
{
  MEASUREMENT_STATE_WAITING=0,
  MEASUREMENT_STATE_MEASURED,
  MEASUREMENT_STATE_CAL_MAX_VAL,
  MEASUREMENT_STATE_CAL1_WAITING,  
  MEASUREMENT_STATE_CAL1_MEASURED,
  MEASUREMENT_STATE_CAL2_WAITING,
  MEASUREMENT_STATE_CAL2_MEASURED,
  MEASUREMENT_STATE_CAL3_WAITING,
  MEASUREMENT_STATE_CAL3_MEASURED,
  MEASUREMENT_STATE_CAL4_WAITING,
} e_measurement_states;

typedef struct 
{
  float m;
  float b;
  float r2;
  uint16_t max_val;
  
} calibration_coefficients_t;


#define CALIBRATION_POINTS 4
typedef struct
{
  float target[CALIBRATION_POINTS];
  float measured[CALIBRATION_POINTS];
  uint8_t next_cal_point;
  
} calibration_measurement_t;

typedef struct 
{
  e_measurement_states state;
  calibration_coefficients_t calibration_coefficients;
  calibration_measurement_t pending_calibration;
  unsigned long last_read;
  unsigned long estimate_maximum_value;
  unsigned int output_format = DEFAULT_OUTPUT_FORMAT;
  
  
} measurement_state_t;


void initiate_calibration();
void abort_calibration();
float get_current_calibration_target();
void calculate_calibration();

float get_current_calibration_slope();

float get_current_calibration_intercept();
void store_calibration_coefficients();
void read_calibration_coefficients();

#endif
