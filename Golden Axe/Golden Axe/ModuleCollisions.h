#ifndef __MODULECOLLISIONS_H_
#define __MODULECOLLISIONS_H_

#include "Module.h"
#include "Collider.h"
#include <list>
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"


typedef std::list<std::pair<Collider*, bool>> ColliderList;


class ModuleCollisions : public Module {
private:

	ColliderList colliderList;
	bool collisionMatrix[CAMERA + 1][CAMERA + 1];

	bool _debug;
	bool DetectCollision(Collider* a, Collider* b);

public:
	ModuleCollisions(bool start_enabled = true);
	~ModuleCollisions();

	void AddCollider(Collider* col);

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();
	void setDebug(bool value);


};

#endif