#include "Enemy.h"
#include "Application.h"
#include "ModuleCameraController.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "Timer.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"

Enemy::Enemy(int y, int h, bool left) {

	y_limit = y;
	charge_it = 0;
	current_state = EIDLE;
	forward_walking = false;
	hasHit = false;
	leftEnemy = left;

	fallingTimer = new Timer(500);
	hitTimer = new Timer(700);
	attackTimer = new Timer(450);
	chargeTimer = new Timer(300);
	idleTimer = new Timer(1400);

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
	
	dying.frames.push_back(lay_down);
	dying.frames.push_back({ 0, 0, 0, 0 });
	dying.speed = 0.2f;

	recovery.x = 430;
	recovery.y = 230;
	recovery.w = 70;
	recovery.h = 70;

	chargeAttack.x = 0;
	chargeAttack.y = 130;
	chargeAttack.w = 70;
	chargeAttack.h = 70;

	idleAttack.frames.push_back({ 90, 100, 70, 110 });
	idleAttack.frames.push_back({ 170, 100, 70, 110 });
	idleAttack.frames.push_back({ 240, 100, 100, 110 });
	idleAttack.speed = 0.1f;

	forward.frames.push_back({ 150, 10, 60, 90 });
	forward.frames.push_back({ 210, 10, 60, 90 });
	forward.frames.push_back({ 280, 10, 60, 90 });
	forward.frames.push_back({ 340, 10, 60, 90 });
	forward.speed = 0.09f;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, ENEMY);
	hitBoxCol = new Collider(pivot.x, pivot.y - enemyHeight, pivot.w, enemyHeight, this, EHITBOX);
	attackCol = new Collider(pivot.x, pivot.y, 50, 50, this, EFINAL);
	chargeAttackCol = new Collider(pivot.x, pivot.y, 10, enemyHeight, this, EFINAL);
	App->collisions->AddCollider(pivotCol);
	App->collisions->AddCollider(hitBoxCol);
	App->collisions->AddCollider(attackCol);
	App->collisions->AddCollider(chargeAttackCol);

	chargeAttackCol->setActive(false);
	attackCol->setActive(false);

	player = App->player;
	fallFx = App->audio->LoadFx("Game/fx/enemy_fall.wav");
	dyingFx = App->audio->LoadFx("Game/fx/dying.wav");
	lifePoints = 5;
	eastLocked = false;
	westLocked = false;

	debug = false;
	if (debug)
	{
		RELEASE(attackTimer);
		attackTimer = new Timer(5000);
	}
}

Enemy::~Enemy() {
	RELEASE(fallingTimer);
	RELEASE(hitTimer);
	RELEASE(attackTimer);
	RELEASE(chargeTimer);
	RELEASE(idleTimer);
}

bool Enemy::CleanUp() {
	return true;
}

