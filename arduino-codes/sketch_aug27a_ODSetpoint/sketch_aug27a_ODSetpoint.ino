#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int inputPin1 = A0;
int inputPin2 = A2;

const int numReadings = 5;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int level = 0;
int average = 0;
int raw_ldr = 0;

int i=0, iteration = -1;

void setup() 
{  
  lcd.begin(16, 2);
  Serial.begin(9600);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
      readings[thisReading] = 0;
  
  lcd.setCursor(0,0);
  lcd.print("  TURBIDOSTAT   ");
  lcd.setCursor(0,1);
  lcd.print("      V1        ");
  delay(2000);
  
  pinMode(7, OUTPUT); 
  pinMode(8, OUTPUT); 

}

void loop() 
{ 
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
    
  delay(300);
  
  lcd.clear();
  iteration++;

  total = total - readings[readIndex];
  raw_ldr = analogRead(inputPin1);
  readings[readIndex] = raw_ldr;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings)readIndex = 0;
      average = total / numReadings;

  level = analogRead(inputPin2);
  
  lcd.setCursor(0,0);
  lcd.print("  Raw OD");
  lcd.setCursor(11,0);
  lcd.print(average);
  
  Serial.print(raw_ldr);
  Serial.print(",");
  Serial.print(average);
  Serial.print(",");

  lcd.setCursor(0,1);
  lcd.print(" Raw Level");
  lcd.setCursor(11,1);
  lcd.print(level);
  Serial.println(level);
    
  /*
  if (average < 780 && iteration > 200)
  {   
    digitalWrite(7, LOW);
    delay(4000);
    digitalWrite(7, HIGH);
    delay(300);

    iteration = 0;
  }

  if (level < 600 && iteration > 100)
  {   
    digitalWrite(8, LOW);
    delay(7000);
    digitalWrite(8, HIGH);
    delay(300);
  }
  */
}
 
