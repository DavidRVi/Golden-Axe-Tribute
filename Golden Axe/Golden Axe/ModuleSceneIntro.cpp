#include "ModuleSceneIntro.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleTextures.h"
#include "ModuleFadeToBlack.h"
#include "Globals.h"

ModuleSceneIntro::ModuleSceneIntro(bool enabled) : Module(enabled)
{
	symbol.x = 20;
	symbol.y = 20;
	symbol.w = 200;
	symbol.h = 150;

	sega.x = 80;
	sega.y = 245;
	sega.w = 170;
	sega.h = 40;

	title.x = 15;
	title.y = 170;
	title.w = 225;
	title.h = 50;

	press_start.frames.push_back({ 60, 340, 150, 10 });
	press_start.frames.push_back({ 0, 0, 0, 0 });
	press_start.speed = 0.05f;

	background.x = 0;
	background.y = 0;
	background.w = 320;
	background.h = 640;
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

bool ModuleSceneIntro::Start()
{
	bool ret = true;
	LOG("Loading Intro Scene");

	graphics = App->textures->Load("Game/Sprites/intro.png");
	background_tex = App->textures->Load("Game/Sprites/title_background.png");

	ret = App->audio->PlayMusic("Game/Music/Intro.ogg");

	
	return ret;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene.");
	App->textures->Unload(graphics);
	App->textures->Unload(background_tex);

	return true;
}


update_status ModuleSceneIntro::Update()
{
	//-----		BACKGROUND LOOP		----------------------
	App->renderer->Blit(background_tex, 0, 0, &background);
	if (background.h <= 240)
	{
		SDL_Rect rest = { 0, 0, 320, 240 - background.h };
		App->renderer->Blit(background_tex, 0, background.h, &rest);
		if (background.h <= 0)
		{
			background.h = 640;
			background.y = 0;
		}
	}
	background.y += 1;
	background.h -= 1;
	//--------------------------------------------------

	App->renderer->Blit(graphics, SCREEN_WIDTH/4 - 20, 0, &symbol);
	App->renderer->Blit(graphics, 45, 85, &title);
	App->renderer->Blit(graphics, SCREEN_WIDTH / 2 - 75, 160, &press_start.GetCurrentFrame());
	App->renderer->Blit(graphics, SCREEN_WIDTH / 2 - 25, 190, &sega);

	
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		App->fade->FadeToBlack((Module*) App->level1, this);

	return UPDATE_CONTINUE;
}


