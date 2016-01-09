#include "Enemy.h"
#include "Application.h"
#include "ModuleCameraController.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"

Enemy::Enemy() {

	current_state = IDLE;
	forward_walking = false;

	pivot.x = App->camController->eastWall->GetRect()->x;
	pivot.y = 200;
	pivot.w = 40;
	pivot.h = 10;

	enemyHeight = 90;

	idle.x = 0;
	idle.y = 0;
	idle.w = 50;
	idle.h = 100;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, ENEMY);
	hitBoxCol = new Collider(pivot.x, pivot.y - enemyHeight, pivot.w, enemyHeight, this, EHITBOX);
	App->collisions->AddCollider(pivotCol);
	App->collisions->AddCollider(hitBoxCol);
}

Enemy::~Enemy() {

}

bool Enemy::CleanUp() {
	return true;
}

bool Enemy::Draw() {
	bool ret = true;
	switch (current_state){
	case(IDLE):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle);
		break;
	}

	return ret;
}

int Enemy::GetScreenHeight() {
	return pivot.y;
}

bool Enemy::OnCollision(Collider* a, Collider* b) {
	return false;
}