
//#include "mpu6050_driver.h"
#include <I2Cdev.h>
#include <MPU6050.h>
#include "complementary_filter.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include <Wire.h>
#endif

/*
 *  DEFINES
 */
#define BAUDRATE  112500
#define SOL_LED   13
#define __SEND_RAW__
#undef __SEND_ANGLES__

/*
 * VARIABLES
 */
 
complementary_filter filter;

#ifdef MY_MPU6050_DRIVER
  mpu6050_driver mpu;
#else
  MPU6050 mpu;
  
  typedef struct mpu_data
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  };
#endif

mpu_data getMotionData();
float last_t = 0.0;

void setup()
{
  int error;
  uint8_t c;
  
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    FastWire::setup(400, true);
  #endif
  
  Serial.begin(BAUDRATE);
  Serial.println("MPU6050 Test");
  
  #ifdef MY_MPU6050_DRIVER
    Wire.begin();
    error = mpu.mpu6050_i2c_read(MPU6050_WHO_AM_I, &c, 1);
    Serial.print("WHO AM I: "); 
    Serial.println(c, HEX);
    error = mpu.mpu6050_i2c_read(MPU6050_PWR_MGMT_1, &c, 1);
    mpu.mpu6050_i2c_write_register(MPU6050_PWR_MGMT_1, 0);
  #else
    mpu.initialize();
    Serial.println("Testing I2C connection ... ");
    Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  #endif
  
  pinMode(SOL_LED, OUTPUT);
}

void loop()
{
  int error;
  unsigned long t_now = millis();
  mpu_data data;
  
  #ifdef MY_MPU6050_DRIVER
    data = mpu.get_mpu6050_data();
  #else
    data = getMotionData();
  #endif
  
  filter.update_filter(t_now, data.x_accel, data.y_accel, data.z_accel, data.x_gyro, data.y_gyro, data.z_gyro);
  
  #ifdef __SEND_ANGLES__
  Serial.print("DEL:");                // Delta T
  Serial.print(t_now, DEC);
  Serial.print("#FIL:");               // Angle
  Serial.print(filter.get_x_angle());
  Serial.print(" , ");
  Serial.print(filter.get_y_angle());
  Serial.print(" , ");
  Serial.print(filter.get_z_angle());
  Serial.println("");
  #elif defined(__SEND_RAW__)
  Serial.print("DEL:");
  float dt = t_now - last_t;
  Serial.print(dt,DEC);
  Serial.print("#ACC:");
  Serial.print(data.x_accel);
  Serial.print(",");
  Serial.print(data.y_accel);
  Serial.print(",");
  Serial.print(data.z_accel);
  Serial.print("#GYR:");
  Serial.print(data.x_gyro);
  Serial.print(",");
  Serial.print(data.y_gyro);
  Serial.print(",");
  Serial.print(data.z_gyro);
  Serial.println("");
  #endif
  last_t = t_now;
  
  delay(100);
}

mpu_data getMotionData()
{
  mpu_data _d;
  
  mpu.getMotion6(&_d.x_accel, &_d.y_accel, &_d.z_accel, &_d.x_gyro, &_d.y_gyro, &_d.z_gyro);
  
  return _d;
}
