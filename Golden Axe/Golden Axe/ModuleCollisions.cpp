#include "ModuleCollisions.h"

using namespace std;

ModuleCollisions::ModuleCollisions(bool start_enabled) : Module(start_enabled)
{
	collisionMatrix[WORLD][WORLD] = false;		collisionMatrix[PLAYER][WORLD] = true;		collisionMatrix[ENEMY][WORLD] = true;		collisionMatrix[CAMERA][WORLD] = false;			collisionMatrix[TRIGGER][WORLD] = false;
	collisionMatrix[WORLD][PLAYER] = true;		collisionMatrix[PLAYER][PLAYER] = false;	collisionMatrix[ENEMY][PLAYER] = false;		collisionMatrix[CAMERA][PLAYER] = true;			collisionMatrix[TRIGGER][PLAYER] = true;
	collisionMatrix[WORLD][ENEMY] = true;		collisionMatrix[PLAYER][ENEMY] = false;		collisionMatrix[ENEMY][ENEMY] = false;		collisionMatrix[CAMERA][ENEMY] = false;			collisionMatrix[TRIGGER][ENEMY] = false;
	collisionMatrix[WORLD][TRIGGER] = false;	collisionMatrix[PLAYER][TRIGGER] = true;	collisionMatrix[ENEMY][TRIGGER] = false;	collisionMatrix[CAMERA][TRIGGER] = false;		collisionMatrix[TRIGGER][TRIGGER] = false;
	collisionMatrix[WORLD][CAMERA] = false;		collisionMatrix[PLAYER][CAMERA] = true;		collisionMatrix[ENEMY][CAMERA] = false;		collisionMatrix[CAMERA][CAMERA] = false;		collisionMatrix[TRIGGER][CAMERA] = false;
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
					if (collisionMatrix[it->first->getType()][jt->first->getType()])
					{
						LOG("Collision processed.");
						it->second = it->first->GetListener()->OnCollision(it->first, jt->first);
						jt->second = jt->first->GetListener()->OnCollision(jt->first, it->first);
					}
					else LOG("COLLISION IGNORED.");
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
		float speed = 1.0f;
		//SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
		for (ColliderList::iterator it = colliderList.begin(); it != colliderList.end(); ++it)
		{
			if (it->first->getType() == CAMERA)
			{
				speed = 1.0f;
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 165, 0, 100);
			}	
			else if (it->first->getType() == PLAYER)
				SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 0, 100);
			else if (it->first->getType() == WORLD)
			{
				SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
			}
			else if (it->first->getType() == ENEMY)
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 100);
			else if (it->first->getType() == TRIGGER)
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 255, 0, 100);

			App->renderer->DrawRectangle(it->first->GetRect(), speed);
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
	return SDL_HasIntersection(a->GetRect(), b->GetRect());
}

void ModuleCollisions::setDebug(bool value) {
	_debug = value;
}
