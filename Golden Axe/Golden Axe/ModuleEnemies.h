#ifndef __MODULEENEMY_H__
#define __MODULEENEMY_H__

#include "Module.h"
#include "Application.h"

class ModuleEnemies : public Module {
private:

public:
	ModuleEnemies(bool enabled = false);
	~ModuleEnemies();

	bool Start();
	bool CleanUp();

	SDL_Texture* graphics = nullptr;
	
	Module* CreateEnemy(int x, int y);		//Creates an enemy in position x, y and returns it
};

#endif