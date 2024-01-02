// #include <stdio.h>
#include <Arduino.h>
#include "turbidostat.h"

/*
Tstat object:
Should on or off the motors
    setter for motor states
Should detect all sensor data and status and loop position
    getter for all the sensors
    generator of log strings
Should do the schedule as per preset
    Need a way to take in the preset - todo - should it be an array/object
    The preset could be a bunch of alphabets signifying the dilution protocol
    Option: Start, Stop, Reset, Play, Pause, O0, O1, I0, I1, W,  [O0, W100, O1, W30000] etc, need to remember the previous state, current and next
    have a scheduler/event loop - with timer as a loggable/resettable - with a status string
Should be able to play pause
    setter for loop play/pause/reset
Should be able to interact
    way to handle user inp ut with switch case and doing corresponding actions
    (no actual variables here)
    Return acknowledgement after interaction
    set state - can start from any point


*/

// TODO
Turbidostat::Turbidostat(int id, bool running, int sensorPin, int inflowPin, int outflowPin, int setPoint = 200){
    this->id = id;
    this->timer = timer;
    this->setPoint = setPoint;
    this->sensorPin = sensorPin;
    this->inflowPin = inflowPin;
    this->outflowPin = outflowPin;
    this->isInflowRunning = false;
    this->isOutflowRunning = false;
    this->isDilutionRunning = false;
    // this->logCommPacket = {'\0'};
    //this->preset = ['L0', 'O1','W1000','O0', 'I1', 'W2000', 'I0', 'L1'];
    //this->protocolState = {};
    //this->protocolTime  = {};
    this->state = 0;
    this->override = false;
    this->running = running;
    // this->timer = TimeoutCallback();
}

// TODO
void Turbidostat::init(){

    // Resting state
    pinMode(this->inflowPin, OUTPUT);
    pinMode(this->outflowPin, OUTPUT);
    stopInflow();
    stopOutflow();

    // Get OD
    // this->currentOD = this->filter.add(analogRead(this->sensorPin));  // filter add...

    // Set state of Tstat, also called when reset
    char preset[100] = "L0, O1, W1000, O0, I1, W2000, I0, L1";
    parsePreset(preset);

    this->filter.add(2000);

    return;
}

//Should detect all sensor data and status and loop position
//    getter for all the sensors
int Turbidostat::readSensor(){
    int reading = analogRead(this->sensorPin);
    this->filter.add(reading);
    // this->currentOD = this->filter.get();
    return getOD();
}

int Turbidostat::getOD(){
    return this->filter.get();
}

boolean Turbidostat::isRunning(){
    return this->running;
}

boolean Turbidostat::setSetpoint(int setpoint){
    this->setPoint = setpoint;
}

boolean Turbidostat::switchONLoop(){
    this->running = true;
}

boolean Turbidostat::switchOFFLoop(){
    this->running = false;
}

// Should on or off the motors
// setter for motor states
boolean Turbidostat::startInflow(){
    digitalWrite(this->inflowPin, LOW);
    this->isInflowRunning = true;
}

boolean Turbidostat::stopInflow(){
    digitalWrite(this->inflowPin, HIGH);
    this->isInflowRunning = false;
}

boolean Turbidostat::startOutflow(){
    digitalWrite(this->outflowPin, LOW);
    this->isOutflowRunning = true;
}

boolean Turbidostat::stopOutflow(){
    digitalWrite(this->outflowPin, HIGH);
    this->isOutflowRunning = false;
}

// generator of log strings
char* Turbidostat::getHealth(){
    // ON/OFF, setpoint, sensor, IN&OUT(ON/OFF/TIMER)

    snprintf(this->healthBuffer, sizeof(this->healthBuffer),
    "T%d:%s,SP:%d,OD:%d,In:%s,OUT:%s,DIL:%s,State:%c%d,Timer:%d",
    this->id, this->running?"ON":"OFF",
    this->setPoint,
    this->getOD(),
    this->isInflowRunning?"ON":"OFF",
    this->isOutflowRunning?"ON":"OFF",
    this->isDilutionRunning?"ON":"OFF",
    this->protocolState[this->state],
    this->protocolTime[this->state],
    this->countDownTimer );

    // Serial.println(this->healthBuffer);
    return this->healthBuffer;
}

