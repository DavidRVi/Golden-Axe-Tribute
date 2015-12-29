#ifndef __MODULEPLAYER_H__
#define __MODULEPLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "Globals.h"
#include "Timer.h"

struct SDL_Texture;
class Collider;

enum state { IDLE=0, FORWARD, BACKWARD, UP, DOWN, UP_FORWARD, UP_BACKWARD, DOWN_FORWARD, DOWN_BACKWARD, RUN_FORWARD, RUN_BACKWARD, JUMPING };
class ModulePlayer : public Module
{
public:
	ModulePlayer(bool enabled = false);
	~ModulePlayer();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	bool OnCollision(Collider* a, Collider* b);

public:

	SDL_Texture* graphics = nullptr;
	SDL_Rect idle;
	Animation forward;
	Animation up;
	Animation run;
	Animation jump;

	SDL_Rect pivot;
	unsigned int attack_fx;

private:
	state current_state;
	bool forward_walking;
	bool repaint_frame;
	SDL_Rect last_frame;
	Timer* idle_timer;

	int player_height;

	Collider* pivotCol;
	
};

#endif