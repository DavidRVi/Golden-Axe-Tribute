#ifndef __MODULESCENELEVEL1_H__
#define __MODULESCENELEVEL1_H__

#include "Module.h"

class Collider;

class ModuleSceneLevel1 : public Module {
public:
	ModuleSceneLevel1(bool enabled = false);
	~ModuleSceneLevel1();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* background = nullptr;
	SDL_Rect ground;
	SDL_Rect parallax;

	Collider* west_wall;
	Collider* centre_wall;
	Collider* east_wall;

};

#endif