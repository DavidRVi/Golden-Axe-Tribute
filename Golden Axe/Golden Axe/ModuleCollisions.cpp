#include "ModuleCollisions.h"

using namespace std;

ModuleCollisions::ModuleCollisions(bool start_enabled) : Module(start_enabled)
{
	for (int i = 0; i < CAMERA; ++i)
	{
		for (int j = 0; j < CAMERA; ++j)
			collisionMatrix[i][j] = false;
	}
	collisionMatrix[PLAYER][WORLD] = true;
	collisionMatrix[WORLD][PLAYER] = true;
	collisionMatrix[CAMERA][PLAYER] = true;
	collisionMatrix[WORLD][ENEMY] = true;
	collisionMatrix[PLAYER][TRIGGER] = true;
	collisionMatrix[PLAYER][CAMERA] = true;
	collisionMatrix[ENEMY][PATTACK] = true;
	collisionMatrix[TRIGGER][PLAYER] = true;
	collisionMatrix[EATTACK][PHITBOX] = true;
	collisionMatrix[PATTACK][EHITBOX] = true;
	collisionMatrix[EHITBOX][PATTACK] = true;
	collisionMatrix[PHITBOX][EATTACK] = true;
	collisionMatrix[EHITBOX][PFINAL] = true;
	collisionMatrix[PHITBOX][EFINAL] = true;
	collisionMatrix[EFINAL][PHITBOX] = true;
	collisionMatrix[PFINAL][EHITBOX] = true;

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
				if (collisionMatrix[it->first->getType()][jt->first->getType()])
				{
					if (it->first->isActive() && jt->first->isActive())
					{
						if (DetectCollision(it->first, jt->first))
						{
							it->second = it->first->GetListener()->OnCollision(it->first, jt->first);
							jt->second = jt->first->GetListener()->OnCollision(jt->first, it->first);
						}
					}
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
			if (it->first->isActive())
			{
				switch (it->first->getType())
				{
				case(CAMERA):
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 165, 0, 100);
					break;
				case(PLAYER):
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 165, 0, 100);
					break;
				case(WORLD):
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 125, 100);
					break;
				case(ENEMY):
					SDL_SetRenderDrawColor(App->renderer->renderer, 125, 0, 0, 100);
					break;
				case(TRIGGER):
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 255, 0, 100);
					break;
				case(PHITBOX):
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
					break;
				case(EHITBOX):
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 100);
					break;
				case(PFINAL):
				case(PATTACK):
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 255, 100);
					break;
				case(EFINAL):
				case(EATTACK):
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 255, 100);
					break;
				}

				App->renderer->DrawRectangle(it->first->GetRect(), speed);
			}
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

bool ModuleCollisions::DetectCollision(Collider* a, Collider* b) const{
	return SDL_HasIntersection(a->GetRect(), b->GetRect());
}

void ModuleCollisions::setDebug(bool value) {
	_debug = value;
}
