#include "Enemy.h"
#include "Application.h"
#include "ModuleCameraController.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "Timer.h"

Enemy::Enemy() {

	charge_it = 0;
	falling_timer = new Timer(1000);
	current_state = IDLE;
	forward_walking = false;

	pivot.x = App->camController->eastWall->GetRect()->x;
	pivot.y = 200;
	pivot.w = 40;
	pivot.h = 10;

	enemyHeight = 90;

	idle.frames.push_back({ 0, 0, 50, 100 });
	idle.frames.push_back({ 50, 0, 50, 100 });
	idle.speed = 0.09f;

	falling_down.x = 0;
	falling_down.y = 240;
	falling_down.w = 80;
	falling_down.h = 60;

	lay_down.x = 90;
	lay_down.y = 250;
	lay_down.w = 100;
	lay_down.h = 40;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, ENEMY);
	hitBoxCol = new Collider(pivot.x, pivot.y - enemyHeight, pivot.w, enemyHeight, this, EHITBOX);
	App->collisions->AddCollider(pivotCol);
	App->collisions->AddCollider(hitBoxCol);

	lifePoints = 5;
}

Enemy::~Enemy() {
	RELEASE(falling_timer);
}

bool Enemy::CleanUp() {
	return true;
}

update_status Enemy::PreUpdate() {
	update_status ret = UPDATE_CONTINUE;
		
	return ret;
}


bool Enemy::Draw() {
	bool ret = true;
	switch (current_state){
	case(IDLE):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle.GetCurrentFrame());
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle.GetCurrentFrame());
		break;
	case(FALLING_DOWN) :
		if (!falling_timer->hasPassed()) {
			if (forward_walking)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 30 - GetFallHeight(charge_it), &falling_down);
			else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - 30 - GetFallHeight(charge_it), &falling_down);
			charge_it++;
		}
		else current_state = IDLE;
		break;
	}

	return ret;
}

int Enemy::GetScreenHeight() const {
	return pivot.y;
}

bool Enemy::OnCollision(Collider* a, Collider* b) {
	if (b->getType() == PFINAL)
	{
		charge_it = 0;
		current_state = FALLING_DOWN;
		falling_timer->resetTimer();
		if (b->GetRect()->x > pivot.x)
			forward_walking = true;
		else forward_walking = false;
	}
	return false;
}

int Enemy::GetFallHeight(int i) const {
	double value = sin(8 * (i*M_PI) / 180);
	return (int)15 * value;
}