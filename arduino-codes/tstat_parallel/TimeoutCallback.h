#ifndef TimeoutCallbacks_h
#define TimeoutCallbacks_h 


// Code taken from https://github.com/notisrac/TimeoutCallback/blob/master/TimeoutCallback.h

class TimeoutCallback
{
public:
	TimeoutCallback();
	~TimeoutCallback();
	void Setup(int param2);
	int Update(void);
	//void Start(unsigned int timeout);
	void Start(int param2, unsigned int timeout);
	bool IsRunning();
private:
	unsigned long _startTime;
	int _param2;
	unsigned int _timeout;
	bool _isRunning;
};
#endif




