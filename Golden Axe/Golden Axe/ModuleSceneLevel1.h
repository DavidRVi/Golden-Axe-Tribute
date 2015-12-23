#ifndef __MODULESCENELEVEL1_H__
#define __MODULESCENELEVEL1_H__

#include "Module.h"

class ModuleSceneLevel1 : public Module {
public:
	ModuleSceneLevel1(bool enabled = true);
	~ModuleSceneLevel1();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* background = nullptr;
	SDL_Rect ground;
	SDL_Rect parallax;

};
#endif