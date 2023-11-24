#include <stdio.h>
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


init():
id=
preset=
timer=

sp=
running=ON/paused/OFF


*/

// TODO
Turbidostat::Turbidostat(int id, int sensorPin, int inflowPin, int outflowPin, int setPoint = 200){
    this->id = id;
    this->preset = preset;
    this->timer = timer;
    this->setPoint = setPoint;
    this->sensorPin = sensorPin;
    this->inflowPin = inflowPin;
    this->outflowPin = outflowPin;
    this->isInflowRunning = false;
    this->isOutflowRunning = false;
    this->isDilutionRunning = false;
    this->logCommPacket = []; 
    // this->timer = TimeoutCallback();
}

// TODO
void Turbidostat::init(){

    // Resting state
    pinMode(this->inflowPin, OUTPUT);
    pinMode(this->outflowPin, OUTPUT);
    digitalWrite(this->inflowPin, HIGH);
    digitalWrite(this->outflowPin, HIGH);

    // Get OD
    this->currentOD = this->filter.add(analogRead(this->sensorPin));  // filter add...

    // Set state of Tstat, also called when reset

    return;
}

//Should detect all sensor data and status and loop position
//    getter for all the sensors 
int Turbidostat::readSensor(){
    int reading = analogRead(this->sensorPin);
    this->currentOD = this->filter.add(reading);
    return reading;
}

int Turbidostat::getOD(){
    return this->currentOD;
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

/*
boolean Turbidostat::runLoop(boolean button){
    if(!this->running) return true;

    this->inflow->timerloop;

    if(this->currentOD < this->setPoint 
       && !this ->inflow->isRunning() 
       && !this ->outflow->isRunning() ){
        //Dilution protocol
        //WRONG!!!!!
        //There is only one timer at any given point for a given flow control
        this->outflow->switchON();
        this->outflow->switchOFFAfter(3000);
        this->inflow->switchONAfter(3000);
        this->inflow->switchOFFAfter(3000 + 2000);
        //wait for 15 seconds


        //Maybe a single timer per this that keeps returning the call back with a queue value
        //and you increment it everytime it comes to the callback
    }
    else if(){

    }
}
*/

// Should on or off the motors
// setter for motor states
boolean Turbidostat::startInflow(){
    digitalWrite(this->inflowPin, HIGH);
    this->isInflowRunning = true;
}

boolean Turbidostat::stopInflow(){
    digitalWrite(this->inflowPin, LOW);
    this->isInflowRunning = false;
}

boolean Turbidostat::startOutflow(){
    digitalWrite(this->outflowPin, HIGH);
    this->isOutflowRunning = true;
}

boolean Turbidostat::stopOutflow(){ 
    digitalWrite(this->outflowPin, LOW);
    this->isOutflowRunning = false;
}

// TODO
// generator of log strings
char* Turbidostat::getHealth(){
    // ON/OFF, setpoint, sensor, IN&OUT(ON/OFF/TIMER)

    snprintf(this->healthBuffer, sizeof(this->healthBuffer), 
    "T%d:%s,SP:%d,OD:%d,In:%s,OUT:%s,DIL:%s", 
    this->id, this->running?"ON":"OFF",
    this->setPoint,
    this->currentOD,
    this->isInflowRunning?"ON":"OFF",
    this->isOutflowRunning?"ON":"OFF",
    this->isDilutionRunning?"ON":"OFF");
    //need to figure out timer log (atleast for debugging)

    Serial.println(this->healthBuffer);

    return this->healthBuffer;
}

// Should do the schedule as per preset
//     Need a way to take in the preset - todo - should it be an array/object
//     The preset could be a bunch of alphabets signifying the dilution protocol
//     Option: Start, Stop, Reset, Play, Pause, O0, O1, I0, I1, W,  [O0, W100, O1, W30000] etc, need to remember the previous state, current and next
//     have a scheduler/event loop - with timer as a loggable/resettable - with a status string
void Turbidostat::scheduler(char preset = 'P'){
    //Switch for different tasks - each of them addressed by their numbers
    // each will be incremented circularly in the beginning of the loop
    // Each will execute their role and then call the timer with the callback as the scheduler with their current caller's index as the parameter
    // We will also need to send the this number as the parameter - if this were an independent utility: while that is fine and things keep going on as the baton keeps getting passed around. There is no ownership of the schedule and no access to it.
    //So a better way to do it would be to have an object called the scheduler which owns the timer, knows the current state of the timing loop(index), perhaps even the time left, the index to be passed around, and the turbidostat number(that need not be known, since the callback is unique)

    switch(preset){
        case 'P': // Pause/Stop

            break;

        case 'O': // Output Valve

            break;

        case 'I': // Input Valve

            break;

        case 'W': // Waiting Time

            break;
    }

    switch(index){
        case 1 :
            //Outflow ON
            //start timer and set callback of scheduler with index 2 and turbidostat number
            // switch on loop scheduler running flag
            this->startOutflow();
            this->timer.Start(2, 3000);
            this->isDilutionRunning = true;
            break;
        case 2 :
            //stop outflow 
            // wait if necessary
            this->stopOutflow();
            this->timer.Start(3, 100);
            break;
        case 3 :
            //start inflow
            //start timer and set callback of scheduler with index 4 and turbidostat number
            this->startInflow();
            this->timer.Start(4, 2000);
            break;
        case 4 :
            //stop inflow
            //start timer and set callback of scheduler with index 0 and turbidostat number
            this->stopInflow();
            this->timer.Start(5, 30000);
            break;
        case 5:
            // switch off loop scheduler running flag
            this->isDilutionRunning = true;
        case -1 :
            // TODO: Switch off dilution when commanded and stop all the timers
        default:
            break;
    }
    return;
}

// Should Loop between multiple entities of the preset and decide to wait otherwise
// Should keep track of the current state and the previous state, current time, wait time
// Should be able to play/Pause from here
void Turbidostat::update(){
    if(this->running){
        if(this->currentOD < this->setPoint)
            if(!this->isDilutionRunning) this->scheduler(1);
        int index = this->timer.Update();
        if(index) this->scheduler(index);
    }


}

// Should be able to interact
//     way to handle user input with switch case and doing corresponding actions
//     (no actual variables here)
//     Return acknowledgement after interaction
//     set state - can start from any point
char* Turbidostat::interact(char * command){

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



//Need a loop function that checks OD and calls scheduler after checking if running
//


'''
TODO
1. State tracker and change
2. Preset handling
3. Loop scheduler
4. Interaction - break string
5. Handle different commands
6. Response for interaction


Hard to start: 
so start minimally
Get a minimum loop running for five of them
Then slowly build on top of it
At least, it will always be running


'''



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


init():
id=
preset=
timer=

sp=
running=ON/paused/OFF


*/