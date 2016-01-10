#include "ModuleCameraController.h"
#include "ModuleRender.h"
#include "Application.h"
#include "ModuleCollisions.h"
#include "ModuleInput.h"
#include "Point.h"
#include "ModuleSceneLevel1.h"
#include "ModuleTextures.h"
#include "ModulePlayer.h"
#include "ModuleAudio.h"

ModuleCameraController::ModuleCameraController(bool enabled) : Module(enabled)
{
	go.frames.push_back({ 0, 80, 70, 40 });
	go.frames.push_back({ 110, 0, 10, 10 });
	go.speed = 0.1f;

	stage.x = 0;
	stage.y = 50;
	stage.w = 70;
	stage.h = 10;

	magic.x = 0;
	magic.y = 0;
	magic.w = 50;
	magic.h = 30;

	charPortrait.x = 50;
	charPortrait.y = 0;
	charPortrait.w = 20;
	charPortrait.h = 20;

	life_3.x = 70;
	life_3.y = 0;
	life_3.w = 10;
	life_3.h = 20;

	life_2.x = 80;
	life_2.y = 0;
	life_2.w = 10;
	life_2.h = 20;

	life_1.x = 90;
	life_1.y = 0;
	life_1.w = 10;
	life_1.h = 20;

	lifeBar.x = 0;
	lifeBar.y = 30;
	lifeBar.w = 24;
	lifeBar.h = 10;

	magicFlask.x = 30;
	magicFlask.y = 30;
	magicFlask.w = 5;
	magicFlask.h = 10;

	encounterCount = 0;
}

ModuleCameraController::~ModuleCameraController() {
	RELEASE(go_timer);
}

bool ModuleCameraController::Start() {
	interface = App->textures->Load("Game/Sprites/interface.png");
	go_fx = App->audio->LoadFx("Game/fx/go.wav");
	show_go = false;

	westWall = new Collider(0, 0, 35, 240, this, CAMERA);
	southWall = new Collider(0, 240, 1800, 10, this, CAMERA);
	eastWall = new Collider(285, 0, 35, 240, this, CAMERA);

	App->collisions->AddCollider(southWall);
	App->collisions->AddCollider(westWall);
	App->collisions->AddCollider(eastWall);

	cameraTrigger = new Collider(westWall->GetRect()->x + (SCREEN_WIDTH / 2) + 20, 0, (SCREEN_WIDTH / 2) - 20 , 240, this, TRIGGER);
	App->collisions->AddCollider(cameraTrigger);

	triggerCount = 0;
	spawnMonsters = false;
	
	return true;
}

update_status ModuleCameraController::PreUpdate() {

	/* //debug
	
	int speed = 3;
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
	{
		App->renderer->camera.x -= speed;
		westWall->SetPosition(westWall->GetRect()->x + 1, 0);
		eastWall->SetPosition(eastWall->GetRect()->x + 1, 0);
		//cameraTrigger->SetPosition(cameraTrigger->GetRect()->x + 1, 0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		App->renderer->camera.x += speed;
		westWall->SetPosition(westWall->GetRect()->x - 1, 0);
		eastWall->SetPosition(eastWall->GetRect()->x - 1, 0);
		//cameraTrigger->SetPosition(cameraTrigger->GetRect()->x - 1, 0);
	}
	*/
	return UPDATE_CONTINUE;
}

update_status ModuleCameraController::Update() {

	if (show_go)
	{
		if (!go_timer->hasPassed())
		{
			if (encounterCount <= 5)
			{
				App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH - go.frames[0].w), 40, &go.GetCurrentFrame());
				
			}
		}
		else {
			if (encounterCount <= 5)
			{
				cameraTrigger = new Collider(westWall->GetRect()->x + (SCREEN_WIDTH / 2) + 10, 0, (SCREEN_WIDTH / 2) - 10, 240, this, TRIGGER);
				App->collisions->AddCollider(cameraTrigger);
				triggerCount = 0;
				show_go = false;
				RELEASE(go_timer);
			}
			else {
				//LEVEL COMPLETE
			}
			
		}
	}

	//--------------- INTERFACE ------------------
	int magicFlasks = App->player->GetMagicFlasks();
	if (magicFlasks > 0)
	{
		App->renderer->Blit(interface, westWall->GetRect()->x + 80, 5, &magic);
		//Draw magicFlasks depending on number
		for (int i = 0; i < magicFlasks; i++)
			App->renderer->Blit(interface, westWall->GetRect()->x + 88 + (i*(magicFlask.w + 3)), 19, &magicFlask);
	}

	App->renderer->Blit(interface, westWall->GetRect()->x, 20, &stage);
	App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH/2) - 30, 215, &charPortrait);

	switch (App->player->GetLives())
	{
	case(3) :
		App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH / 2) - 37, 215, &life_3);
		break;
	case(2):
		App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH / 2) - 37, 215, &life_2);
		break;
	case(1):
		App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH / 2) - 37, 215, &life_1);
		break;
	}

	int lifeBars = App->player->GetLifeBars();

	for (int i = 0; i < lifeBars; i++)
		App->renderer->Blit(interface, westWall->GetRect()->x + (SCREEN_WIDTH / 2) - 60 - (i*lifeBar.w), 215, &lifeBar);

	//-----------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleCameraController::PostUpdate() {

	return UPDATE_CONTINUE;
}

bool ModuleCameraController::CleanUp() {
	App->textures->Unload(interface);
	return true;
}

bool ModuleCameraController::OnCollision(Collider* a, Collider* b)
{
	if (a->getType() == TRIGGER)
	{
		App->renderer->camera.x -= 3;
		westWall->SetPosition(westWall->GetRect()->x + 1, 0);
		eastWall->SetPosition(eastWall->GetRect()->x + 1, 0);
		cameraTrigger->SetPosition(cameraTrigger->GetRect()->x + 1, 0);
		triggerCount++;
		if (triggerCount >= 250)
		{
			triggerCount = 0;
			spawnMonsters = false;
			cameraTrigger = nullptr;
			App->level1->ChangeState(BATTLE);
			return true;
		}
		else if (triggerCount >= 50 && !spawnMonsters)
		{
			spawnMonsters = true;
			App->level1->spawnEnemies();
		}

	}

	
	return false;
}

void ModuleCameraController::ShowGoAnimation() {
	encounterCount++;
	if (encounterCount <= 5)
	{
		App->audio->PlayFx(go_fx, 2);
		show_go = true;
		go_timer = new Timer(1000);
	}
}