#ifndef __MODULESCENELEVEL1_H__
#define __MODULESCENELEVEL1_H__

#include "Module.h"

class Collider;

enum LevelState { BATTLE, NO_BATTLE };
class ModuleSceneLevel1 : public Module {
public:
	ModuleSceneLevel1(bool enabled = false);
	~ModuleSceneLevel1();

	bool Start();
	update_status Update();
	bool CleanUp();

	void ChangeState(LevelState state);
	void spawnEnemies();			//Always spawn 2 enemies right of the screen

public:

	SDL_Texture* background = nullptr;
	SDL_Rect ground;
	SDL_Rect parallax;

private:

	Collider* west_wall;
	Collider* centre_wall;
	Collider* east_wall;

	LevelState current_state;

};

#endif