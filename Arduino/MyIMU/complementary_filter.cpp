#include <Wire.h>
#include <math.h>

#include "complementary_filter.h"

complementary_filter::complementary_filter()
{
  x_angle = last_x_angle = 0.0;
  y_angle = last_y_angle = 0.0;
  z_angle = last_z_angle = 0.0;
//t_now = millis();
  last_time = 0;
}

complementary_filter::~complementary_filter()
{
  
}

void complementary_filter::update_filter(unsigned long time, float x, float y, 
  float z, float gx, float gy, float gz)
{
  last_time = time;
  last_x_angle = x;
  last_y_angle = y;
  last_z_angle = z;
  last_x_gyro = gx;
  last_y_gyro = gy;
  last_z_gyro = gz;
}

float complementary_filter::get_x_angle()
{
  return x_angle;
}

float complementary_filter::get_y_angle()
{
  return y_angle;
}

float complementary_filter::get_z_angle()
{
  return z_angle;
}

float complementary_filter::get_x_gyro()
{
  return x_gyro;
}

float complementary_filter::get_y_gyro()
{
  return y_gyro;
}

float complementary_filter::get_z_gyro()
{
  return z_gyro;
}
    
void complementary_filter::update_angle_data(unsigned long t, float x, float y, float z,
  float gx, float gy, float gz)
{
  float RAD_TO_DEG = 180 / 3.14159;
  
  y_angle = atan(-1 * x / sqrt(pow(y,2) + pow(z,2))) * RAD_TO_DEG;
  x_angle = atan(y / sqrt(pow(x,2) + pow(z,2))) * RAD_TO_DEG;
  z_angle = 0.0;
  
  float dt = (t_now - last_time) / 1000;
  
  x_gyro = gx * dt + last_x_gyro;
  y_gyro = gy * dt + last_y_gyro;
  z_gyro = gz * dt + last_z_gyro;
  
  x_angle = alpha * x_gyro + (1.0 - alpha) * x_angle;
  y_angle = alpha * y_gyro + (1.0 - alpha) * y_angle;
  z_angle = 0.0;
  
}
