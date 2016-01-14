#include "ModuleEnemies.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "Enemy.h"
#include "ModuleSceneLevel1.h"

ModuleEnemies::ModuleEnemies(bool enabled) : Module(enabled) {

}

ModuleEnemies::~ModuleEnemies() {

}

update_status ModuleEnemies::PreUpdate() {
	update_status ret = UPDATE_CONTINUE;
	for (vector<EnemyState*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		ret = (*it)->first->PreUpdate();

	return ret;
}

update_status ModuleEnemies::Update() {
	update_status ret = UPDATE_CONTINUE;
	for (vector<EnemyState*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		ret = (*it)->first->Update();

	return ret;
}

update_status ModuleEnemies::PostUpdate() {
	update_status ret = UPDATE_CONTINUE;

	for (vector<EnemyState*>::iterator it = enemies.begin(); it != enemies.end(); )
	{
		if ((*it)->second)
		{
			App->level1->DeleteEnemy((*it)->first);
			delete (*it)->first;
			delete (*it);
			it = enemies.erase(it);
		}
		else ++it;
	}

	return ret;
}
bool ModuleEnemies::CleanUp() {
	App->textures->Unload(graphics);


	vector<EnemyState*>::iterator it = enemies.begin();
	while (it != enemies.end()) {
		(*it)->first->CleanUp();
		delete (*it)->first;
		delete	(*it);
		it = enemies.erase(it);
	}

	enemies.clear();

	return true;
}

bool ModuleEnemies::Start() {
	graphics = App->textures->Load("Game/Sprites/enemy.png");

	return true;
}

Module* ModuleEnemies::CreateEnemy(int y, int h, bool left) {
	Enemy* e = new Enemy(y, h, left);
	enemies.push_back(new EnemyState(e, false));

	return e;
}

void ModuleEnemies::DestroyMe(const Module* enemy) {
	for (vector<EnemyState*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
	{
		if ((*it)->first == enemy)
		{
			(*it)->second = true;
		}
	}
}