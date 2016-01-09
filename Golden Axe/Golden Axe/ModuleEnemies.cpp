#include "ModuleEnemies.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "Enemy.h"

ModuleEnemies::ModuleEnemies(bool enabled) : Module(enabled) {

}

ModuleEnemies::~ModuleEnemies() {

}

bool ModuleEnemies::CleanUp() {
	App->textures->Unload(graphics);

	return true;
}

bool ModuleEnemies::Start() {
	graphics = App->textures->Load("Game/Sprites/enemy.png");

	return true;
}

Module* ModuleEnemies::CreateEnemy(int x, int y) {
	return new Enemy();
}