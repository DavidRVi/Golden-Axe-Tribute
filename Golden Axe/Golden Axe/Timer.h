#ifndef __TIMER_H__
#define __TIMER_H__

#include "SDL/include/SDL.h"

class Timer
{
public:
	Timer() {}

	Timer(float value) {
		initTime = SDL_GetTicks();
		timeLimit = value;
	}

	~Timer() {};

	bool hasPassed() {
		return (SDL_GetTicks() - initTime) >= timeLimit;
	}

	void resetTimer() {
		initTime = SDL_GetTicks();
	}

private:
	float timeLimit;
	float initTime;
};

#endif