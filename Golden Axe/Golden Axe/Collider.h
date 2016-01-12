#ifndef _COLLIDER_H_
#define _COLLIDER_H_

#include "SDL/include/SDL.h"

class Module;
struct SDL_Rect;


enum colliderType { WORLD = 0, PLAYER, ENEMY, TRIGGER, PATTACK, EATTACK, PFINAL, EFINAL, PHITBOX, EHITBOX, CAMERA, };


class Collider {
private:
	SDL_Rect* size = nullptr;
	Module* listener = nullptr;
	colliderType type;
	bool _isDirty;

	bool active;

public:

	Collider(int x, int y, int w, int h, Module* listener, colliderType aType)
	{
		size = new SDL_Rect();
		size->x = x;
		size->y = y;
		size->w = w;
		size->h = h;
		type = aType;
		_isDirty = false;
		this->listener = listener;
		active = true;
	}

	Collider(SDL_Rect* rect, Module* listener, colliderType aType)
	{
		size = rect;
		this->listener = listener;
		type = aType;
	}

	~Collider() {
	}

	SDL_Rect* GetRect(){
		return size;
	}

	Module* GetListener() {
		return listener;
	}

	colliderType getType() {
		return type;
	}

	void SetPosition(int x, int y)
	{
		size->x = x;
		size->y = y;
	}

	void CleanUp() {
		RELEASE(size);
		listener = nullptr;
	}

	bool isDirty() {
		return _isDirty;
	}

	void SetDirty(bool value) {
		_isDirty = value;
	}

	void setActive(bool value) {
		active = value;
	}

	bool isActive() {
		return active;
	}
};

#endif