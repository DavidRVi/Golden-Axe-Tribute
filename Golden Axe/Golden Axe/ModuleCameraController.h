#ifndef __MODULECAMERACONTROLLER_H__
#define __MODULECAMERACONTROLLER_H__

#include "Module.h"
#include "Animation.h"
#include "Timer.h"

class Collider;
class Animation;

class ModuleCameraController : public Module {
public:
	ModuleCameraController(bool enabled = false);
	~ModuleCameraController();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	bool OnCollision(Collider* a, Collider* b);
	void ShowGoAnimation();

public:
	SDL_Texture* interface = nullptr;
	SDL_Texture* magicGr = nullptr;

	Collider* westWall;
	Collider* southWall;
	Collider* eastWall;

private:
	Collider* cameraTrigger;
	Collider* magicFlaskCol;

	int triggerCount;
	bool spawnMonsters;

	Animation go;
	unsigned int go_fx;
	bool show_go;
	Timer* go_timer;

	SDL_Rect stage;
	SDL_Rect magic;
	SDL_Rect charPortrait;
	SDL_Rect life_3;
	SDL_Rect life_2;
	SDL_Rect life_1;
	SDL_Rect lifeBar;
	SDL_Rect magicFlask;
	SDL_Rect gameOver;
	SDL_Rect lightning;
	int lightning_it;
	bool lightning_back;

	int encounterCount;

};
#endif