void Turbidostat::scheduler(){
    // Parse string
    // the string will have no whitespaces - removed when storing preset
    // this->preset = ['L0', 'O1','W1000','O0', 'I1', 'W2000', 'I0', 'L1'];
    // char state = this->preset[this->state].charAt(0);
    // int num    = this->preset[this->state].substring(1).toInt();
    // bool done  = this->setstate(state, num);

    if (!this->override){
        bool done  = this->setstate(this->protocolState[this->state], this->protocolTime[this->state]);
        if (done){
            this->state++ ;
            if (this->protocolState[this->state] == '-' || this->state > (sizeof(this->protocolState)-1))
                this->state = 0;
        }
    }
    // What to do during override - continue as it is, or pause ??

}

// Input string like "L0, O1, W1000, O0, I1, W2000, I0, L1"
// Trim the spaces and put it into the arrays
void Turbidostat::parsePreset(char* preset){

  Serial.println(preset);
    // initialise both arrays
    for(int i=0; i<sizeof(this->protocolState); i++){
        this->protocolState[i] = '-';
        this->protocolTime[i]  = 0;
    }

    // // Get next command from Serial (add 1 for final 0)
    // char input[100 + 1];
    // byte size = Serial.readBytes(input, INPUT_SIZE);
    // // Add the final 0 to end the C string
    // input[size] = 0;

    // // Read each command pair 
    // int i=0;
    // char* command = strtok(input, ",");
    // while (command != '\n')
    // {
    //     // Split the command in two values
    //     char* separator = strchr(command, '-');
    //     if (separator != 0)
    //     {
    //         // Actually split the string in 2: replace ':' with 0
    //         *separator = 0;
    //         this->protocolState[i] = command;
    //         ++separator;
    //         this->protocolTime[i] = atoi(separator);

    //         // Do something with servoId and position
    //         i++;
    //     }
    //     // Find the next command in input string
    //     command = strtok(0, ",");
    // }

    // remove spaces
    // preset.replace(" ", "";)
    // split string by commas into a list

    // put first character into first array
    // put remaining into an integer into another array
    // append to the arrays
    
    char* token = strtok(preset, ", ");
    int i = 0;

    // while (token != NULL && i < 8) {
    while (token != NULL) {
      // Extract letter and value
      char letter = token[0];
      int value = atoi(token + 1);

      // Store in arrays
      this->protocolState[i] = letter;
      this->protocolTime[i] = value;

      // Move to the next token
      token = strtok(NULL, ", ");
      i++;
    }

    for(int i=0; i<sizeof(this->protocolState); i++){
        Serial.print(this->protocolState[i]);
        Serial.print(this->protocolTime[i]);
        Serial.print(", ");
    }
    Serial.println();
}


