#include "Enemy.h"
#include "Application.h"
#include "ModuleCameraController.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "Timer.h"

Enemy::Enemy() {

	charge_it = 0;
	falling_timer = new Timer(500);
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

	recovery.x = 430;
	recovery.y = 230;
	recovery.w = 70;
	recovery.h = 70;

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
	switch (current_state) {
	case(FALLING_DOWN) :
		if (falling_timer->hasPassed())
		{
			current_state = LAY_DOWN;
			falling_timer->resetTimer();
		}
			
		break;
	case(LAY_DOWN):
		if (falling_timer->hasPassed())
		{
			current_state = RECOVERING;
			falling_timer->resetTimer();
			hitBoxCol->setActive(true);
		}
	case(RECOVERING):
		if (falling_timer->hasPassed())
		{
			current_state = IDLE;
		}
	}
	return ret;
}

update_status Enemy::Update() {
	update_status ret = UPDATE_CONTINUE;

	switch (current_state) {
	case(FALLING_DOWN):
		if (forward_walking)
			pivot.x -=3;
		else pivot.x +=3;
		break;
	}

	pivotCol->SetPosition(pivot.x, pivot.y);
	hitBoxCol->SetPosition(pivot.x, pivot.y - enemyHeight);

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
			if (forward_walking)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &falling_down);
			else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &falling_down);
			charge_it++;
		break;

	case(LAY_DOWN):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - lay_down.h + 15, &lay_down);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - lay_down.h + 15, &lay_down);
		break;

	case(RECOVERING):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - 60, &recovery);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - 60, &recovery);
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
		charge_it = 5;
		current_state = FALLING_DOWN;
		falling_timer->resetTimer();
		hitBoxCol->setActive(false);
		if (b->GetRect()->x > pivot.x)
			forward_walking = true;
		else forward_walking = false;
	}
	return false;
}

int Enemy::GetFallHeight(int i) const {
	double value = sin(8 * (i*M_PI) / 180);
	return (int)20 * value;
}