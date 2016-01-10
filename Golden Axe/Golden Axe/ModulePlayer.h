#ifndef __MODULEPLAYER_H__
#define __MODULEPLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include "Globals.h"
#include "Timer.h"

struct SDL_Texture;
class Collider;

enum state { IDLE=0, FORWARD, BACKWARD, UP, DOWN, UP_FORWARD, UP_BACKWARD, DOWN_FORWARD, DOWN_BACKWARD, RUN_FORWARD, RUN_BACKWARD, JUMPING, ATTACKING, WAITING_INPUT };
enum AttackState { NONE = 0, JUMPATTACK, IDLEATTACK, CHARGEATTACK, COMBO_2, COMBO_3, COMBO_4, COMBO_5, PICKUP };

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

	int GetScreenHeight();

	bool Draw();

	int GetLifeBars();
	int GetLives();
	int GetMagicFlasks();

	void AddMagicFlask();

public:

	SDL_Texture* graphics = nullptr;
	SDL_Rect idle;
	Animation forward;
	Animation up;
	Animation run;

	SDL_Rect jumpattack;
	Animation idleattack;
	Animation combo2;
	SDL_Rect chargeattack;
	SDL_Rect waitingCombo2;

	SDL_Rect jump_up;
	SDL_Rect jump_down;

	SDL_Rect pivot;
	unsigned int attack_fx;

private:
	state current_state;
	bool forward_walking;
	bool repaint_frame;
	SDL_Rect last_frame;
	Timer* idle_timer;


	Timer* run_timer;
	bool isRunning;
	
	bool northLocked;
	bool southLocked;
	bool eastLocked;
	bool westLocked;

	int player_height;
	int charge_it;

	int jump_it;
	int alpha_jump;
	int max_jumpheight;
	bool jumping_up;

	Collider* pivotCol;
	Collider* hitBoxCol;

	Collider* chargeAttackCol;
	Collider* idleAttackCol;
	Collider* jumpAttackCol;

	AttackState attackState;
	Timer* attackAnimation;
	Timer* attackWindow;
	Timer* jumpAttackWindow;
	Timer* chargeAttackTimer;		//Indicates the time it takes the charge attack to complete
	bool hasHit;

	int getJumpHeight(int i);
	int getChargeHeight(int i);
	
	int lifeBars;
	int lives;
	int magicFlasks;
};

#endif