// Should do the schedule as per preset
//     Need a way to take in the preset - todo - should it be an array/object
//     The preset could be a bunch of alphabets signifying the dilution protocol
//     Option: Start, Stop, Reset, Play, Pause, O0, O1, I0, I1, W,  [O0, W100, O1, W30000] etc, need to remember the previous state, current and next
//     have a scheduler/event loop - with timer as a loggable/resettable - with a status string
boolean Turbidostat::setstate(char state = 'P', int num = 0){
    //Switch for different tasks - each of them addressed by their numbers
    // each will be incremented circularly in the beginning of the loop
    // Each will execute their role and then call the timer with the callback as the scheduler with their current caller's index as the parameter
    // We will also need to send the this number as the parameter - if this were an independent utility: while that is fine and things keep going on as the baton keeps getting passed around. There is no ownership of the schedule and no access to it.
    //So a better way to do it would be to have an object called the scheduler which owns the timer, knows the current state of the timing loop(index), perhaps even the time left, the index to be passed around, and the turbidostat number(that need not be known, since the callback is unique)

    boolean ifDone = false;

    switch(state){
        case 'P': // Play/Pause // Need to have an override option in the tstat //TODO LATER
            if (num == 0){
                this->override = true;
            }
            else{
                this->override = false;
            }
            break;

        case 'L': // Loop ON means monitoring
            if (num == 0) ifDone = true;
            else if(this->getOD() < this->setPoint) {ifDone = true;}
            break;

        case 'O': // Output Valve
            if (num == 0) stopOutflow();
            else startOutflow();

            ifDone = true;
            break;

        case 'I': // Input Valve
            if (num == 0) stopInflow();
            else startInflow();

            ifDone = true;
            break;

        case 'W': // Waiting Time millis and keep checking
            unsigned long currentTime;
            currentTime = millis();

            this->countDownTimer = num - (currentTime - startTime);
            
            if (!startTime){
              this->startTime = currentTime; //initialise
              ifDone = false;
              this->countDownTimer = num;
            }
            // else if ((currentTime - startTime) > num){
            else if (this->countDownTimer < 0){
              ifDone = true;
              this->startTime = 0;
              this->countDownTimer = 0;
            }
            break;
        default:
            break;
    }

    // Serial.print(state);
    // Serial.print(" ");
    // Serial.print(num);
    // Serial.print(" ");
    // // Serial.println(this->startTime);
    // Serial.println(this->countDownTimer);

    return ifDone;
}

// Should Loop between multiple entities of the preset and decide to wait otherwise
// Should keep track of the current state and the previous state, current time, wait time
// Should be able to play/Pause from here
void Turbidostat::update(){

    // Either off, or {monitoring, or diluting}
    if(this->running){
            this->scheduler();
    }

}

/*
// Should be able to interact
//     way to handle user input with switch case and doing corresponding actions
//     (no actual variables here)
//     Return acknowledgement after interaction
//     set state - can start from any point
char* Turbidostat::interact(char* command, char* argument){

    // Parse input and get command
    // have a switch for

    if (strcmp(command, 'ON') == 0){ // Switch On Tstat


    }
    else if (strcmp(command, 'OFF') == 0){ // Switch off Tstat


    }
    else if (strcmp(command, 'Play') == 0){ // Output Valve


    }
    else if (strcmp(command, 'Pause') == 0){ // Pause/Stop


    }
    else if (strcmp(command, 'Start Inlet Pump') == 0){ // Input Valve


    }
    else if (strcmp(command, 'Start Outlet Pump') == 0){ // Output Valve


    }
    else if (strcmp(command, 'Stop Inlet Pump') == 0){ // Input Valve


    }
    else if (strcmp(command, 'Stop Outlet Pump') == 0){ // Output Valve


    }
    else if (strcmp(command, 'Config') == 0){ // Waiting Time

            switch (config){
                case 'Preset':

                case 'SetPoint':

                case 'Frequency': // Min delay between dilutions

            }



        default:
        break;
    }
}

*/


/*
TODO
1. State tracker and change
2. Preset handling
3. Loop scheduler
4. Interaction - break string
5. Handle different commands
6. Response for interaction


*/



/*
Tstat object:
Should do the schedule as per preset
    Need a way to take in the preset - todo - should it be an array/object
    The preset could be a bunch of alphabets signifying the dilution protocol
    Option: Start, Stop, Reset, Play, Pause, O0, O1, I0, I1, W,  [O0, W100, O1, W30000] etc, need to remember the previous state, current and next
    have a scheduler/event loop - with timer as a loggable/resettable - with a status string
Should on or off the motors
    setter for motor states
Should detect all sensor data and status and loop position
    getter for all the sensors
    generator of log strings
Should be able to play pause
    setter for loop play/pause/reset
Should be able to interact
    way to handle user input with switch case and doing corresponding actions
    (no actual variables here)
    Return acknowledgement after interaction
    set state - can start from any point
*/

/*
Todo:
1. Parse string - get first character - for switch, remaining characters - to numbers
2. State maintenance - index along the protocol and cycle them
4. Timer handling - test and check - two timers - one based on millis and other on timer callback
3. Interaction - whether to respond back - but that is easy


What is to be done:
* Play pause override
* reset and initialisation - on disconnect
* commands and acknowledge
* debug state

*/