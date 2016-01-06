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

private:
	Collider* westWall;
	Collider* southWall;
	Collider* eastWall;

	Collider* cameraTrigger;

	int triggerCount;
	bool spawnMonsters;

	Animation go;
	unsigned int go_fx;
	bool show_go;
	Timer* go_timer;

	int encounterCount;

};
#endif