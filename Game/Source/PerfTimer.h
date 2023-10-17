#ifndef __PERFTIMER_H__
#define __PERFTIMER_H__

#include "Defs.h"

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64_t ReadTicks() const;

private:
	uint64_t startTime;
	static uint64_t frequency;
};

#endif //__PERFTIMER_H__