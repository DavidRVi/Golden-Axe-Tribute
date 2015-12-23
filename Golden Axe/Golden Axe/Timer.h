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

private:
	float timeLimit;
	float initTime;
};