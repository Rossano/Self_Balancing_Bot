

import processing.serial.*;

Serial myPort;
short portIndex = 0;
int BaudRate = 9600;
int LF = 10;
String inString;
int calibString;
float RADIANS_TO_DEGREE = 180 / 3.1415;
float alpha = 0.96;

float dt;
float x_gyr;
float y_gyr;
float z_gyr;
float x_acc;
float y_acc;
float z_acc;
float x_fil;
float y_fil;
float z_fil;

float last_x_gyro = 0.0;
float last_y_gyro = 0.0;
float last_z_gyro = 0.0;
int base_x_gyro = 0;
int base_y_gyro = 0;
int base_z_gyro = 0;

// x,y coordinates of circles 1, 2, 3 (pitch, roll, yaw)
int cx[] = {150, 450, 750};
int cy[] = {200, 200, 200};


// Data from dmp and complementary filters
float dmp[] = new float[3];
float cmp[] = new float[3];

// circle diameters
int d   = 200; 

/*
 * Draws a line of length len, centered at x, y at the specified angle
 */
void drawLine(int x, int y, int len, float angle) {
  pushMatrix();
  translate(x, y);
  rotate(angle);
  line(-len/2, 0, len/2, 0);
  popMatrix();
}

void draw_rect_rainbow() {
  scale(90);
  beginShape(QUADS);

  fill(0, 1, 1); vertex(-1,  1.5,  0.25);
  fill(1, 1, 1); vertex( 1,  1.5,  0.25);
  fill(1, 0, 1); vertex( 1, -1.5,  0.25);
  fill(0, 0, 1); vertex(-1, -1.5,  0.25);

  fill(1, 1, 1); vertex( 1,  1.5,  0.25);
  fill(1, 1, 0); vertex( 1,  1.5, -0.25);
  fill(1, 0, 0); vertex( 1, -1.5, -0.25);
  fill(1, 0, 1); vertex( 1, -1.5,  0.25);

  fill(1, 1, 0); vertex( 1,  1.5, -0.25);
  fill(0, 1, 0); vertex(-1,  1.5, -0.25);
  fill(0, 0, 0); vertex(-1, -1.5, -0.25);
  fill(1, 0, 0); vertex( 1, -1.5, -0.25);

  fill(0, 1, 0); vertex(-1,  1.5, -0.25);
  fill(0, 1, 1); vertex(-1,  1.5,  0.25);
  fill(0, 0, 1); vertex(-1, -1.5,  0.25);
  fill(0, 0, 0); vertex(-1, -1.5, -0.25);

  fill(0, 1, 0); vertex(-1,  1.5, -0.25);
  fill(1, 1, 0); vertex( 1,  1.5, -0.25);
  fill(1, 1, 1); vertex( 1,  1.5,  0.25);
  fill(0, 1, 1); vertex(-1,  1.5,  0.25);

  fill(0, 0, 0); vertex(-1, -1.5, -0.25);
  fill(1, 0, 0); vertex( 1, -1.5, -0.25);
  fill(1, 0, 1); vertex( 1, -1.5,  0.25);
  fill(0, 0, 1); vertex(-1, -1.5,  0.25);

  endShape();
  
  
}

void draw_rect(int r, int g, int b) {
  scale(90);
  beginShape(QUADS);
  
  fill(r, g, b);
  vertex(-1,  1.5,  0.25);
  vertex( 1,  1.5,  0.25);
  vertex( 1, -1.5,  0.25);
  vertex(-1, -1.5,  0.25);

  vertex( 1,  1.5,  0.25);
  vertex( 1,  1.5, -0.25);
  vertex( 1, -1.5, -0.25);
  vertex( 1, -1.5,  0.25);

  vertex( 1,  1.5, -0.25);
  vertex(-1,  1.5, -0.25);
  vertex(-1, -1.5, -0.25);
  vertex( 1, -1.5, -0.25);

  vertex(-1,  1.5, -0.25);
  vertex(-1,  1.5,  0.25);
  vertex(-1, -1.5,  0.25);
  vertex(-1, -1.5, -0.25);

  vertex(-1,  1.5, -0.25);
  vertex( 1,  1.5, -0.25);
  vertex( 1,  1.5,  0.25);
  vertex(-1,  1.5,  0.25);

  vertex(-1, -1.5, -0.25);
  vertex( 1, -1.5, -0.25);
  vertex( 1, -1.5,  0.25);
  vertex(-1, -1.5,  0.25);

  endShape();
  
  
}

void setup()
{
  // Setup the display window
  size(800, 400, P3D);
  noStroke();
  colorMode(RGB, 256);
  
  println("Inizia list porte:");
  
  //myPort = new Serial(this, Serial.list()[0], 9600); 
  println(Serial.list());
  // Setup the serial port
  String portName = Serial.list()[portIndex];
  println(portName);
  println("Open the serial port...");
  //myPort = new Serial(this, portName, BaudRate);
  myPort = new Serial(this, Serial.list()[0], BaudRate);
  myPort.clear();
  myPort.bufferUntil(LF);
  
  println("Done. End initialization");
}

