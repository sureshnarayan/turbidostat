#ifndef TURBIDOSTAT_H
#define TURBIDOSTAT_H


#include "TimeoutCallback.h"
#include "MovingAverage.h"


class Turbidostat {



public:

    Turbidostat(int id, bool running,  int sensorPin, int inflowPin, int outflowPin, int setPoint);

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

    void scheduler();

    boolean IsDilutionProtocolRunning();

    void update();

    char* getHealth();

    boolean setstate(char state , int num);



protected:


private:

    int id;
    boolean running = false;
    // int currentOD;
    int setPoint;
    int  inflowPin, outflowPin;
    boolean isInflowRunning, isOutflowRunning;
    int sensorPin;
    char protocolState[11] = {'L', 'O', 'W' , 'O', 'I', 'W' , 'I',  'W', 'L', '-', '-'};
    int  protocolTime[11] =  {  0,   1, 1000,   0,   1, 2000,   0, 3000,   1,   0,   0};
    char *logCommPacket;
    boolean override;
    int state;
    unsigned long startTime = 0;
    int countDownTimer = 0;

    char healthBuffer [90];
    // Buffer (and added samples) will be initialised as uint8_t, total 16 samples
    MovingAverage <uint8_t, 10> filter;
    TimeoutCallback timer;
    boolean isDilutionRunning;

    void parsePreset(char* preset);

};


#endif