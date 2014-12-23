
#define SOL_LED  13

void setup()
{
  Serial.begin(57800);
  while (!Serial)
  {
    digitalWrite(SOL_LED, LOW);
    delay(1000);
    digitalWrite(SOL_LED, HIGH);
    delay(1000);
    Serial.println("Setup tick");
  }
  Serial.println("Serial Port configured");
}

void loop()
{
  Serial.println("Tick");
  delay(1000);
}
