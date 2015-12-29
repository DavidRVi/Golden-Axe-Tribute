#include "ModuleSceneLevel1.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"

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
	LOG("Loading Level 1 Scene");

	background = App->textures->Load("Game/Sprites/level1.png");
	//ret = App->audio->PlayMusic("Game/Music/level1.ogg");

	App->player->Enable();
	return ret;
}

bool ModuleSceneLevel1::CleanUp() {
	LOG("Unloading Level 1 Scene");
	App->textures->Unload(background);
	App->player->Disable();

	return true;
}

update_status ModuleSceneLevel1::Update() {
	App->renderer->Blit(background, 0, 0, &parallax, 0.8f);
	App->renderer->Blit(background, 0, 0, &ground, 0.9f);

	return UPDATE_CONTINUE;
}

