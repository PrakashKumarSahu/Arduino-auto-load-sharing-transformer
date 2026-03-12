#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#define RELAY1 7
#define RELAY2 8
#define SENSOR A0

float sensitivity = 0.185;
float offset = 0;

float ON_THRESHOLD = 1.0;
float OFF_THRESHOLD = 0.8;

bool transformer2 = false;

float readCurrent()
{
  float sum = 0;

  for(int i=0;i<400;i++)
  {
    float value = analogRead(SENSOR) - offset;
    float voltage = value * (5.0 / 1023.0);
    float current = voltage / sensitivity;

    sum += abs(current);
  }

  return sum / 400.0;
}

void calibrateSensor()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calibrating...");

  long sum = 0;

  for(int i=0;i<1200;i++)
  {
    sum += analogRead(SENSOR);
    delay(2);
  }

  offset = sum / 1200.0;

  lcd.setCursor(0,2);
  lcd.print("Calibration OK");
  delay(2000);
}

void setup()
{
  Serial.begin(9600);

  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);

  digitalWrite(RELAY1,HIGH);
  digitalWrite(RELAY2,HIGH);

  lcd.begin();
  lcd.backlight();

  calibrateSensor();

  // STEP 1: Transformer 1 only
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Demo: T1 Only");

  digitalWrite(RELAY1,LOW);
  digitalWrite(RELAY2,HIGH);

  delay(2000);

  float t1Current = readCurrent();

  lcd.setCursor(0,2);
  lcd.print("Current:");
  lcd.print(t1Current,2);
  lcd.print("A");

  delay(3000);

  // STEP 2: Transformer 2 only
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Demo: T2 Only");

  digitalWrite(RELAY1,HIGH);
  digitalWrite(RELAY2,LOW);

  delay(2000);

  float t2Current = readCurrent();

  lcd.setCursor(0,2);
  lcd.print("Current:");
  lcd.print(t2Current,2);
  lcd.print("A");

  delay(3000);

  // STEP 3: Both transformers
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Load Balancing");

  digitalWrite(RELAY1,LOW);
  digitalWrite(RELAY2,HIGH);

  delay(2000);
}

void loop()
{
  float current = readCurrent();

  if(current >= ON_THRESHOLD && !transformer2)
  {
    lcd.setCursor(0,3);
    lcd.print("Adding T2...");

    delay(2000);

    digitalWrite(RELAY2,LOW);
    transformer2 = true;
  }

  if(current <= OFF_THRESHOLD && transformer2)
  {
    digitalWrite(RELAY2,HIGH);
    transformer2 = false;
  }

  lcd.setCursor(0,1);
  lcd.print("Current: ");
  lcd.print(current,2);
  lcd.print("A   ");

  lcd.setCursor(0,2);
  lcd.print("T1:ON ");

  lcd.setCursor(10,2);
  lcd.print("T2:");

  if(transformer2)
  lcd.print("ON ");
  else
  lcd.print("OFF ");

  delay(500);
}
