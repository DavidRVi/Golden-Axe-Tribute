#include "ModuleCameraController.h"
#include "ModuleRender.h"
#include "Application.h"
#include "ModuleCollisions.h"
#include "ModuleInput.h"
#include "Point.h"
#include "ModuleSceneLevel1.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"

ModuleCameraController::ModuleCameraController(bool enabled) : Module(enabled)
{
	go.frames.push_back({ 0, 80, 70, 40 });
	go.frames.push_back({ 110, 0, 10, 10 });
	go.speed = 0.1f;

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

	cameraTrigger = new Collider(westWall->GetRect()->x + (SCREEN_WIDTH / 2), 0, (SCREEN_WIDTH / 2), 240, this, TRIGGER);
	App->collisions->AddCollider(cameraTrigger);

	triggerCount = 0;
	spawnMonsters = false;
	
	return true;
}

update_status ModuleCameraController::PreUpdate() {

	int speed = 3;
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
	{
		App->renderer->camera.x -= speed;
		westWall->SetPosition(westWall->GetRect()->x+1, 0);
		eastWall->SetPosition(eastWall->GetRect()->x + 1, 0);
		//cameraTrigger->SetPosition(cameraTrigger->GetRect()->x + 1, 0);
	}
	else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		App->renderer->camera.x += speed;
		westWall->SetPosition(westWall->GetRect()->x-1, 0);
		eastWall->SetPosition(eastWall->GetRect()->x - 1, 0);
		//cameraTrigger->SetPosition(cameraTrigger->GetRect()->x - 1, 0);
	}

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
				cameraTrigger = new Collider(westWall->GetRect()->x + (SCREEN_WIDTH / 2), 0, (SCREEN_WIDTH / 2), 240, this, TRIGGER);
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