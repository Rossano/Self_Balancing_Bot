
#ifdef ARDUINO
    #if ARDUINO < 100
        #include "WProgram.h"
    #else
        #include "Arduino.h"
    #endif
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        #include <Wire.h>
    #endif
#else
    #include "ArduinoWrapper.h"
#endif

#include <stdint.h>
#include <Wire.h>

#include "mpu6050_driver.h"

extern void delay(unsigned long);
extern class TwoWire Wire;

mpu6050_driver::mpu6050_driver()
{
  _data.x_accel = 0;
  _data.y_accel = 0;
  _data.z_accel = 0;  
  _data.x_gyro = 0;
  _data.y_gyro = 0;
  _data.z_gyro = 0;  
  temperature = 0;
  calibrated_data.x_accel = 0;
  calibrated_data.y_accel = 0;
  calibrated_data.z_accel = 0;
  calibrated_data.x_gyro = 0;
  calibrated_data.y_gyro = 0;
  calibrated_data.z_gyro = 0;  
}

mpu6050_driver::~mpu6050_driver()
{
  
}

int mpu6050_driver::read_gyro_accel_values(uint8_t *pData)
{
  accel_gyro_union_t *accel_gyro = (accel_gyro_union_t *) pData;

  int error = mpu6050_i2c_read(MPU6050_ACCEL_XOUT_H, (uint8_t *) accel_gyro, sizeof(*accel_gyro));
  
  uint8_t swap;
  #define SWAP(x, y) swap = x; x = y; y = swap;
  
  SWAP((*accel_gyro).reg.x_accel_h, (*accel_gyro).reg.x_accel_l);
  SWAP((*accel_gyro).reg.y_accel_h, (*accel_gyro).reg.y_accel_l);
  SWAP((*accel_gyro).reg.z_accel_h, (*accel_gyro).reg.z_accel_l);  
  SWAP((*accel_gyro).reg.x_gyro_h, (*accel_gyro).reg.x_gyro_l);
  SWAP((*accel_gyro).reg.y_gyro_h, (*accel_gyro).reg.y_gyro_l);
  SWAP((*accel_gyro).reg.z_gyro_h, (*accel_gyro).reg.z_gyro_l);   
  
  return 0;
}

void mpu6050_driver::calibrate_sensor()
{
  int num_readings = 10;
  float x_accel = 0.0;
  float y_accel = 0.0;
  float z_accel = 0.0;  
  float x_gyro = 0.0;
  float y_gyro = 0.0;
  float z_gyro = 0.0;  
  accel_gyro_union_t accel_gyro;
  
  read_gyro_accel_values((uint8_t *) &accel_gyro);
  
  for(int i=0; i<num_readings; i++)
  {
    read_gyro_accel_values((uint8_t *) &accel_gyro);
    x_accel += accel_gyro.value.x_accel;
    y_accel += accel_gyro.value.y_accel;
    z_accel += accel_gyro.value.z_accel;
    x_gyro += accel_gyro.value.x_gyro;    
    y_gyro += accel_gyro.value.y_gyro;
    z_gyro += accel_gyro.value.z_gyro;    
    delay(100);
  }
  
  x_accel /= num_readings;
  y_accel /= num_readings;
  z_accel /= num_readings;
  x_gyro /= num_readings;
  y_gyro /= num_readings;
  z_gyro /= num_readings;
    
  /*
    Store calibrated data
  */
  calibrated_data.x_accel = x_accel;
  calibrated_data.y_accel = y_accel;
  calibrated_data.z_accel = z_accel;  
  calibrated_data.x_gyro = x_gyro;  
  calibrated_data.y_gyro = y_gyro;  
  calibrated_data.z_gyro = z_gyro;    
}
  
mpu_data mpu6050_driver::get_mpu6050_data()
{
  return _data;
}

void mpu6050_driver::set_mpu6050_data(mpu_data values)
{
  _data = values;
}

int mpu6050_driver::mpu6050_i2c_read(int start, uint8_t *pData, int size)
{
  int i, n, error;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  
  if(n != 1) return -10;
  
  n = Wire.endTransmission(false);
  if (n != 0) return n;
  
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  
  i = 0;
  while (Wire.available() && i < size)
  {
    buffer[i++] = Wire.read();
  }
  
  if (i != size) return -11;
  
  return 0;
}

int mpu6050_driver::mpu6050_i2c_write(int start, const uint8_t *pData, int size)
{
  int n, error;
  
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  
  n = Wire.write(start);
  
  if (n != 1) return -20;
  
  n =Wire.write(pData, size);
  
  if(n != size) return -21;
  
  Wire.endTransmission(true);
}

int mpu6050_driver::mpu6050_i2c_write_register(int reg, uint8_t data)
{
  int error = mpu6050_i2c_write(reg, &data, 1);
  return error;
}
  