void draw()
{
  background(0);
  lights();
  
  int distance = 50;
  int x_rotation = 90;
  
  // Show Gyro data
  pushMatrix();
  translate(width/6, height/2, -50);
  rotateX(radians(-x_gyr - x_rotation));
  rotateY(radians(-y_gyr));
  draw_rect(249, 250, 50);
  popMatrix();
  
  // Show Acceleration data
  pushMatrix();
  translate(width/2, height/2, -50);
  rotateX(radians(-x_acc - x_rotation));
  rotateY(radians(-y_acc));
  draw_rect(56, 140, 206);
  popMatrix();
  
  // Show Combined data
  pushMatrix();
  translate(5*width/6, height/2, -50);
  rotateX(radians(-x_fil - x_rotation));
  rotateY(radians(-y_fil));
  draw_rect(93, 175, 83);
  popMatrix();
  
  // Show Strings
  textSize(24);
  String accStr = "(" + (int)x_acc + ", " + (int)y_acc + ")";
  String gyrStr = "(" + (int)x_gyr + ", " + (int)y_gyr + ")";
  String filStr = "(" + (int)x_fil + ", " + (int)y_fil + ")";
  
  fill(249, 250, 50);
  text("Gyroscope", (int)width/6.0 - 60, 25);
  text(gyrStr, (int)(width/6.0) - 40, 50);
  
  fill(56, 140, 206);
  text("Accelerometer", (int)width/2.0 - 50, 25);
  text(accStr, (int)(width/2.0) - 30, 50);
 
  fill(83, 175, 93);
  text("Combination", (int)(5.0*width/6.0) - 40, 25);
  text(filStr, (int)(5.0*width/6.0) - 20, 50);
  
  fill(225);
  for (int i = 0; i < 3; i++) {
    ellipse(cx[i], cy[i], d, d);
  }
  
  // Draw the lines representing the angles
  for (int i = 0; i < 3; i++) {
    strokeWeight(3);
    stroke(255, 0, 0);
    drawLine(cx[i], cy[i], d, radians(cmp[i]));
    stroke(#2EBDF0);
    drawLine(cx[i], cy[i], d, radians(dmp[i]));
  }
  
  // Draw the explanatory text
  textSize(20);
  fill(255, 0, 0);
  text("Complementary Filter", 10, 20);
  
  fill(#2EBDF0);
  text("DMP", 10, 50);
  
  fill(255);
  text("Pitch", cx[0]-25, 75);
  text("Roll", cx[1]-22, 75);
  text("Yaw", cx[2]-22, 75);
  
  for (int i = 0; i < 3; i++) {
    fill(255, 0, 0);
    String str = String.format("%5.1f", cmp[i]);
    text(str, cx[i]-22, 350);
    fill(#2EBDF0);
    str = String.format("%5.1f", dmp[i]);
    text(str, cx[i]-22, 380);
  }
}

//
// Serial Event Manager
//
void serialEvent(Serial p)
{
  inString = (myPort.readString());
  println(inString);
  try
  {
    String[] dataStrings = split(inString, "#");
    for(int i = 0; i < dataStrings.length; i++)
    {
      String type = dataStrings[i].substring(0,4);
      String dataval = dataStrings[i].substring(4);
           
      // Get the time interval
      if(type.equals("DEL:"))
      {
        dt = float(dataval);
      }
      // Get Accelerometer data
      else if(type.equals("ACC:"))
      {
        String data[] = split(dataval, ',');
        // Get raw data
        int ax = int(data[0]);
        int ay = int(data[1]);
        int az = int(data[2]);
        // Carry out accelerations
        float accel_y = atan(-1*ax/sqrt(pow(ay,2) + pow(az,2))) * RADIANS_TO_DEGREE;
        float accel_x = atan(ay/sqrt(pow(ax,2) + pow(az,2))) * RADIANS_TO_DEGREE;
        float accel_z = 0;
        x_acc = accel_x;
        y_acc = accel_y;
        z_acc = accel_z;
      }
      // Get Gyrometer data
      else if(type.equals("GYR:"))
      {
        float FS_SEL = 131;
        String data[] = split(dataval, ',');
        // Cumulate gyro values
        float gx = float(int(data[0]) - base_x_gyro) / FS_SEL;
        float gy = float(int(data[1]) - base_y_gyro) / FS_SEL;
        float gz = float(int(data[2]) - base_z_gyro) / FS_SEL;
        x_gyr = gx*dt + last_x_gyro;
        y_gyr = gy*dt + last_y_gyro;
        z_gyr = gz*dt + last_z_gyro;
        // Store last value for next iteration
        last_x_gyro = x_gyr;
        last_y_gyro = y_gyr;
        last_z_gyro = z_gyr;
        // Carry out Complementaty filtre
        x_fil = alpha*x_gyr + (1 - alpha)*x_acc;
        y_fil = alpha*y_gyr + (1 - alpha)*y_gyr;
        z_fil = alpha*z_gyr + (1 - alpha)*z_gyr;
      }
      // Retrieve filtered data from IMU
      else if(type.equals("FIL:"))
      {
        String data[] = split(dataval, ',');
        x_fil = float(data[0]);
        y_fil = float(data[1]);
        z_fil = float(data[2]);
      }
      print("Angle: "); print (x_fil); print(" "); print(y_fil); print(" "); println(z_fil);
      cmp[0] = x_fil;
      cmp[1] = y_fil;
      cmp[2] = z_fil;
    }
  }
  catch(Exception e)
  {
    println("Exception caught on Serial Port!");
  }
}

  
