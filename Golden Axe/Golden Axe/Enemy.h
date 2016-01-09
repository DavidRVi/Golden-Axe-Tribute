#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Module.h"
#include "Globals.h"
#include "Animation.h"
#include "Point.h"

class Collider;
struct SDL_Rect;

enum state { IDLE = 0, FORWARD, BACKWARD, UP, DOWN, UP_FORWARD, UP_BACKWARD, DOWN_FORWARD, DOWN_BACKWARD, ATTACKING };
class Enemy : public Module {
private:
	SDL_Rect pivot;
	int enemyHeight;

	state current_state;
	bool forward_walking;
public:

	Enemy();
	~Enemy();

	bool CleanUp();
	bool Draw();
	bool OnCollision(Collider* a, Collider* b);

	void SetPosition(int x, int y);
	int GetScreenHeight();

	SDL_Rect idle;
	Animation forward;

	SDL_Rect chargeAttack;
	Animation idleAttack;
	Animation kickAttack;
	Collider* pivotCol;
	Collider* hitBoxCol;


};

#endif