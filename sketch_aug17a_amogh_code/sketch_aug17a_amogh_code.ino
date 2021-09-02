#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int inputPin = A0;

const int numReadings = 1;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;
int iteration = -1;

void setup() 
{  
  lcd.begin(16, 2);
  Serial.begin(9600);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
      readings[thisReading] = 0;
  
  lcd.setCursor(0,0);
  lcd.print("Raw values      ");
  lcd.setCursor(0,1);
  lcd.print("from the sensor ");
  delay(5000);
  
  //pinMode(7, OUTPUT); 
  //pinMode(8, OUTPUT); 

}

void loop() 
{   
  delay(1000);
  
  lcd.clear();
  iteration++;

  total = total - readings[readIndex];
  readings[readIndex] = analogRead(inputPin);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings)readIndex = 0;
      average = total / numReadings;
  
  lcd.setCursor(0,0);
  lcd.print("Light Intensity ");
  lcd.setCursor(6,1);
  lcd.print(average);
  Serial.println(average);

    /*
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);  
  
  if (average < 970 && iteration > 40)
  {      
      digitalWrite(7, LOW);
      delay(1000);
      digitalWrite(7, HIGH);

      delay(1000);

      digitalWrite(8, LOW);
      delay(20);
      digitalWrite(8, HIGH);
  }
  */
}
