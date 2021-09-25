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
long unsigned int total = 0;
int level = 0;
int averageOD = 0;
int readingOD = 0;
unsigned long startTimeInflow = 0;
unsigned long startTimeOutflow = 0;
unsigned long waitTime = 0;

unsigned long inflowTime = 10000;
unsigned long outflowTime = 20000;

unsigned long startTimeValve = 0;

int ODSetpoint = 780;


String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

bool inflowON = false;
bool outflowON = false;

bool switchOffLoop = false;
bool valveManualOverride = false;
bool valveOffManual = false;
unsigned long valveOnTimeManual = 2000; //
int manualOverrideValve = OutflowPin; //Which pin for manual override
unsigned long startTimeOverride = 0;

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
  Serial.print("LOG,");
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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Raw OD");
  lcd.setCursor(11, 0);
  lcd.print(readingOD);

  lcd.setCursor(0, 1);
  lcd.print(" Average");
  lcd.setCursor(11, 1);
  lcd.print(averageOD);

  if (switchOffLoop)return;

  if (valveManualOverride)
  {
    if (millis() - startTimeOverride > valveOnTimeManual)
    {
      digitalWrite(manualOverrideValve, HIGH);
      startTimeOverride = 0;
      valveManualOverride = false;
      Serial.print("LOOPLOG,");
      Serial.print((manualOverrideValve==InflowPin)?"IN":"OUT");
      Serial.print(" Valve Switched OFF after (ms)");
      Serial.println(valveOnTimeManual);
    }
    return;
  }

  if (valveOffManual) // to manually switch off a valve
  {
    digitalWrite(manualOverrideValve, HIGH);
    valveOffManual = false;
    Serial.print("LOOPLOG,");
    Serial.print("Manually switching OFF Valve ");
    Serial.println((manualOverrideValve==InflowPin)?"IN":"OUT");
    return;
  }

//----------------------------------------------------------------

  //Outflow
  if (!inflowON && !outflowON && 
      averageOD < ODSetpoint)
  {
    if (millis() - (startTimeInflow + inflowTime) < 30000)
    {
      Serial.print("Outflow_Waiting (min 10000ms)");
      Serial.print(",");
      Serial.println(millis() - (startTimeInflow + inflowTime));
    }
    else
    {
      digitalWrite(OutflowPin, LOW);

      Serial.print("LOOPLOG,");
      Serial.print("Outflow_ON");
      Serial.print(",");
      Serial.print(averageOD);
      Serial.print(",");
      Serial.print(String(millis() - startTimeOutflow, DEC));
      Serial.print(",");
      Serial.println(String(millis(), DEC));
  
      startTimeOutflow = millis();
      outflowON = true;
    }
  }
  else if (outflowON && (millis() - startTimeOutflow > outflowTime))
  {
    digitalWrite(OutflowPin, HIGH);
    outflowON = false;

    Serial.print("LOOPLOG,");
    Serial.print("Outflow_OFF");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeOutflow, DEC));
    Serial.print(",");
    Serial.println(String(millis(), DEC));


    // Start inflow after outflow ends

    if (millis() - (startTimeInflow + inflowTime) < 30000)
    {
      Serial.print("Inflow_Waiting (30000ms)");
      Serial.print(" , ");
      Serial.println(millis() - (startTimeInflow + inflowTime));
    }
    else
    {
      digitalWrite(InflowPin, LOW);
      inflowON = true;

      Serial.print("LOOPLOG,");
      Serial.print("Inflow_ON");
      Serial.print(",");
      Serial.print(averageOD);
      Serial.print(",");
      Serial.print(String(millis() - startTimeInflow, DEC));
      Serial.print(",");
      Serial.println(String(millis(), DEC));
  
      startTimeInflow = millis();
    }
  }
  else if (inflowON && 
           (millis() - startTimeInflow > inflowTime))
  {
    digitalWrite(InflowPin, HIGH);
    inflowON = false;

    Serial.print("LOOPLOG,");
    Serial.print("Inflow_OFF");
    Serial.print(",");
    Serial.print(averageOD);
    Serial.print(",");
    Serial.print(String(millis() - startTimeInflow, DEC));
    Serial.print(",");
    Serial.println(String(millis(), DEC));
  }


}

void serialEvent() {
  while (Serial.available()) {
    String command = "";
    unsigned long value = 0;
    int ind1 = 0;
    
    // get the new byte:
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
      ind1 = inputString.indexOf(':');  //finds location of first :
      command = inputString.substring(0, ind1);
      command.trim();
      //ind2 = readString.indexOf('\n', ind1+1 );   //finds location of second ,
      if (command == "SP")
      {
        value = inputString.substring(ind1+1, inputString.length()).toInt();
        if (value) ODSetpoint = value;
        Serial.print("Setpoint");
        Serial.print(",");
        Serial.println(ODSetpoint);
      }
      else if (command == "IN")
      {
        value = inputString.substring(ind1+1, inputString.length()).toInt();
        if (value) inflowTime = value;
        Serial.print("Inflow ON for");
        Serial.print(",");
        Serial.println(inflowTime);
      }
      else if (command == "OUT")
      {
        value = inputString.substring(ind1+1, inputString.length()).toInt();
        if (value) outflowTime = value;
        Serial.print("Outflow ON for");
        Serial.print(",");
        Serial.println(outflowTime);
      }
      else if (command == "VALVE_ON")
      {
        int ind2 = inputString.indexOf(':', ind1+1 );   //finds location of second :
        String option = inputString.substring(ind1+1, ind2);
        option.trim();
        value = inputString.substring(ind2+1, inputString.length()).toInt();
        
        if (option == "OUT") 
          manualOverrideValve = OutflowPin;
        else if (option == "IN")
          manualOverrideValve = InflowPin;

        if(value) valveOnTimeManual = value;

        
        {
          startTimeOverride = millis();
          digitalWrite(manualOverrideValve, HIGH);
          valveManualOverride = true;

        }


        
        Serial.print("LOOPLOG,");
        Serial.print("Manually Switching on valve:");
        Serial.print(option);
        Serial.print((manualOverrideValve==InflowPin)?" - IN":" - OUT");
        Serial.print("; Switching ON for (ms): ");
        Serial.println(valveOnTimeManual);
      }
      else if (command == "VALVE_OFF")
      {
        String option = inputString.substring(ind1+1, inputString.length());
        option.trim();
        if (option == "OUT") 
          manualOverrideValve = OutflowPin;
        else if (option == "IN")
          manualOverrideValve = InflowPin;
        valveOffManual = false;
        Serial.print("LOOPLOG,");
        Serial.print("Manually Switching off valve");
        Serial.print(":");
        Serial.print(option);
        Serial.print(" - ");
        Serial.println((manualOverrideValve==InflowPin)?"IN":"OUT");
      }
      else if (command == "LOOP_OFF")
      {
        switchOffLoop = true;
        Serial.println("Switching OFF loop");
      }
      else if (command == "LOOP_ON")
      {
        switchOffLoop = false;
        Serial.println("Switching ON loop");
      }
      else
      {
        value = inputString.substring(ind1+1, inputString.length()).toInt();
        inputString.trim();
        Serial.print("Invalid Command");
        Serial.print(" \"");
        Serial.print(command);
        Serial.print(":");
        Serial.print(value);
        Serial.print("\",");
        Serial.println(inputString);
      }
      inputString = "";
    }
  }
}
