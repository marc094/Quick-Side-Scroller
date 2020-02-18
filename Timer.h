#ifndef _TIMER_H_
#define _TIMER_H_
#include "Defs.h"
class Timer
{
public:
	Timer();
	virtual ~Timer();

	void Start();
	uint32 Readms() const;
	float ReadSec() const;
	bool CountSec(float num);
	bool Countms(float num);
	void Set(float num);
	uint32 Started() const;

private:
	uint32	startTime;
};

#endif