#include "Timer.h"
#include "SDL\include\SDL_timer.h"

Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::Start()
{
	startTime = SDL_GetTicks();
}

uint32 Timer::Readms() const
{
	return SDL_GetTicks() - startTime;
}

float Timer::ReadSec() const
{
	return (SDL_GetTicks() - startTime) / 1000.0f;
}

bool Timer::CountSec(float num)
{
	return num <= (SDL_GetTicks() - startTime) / 1000.0f;
}

bool Timer::Countms(float num)
{
	return num <= SDL_GetTicks() - startTime;
}

void Timer::Set(float num)
{
	startTime = num;
}

uint32 Timer::Started() const
{
	return startTime;
}
