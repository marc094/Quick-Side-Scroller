#ifndef __TIMER_H__
#define __TIMER_H__

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