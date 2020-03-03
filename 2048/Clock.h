#pragma once
#include <corecrt_math.h>
#include <stdint.h>
#include "SDL-2.0.7/include/SDL_timer.h"

class SimulationClock
{
	using TimePoint_t = decltype(SDL_GetTicks());

public:

	TimePoint_t nextStep();

	void restart();

	void setWorldTime(uint32_t wTime);


	TimePoint_t getDeltaTime() const;

	TimePoint_t getWorldTime() const;

private:

	TimePoint_t first = 0;

	TimePoint_t last = 0;

	TimePoint_t deltaTime = 0;

	TimePoint_t timestep = 1000 / 60;

};