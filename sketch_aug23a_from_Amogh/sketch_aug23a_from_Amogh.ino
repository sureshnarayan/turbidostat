#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int inputPin1 = A0;
int inputPin2 = A2;
const int numReadings = 1;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int level = 0;
int average = 0;
int iteration = -1;
void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
      readings[thisReading] = 0;
  lcd.setCursor(0,0);
  lcd.print("  CHEMOSTAT v1  ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  delay(5000);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}
void loop()
{
  delay(200);
  lcd.clear();
  iteration++;
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(inputPin1);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings)readIndex = 0;
      average = total / numReadings;
  level = analogRead(inputPin2);
  lcd.setCursor(0,0);
  lcd.print("  Raw OD");
  lcd.setCursor(11,0);
  lcd.print(average);
  Serial.print(average);
  Serial.print(",");
  lcd.setCursor(0,1);
  lcd.print(" Raw Level");
  lcd.setCursor(11,1);
  lcd.print(level);
  Serial.println(level);

  delay(1000);
  
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  /*if (average1 < 880 && iteration > 40)
  {   digitalWrite(8, LOW);
      delay(20);
      digitalWrite(8, HIGH);
      digitalWrite(7, LOW);
      delay(20);
      digitalWrite(7, HIGH);
      delay(3000);
  }*/
}
