#include "Clock.h"
#include <stdio.h>

SimulationClock::TimePoint_t SimulationClock::nextStep()
{
	if (first == 0)
		restart();

	int32_t sleepDuration = timestep - (SDL_GetTicks() - last);

	if(sleepDuration > 0)
		SDL_Delay(sleepDuration);

	TimePoint_t now = SDL_GetTicks();

	deltaTime = now - last;
	last = now;

	return deltaTime;
}

void SimulationClock::restart()
{
	first = SDL_GetTicks();
	last = first;
	deltaTime = 0;
}

void SimulationClock::setWorldTime(uint32_t wTime)
{
	first = SDL_GetTicks() - wTime;
}

SimulationClock::TimePoint_t SimulationClock::getDeltaTime() const
{
	return deltaTime;
}

SimulationClock::TimePoint_t SimulationClock::getWorldTime() const
{
	return last - first;
}
