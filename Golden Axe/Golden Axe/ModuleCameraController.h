#ifndef __MODULECAMERACONTROLLER_H__
#define __MODULECAMERACONTROLLER_H__

#include "Module.h"

class Collider;

class ModuleCameraController : public Module {
public:
	ModuleCameraController(bool enabled = false);
	~ModuleCameraController();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	bool OnCollision(Collider* a, Collider* b);

private:
	Collider* westWall;
	Collider* southWall;
	Collider* eastWall;
};
#endif