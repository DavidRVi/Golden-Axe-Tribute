#include "ModuleCameraController.h"
#include "ModuleRender.h"
#include "Application.h"
#include "ModuleCollisions.h"
#include "ModuleInput.h"
#include "Point.h"


ModuleCameraController::ModuleCameraController(bool enabled) : Module(enabled)
{
}

ModuleCameraController::~ModuleCameraController() {

}

bool ModuleCameraController::Start() {
	westWall = new Collider(0, 0, 35, 240, this, CAMERA);
	southWall = new Collider(0, 240, 1800, 10, this, CAMERA);
	eastWall = new Collider(285, 0, 35, 240, this, CAMERA);

	App->collisions->AddCollider(southWall);
	App->collisions->AddCollider(westWall);
	App->collisions->AddCollider(eastWall);
	
	return true;
}

update_status ModuleCameraController::PreUpdate() {

	int speed = 3;
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
	{
		App->renderer->camera.x -= speed;
		westWall->SetPosition(westWall->GetRect()->x+1, westWall->GetRect()->y);
		eastWall->SetPosition(eastWall->GetRect()->x + 1, westWall->GetRect()->y);
		//westWall->GetRect()->w = westWall->GetRect()->w+1;
	}
	else if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		App->renderer->camera.x += speed;
		westWall->SetPosition(westWall->GetRect()->x-1, westWall->GetRect()->y);
		eastWall->SetPosition(eastWall->GetRect()->x - 1, eastWall->GetRect()->y);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleCameraController::Update() {


	return UPDATE_CONTINUE;
}

update_status ModuleCameraController::PostUpdate() {

	return UPDATE_CONTINUE;
}

bool ModuleCameraController::CleanUp() {
	return true;
}

bool ModuleCameraController::OnCollision(Collider* a, Collider* b)
{
	return false;
}