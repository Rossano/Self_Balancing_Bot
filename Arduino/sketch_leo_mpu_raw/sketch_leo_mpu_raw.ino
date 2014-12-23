#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

#define BAUDRATE  38400
#define SOL_LED   13

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

bool blinkState = false;

void setup()
{
  pinMode(SOL_LED, OUTPUT);
  Wire.begin();
  
  Serial.begin(BAUDRATE);  
  while (!Serial)
  {
    digitalWrite(SOL_LED, LOW);
    delay(1000);
    digitalWrite(SOL_LED, HIGH);
    delay(1000);
  }
  Serial.print("Serial COM Initialization ... ");
  Serial.println("done");
  
  Serial.println("Initialize I2C device ...");
  mpu.initialize();
  Serial.print("Testing MPU connection ... ");
  Serial.println(mpu.testConnection() ? "OK" : "FAIL");
}

void loop()
{
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("ax: "); Serial.print(ax); Serial.print("\tay: "); Serial.print(ay); Serial.print("\taz: "); Serial.println(az);
  Serial.print("gx: "); Serial.print(gx); Serial.print("\tgy: "); Serial.print(gy); Serial.print("\tgz: "); Serial.println(gz);  
  Serial.println();
  
  blinkState = !blinkState;
  digitalWrite(SOL_LED, blinkState);

  delay(1000);
}
