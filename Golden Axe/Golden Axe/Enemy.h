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

enum Enemy_state { EIDLE = 0, EFORWARD, EBACKWARD, EUP, EDOWN, EUP_FORWARD, EUP_BACKWARD, EDOWN_FORWARD, EDOWN_BACKWARD, EATTACKING, ECHARGING, EFALLING_DOWN, ELAY_DOWN, ERECOVERING, EHIT, EDYING};
class Enemy : public Module {
private:
	SDL_Rect pivot;
	int enemyHeight;
	Enemy_state current_state;
	bool forward_walking;
	int lifePoints;
	int y_limit;
	Timer* fallingTimer;
	Timer* hitTimer;
	Timer* attackTimer;
	Timer* chargeTimer;
	Timer* idleTimer;
	bool hasHit;
	bool westLocked;
	bool eastLocked;

	int charge_it;
	ModulePlayer* player;
	bool debug;

	unsigned int fallFx;
	unsigned int dyingFx;


	int GetFallHeight(int i) const;
	bool inRange(int y) const;
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
	Animation dying;
	SDL_Rect falling_down;
	SDL_Rect lay_down;
	SDL_Rect recovery;
	SDL_Rect chargeAttack;
	Animation idleAttack;
	Collider* attackCol;
	Collider* chargeAttackCol;
	Collider* pivotCol;
	Collider* hitBoxCol;


};

#endif