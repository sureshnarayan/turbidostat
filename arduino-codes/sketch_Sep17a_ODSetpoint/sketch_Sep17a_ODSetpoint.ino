#include <LiquidCrystal.h>
#include <TimerOne.h>

#define SensorOD A0
#define SensorLevel A2
#define InflowPin 7
#define OutflowPin 8

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int numReadings = 10;
int readings[numReadings] = {0, 0, 0, 0, 0};
int readIndex = 0;
long int total = 0;
int level = 0;
int averageOD = 0;
int readingOD = 0;
unsigned long startTimeInflow = 0;
unsigned long startTimeOutflow = 0;
unsigned long waitTime = 0;

const unsigned long int inflowTime = 5000;
const unsigned long int outflowTime = 30000;

int ODSetpoint = 780;

int iteration = 0;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

bool inflowON = false;
bool outflowON = false;

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = analogRead(SensorOD);
    total += readings[thisReading];
  }

  lcd.setCursor(0, 0);
  lcd.print("  TURBIDOSTAT   ");
  lcd.setCursor(0, 1);
  lcd.print("      V1        ");
  delay(500);

  pinMode(InflowPin, OUTPUT);
  pinMode(OutflowPin, OUTPUT);
  digitalWrite(InflowPin, HIGH);
  digitalWrite(OutflowPin, HIGH);

  Timer1.initialize(300000);// every 300ms
  Timer1.attachInterrupt(senseOD);// sense and log

}

void senseOD(void)
{
  readingOD = analogRead(SensorOD);
  //interrupts();
  Serial.print(readingOD);
  Serial.print(",");
  Serial.println(averageOD);

  total = total - readings[readIndex];
  readings[readIndex] = readingOD;
  total = total + readings[readIndex];
  readIndex++;
  if (readIndex >= numReadings) readIndex = 0;
  averageOD = total / numReadings;

}

void loop()
{
  delay(100);
  iteration++;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Raw OD");
  lcd.setCursor(11, 0);
  lcd.print(readingOD);

  lcd.setCursor(0, 1);
  lcd.print(" Average");
  lcd.setCursor(11, 1);
  lcd.print(averageOD);

  if (!inflowON && !outflowON && averageOD < ODSetpoint && iteration > 300)
  {
    digitalWrite(InflowPin, LOW);

    Serial.print("Inflow_ON");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeInflow, DEC));
    Serial.print(",");
    Serial.print(String(millis(), DEC));
    Serial.print(",");
    Serial.println(iteration);

    startTimeInflow = millis();
    //iteration = ;
    inflowON = true;
  }

  if (inflowON && (millis() - startTimeInflow > inflowTime))
  {
    digitalWrite(InflowPin, HIGH);
    Serial.print("Inflow_OFF");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeInflow, DEC));
    Serial.print(",");
    Serial.print(String(millis(), DEC));
    Serial.print(",");
    Serial.println(iteration);

    iteration = 0;
    inflowON = false;
  }

  //1800000L - every 30 mins
  if (!inflowON && !outflowON && (millis() - startTimeOutflow > 1800000L) && (iteration >100))
  {
    digitalWrite(OutflowPin, LOW);
    
    Serial.print("Outflow_ON");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeOutflow, DEC));
    Serial.print(",");
    Serial.print(String(millis(), DEC));
    Serial.print(",");
    Serial.println(iteration);

    startTimeOutflow = millis();
    outflowON = true;
  }
  if (outflowON && (millis() - startTimeOutflow > outflowTime))
  {
    digitalWrite(OutflowPin, HIGH);
    outflowON = false;
    
    Serial.print("Outflow_OFF");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeOutflow, DEC));
    Serial.print(",");
    Serial.print(String(millis(), DEC));
    Serial.print(",");
    Serial.println(iteration);
  }

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      ODSetpoint = inputString.toInt();
    }
  }
}
