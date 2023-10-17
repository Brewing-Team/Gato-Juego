// ----------------------------------------------------
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"

#ifdef __linux__
#include <SDL_timer.h>
#elif _WIN32
#include "SDL\include\SDL_timer.h"
#endif

uint64_t PerfTimer::frequency = 0;

PerfTimer::PerfTimer()
{
	Start();
}

void PerfTimer::Start()
{
	frequency = SDL_GetPerformanceFrequency();
	startTime = SDL_GetPerformanceCounter();
}

double PerfTimer::ReadMs() const
{
	return ((double)(SDL_GetPerformanceCounter() - startTime) / frequency * 1000);
}

uint64_t PerfTimer::ReadTicks() const
{
	return SDL_GetPerformanceCounter() - startTime;
}
