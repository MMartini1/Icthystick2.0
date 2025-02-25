#include "distance_sensor.h"

measurement_state_t* measurement_state;

void setup_distance_sensor(measurement_state_t* ms)
{
  unsigned long last_read = millis();
  
  measurement_state = ms;
  pinMode(D3, OUTPUT);
  digitalWrite(D3, 0);
  pinMode(D2, INPUT);

  read_calibration_coefficients();
  measurement_state->pending_calibration.next_cal_point = 0;
  measurement_state->state = MEASUREMENT_STATE_WAITING;

  measurement_state->pending_calibration.target[0] = 0;
  measurement_state->pending_calibration.target[1] = 20;
  measurement_state->pending_calibration.target[2] = 40;
  measurement_state->pending_calibration.target[3] = 60;
  
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(D2), meas_done, RISING);
  
}

void read_calibration_coefficients()
{
  EEPROM.get(0, measurement_state->calibration_coefficients);
}

void store_calibration_coefficients()
{
  EEPROM.put(0, measurement_state->calibration_coefficients);  
}




static unsigned long start_time;
static unsigned long stop_time;
static bool done = false;
void meas_done()
{
  stop_time = micros();  
  done = true;
}

float get_current_calibration_target()
{
  return measurement_state->pending_calibration.target[measurement_state->pending_calibration.next_cal_point];
}

float get_current_calibration_slope()
{
  return measurement_state->calibration_coefficients.m;
}

float get_current_calibration_intercept()
{
  return measurement_state->calibration_coefficients.b;
}

void initiate_calibration()
{
  measurement_state->pending_calibration.next_cal_point = 0;
  measurement_state->state = MEASUREMENT_STATE_CAL_MAX_VAL;
  // TODO: set display for first cal target
}

void abort_calibration()
{
  measurement_state->pending_calibration.next_cal_point = 0;
  measurement_state->state = MEASUREMENT_STATE_WAITING;
}

void calculate_calibration()
{
  float x_sum=0;
  float y_sum=0;  
  for (int i=0; i< CALIBRATION_POINTS; i++)
  {
    x_sum += measurement_state->pending_calibration.target[i];
    y_sum += measurement_state->pending_calibration.measured[i];

    Serial.println(measurement_state->pending_calibration.target[i]);
    Serial.println(measurement_state->pending_calibration.measured[i]);
    Serial.println("----");
    
  }
  float x_mean = x_sum/CALIBRATION_POINTS;
  float y_mean = y_sum/CALIBRATION_POINTS;
  Serial.println(CALIBRATION_POINTS);
  Serial.println(x_sum);
  Serial.println(y_sum);

  Serial.println(x_mean);
  Serial.println(y_mean);
    
  float Sxy = 0;
  float Sxx = 0;
  for (int i=0; i< CALIBRATION_POINTS; i++)
  {
    Sxy += (measurement_state->pending_calibration.target[i] - x_mean) * (measurement_state->pending_calibration.measured[i] - y_mean);
    Sxx += (measurement_state->pending_calibration.target[i] - x_mean) * (measurement_state->pending_calibration.target[i] - x_mean) ;
    Serial.println(Sxx);
    Serial.println(Sxy);

  
  }
  float m = Sxy/Sxx;
  float b = y_mean - m * x_mean;

  measurement_state->calibration_coefficients.m = m;
  measurement_state->calibration_coefficients.b = b;
  
  
}

// run one measurement. This is blocking for up to MAX_DELAY_US.
long make_measurement_us()
{
  long retval = -1;
  noInterrupts();
  digitalWrite(D3,1);
  for (int i=0; i<10; i++)
  {
  __asm__("nop\n\t");
  }
  digitalWrite(D3,0);
  interrupts();
  start_time = micros();
  delayMicroseconds(BLANK_TIME_US);
  done = false;

  while(!done)
  {
    if ((micros() - start_time)   > MAX_DELAY_US)
    {

      break;  
    }
  }
  //unsigned long measured_delay = micros() - start_time;
  
  if (done)
  {
    retval = stop_time - start_time;

    //Serial.println((stop_time - start_time)/9.0462);
  }  

  return retval;
}
