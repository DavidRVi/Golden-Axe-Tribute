#ifndef __MODULEENEMY_H__
#define __MODULEENEMY_H__

#include "Module.h"
#include "Application.h"
#include <vector>

typedef std::pair<Module*, bool> EnemyState;

class ModuleEnemies : public Module {
private:

public:
	ModuleEnemies(bool enabled = false);
	~ModuleEnemies();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	Module* CreateEnemy(int y, int h);		//Creates an enemy and returns it
	void DestroyMe(const Module* enemy);

	SDL_Texture* graphics = nullptr;
	
	

private:
	std::vector<EnemyState*> enemies;
};

#endif