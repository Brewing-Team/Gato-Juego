#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"

class Timer
{
public:

	// Constructor
	Timer();
	Timer(uint32_t startTime);

	void Start();
	uint32_t ReadSec() const;
	float ReadMSec() const;

private:
	uint32_t startTime;
};

#endif //__TIMER_H__