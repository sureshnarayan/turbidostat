#ifndef TURBIDOSTAT_H
#define TURBIDOSTAT_H


#include "TimeoutCallback.h"
#include "MovingAverage.h"


class Turbidostat {

// Turbidostat objects
// Has inflow, outflow, OD, setpoint, status
// 

public:

    Turbidostat(int id, int sensorPin, int inflowPin, int outflowPin, int setPoint);

    void init();

    int getOD();

    int readSensor();

    boolean isRunning();

    boolean setSetpoint(int setpoint);

    boolean switchONLoop();

    boolean switchOFFLoop();

    boolean startInflow();

    boolean stopInflow();

    boolean startOutflow();

    boolean stopOutflow();

    void scheduler(int index);

    boolean IsDilutionProtocolRunning();

    void update();

    char* getHealth();




protected:


private:

    int id;
    boolean running = false;
    int currentOD;
    int setPoint;
    int  inflowPin, outflowPin;
    boolean isInflowRunning, isOutflowRunning;
    int sensorPin;

    char healthBuffer [60];
    // Buffer (and added samples) will be initialised as uint8_t, total 16 samples
    MovingAverage <uint8_t, 10> filter;
    TimeoutCallback timer;
    boolean isDilutionRunning;
};


#endif