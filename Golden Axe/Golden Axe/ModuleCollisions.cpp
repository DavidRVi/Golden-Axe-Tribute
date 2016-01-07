#include "ModuleCollisions.h"

using namespace std;

ModuleCollisions::ModuleCollisions(bool start_enabled) : Module(start_enabled)
{
	collisionMatrix[WORLD][WORLD] = false;		collisionMatrix[PLAYER][WORLD] = true;		collisionMatrix[ENEMY][WORLD] = false;		collisionMatrix[CAMERA][WORLD] = false;		collisionMatrix[TRIGGER][WORLD] = false;	collisionMatrix[PHITBOX][WORLD] = false;	collisionMatrix[EHITBOX][WORLD] = false;	collisionMatrix[PATTACK][WORLD] = false;		collisionMatrix[EATTACK][WORLD] = false;
	collisionMatrix[WORLD][PLAYER] = true;		collisionMatrix[PLAYER][PLAYER] = false;	collisionMatrix[ENEMY][PLAYER] = false;		collisionMatrix[CAMERA][PLAYER] = true;		collisionMatrix[TRIGGER][PLAYER] = true;	collisionMatrix[PHITBOX][PLAYER] = false;	collisionMatrix[EHITBOX][PLAYER] = false;	collisionMatrix[PATTACK][PLAYER] = false;		collisionMatrix[EATTACK][PLAYER] = false;
	collisionMatrix[WORLD][ENEMY] = true;		collisionMatrix[PLAYER][ENEMY] = false;		collisionMatrix[ENEMY][ENEMY] = false;		collisionMatrix[CAMERA][ENEMY] = false;		collisionMatrix[TRIGGER][ENEMY] = false;	collisionMatrix[PHITBOX][ENEMY] = false;	collisionMatrix[EHITBOX][ENEMY] = false;	collisionMatrix[PATTACK][ENEMY] = false;		collisionMatrix[EATTACK][ENEMY] = false;
	collisionMatrix[WORLD][TRIGGER] = false;	collisionMatrix[PLAYER][TRIGGER] = true;	collisionMatrix[ENEMY][TRIGGER] = false;	collisionMatrix[CAMERA][TRIGGER] = false;	collisionMatrix[TRIGGER][TRIGGER] = false;	collisionMatrix[PHITBOX][TRIGGER] = false;	collisionMatrix[EHITBOX][TRIGGER] = false;	collisionMatrix[PATTACK][TRIGGER] = false;		collisionMatrix[EATTACK][TRIGGER] = false;
	collisionMatrix[WORLD][CAMERA] = false;		collisionMatrix[PLAYER][CAMERA] = true;		collisionMatrix[ENEMY][CAMERA] = false;		collisionMatrix[CAMERA][CAMERA] = false;	collisionMatrix[TRIGGER][CAMERA] = false;	collisionMatrix[PHITBOX][CAMERA] = false;	collisionMatrix[EHITBOX][CAMERA] = false;	collisionMatrix[PATTACK][CAMERA] = false;		collisionMatrix[EATTACK][CAMERA] = false;
	collisionMatrix[WORLD][PHITBOX] = false;	collisionMatrix[PLAYER][PHITBOX] = false;	collisionMatrix[ENEMY][PHITBOX] = false;	collisionMatrix[CAMERA][PHITBOX] = false;	collisionMatrix[TRIGGER][PHITBOX] = false;	collisionMatrix[PHITBOX][PHITBOX] = false;	collisionMatrix[EHITBOX][PHITBOX] = false;	collisionMatrix[PATTACK][PHITBOX] = false;		collisionMatrix[EATTACK][PHITBOX] = true;
	collisionMatrix[WORLD][EHITBOX] = false;	collisionMatrix[PLAYER][EHITBOX] = false;	collisionMatrix[ENEMY][EHITBOX] = false;	collisionMatrix[CAMERA][EHITBOX] = false;	collisionMatrix[TRIGGER][EHITBOX] = false;	collisionMatrix[PHITBOX][EHITBOX] = false;	collisionMatrix[EHITBOX][EHITBOX] = false;	collisionMatrix[PATTACK][EHITBOX] = true;		collisionMatrix[EATTACK][EHITBOX] = false;
	collisionMatrix[WORLD][PATTACK] = false;	collisionMatrix[PLAYER][PATTACK] = false;	collisionMatrix[ENEMY][PATTACK] = true;		collisionMatrix[CAMERA][PATTACK] = false;	collisionMatrix[TRIGGER][PATTACK] = false;	collisionMatrix[PHITBOX][PATTACK] = false;  collisionMatrix[EHITBOX][PATTACK] = true;	collisionMatrix[PATTACK][PATTACK] = false;		collisionMatrix[EATTACK][PATTACK] = false;
	collisionMatrix[WORLD][EATTACK] = false;	collisionMatrix[PLAYER][EATTACK] = false;	collisionMatrix[ENEMY][EATTACK] = false;	collisionMatrix[CAMERA][EATTACK] = false;	collisionMatrix[TRIGGER][EATTACK] = false;	collisionMatrix[PHITBOX][EATTACK] = true;	collisionMatrix[EHITBOX][EATTACK] = false;	collisionMatrix[PATTACK][EATTACK] = false;		collisionMatrix[EATTACK][EATTACK] = false;
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
				if (it->first->getType() == CAMERA)
				{
					speed = 1.0f;
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 165, 0, 100);
				}
				else if (it->first->getType() == PLAYER)
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 0, 100);
				else if (it->first->getType() == WORLD)
				{
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 125, 100);
				}
				else if (it->first->getType() == ENEMY)
					SDL_SetRenderDrawColor(App->renderer->renderer, 125, 0, 0, 100);
				else if (it->first->getType() == TRIGGER)
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 255, 0, 100);
				else if (it->first->getType() == PHITBOX)
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
				else if (it->first->getType() == EHITBOX)
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 100);
				else if (it->first->getType() == PATTACK)
					SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 255, 100);
				else if (it->first->getType() == EATTACK)
					SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 255, 100);

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

bool ModuleCollisions::DetectCollision(Collider* a, Collider* b) {
	return SDL_HasIntersection(a->GetRect(), b->GetRect());
}

void ModuleCollisions::setDebug(bool value) {
	_debug = value;
}