update_status Enemy::PreUpdate() {

	if (player->GetState() == MAGIC)
	{
		current_state = FROZEN;
	}
	else {
		if (current_state == FROZEN)
		{
			current_state = EFALLING_DOWN;
			fallingTimer->resetTimer();
			lifePoints = 0;
		}

	}
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
		if (fallingTimer->hasPassed())
		{
			App->audio->PlayFx(fallFx, 0);
			current_state = ELAY_DOWN;
			fallingTimer->resetTimer();
		}
			
		break;

	case(EDYING):
		if (fallingTimer->hasPassed())
		{
			pivotCol->SetDirty(true);
			hitBoxCol->SetDirty(true);
			attackCol->SetDirty(true);
			chargeAttackCol->SetDirty(true);
			App->enemies->DestroyMe(this);
		}
		break;

	case(ELAY_DOWN):
		if (fallingTimer->hasPassed())
		{
			
			if (lifePoints <= 0)
			{
				App->audio->PlayFx(dyingFx, 0);
				current_state = EDYING;
				fallingTimer->resetTimer();
				//App->enemies->DestroyMe(this);
				
			}
			else {
				current_state = ERECOVERING;
				fallingTimer->resetTimer();
				hitBoxCol->setActive(true);
			}
		}
		break;

	case(ERECOVERING):
		if (fallingTimer->hasPassed())
		{
			current_state = EIDLE;
		}
		break;

	case(EHIT) :
		if (hitTimer->hasPassed())
		{
			current_state = EIDLE;
		}
		break;

	case(EIDLE):
		if ((pivot.x + pivot.w/2) > player->GetScreenWidth())
			forward_walking = false;
		else forward_walking = true;
		if (player->GetState() == FALLING_DOWN || player->GetState() == RECOVERY || player->GetState() == LAY_DOWN)
			return UPDATE_CONTINUE;
		if (idleTimer->hasPassed())
		{
			if (inRange(player->GetScreenHeight()))
			{
				if (!forward_walking)
				{
					if ((player->GetScreenWidth() + 30) >= (pivot.x - attackCol->GetRect()->w + 10) && ((player->GetScreenWidth() + 30) <= pivot.x))
					{
						if (player->GetState() != ATTACKING || player->GetState() != WAITING_INPUT)
						{
							attackTimer->resetTimer();
							idleAttack.resetAnimation();
							//attackCol->setActive(true);
							current_state = EATTACKING;
						}
					}
					else if ((player->GetScreenWidth()+ 30) >= (pivot.x - 150) && (player->GetScreenWidth() <= pivot.x - 100))
					{
						charge_it = 7;
						chargeTimer->resetTimer();
						current_state = ECHARGING;
						chargeAttackCol->setActive(true);
					}
					else{
						current_state = EWALKING;
					}
				}
				else {
					if (player->GetScreenWidth()<= (pivot.x + pivot.w + attackCol->GetRect()->w - 10) && (player->GetScreenWidth() >= pivot.x))
					{
						if (player->GetState() != ATTACKING || player->GetState() != WAITING_INPUT)
						{
							attackTimer->resetTimer();
							idleAttack.resetAnimation();
							//attackCol->setActive(true);
							current_state = EATTACKING;
						}
						
					}
					else if (player->GetScreenWidth() <= (pivot.x + 175) && (player->GetScreenWidth() >= pivot.x + 125))
					{
						charge_it = 7;
						chargeTimer->resetTimer();
						chargeAttackCol->setActive(true);
						current_state = ECHARGING;
					}
					else {
						current_state = EWALKING;
					}
				}
			}
			else current_state = EWALKING;
		}
		break;

	case(EATTACKING) :
		if (attackTimer->hasPassed())
		{
			current_state = EIDLE;
			attackCol->setActive(false);
		}

		if (hasHit)
		{
			hasHit = false;
			idleTimer->resetTimer();
		}
		else {
			if (idleAttack.GetActualFrame().x == idleAttack.frames[2].x)
				attackCol->setActive(true);
		}
		break;

	case(ECHARGING):
		if (chargeTimer->hasPassed())
		{
			chargeAttackCol->setActive(false);
			current_state = EIDLE;
			eastLocked = false;
			westLocked = false;
			if (hasHit)
			{
				hasHit = false;
				idleTimer->resetTimer();
			}
		}
		break;

	case(EWALKING) :
		if ((pivot.x + pivot.w / 2) > player->pivot.x)
			forward_walking = false;
		else forward_walking = true;

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
	case(ECHARGING):
		if (forward_walking)
		{
			if (!eastLocked)
				pivot.x += 9;
		}
		else {
			if (!westLocked)
				pivot.x -= 9;
		}

		break;

	case(EWALKING) :
		if (leftEnemy)
		{
			if (player->GetState() == ATTACKING || player->GetState() == WAITING_INPUT || player->GetState() == JUMPING)
			{
				pivot.x--;
			}
			else {
				if ((pivot.x + pivot.w + attackCol->GetRect()->w - 10) < player->GetScreenWidth())
				{
					pivot.x++;
				}
				else{
					if ((pivot.x + pivot.w) >= player->GetScreenWidth())
						pivot.x--;
					if (!inRange(player->GetScreenHeight()))
					{
						if (pivot.y > player->GetScreenHeight())
							pivot.y--;
						else pivot.y++;
					}
					else current_state = EIDLE;
				}
			}
		}
		else {
			if (player->GetState() == ATTACKING || player->GetState() == WAITING_INPUT || player->GetState() == JUMPING)
			{
				pivot.x++;
			}
			else {
				if ((pivot.x + 10) < player->GetScreenWidth() + 30)
				{
					pivot.x++;
					if (!inRange(player->GetScreenHeight()))
					{
						if (pivot.y > player->GetScreenHeight())
							pivot.y--;
						else pivot.y++;
					}
				}
				else{
					if ((pivot.x) >= player->GetScreenWidth() + 30)
						pivot.x--;
					if (!inRange(player->GetScreenHeight()))
					{
						if (pivot.y > player->GetScreenHeight())
							pivot.y--;
						else pivot.y++;
					}
					else current_state = EIDLE;
				}
			}

	}
	break;
}

	pivotCol->SetPosition(pivot.x, pivot.y);
	hitBoxCol->SetPosition(pivot.x, pivot.y - enemyHeight);
	if (forward_walking)
	{
		attackCol->SetPosition(pivot.x + pivot.w, pivot.y - enemyHeight);
		chargeAttackCol->SetPosition(pivot.x + pivot.w, pivot.y - enemyHeight);
	}
		
	else {
		attackCol->SetPosition(pivot.x - attackCol->GetRect()->w, pivot.y - enemyHeight);
		chargeAttackCol->SetPosition(pivot.x - chargeAttackCol->GetRect()->w, pivot.y - enemyHeight);
	}

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

	
	case(ECHARGING):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &chargeAttack);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - 60 - GetFallHeight(charge_it), &chargeAttack);
		if (charge_it <= 28)
			charge_it++;
		break;

	case(EDYING):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x + 10, pivot.y - lay_down.h + 15, &dying.GetCurrentFrame());
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - lay_down.h + 15, &dying.GetCurrentFrame());
		break;

	case(EWALKING):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - enemyHeight + 10, &forward.GetCurrentFrame());
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - enemyHeight + 10, &forward.GetCurrentFrame());
		break;

	case(FROZEN):
		if (forward_walking)
			ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - enemyHeight + 10, &forward.frames[1]);
		else ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x, pivot.y - enemyHeight + 10, &forward.frames[1]);
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
				ret = App->renderer->Blit(App->enemies->graphics, pivot.x, pivot.y - 105, &frame);
		}
		else {
			if (frame.x == idleAttack.frames[0].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 10, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[1].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 20, pivot.y - 105, &frame);
			else if (frame.x == idleAttack.frames[2].x)
				ret = App->renderer->BlitFlipH(App->enemies->graphics, pivot.x - 50, pivot.y - 105, &frame);
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
			fallingTimer->resetTimer();
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
				fallingTimer->resetTimer();
				hitBoxCol->setActive(false);
			}
			else{
				current_state = EHIT;
				hitTimer->resetTimer();
			}
			if (player->pivot.x > pivot.x)
				forward_walking = true;
			else forward_walking = false;
		}
		
	}
	else if (b->getType() == PHITBOX) {
		if (inRange(player->GetScreenHeight()))
		{
			hasHit = true;
			attackCol->setActive(false);
			if (current_state == ECHARGING)
			{
				chargeAttackCol->setActive(false);
				if (forward_walking)
					eastLocked = true;
				else westLocked = true;
			}
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

int Enemy::GetScreenWidth() const {
	return pivot.x;
}