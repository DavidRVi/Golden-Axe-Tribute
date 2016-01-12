#pragma once
#include <vector>

using namespace std;
struct SDL_Rect;
class Animation
{
public:
	float speed;
	vector<SDL_Rect> frames;

private:
	float current_frame;

public:
	Animation() : frames(), speed(1.0f), current_frame(0.0f)
	{}

	SDL_Rect& GetCurrentFrame()
	{
		current_frame += speed;
		if (current_frame >= frames.size())
			current_frame = 0.0f;
		return frames[(int)current_frame];
	}

	void resetAnimation()
	{
		current_frame = 0.0f;
	}

	SDL_Rect GetActualFrame()
	{
		return frames[(int)current_frame];
	}
};