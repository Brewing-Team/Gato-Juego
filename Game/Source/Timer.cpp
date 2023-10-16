// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#ifdef __linux__
#include <SDL_timer.h>
#elif _WIN32
#include "SDL\include\SDL_timer.h"
#endif
	
Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	startTime = SDL_GetTicks();
}

uint32 Timer::ReadSec() const
{
	return (SDL_GetTicks() - startTime) / 1000;
}

float Timer::ReadMSec() const
{
	return (float)(SDL_GetTicks() - startTime);
}