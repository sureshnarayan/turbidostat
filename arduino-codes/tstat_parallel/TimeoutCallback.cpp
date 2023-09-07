#include <Arduino.h>
#include "TimeoutCallback.h"

// Constructor - resets all the variables
TimeoutCallback::TimeoutCallback()
{
	_startTime = 0;
	_timeout = 0;
	_isRunning = false;
	_param2 = 0;
}

TimeoutCallback::~TimeoutCallback()
{
}

// Checks whether the timeout has elapsed. You need to call this method periodically
int TimeoutCallback::Update(void)
{
	if (!_isRunning) return 0;

	if (millis() - _startTime >= _timeout)
	{
		_isRunning = false;
		return _param2;
	}
	return 0;
}

// Stores the name of the method to call back
// void TimeoutCallback::Setup(CallbackType callbackFunction)
// {
// 	Setup(callbackFunction, nullptr, 0);
// }

// Stores the name of the method to call back
void TimeoutCallback::Setup(int param2)
{
	_isRunning = false;
	_param2 = param2;
}

// Starts the timer looking for timeout to expire


// Stores the name of the method to call back and starts the timer looking for timeout to expire
// void TimeoutCallback::Start(CallbackType callbackFunction, unsigned int timeout)
// {
// 	Start(callbackFunction, nullptr, 0, timeout);
// }

// Stores the name of the method to call back and starts the timer looking for timeout to expire
void TimeoutCallback::Start(int param2, unsigned int timeout)
{
	Setup(param2);
	//Start(timeout);
	_isRunning = true;
	_startTime = millis();
	_timeout = timeout;
}

bool TimeoutCallback::IsRunning(){
    return _isRunning;
}