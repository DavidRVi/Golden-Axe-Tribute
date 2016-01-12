#ifndef __MODULESCENELEVEL1_H__
#define __MODULESCENELEVEL1_H__

#include <vector>
#include "Module.h"

class Collider;
class ModulePlayer;

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
	bool DeleteEnemy(const Module* enemy);	


public:

	SDL_Texture* background = nullptr;
	SDL_Rect ground;
	SDL_Rect parallax;

private:

	Collider* background_wall1;
	Collider* background_wall2;
	Collider* rightLimiter;
	Collider* leftLimiter;
	Collider* background_wall3;

	LevelState current_state;

	std::vector<Module*> gameElements;
	int activeEnemies;

};

#endif