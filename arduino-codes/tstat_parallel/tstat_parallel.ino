#include <LiquidCrystal.h>
#include <TimerOne.h>
#include "turbidostat.h"
#include <string.h>


#define NumTstat 5


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


/*
#define PIN_A0   (14)
#define PIN_A1   (15)
#define PIN_A2   (16)
#define PIN_A3   (17)
#define PIN_A4   (18)
#define PIN_A5   (19)
#define PIN_A6   (20)
#define PIN_A7   (21)
*/

//init 5 Turbidostat array with all their settings
//Turbidostat(sensorPin, inflowPin, outflowPin, setPoint)
Turbidostat tstat[NumTstat] = {
    Turbidostat(1, 14, 1, 2, 500),
    Turbidostat(2, 15, 1, 2, 500),
    Turbidostat(3, 16, 1, 2, 500),
    Turbidostat(4, 17, 1, 2, 500),
    Turbidostat(5, 18, 1, 2, 500)
};

unsigned long startTime = 0;

String inputString = "";         // a String to hold incoming data
// char inputString [60];
bool stringComplete = false;  // whether the string is complete


void setup(){
    //lcd init

    //sensor value init

    //pin mode and pin level init 

    //timer init for sensing

    //TODO comm init - send hello signal and act on init

    //tstat init - having comm

    lcd.begin(16, 2);
    Serial.begin(9600);
    
    lcd.setCursor(0, 0);
    lcd.print("  TURBIDOSTAT   ");
    lcd.setCursor(0, 1);
    lcd.print("      V3        ");
    delay(500);
    
    for(int i=0; i<NumTstat; i++){
        tstat[i].init();
    }
    
    Timer1.initialize(300000);// every 300ms
    Timer1.attachInterrupt(sensorRead);// sense and log
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
    //  get od and compare
    //  take action - timeout function call
    //  timer loop
    for (int i = 0; i < NumTstat; i++)
        tstat[i].update();

    //Get health periodically
    if (millis() - startTime >= 10000){
        getHealth();
    }

}

void getHealth(){
    //send health data of each tstat
    // ON/OFF, setpoint, sensor, IN&OUT(ON/OFF/TIMER)
    //setTimeout event to call it again
    
    // send health
    for (int i = 0; i < NumTstat; i++)
        tstat[i].getHealth();

    startTime = millis();

}

void serialEvent(){
    //TODO
    //unpack command and send to tstat
    // refresh, switchON/OFF flow manually, setpoint change, ON/OFF
    // change config - in the beginning and anytime inbetween

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
            command = inputString.substring(0, param1).trim();
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

}



/*
Parallel loop:

Initial presetter
Update loop of each Tstat
Log health/status of each Tstat - only the ones that are ON/running 
Parser for each input, and corresponding return printing if required


*/