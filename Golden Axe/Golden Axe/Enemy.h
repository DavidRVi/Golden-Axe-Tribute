#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Module.h"
#include "Globals.h"
#include "Animation.h"
#include "Point.h"

class ModulePlayer;
class Collider;
struct SDL_Rect;
class Timer;

enum Enemy_state { EIDLE = 0, EFORWARD, EBACKWARD, EUP, EDOWN, EUP_FORWARD, EUP_BACKWARD, EDOWN_FORWARD, EDOWN_BACKWARD, EATTACKING, EFALLING_DOWN, ELAY_DOWN, ERECOVERING, EHIT};
class Enemy : public Module {
private:
	SDL_Rect pivot;
	int enemyHeight;

	Enemy_state current_state;
	bool forward_walking;

	int lifePoints;

	Timer* falling_timer;
	Timer* hit_timer;
	Timer* attackTimer;
	int charge_it;
	int GetFallHeight(int i) const;
	bool inRange(int y) const;

	ModulePlayer* player;

	bool debug;

	int y_limit;
public:

	Enemy(int y, int h);
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
	Collider* attackCol;
	Collider* pivotCol;
	Collider* hitBoxCol;


};

#endif