#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
//#include <MPU6050_9Axis_MotionApps41.h>
#include <helper_3dmath.h>

#define BAUDRATE  38400
#define SOL_LED   13
#define DMP_IRQ  2
#define DMP_IRQ_PIN  0

#undef OUTPUT_TEAPOT
#define OUTPUT_READABLE_EULER 
#define OUTPUT_READABLE_YAWPITCHROLL
#define OUTPUT_READABLE_QUATERNION
#define FIFO_LEN    64
#define DEBUG

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

bool blinkState = false;

bool dmpReady = false;


uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[FIFO_LEN];

Quaternion q;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWord;
VectorFloat gravity;
float euler[3];
float ypr[3];

uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;

void dmpISR()
{
  mpuInterrupt = true;
}

void readData()
{
  
        #ifdef OUTPUT_READABLE_QUATERNION
            // display quaternion values in easy matrix form: w x y z
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            Serial.print("quat\t");
            Serial.print(q.w);
            Serial.print("\t");
            Serial.print(q.x);
            Serial.print("\t");
            Serial.print(q.y);
            Serial.print("\t");
            Serial.println(q.z);
        #endif

        #ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetEuler(euler, &q);
            Serial.print("euler\t");
            Serial.print(euler[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(euler[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
        #endif
    
        #ifdef OUTPUT_TEAPOT
            // display quaternion values in InvenSense Teapot demo format:
            teapotPacket[2] = fifoBuffer[0];
            teapotPacket[3] = fifoBuffer[1];
            teapotPacket[4] = fifoBuffer[4];
            teapotPacket[5] = fifoBuffer[5];
            teapotPacket[6] = fifoBuffer[8];
            teapotPacket[7] = fifoBuffer[9];
            teapotPacket[8] = fifoBuffer[12];
            teapotPacket[9] = fifoBuffer[13];
            Serial.write(teapotPacket, 14);
            teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
        #endif
}

void setup()
{
  pinMode(SOL_LED, OUTPUT);
  pinMode(DMP_IRQ_PIN, INPUT);
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
  
  Serial.println("Send any character to begin DMP programming and demo ...");
  while(Serial.available() && Serial.read()); // empty buffer
  while(!Serial.available()) ;                // wait for data
  while(Serial.available() && Serial.read()); // empty buffer again
  
  // Load and configure DMP
  Serial.println("Initialize DMP ...");
  devStatus = mpu.dmpInitialize();
  
  if(devStatus == 0)
  {
    Serial.println("Enabling DMP ...");
    mpu.setDMPEnabled(true);
    
    Serial.println("Enabling interrupt detection (EXTI ???)");
    attachInterrupt(DMP_IRQ, dmpISR, RISING);
    mpuIntStatus = mpu.getIntStatus();
    
    Serial.println("DMP ready waiting for first interrupt!");
    dmpReady = true;
    
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    Serial.print("DMP initialization failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }
}

void loop()
{
  if(!dmpReady) return;
  
  while (!mpuInterrupt && fifoCount < packetSize)
  {
  }
  
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  
  fifoCount = mpu.getFIFOCount();
  
  if((mpuIntStatus & 0x10) || fifoCount == 1024)
  {
    mpu.resetFIFO();
    Serial.println("FIFO Overflow!");
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    
    fifoCount -= packetSize;    
    
    readData();
    
  }
  else if (mpuIntStatus & 0x02)
  {
    while(fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    
    fifoCount -= packetSize;
    
    readData();
    
  }    
  
  blinkState = !blinkState;
  digitalWrite(SOL_LED, blinkState);

  delay(1000);
}
