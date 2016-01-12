#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Module.h"
#include "Globals.h"
#include "Animation.h"
#include "Point.h"

class Collider;
struct SDL_Rect;
class Timer;

enum state { IDLE = 0, FORWARD, BACKWARD, UP, DOWN, UP_FORWARD, UP_BACKWARD, DOWN_FORWARD, DOWN_BACKWARD, ATTACKING, FALLING_DOWN, LAY_DOWN, RECOVERING};
class Enemy : public Module {
private:
	SDL_Rect pivot;
	int enemyHeight;

	state current_state;
	bool forward_walking;

	int lifePoints;

	Timer* falling_timer;
	int charge_it;
	int GetFallHeight(int i) const;
public:

	Enemy();
	~Enemy();

	bool CleanUp();
	update_status PreUpdate();
	update_status Update();
	bool Draw();
	bool OnCollision(Collider* a, Collider* b);

	void SetPosition(int x, int y);
	int GetScreenHeight() const;

	Animation idle;
	Animation forward;
	SDL_Rect falling_down;
	SDL_Rect lay_down;
	SDL_Rect recovery;

	SDL_Rect chargeAttack;
	Animation idleAttack;
	Animation kickAttack;
	Collider* pivotCol;
	Collider* hitBoxCol;


};

#endif