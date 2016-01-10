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

ModuleSceneLevel1::ModuleSceneLevel1(bool enabled) : Module(enabled)
{

	activeEnemies = 0;

	ground.x = 0;
	ground.y = 0;
	ground.w = 1820;
	ground.h = 240;

	parallax.x = 0;
	parallax.y = 260;
	parallax.w = 1820;
	parallax.h = 240;

	east_wall = new Collider(0, 0, 780, 160, this, WORLD);

	current_state = NO_BATTLE;
}

ModuleSceneLevel1::~ModuleSceneLevel1() {

}

bool ModuleSceneLevel1::Start()
{
	bool ret = true;
	LOG("Loading Level 1 Scene");

	background = App->textures->Load("Game/Sprites/level1.png");
	ret = App->audio->PlayMusic("Game/Music/level1.ogg");
	App->player->Enable();
	gameElements.push_back(App->player);
	App->enemies->Enable();
	App->camController->Enable();

	App->collisions->AddCollider(east_wall);
	return ret;
}

bool ModuleSceneLevel1::CleanUp() {
	LOG("Unloading Level 1 Scene");
	App->textures->Unload(background);
	App->player->Disable();
	App->enemies->Disable();

	return true;
}

update_status ModuleSceneLevel1::Update() {
	bool ret;
	ret = App->renderer->Blit(background, 0, 0, &parallax, 0.9f);
	ret = App->renderer->Blit(background, 0, 0, &ground, 1.0f);

	if (gameElements.size() >= 2)
	{
		quicksort(gameElements, 0, gameElements.size());
	}

	for (int i = 0; i < gameElements.size(); ++i)
		gameElements[i]->Draw();

	if (current_state == BATTLE)
	{
		if (activeEnemies == 0)
		{
			App->camController->ShowGoAnimation();
			current_state = NO_BATTLE;
		}

	}
	
	if (ret)
		return UPDATE_CONTINUE;
	else return UPDATE_ERROR;
}

void ModuleSceneLevel1::ChangeState(LevelState state) {
	current_state = state;
}

void ModuleSceneLevel1::spawnEnemies() {
	//current_state = BATTLE;
	gameElements.push_back(App->enemies->CreateEnemy(0, 0));
}

