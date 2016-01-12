#include "Enemy.h"
#include "Application.h"
#include "ModuleCameraController.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "Timer.h"
#include "ModulePlayer.h"

Enemy::Enemy(int y, int h) {

	y_limit = y;
	charge_it = 0;
	falling_timer = new Timer(500);
	current_state = EIDLE;
	forward_walking = false;

	hit_timer = new Timer(700);

	attackTimer = new Timer(400);

	pivot.x = App->camController->eastWall->GetRect()->x;
	pivot.y = h;
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

	idleAttack.frames.push_back({ 90, 100, 70, 110 });
	idleAttack.frames.push_back({ 170, 100, 70, 110 });
	idleAttack.frames.push_back({ 240, 100, 100, 110 });
	idleAttack.speed = 0.1f;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, ENEMY);
	hitBoxCol = new Collider(pivot.x, pivot.y - enemyHeight, pivot.w, enemyHeight, this, EHITBOX);
	attackCol = new Collider(pivot.x, pivot.y, 30, 50, this, EFINAL);
	App->collisions->AddCollider(pivotCol);
	App->collisions->AddCollider(hitBoxCol);
	App->collisions->AddCollider(attackCol);

	player = App->player;

	lifePoints = 5;

	debug = false;
	if (debug)
	{
		RELEASE(attackTimer);
		attackTimer = new Timer(5000);
	}
}

Enemy::~Enemy() {
	RELEASE(falling_timer);
	RELEASE(hit_timer);
	RELEASE(attackTimer);
}

bool Enemy::CleanUp() {
	return true;
}

update_status Enemy::PreUpdate() {

	/*
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
	{
		App->enemies->DestroyMe(this);
		pivotCol->SetDirty(true);
		hitBoxCol->SetDirty(true);
		attackCol->SetDirty(true);
	}
	*/
	attackCol->setActive(false);
	update_status ret = UPDATE_CONTINUE;
	switch (current_state) {
	case(EFALLING_DOWN) :
		if (falling_timer->hasPassed())
		{
			current_state = ELAY_DOWN;
			falling_timer->resetTimer();
		}
			
		break;

	case(ELAY_DOWN):
		if (falling_timer->hasPassed())
		{
			
			if (lifePoints <= 0)
			{
				//Play Dead fx
				App->enemies->DestroyMe(this);
				pivotCol->SetDirty(true);
				hitBoxCol->SetDirty(true);
				attackCol->SetDirty(true);
			}
			else {
				current_state = ERECOVERING;
				falling_timer->resetTimer();
				hitBoxCol->setActive(true);
			}
		}
		break;

	case(ERECOVERING):
		if (falling_timer->hasPassed())
		{
			current_state = EIDLE;
		}
		break;

	case(EHIT) :
		if (hit_timer->hasPassed())
		{
			current_state = EIDLE;
		}
		break;

	case(EIDLE):
		if (pivot.x > player->pivot.x)
			forward_walking = false;
		else forward_walking = true;

		if (inRange(player->GetScreenHeight()))
		{
			if (!forward_walking)
			{
				if (player->pivot.x >= (pivot.x - 2 * attackCol->GetRect()->w) && (player->pivot.x <= pivot.x))
				{
					attackTimer->resetTimer();
					idleAttack.resetAnimation();
					//attackCol->setActive(true);
					current_state = EATTACKING;
				}
			}
			else {
				if (player->pivot.x <= (pivot.x + 2 * attackCol->GetRect()->w) && (player->pivot.x >= pivot.x))
				{
					attackTimer->resetTimer();
					idleAttack.resetAnimation();
					//attackCol->setActive(true);
					current_state = EATTACKING;
				}
			}
			
		}
		break;

	case(EATTACKING) :
		if (attackTimer->hasPassed())
			current_state = EIDLE;
		else {
			if (idleAttack.GetActualFrame().x == idleAttack.frames[1].x)
				attackCol->setActive(true);
		}
		break;

	}
	return ret;
}

update_status Enemy::Update() {
	update_status ret = UPDATE_CONTINUE;

	switch (current_state) {
	case(EFALLING_DOWN):
		if (forward_walking)
			pivot.x -=3;
		else pivot.x +=3;
		break;
	}

	pivotCol->SetPosition(pivot.x, pivot.y);
	hitBoxCol->SetPosition(pivot.x, pivot.y - enemyHeight);
	if (forward_walking)
		attackCol->SetPosition(pivot.x + pivot.w, pivot.y - enemyHeight);
	else attackCol->SetPosition(pivot.x - attackCol->GetRect()->w, pivot.y - enemyHeight);

	return ret;
}

bool Enemy::Draw() {
	if (debug)
		idleAttack.speed = 0.01f;
	bool ret = true;
	switch (current_state){
	case(EIDLE):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle.GetCurrentFrame());
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - enemyHeight, &idle.GetCurrentFrame());
		break;
	case(EFALLING_DOWN) :
			if (forward_walking)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &falling_down);
			else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &falling_down);
			if (charge_it <= 28)
				charge_it++;
		break;

	case(ELAY_DOWN):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - lay_down.h + 15, &lay_down);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - lay_down.h + 15, &lay_down);
		break;

	case(ERECOVERING):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - 60, &recovery);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - 60, &recovery);
		break;

	case(EHIT):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - 60, &recovery);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - 60, &recovery);
		break;

	case(EATTACKING) :
		SDL_Rect frame = idleAttack.GetCurrentFrame();
		if (forward_walking)
		{
			if (frame.x == idleAttack.frames[0].x)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x - 10, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[1].x)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[2].x)
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 105, &idleAttack.GetCurrentFrame());
		}
		else {
			if (frame.x == idleAttack.frames[0].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[1].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x- 20, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[2].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 50, pivot.y - 105, &idleAttack.GetCurrentFrame());
		}
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
		if (inRange(player->pivot.y))
		{
			lifePoints--;
			charge_it = 5;
			current_state = EFALLING_DOWN;
			falling_timer->resetTimer();
			hitBoxCol->setActive(false);
			if (player->pivot.x > pivot.x)
				forward_walking = true;
			else forward_walking = false;
		}
		
	}
	else if (b->getType() == PATTACK) {
		if (inRange(player->pivot.y))
		{
			lifePoints--;
			if (lifePoints <= 0)
			{
				charge_it = 5;
				current_state = EFALLING_DOWN;
				falling_timer->resetTimer();
				hitBoxCol->setActive(false);
			}
			else{
				current_state = EHIT;
				hit_timer->resetTimer();
			}
			if (player->pivot.x > pivot.x)
				forward_walking = true;
			else forward_walking = false;
		}
		
	}
	return false;
}

int Enemy::GetFallHeight(int i) const {
	double value = sin(8 * (i*M_PI) / 180);
	return (int)30 * value;
}

bool Enemy::inRange(int y) const {
	return ((y - 5) <= pivot.y) && ((y + 5) >= pivot.y);
}