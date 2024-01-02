#include <LiquidCrystal.h>
#include <TimerOne.h>
#include "turbidostat.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//init 5 Turbidostat array with all their settings
//Turbidostat(id, running, sensorPin, inflowPin, outflowPin, setPoint)
#define NumTstat 5
Turbidostat tstat[NumTstat] = {
    Turbidostat(1,  true, 15, 47, 45, 500),
    Turbidostat(2,  true, 13, 43, 41, 500),
    Turbidostat(3, false, 11, 39, 37, 500),
    Turbidostat(4, false,  9, 35, 33, 500),
    Turbidostat(5, false,  7, 31, 29, 500)
};

unsigned long startTimeHealth = 0;

// char inputString[100];         // a String to hold incoming data
// String inputString = "";
char command[100];
char argument[100];
bool stringComplete = false;  // whether the string is complete

// Define the maximum length of the input string
const int maxInputLength = 100;
char inputString[maxInputLength];


void setup(){
    //lcd init

    //sensor value init

    //pin mode and pin level init 

    //timer init for sensing

    //TODO comm init - send hello signal and act on init

    //tstat init - having comm

    Serial.begin(9600);

    lcd.begin(16, 2);    
    lcd.setCursor(0, 0);
    lcd.print("  TURBIDOSTAT   ");
    lcd.setCursor(0, 1);
    lcd.print("      V3        ");
    delay(200);
    
    for(int i=0; i<NumTstat; i++){
        tstat[i].init();
    }
    
    Timer1.initialize(300000);// every 300ms
    Timer1.attachInterrupt(sensorRead);// sense and log

    // inputString.reserve(200);
}

void sensorRead(void)
{
    for(int i=0; i < NumTstat; i++){
        tstat[i].readSensor();
    }
}

void loop(){

    //lcd print
    delay(100);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(tstat[0].getOD());
    lcd.setCursor(6,0);
    lcd.print(tstat[1].getOD());
    lcd.setCursor(12,0);
    lcd.print(tstat[2].getOD());
    lcd.setCursor(0,1);
    lcd.print(tstat[3].getOD());
    lcd.setCursor(6,1);
    lcd.print(tstat[4].getOD());


    //For every tstat
    //  event loop
    for (int i = 0; i < NumTstat; i++)
        tstat[i].update();

    //Get health periodically
    if (millis() - startTimeHealth >= 10000){
        getHealth();
        startTimeHealth = millis();
    }

    if(stringComplete){
      Serial.println(inputString);
      parseAndPrint();
      stringComplete = false;
    }

}

void getHealth(){
    //send health data of each tstat
    // ON/OFF, setpoint, sensor, IN&OUT(ON/OFF/TIMER)
    for (int i = 0; i < NumTstat; i++)
        Serial.println(tstat[i].getHealth());
}

void serialEvent(){
    //TODO
    //unpack command and send to tstat
    // refresh, switchON/OFF flow manually, setpoint change, ON/OFF
    // change config - in the beginning and anytime inbetween


  // Read serial input into the character array until newline character ('\n') is encountered
    int index = 0;
    while (Serial.available() > 0) {
      char incomingChar = Serial.read();

      if (incomingChar == '\n') {
        // End of input, add null terminator
        inputString[index] = '\0';
        stringComplete = true;
        break;
      } else {
        // Add character to the input string
        inputString[index] = incomingChar;
        index++;
        stringComplete = false;

        // Check if the input exceeds the maximum length
        if (index >= maxInputLength - 1) {
          break;
        }
      }
    }
  
    /*
    while (Serial.available()) {
        String command = "";
        unsigned long value = 0;
        int tstat_index = 0;
        int param1 = 0;

        // get the new byte:
        char inChar = (char)Serial.read();
        inputString += inChar;
        if (inChar == '\n') {
            stringComplete = true;
            tstat_index = inputString.indexOf(':');  //finds location of first : (returns -1 if not)
            command = inputString.substring(0, param1);
            command.trim();
            param1 = inputString.indexOf(':');  //finds location of first : (returns -1 if not)
            command = inputString.substring(0, param1);
            command.trim();
            //param2 = readString.indexOf('\n', param1+1 );   //finds location of second ,
            if (command == "SP")
            {
                value = inputString.substring(param1+1, inputString.length()).toInt();
                if (value) ODSetpoint = value;
                Serial.print("Setpoint");
                Serial.print(",");
                Serial.println(ODSetpoint);
            }
            else if (command == "IN")
            {
                value = inputString.substring(param1+1, inputString.length()).toInt();
                if (value) inflowTime = value;
                Serial.print("Inflow ON for");
                Serial.print(",");
                Serial.println(inflowTime);
            }
            else if (command == "OUT")
            {
                value = inputString.substring(param1+1, inputString.length()).toInt();
                if (value) outflowTime = value;
                Serial.print("Outflow ON for");
                Serial.print(",");
                Serial.println(outflowTime);
            }
            else if (command == "VALVE_ON")
            {
                int param2 = inputString.indexOf(':', param1+1 );   //finds location of second :
                String option = inputString.substring(param1+1, param2);
                option.trim();
                value = inputString.substring(param2+1, inputString.length()).toInt();
                
                if (option == "OUT") 
                manualOverrideValve = OutflowPin;
                else if (option == "IN")
                manualOverrideValve = InflowPin;

                if(value) valveOnTimeManual = value;

                
                {
                startTimeOverride = millis();
                digitalWrite(manualOverrideValve, LOW);
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
                String option = inputString.substring(param1+1, inputString.length());
                option.trim();
                if (option == "OUT") 
                manualOverrideValve = OutflowPin;
                else if (option == "IN")
                manualOverrideValve = InflowPin;
                valveOffManual = true;
                valveManualOverride = true;
            }
            else if (command == "LOOP")
            {
                String option = inputString.substring(param1+1, inputString.length());
                option.trim();
                if(option == "OFF")
                switchOffLoop = true;
                else if (option == "ON")
                switchOffLoop = false;

                if(switchOffLoop)
                Serial.println("Tubidostat loop OFF");
                else
                Serial.println("Tubidostat loop ON");
            }

                    
            else
            {
                value = inputString.substring(param1+1, inputString.length()).toInt();
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
    */

}

void parseAndPrint() {
  // Print each part of the input string

  char* token = strtok(inputString, ":");
  
  if (token != NULL) {
    // Extract command
    int tstat_index = atoi(token) - 1;

    // Move to the next token
    token = strtok(NULL, ":");

    // Extract parameter 1
    if (token != NULL) {
      strncpy(command, token, maxInputLength);
      command[maxInputLength - 1] = '\0'; // Ensure null-termination
    }

    // Move to the next token
    token = strtok(NULL, ":");

    // Extract parameter 2
    if (token != NULL) {
      strncpy(argument, token, maxInputLength);
      argument[maxInputLength - 1] = '\0'; // Ensure null-termination
    }

    while (token != NULL) 
      token = strtok(NULL, ":");

    tstat[tstat_index].interact(command, argument);

    // Serial.println(tstat_index);
    // Serial.println(command);
    // Serial.println(argument);
    // Serial.println(bool(strcmp("cmd",command)));
    // Serial.println(bool(strcmp("arg", argument)));
  }

}


/*
Parallel loop:

Initial presetter
Update loop of each Tstat
Log health/status of each Tstat - only the ones that are ON/running 
Parser for each input, and corresponding return printing if required


*/