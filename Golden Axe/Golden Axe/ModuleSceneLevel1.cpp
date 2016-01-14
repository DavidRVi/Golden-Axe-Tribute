#include "ModuleSceneLevel1.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleCollisions.h"
#include "ModuleCameraController.h"
#include "ModuleInput.h"
#include "ModuleEnemies.h"
#include "quicksort.h"
#include "ModuleInput.h"
#include "ModuleFadeToBlack.h"

ModuleSceneLevel1::ModuleSceneLevel1(bool enabled) : Module(enabled)
{
	ground.x = 0;
	ground.y = 0;
	ground.w = 1820;
	ground.h = 240;

	parallax.x = 0;
	parallax.y = 260;
	parallax.w = 1820;
	parallax.h = 240;

}

ModuleSceneLevel1::~ModuleSceneLevel1() {

}

bool ModuleSceneLevel1::Start()
{
	bool ret = true;

	playMusic = false;
	LOG("Loading Level 1 Scene");

	background = App->textures->Load("Game/Sprites/level1.png");
	ret = App->audio->PlayMusic("Game/Music/level1.ogg");
	App->collisions->Enable();
	App->player->Enable();
	App->enemies->Enable();
	App->camController->Enable();
	gameElements.push_back(App->player);

	activeEnemies = 0;
	current_state = NO_BATTLE;

	background_wall1 = new Collider(0, 150, 880, 10, this, WORLD);
	background_wall2 = new Collider(890, 180, 485, 10, this, WORLD);
	background_wall3 = new Collider(1375, 95, 425, 10, this, WORLD);
	leftLimiter = new Collider(880, 150, 10, 30, this, CAMERA);
	rightLimiter = new Collider(1375, 95, 10, 90, this, CAMERA);

	App->collisions->AddCollider(background_wall1);
	App->collisions->AddCollider(background_wall2);
	App->collisions->AddCollider(background_wall3);
	App->collisions->AddCollider(leftLimiter);
	App->collisions->AddCollider(rightLimiter);
	return ret;
}

bool ModuleSceneLevel1::CleanUp() {
	LOG("Unloading Level 1 Scene");
	App->textures->Unload(background);
	App->player->Disable();
	App->enemies->Disable();
	/*
	for (vector<Module*>::iterator it = gameElements.begin(); it != gameElements.end();)
		*it = nullptr;*/

	gameElements.clear();

	return true;
}

update_status ModuleSceneLevel1::Update() {
	bool ret;
	ret = App->renderer->Blit(background, 0, 0, &parallax, 0.9f);
	ret = App->renderer->Blit(background, 0, 0, &ground, 1.0f);

	switch (current_state) {
	case(BATTLE):
		if (gameElements.size() >= 2)
		{
			quicksort(gameElements, 0, gameElements.size());
		}

		for (int i = 0; i < gameElements.size(); ++i)
			gameElements[i]->Draw();

		if (activeEnemies == 0)
		{
			App->camController->ShowGoAnimation();
			current_state = NO_BATTLE;
		}
		break;

	case(NO_BATTLE):
		if (gameElements.size() >= 2)
		{
			quicksort(gameElements, 0, gameElements.size());
		}

		for (int i = 0; i < gameElements.size(); ++i)
			gameElements[i]->Draw();

		break;

	case(GAME_OVER):
		if (!playMusic)
		{
			playMusic = true;
			App->audio->PlayMusic("Game/Music/game_over.ogg", 0.0f, 1);
		}
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{

			App->camController->Disable();
			App->enemies->Disable();
			App->player->Disable();
			App->collisions->Disable();
			App->fade->FadeToBlack((Module*)App->intro, this);
		}
		break;
	}
	if (ret)
		return UPDATE_CONTINUE;
	else return UPDATE_ERROR;
}

void ModuleSceneLevel1::ChangeState(LevelState state) {
	current_state = state;
}

void ModuleSceneLevel1::spawnEnemies(int y, int h, bool left) {
	//current_state = BATTLE;
	activeEnemies++;
	gameElements.push_back(App->enemies->CreateEnemy(y, h, left));
}

bool ModuleSceneLevel1::DeleteEnemy(const Module* enemy) {
	bool hasDeleted = false;
	for (vector<Module*>::iterator it = gameElements.begin(); it != gameElements.end() && !hasDeleted; )
	{
		if (*it == enemy)
		{
			it = gameElements.erase(it);
			hasDeleted = true;
			activeEnemies--;
		}
		else ++it;
	}

	return hasDeleted;
}

void ModuleSceneLevel1::TriggerGameOver() {
	current_state = GAME_OVER;
	
}

LevelState ModuleSceneLevel1::getLevelState() const {
	return current_state;
}