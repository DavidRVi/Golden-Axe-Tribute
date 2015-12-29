#include "ModuleCollisions.h"

using namespace std;

ModuleCollisions::ModuleCollisions(bool start_enabled) : Module(start_enabled)
{
	collisionMatrix[WORLD][WORLD] = false;		collisionMatrix[PLAYER][WORLD] = true;		collisionMatrix[ENEMY][WORLD] = true;		collisionMatrix[CAMERA][WORLD] = false;
	collisionMatrix[WORLD][PLAYER] = true;		collisionMatrix[PLAYER][PLAYER] = false;	collisionMatrix[ENEMY][PLAYER] = false;		collisionMatrix[CAMERA][PLAYER] = true;
	collisionMatrix[WORLD][ENEMY] = false;		collisionMatrix[PLAYER][ENEMY] = false;		collisionMatrix[ENEMY][ENEMY] = false;		collisionMatrix[CAMERA][ENEMY] = false;
	collisionMatrix[WORLD][CAMERA] = false;		collisionMatrix[PLAYER][CAMERA] = true;		collisionMatrix[ENEMY][CAMERA] = false;		collisionMatrix[CAMERA][CAMERA] = false;
	_debug = false;
}

ModuleCollisions::~ModuleCollisions() {

}

void ModuleCollisions::AddCollider(Collider* col) {
	colliderList.push_back(pair <Collider*, bool>(col, false));
}

bool ModuleCollisions::CleanUp() {
	for (ColliderList::iterator it = colliderList.begin(); it != colliderList.end(); ++it)
	{
		it->first->CleanUp();
		delete it->first;
	}
	colliderList.clear();
	return true;
}

update_status ModuleCollisions::PreUpdate() {
	if (!colliderList.empty())
	{
		ColliderList::iterator jt = colliderList.end();
		--jt;
		for (ColliderList::iterator it = colliderList.begin(); it != colliderList.end(); ++it)
		{
			for (; jt != it; --jt)
			{

				if (DetectCollision(it->first, jt->first))
				{
					//LOG("COLLISIOOON")
					it->second = it->first->GetListener()->OnCollision(it->first, jt->first);
					jt->second = jt->first->GetListener()->OnCollision(jt->first, it->first);
				}
			}
			jt = colliderList.end();
			--jt;
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleCollisions::Update() {
	if (_debug)
	{
		SDL_Surface* screen = App->window->screen_surface;
		Uint32 blue = SDL_MapRGB(screen->format, 0, 0, 255);
		SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
		for (ColliderList::iterator it = colliderList.begin(); it != colliderList.end(); ++it)
		{
			if (it->first->getType() == CAMERA)
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 165, 0, 100);
			else if (it->first->getType() == PLAYER)
				SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 0, 100);
			else if (it->first->getType() == ENEMY)
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 100);

			App->renderer->DrawRectangle(it->first->GetRect());
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		setDebug(!_debug);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleCollisions::PostUpdate() {
	ColliderList::iterator it = colliderList.begin();
	while (it != colliderList.end()) {
		if (it->second || it->first->isDirty())
		{
			it->first->CleanUp();
			delete it->first;
			colliderList.erase(it++);
		}
		else it++;
	}

	return UPDATE_CONTINUE;
}

bool ModuleCollisions::DetectCollision(Collider* a, Collider* b) {
	bool _hasCollided = false;

	return _hasCollided;
}

void ModuleCollisions::setDebug(bool value) {
	_debug = value;
}
