#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleCollisions.h"
#include "ModuleSceneLevel1.h"

ModulePlayer::ModulePlayer(bool enabled) : Module(enabled) {
	player_height = 50;
	repaint_frame = false;

	jumping_up = false;
	max_jumpheight = 80;
	alpha_jump = 4;

	northLocked = false;
	eastLocked = false;
	westLocked = false;
	southLocked = false;
	
	
	attackState = NONE;
	jumpAttackWindow = new Timer(200);
	attackWindow = new Timer(400);
	attackAnimation = new Timer(300);
	chargeAttackTimer = new Timer(400);
	fallingTimer = new Timer(400);
	hasHit = false;
	//Time to wait to change to idle frame when idle
	idleTimer = new Timer(750);

	runTimer = new Timer(300);
	isRunning = false;

	pivot.x = 60;
	pivot.y = 200;
	pivot.h = 10;
	pivot.w = 30;

	

	// Idle frame has to be painted at pivor.x - 15 to fit in
	idle.x = 135;
	idle.y = 10;
	idle.w = 60;
	idle.h = 60;
	last_frame = idle;

	//Flipped forward animation has to be painted at pivot.x - 30 to fit in
	forward.frames.push_back({ 0, 90, 60, 60 });
	forward.frames.push_back({ 60, 90, 60, 60 });
	forward.frames.push_back({ 120, 90, 60, 60 });
	forward.frames.push_back({ 180, 90, 60, 60 });
	forward.speed = 0.09f;

	up.frames.push_back({ 260, 90, 30, 60 });
	up.frames.push_back({ 290, 90, 30, 60 });
	up.frames.push_back({ 320, 90, 30, 60 });
	up.frames.push_back({ 350, 90, 30, 60 });
	up.speed = 0.09f;

	jump_up.x = 350;
	jump_up.y = 310;
	jump_up.w = 60;
	jump_up.h = 80;

	jump_down.x = 420;
	jump_down.y = 310;
	jump_down.w = 40;
	jump_down.h = 80;

	run.frames.push_back({ 0, 170, 50, 50 });
	run.frames.push_back({ 50, 170, 50, 50 });
	run.frames.push_back({ 100, 170, 50, 50 });
	run.frames.push_back({ 150, 170, 50, 50 });
	run.speed = 0.09f;

	jumpattack.x = 470;
	jumpattack.y = 320;
	jumpattack.w = 80;
	jumpattack.h = 70;

	chargeattack.x = 220;
	chargeattack.y = 180;
	chargeattack.w = 50;
	chargeattack.h = 40;

	idleattack.frames.push_back({ 0, 250, 60, 50 });
	idleattack.frames.push_back({ 70, 250, 80, 50 });
	idleattack.frames.push_back({ 150, 250, 70, 50 });
	idleattack.speed = 0.09f;

	waitingCombo2.x = 150;
	waitingCombo2.y = 250;
	waitingCombo2.w = 70;
	waitingCombo2.h = 50;
	
	waitingCombo3.x = 0;
	waitingCombo3.y = 250;
	waitingCombo3.w = 60;
	waitingCombo3.h = 50;

	combo2.frames.push_back({ 150, 250, 70, 50 });
	combo2.frames.push_back({ 220, 250, 80, 50 });
	combo2.frames.push_back({ 300, 250, 80, 50 });
	combo2.speed = 0.09f;

	combo3.frames.push_back({ 0, 320, 60, 70 });
	combo3.frames.push_back({ 60, 310, 60, 80 });
	combo3.frames.push_back({ 120, 330, 70, 60 });
	combo3.speed = 0.15f;

	falling_down.x = 50;
	falling_down.y = 560;
	falling_down.w = 70;
	falling_down.h = 60;

	lay_down.x = 120;
	lay_down.y = 560;
	lay_down.w = 80;
	lay_down.h = 60;

	recovery.frames.push_back({ 220, 570, 50, 50 });
	recovery.frames.push_back({ 270, 570, 50, 50 });
	recovery.speed = 0.08f;

	dying.frames.push_back({ 120, 560, 80, 60 });
	dying.frames.push_back({ 0, 0, 0, 0 });
	dying.speed = 0.09f;

	debug = false;
	if (debug)
	{
		RELEASE(fallingTimer);
		fallingTimer = new Timer(5000);
		recovery.speed = 0.01f;
	}
}

ModulePlayer::~ModulePlayer() { 
	RELEASE(idleTimer);
	RELEASE(runTimer);
	RELEASE(attackWindow);
	RELEASE(jumpAttackWindow);
	RELEASE(attackAnimation);
	RELEASE(chargeAttackTimer);
	RELEASE(fallingTimer);
}

bool ModulePlayer::Start() {
	bool ret = true;

	pivot.x = 60;
	pivot.y = 200;
	current_state = IDLE;
	barHits = 2;
	lifeBars = 3;
	lives = 3;
	magicFlasks = 1;
	forward_walking = true;

	graphics = App->textures->Load("Game/Sprites/dwarf.png");	// Sprite sheet
	attack_fx = App->audio->LoadFx("Game/fx/attack.wav");		// Attack audio effect
	hit1_fx = App->audio->LoadFx("Game/fx/hit1.wav");
	hit2_fx = App->audio->LoadFx("Game/fx/hit2.wav");
	charge_fx = App->audio->LoadFx("Game/fx/charge.wav");
	dying_fx = App->audio->LoadFx("Game/fx/dying.wav");
	fall_fx = App->audio->LoadFx("Game/fx/enemy_fall.wav");

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, PLAYER);
	hitBoxCol = new Collider(pivot.x, pivot.y - player_height, pivot.w, player_height, this, PHITBOX);
	chargeAttackCol = new Collider(pivot.x + pivot.w, pivot.y - player_height, 10, 30, this, PFINAL);
	idleAttackCol = new Collider(pivot.x + pivot.w, pivot.y - 40, 40, 40, this, PATTACK);
	finalComboCol = new Collider(pivot.x + pivot.w, pivot.y - 40, 40, 40, this, PFINAL);
	jumpAttackCol = new Collider(pivot.x + pivot.w, pivot.y - player_height, 30, player_height + 20, this, PATTACK);

	App->collisions->AddCollider(pivotCol);
	App->collisions->AddCollider(hitBoxCol);
	App->collisions->AddCollider(chargeAttackCol);
	App->collisions->AddCollider(idleAttackCol);
	App->collisions->AddCollider(jumpAttackCol);
	App->collisions->AddCollider(finalComboCol);

	chargeAttackCol->setActive(false);
	idleAttackCol->setActive(false);
	jumpAttackCol->setActive(false);
	finalComboCol->setActive(false);
	return ret;
}


update_status ModulePlayer::PreUpdate() {
	
	if (chargeAttackTimer->hasPassed())
	{
		southLocked = false;
		northLocked = false;
		eastLocked = false;
		westLocked = false;
	}
	if (current_state == FALLING_DOWN)
	{
		if (fallingTimer->hasPassed())
		{
			current_state = LAY_DOWN;
			fallingTimer->resetTimer();
			App->audio->PlayFx(fall_fx, 0);
		}
		return UPDATE_CONTINUE;
	}
	else if (current_state == LAY_DOWN)
	{
		
		if (fallingTimer->hasPassed())
		{
			if (lifeBars <= 0)
			{
				App->audio->PlayFx(dying_fx, 0);
				lives--;
				//current_state = DYING;
				if (lives > 0)
					lifeBars = 3;
				current_state = DYING;
				idleTimer->resetTimer();
			}
			else {
				current_state = RECOVERY;
				fallingTimer->resetTimer();
			}
		}
		return UPDATE_CONTINUE;
	}
	else if (current_state == RECOVERY)
	{
		if (fallingTimer->hasPassed())
		{
			hitBoxCol->setActive(true);
			current_state = IDLE;
		}
		return UPDATE_CONTINUE;
	}
	else if (current_state == DYING)
	{
		if (idleTimer->hasPassed())
		{
			if (lives <= 0)
				App->level1->TriggerGameOver();
			else {
				hitBoxCol->setActive(true);
				current_state = IDLE;
			}
		}
		return UPDATE_CONTINUE;
	}

	if (current_state == ATTACKING)
	{
		if (attackAnimation->hasPassed())
		{
			if (hasHit)		//hit Registered, go to next attackState
			{
				hasHit = false;
				switch (attackState) {
				case(IDLEATTACK) :
					App->audio->PlayFx(hit1_fx, 0);
					idleattack.resetAnimation();
					attackState = COMBO_2;
					current_state = WAITING_INPUT;
					attackWindow->resetTimer();
					break;
				case(COMBO_2) :
					App->audio->PlayFx(hit2_fx, 0);
					combo2.resetAnimation();
					attackState = COMBO_3;
					current_state = WAITING_INPUT;
					attackWindow->resetTimer();
					break;
				case(COMBO_3) :
					//App->audio->PlayFx(hit2_fx, 0);
					App->audio->PlayFx(charge_fx, 0);
					attackState = FINISH_AXE;
					current_state = WAITING_INPUT;
					attackWindow->resetTimer();
					break;
				}
				return UPDATE_CONTINUE;
			}
			else {		//Animation ended and no hit, back to normal
				if (combo3.GetActualFrame().x == combo3.frames[2].x)
				{
					finalComboCol->setActive(true);
					App->audio->PlayFx(charge_fx, 0);
				}
				else App->audio->PlayFx(attack_fx, 0);
				current_state = IDLE;
				attackState = NONE;
				idleattack.resetAnimation();
				combo2.resetAnimation();
				combo3.resetAnimation();
			}
		}
		else{
			if (!hasHit)
			{
				if (idleattack.GetActualFrame().x == idleattack.frames[1].x)
					idleAttackCol->setActive(true);
				else if (combo2.GetActualFrame().x == combo2.frames[1].x)
					idleAttackCol->setActive(true);
			}
			
			return UPDATE_CONTINUE;
		}
			
	}
	if (jumpAttackWindow->hasPassed() && attackWindow->hasPassed() && attackState != CHARGEATTACK)
	{
		attackState = NONE;
		//current_state = IDLE;
	}

	if (current_state != JUMPING)
	{
		
		if (!chargeAttackTimer->hasPassed() && hasHit)
		{
			if (forward_walking)
				eastLocked = true;
			else westLocked = true;
		}

		if (chargeAttackTimer->hasPassed() && attackWindow->hasPassed())
		{
			chargeAttackCol->setActive(false);
			idleAttackCol->setActive(false);
			jumpAttackCol->setActive(false);
			finalComboCol->setActive(false);
			current_state = IDLE;
			attackState = NONE;
			hasHit = false;
		}

		if (current_state == IDLE)
		{
			if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
				isRunning = false;

			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			{
				if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
				{
					if (!runTimer->hasPassed())	//Inside the run timer window
					{
						current_state = RUN_FORWARD;
						last_frame = forward.frames[0];
						forward_walking = true;
						isRunning = true;
					}
					else {
						runTimer->resetTimer();
						current_state = FORWARD;
						forward_walking = true;
						isRunning = false;
					}
				}
				else {
					if (!isRunning) {
						current_state = FORWARD;
						forward_walking = true;
						if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
							current_state = UP_FORWARD;
						else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
							current_state = DOWN_FORWARD;
					}
					else{
						current_state = RUN_FORWARD;
						if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
						{
							if (attackState != CHARGEATTACK)
							{
								chargeAttackCol->setActive(true);
								charge_it = 0;
								attackState = CHARGEATTACK;
								chargeAttackTimer->resetTimer();
							}
						}

					}
				}

				idleTimer->resetTimer();
			}
			else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
			{
				if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
				{
					if (!runTimer->hasPassed())
					{
						current_state = RUN_BACKWARD;
						last_frame = forward.frames[0];
						forward_walking = false;
						isRunning = true;
					}
					else{
						runTimer->resetTimer();
						current_state = BACKWARD;
						forward_walking = false;
						isRunning = false;
					}
				}
				else {
					if (!isRunning) {
						current_state = BACKWARD;
						forward_walking = false;
						if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
							current_state = DOWN_BACKWARD;
						else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
							current_state = UP_BACKWARD;
					}
					else{
						current_state = RUN_BACKWARD;
						if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
						{
							if (attackState != CHARGEATTACK)
							{
								chargeAttackCol->setActive(true);
								charge_it = 0;
								attackState = CHARGEATTACK;
								chargeAttackTimer->resetTimer();
							}
						}
					}
				}

				idleTimer->resetTimer();
			}
			else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			{
				isRunning = false;
				current_state = DOWN;
				idleTimer->resetTimer();
			}
			else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			{
				isRunning = false;
				current_state = UP;
				idleTimer->resetTimer();
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			{
				if (isRunning)
					max_jumpheight = 140;
				isRunning = false;
				current_state = JUMPING;
				jumping_up = true;
				jump_it = 1;
			}
			else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
			{
				if (attackWindow->hasPassed() && attackState != CHARGEATTACK)
				{
					idleTimer->resetTimer();
					attackAnimation->resetTimer();
					idleattack.resetAnimation();
					current_state = ATTACKING;
					attackState = IDLEATTACK;
					attackWindow->resetTimer();
					//App->audio->PlayFx(attack_fx, 0);
					//idleAttackCol->setActive(true);
				}
			}
		}
		else if (current_state == WAITING_INPUT)
		{
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
			{
				idleTimer->resetTimer();
				current_state = ATTACKING;
				attackWindow->resetTimer();
				attackAnimation->resetTimer();
				// Play fx
				//idleAttackCol->setActive(true);
				if (attackState == COMBO_2)
					combo2.resetAnimation();
				else if (attackState == COMBO_3)
				{
					combo3.resetAnimation();
					idleAttackCol->setActive(false);
				}
				else if (attackState == FINISH_AXE)
				{
					idleattack.resetAnimation();
					attackState = IDLEATTACK;
				}
			}
			else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			{
				idleTimer->resetTimer();
				current_state = IDLE;
				attackState = NONE;
				hasHit = false;
			}
		}
		
	}
	else{			//JUMPING
		if (alpha_jump*jump_it >= 175 && !jumping_up)
		{
			current_state = IDLE;
			max_jumpheight = 80;		
			hasHit = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			if (jumpAttackWindow->hasPassed() && !hasHit)
			{
				jumpAttackCol->setActive(true);
				App->audio->PlayFx(attack_fx, 0);
				attackState = JUMPATTACK;
				jumpAttackWindow->resetTimer();
			}
		}
		if (jumpAttackWindow->hasPassed())
		{
			attackState = NONE;
			jumpAttackCol->setActive(false);
		}


		if (alpha_jump*jump_it >= 90)	//Max jump degree
			jumping_up = false;
	}
	
	if (idleTimer->hasPassed())
		repaint_frame = false;
	else repaint_frame = true;

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::Update() {
	bool ret = true;
	
	//Set pivot position

	switch (current_state) {
	case(FALLING_DOWN):
		if (forward_walking)
		{
			if (!westLocked)
				pivot.x -= 2;
		}
		else{
			if (!eastLocked)
				pivot.x += 2;
		}
		break;

	case(FORWARD) :
		if (!eastLocked)
			pivot.x += 2;
		break;
	case(BACKWARD) :
		if (!westLocked)
			pivot.x -= 2;
		break;
	case(DOWN) :
		if (!southLocked)
			pivot.y++;
		break;
	case(DOWN_BACKWARD) :
		if (!southLocked)
			pivot.y++;
		if (!westLocked)
			pivot.x--;
		break;
	case(DOWN_FORWARD) :
		if (!southLocked)
			pivot.y++;
		if (!eastLocked)
			pivot.x++;
		break;
	case(UP) :
		if (!northLocked)
			pivot.y--;
		break;
	case(UP_FORWARD) :
		if (!northLocked)
			pivot.y--;
		if (!eastLocked)
			pivot.x++;
		break;
	case(UP_BACKWARD) :
		if (!northLocked)
			pivot.y--;
		if (!westLocked)
			pivot.x--;
		break;
	case(RUN_FORWARD) :
		if (!eastLocked)
			pivot.x += 4;
		else {
			if (attackState != CHARGEATTACK)
			{
				isRunning = false;
				current_state = FORWARD;
			}
		}
		break;
	case(RUN_BACKWARD) :
		if (!westLocked)
			pivot.x -= 4;
		else {
			if (attackState != CHARGEATTACK)
			{
				current_state = BACKWARD;
				isRunning = false;
			}
		}
		break;
	}

	if (current_state == JUMPING)
		hitBoxCol->SetPosition(pivot.x, pivot.y - player_height - getJumpHeight(jump_it) + 5);
	else hitBoxCol->SetPosition(pivot.x, pivot.y - player_height);
	pivotCol->SetPosition(pivot.x, pivot.y);
	if (forward_walking)
	{
		idleAttackCol->SetPosition(pivot.x + pivot.w, pivot.y - 40);
		jumpAttackCol->SetPosition(pivot.x + pivot.w + 10, pivot.y - 50 - getJumpHeight(jump_it));
		chargeAttackCol->SetPosition(pivot.x + pivot.w, pivot.y - player_height);
		finalComboCol->SetPosition(pivot.x + pivot.w, pivot.y - 40);
	}
	else {
		idleAttackCol->SetPosition(pivot.x - idleAttackCol->GetRect()->w, pivot.y - 40);
		jumpAttackCol->SetPosition(pivot.x - 50, pivot.y - 50 - getJumpHeight(jump_it));
		chargeAttackCol->SetPosition(pivot.x - 10, pivot.y - player_height);
		finalComboCol->SetPosition(pivot.x - finalComboCol->GetRect()->w, pivot.y - 40);
	}

	if (ret)
		return UPDATE_CONTINUE;
	else return UPDATE_ERROR;
}

update_status ModulePlayer::PostUpdate(){

	return UPDATE_CONTINUE;
}


bool ModulePlayer::CleanUp() {
	LOG("Unloading player.");

	App->textures->Unload(graphics);
	return true;
}

bool ModulePlayer::OnCollision(Collider* a, Collider* b){
	if (b->getType() == WORLD)
		northLocked = true;
	else if (b->getType() == CAMERA)
	{
		if (b->GetRect()->y >= pivot.y)
			southLocked = true;
		else if (b->GetRect()->x <= pivot.x)
			westLocked = true;
		else if (b->GetRect()->x >= pivot.x)
			eastLocked = true;
	}

	if (a->getType() == PATTACK || a->getType() == PFINAL) {
		if (inRange(b->GetListener()->GetScreenHeight()))
		{
			if (attackState != JUMPATTACK && attackState != CHARGEATTACK)
			{
				idleAttackCol->setActive(false);
				finalComboCol->setActive(false);
				attackWindow->resetTimer();
				hasHit = true;
			}
			else if (attackState == CHARGEATTACK)
			{
				App->audio->PlayFx(charge_fx, 0);
				chargeAttackCol->setActive(false);
				hasHit = true;
			}
			else if (attackState == JUMPATTACK)
			{
				hasHit = true;
				jumpAttackCol->setActive(false);
				//App->audio->PlayFx(hit1_fx, 0);
			}
				
		}
	}

	if (b->getType() == EFINAL)
	{
		if (inRange(b->GetListener()->GetScreenHeight()))
		{
			App->audio->PlayFx(charge_fx, 0);
			if (b->GetListener()->GetScreenWidth() > pivot.x)
				forward_walking = true;
			else forward_walking = false;
			charge_it = 0;
			hitBoxCol->setActive(false);
			barHits--;
			if (barHits <= 0)
			{
				lifeBars--;
				barHits = 2;
			}
			fallingTimer->resetTimer();
			current_state = FALLING_DOWN;
		}
		
	}
	
	return false;
}

int ModulePlayer::getJumpHeight(int i) const{
	double value = sin((alpha_jump*i*M_PI)/ 180);
	return (int)(max_jumpheight*value);
}

int ModulePlayer::getChargeHeight(int i) const{
	double value = sin(8*(i*M_PI) / 180);
	return (int)15 * value;
}

bool ModulePlayer::Draw() {
	if (debug)
	{
		forward_walking = true;
		current_state = DYING;
	}

	bool ret = true;
	//Draw Animations
	switch (current_state)
	{
	case(IDLE) :
		if (repaint_frame)
		{
			if (forward_walking)
			{
				if (last_frame.y == 250)
					ret = App->renderer->Blit(graphics, pivot.x - 16, pivot.y - player_height + 6, &last_frame);
				else ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
			}
			else {
				if (last_frame.w == 60 && last_frame.y != 250)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
				else if (last_frame.y == 250)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 14, pivot.y - player_height + 6, &last_frame);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
			}
		}
		else if (forward_walking) ret = App->renderer->Blit(graphics, pivot.x - 15, pivot.y - player_height, &idle);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x - 15, pivot.y - player_height, &idle);
		break;
	case(FORWARD) :
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(BACKWARD) :
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case(DOWN) :
		last_frame = forward.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_FORWARD) :
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_BACKWARD) :
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case (UP) :
		last_frame = up.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (UP_FORWARD) :
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(UP_BACKWARD) :
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (JUMPING) :
		if (attackState == JUMPATTACK)
		{
			if (forward_walking)
				ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height - getJumpHeight(jump_it), &jumpattack);
			else ret = App->renderer->BlitFlipH(graphics, pivot.x - 50, pivot.y - player_height - getJumpHeight(jump_it), &jumpattack);
		}
		else {
			if (jumping_up)
			{
				if (forward_walking)
					ret = App->renderer->Blit(graphics, pivot.x - 25, pivot.y - player_height - getJumpHeight(jump_it), &jump_up);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x - 5, pivot.y - player_height - getJumpHeight(jump_it), &jump_up);

			}
			else {

				if (forward_walking)
					ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height - getJumpHeight(jump_it), &jump_down);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x - 5, pivot.y - player_height - getJumpHeight(jump_it), &jump_down);
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			forward_walking = true;
			if (!eastLocked)
				pivot.x++;
		}
		else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			forward_walking = false;
			if (!westLocked)
				pivot.x--;
		}

		jump_it++;
		break;
	case (RUN_FORWARD) :
		if (attackState != CHARGEATTACK)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height + 5, &run.GetCurrentFrame());
		else{
			if (!eastLocked)
				pivot.x++;
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height - getChargeHeight(charge_it) + 10, &chargeattack);
			charge_it++;
		}
		break;
	case (RUN_BACKWARD) :
		if (attackState != CHARGEATTACK)
			ret = App->renderer->BlitFlipH(graphics, pivot.x - 15, pivot.y - player_height + 5, &run.GetCurrentFrame());
		else{
			if (!westLocked)
				pivot.x--;
			ret = App->renderer->BlitFlipH(graphics, pivot.x - 5, pivot.y - player_height - getChargeHeight(charge_it) + 10, &chargeattack);
			charge_it++;
		}
		break;
	case(ATTACKING) :
		
		SDL_Rect frame;
		switch (attackState) {
		case(IDLEATTACK):
			last_frame = idleattack.frames[0];
			frame = idleattack.GetCurrentFrame();
			if (forward_walking)
			{
				if (frame.x == idleattack.frames[2].x)
					ret = App->renderer->Blit(graphics, pivot.x - 25, pivot.y - player_height + 5, &frame);
				else if (frame.x == idleattack.frames[1].x)
					ret = App->renderer->Blit(graphics, pivot.x - 10, pivot.y - player_height + 5, &frame);
				else ret = App->renderer->Blit(graphics, pivot.x - 16, pivot.y - player_height + 6, &frame);
			}
			else {
				if (frame.x == idleattack.frames[0].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 14, pivot.y - player_height + 6, &frame);	//frame 0 idleattack animation
				else if (frame.x == idleattack.frames[2].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 10, pivot.y - player_height + 6, &frame);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x - 40, pivot.y - player_height + 5, &frame);	//frame 1
			}
			break;
		case(COMBO_2) :
			frame = combo2.GetCurrentFrame();
			if (forward_walking)
			{
				if (frame.x == combo2.frames[1].x)
					ret = App->renderer->Blit(graphics, pivot.x-3, pivot.y - player_height + 5, &frame);
				else if (frame.x == combo2.frames[2].x)
					ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height + 5, &frame);
				else ret = App->renderer->Blit(graphics, pivot.x - 25, pivot.y - player_height + 5, &frame);
			}
			else  {
				if (frame.x == combo2.frames[1].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 46, pivot.y - player_height + 6, &frame);
				else if (frame.x == combo2.frames[2].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 50, pivot.y - player_height + 6, &frame);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x - 13, pivot.y - player_height + 6, &frame);
			}
			break;
		case(COMBO_3) :
			frame = combo3.GetCurrentFrame();
			if (forward_walking)
			{
				if (frame.x == combo3.frames[0].x)
					ret = App->renderer->Blit(graphics, pivot.x - 12, pivot.y - player_height - 10, &frame);
				else if (frame.x == combo3.frames[1].x)
					ret = App->renderer->Blit(graphics, pivot.x + 4, pivot.y - player_height - 20, &frame);
				else ret = App->renderer->Blit(graphics, pivot.x + 2, pivot.y - player_height, &frame);
			}
			else {
				if (frame.x == combo3.frames[0].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 14, pivot.y - player_height - 9, &frame);
				else if (frame.x == combo3.frames[1].x)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 32, pivot.y - player_height - 19, &frame);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x - 40, pivot.y - player_height + 1, &frame);
			}
		}
		break;
	case(WAITING_INPUT):
		switch (attackState) {
		case(COMBO_2):
			if (forward_walking)
			{
				ret = App->renderer->Blit(graphics, pivot.x - 25, pivot.y - player_height + 5, &waitingCombo2);
			}
			else {
				ret = App->renderer->BlitFlipH(graphics, pivot.x - 13, pivot.y - player_height + 6, &waitingCombo2);
			}
			break;
		case(COMBO_3):
			if (forward_walking)
			{
				ret = App->renderer->Blit(graphics, pivot.x - 16, pivot.y - player_height + 6, &waitingCombo3);
			}
			else {
				ret = App->renderer->BlitFlipH(graphics, pivot.x - 14, pivot.y - player_height + 6, &waitingCombo3);
			}
			break;
		case(FINISH_AXE):
			if (forward_walking)
			{
				ret = App->renderer->Blit(graphics, pivot.x + 2, pivot.y - player_height, &combo3.frames[2]);
			}
			else {
				ret = App->renderer->BlitFlipH(graphics, pivot.x - 40, pivot.y - player_height + 1, &combo3.frames[2]);
			}
			break;
		}
		break;

	case(FALLING_DOWN):
		if (forward_walking)
		{
			ret = App->renderer->Blit(graphics, pivot.x - 10, pivot.y - player_height - getChargeHeight(charge_it), &falling_down);
		}
		else {
			ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height - getChargeHeight(charge_it), &falling_down);
		}
		charge_it++;
		break;

	case(LAY_DOWN):
		if (forward_walking)
		{
			ret = App->renderer->Blit(graphics, pivot.x - 10, pivot.y - player_height, &lay_down);
		}
		else {
			ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &lay_down);
		}
		break;

	case(RECOVERY):
		if (forward_walking)
		{
			ret = App->renderer->Blit(graphics, pivot.x-10, pivot.y - player_height +5, &recovery.GetCurrentFrame());
		}
		else {
			ret = App->renderer->BlitFlipH(graphics, pivot.x - 10, pivot.y - player_height + 5, &recovery.GetCurrentFrame());
		}
		break;

	case(DYING):
		if (forward_walking)
		{
			ret = App->renderer->Blit(graphics, pivot.x - 7, pivot.y - player_height, &dying.GetCurrentFrame());
		}
		else {
			ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &dying.GetCurrentFrame());
		}
		break;
	}

	return ret;
}

int ModulePlayer::GetScreenHeight() const {
	return pivot.y;
}

int ModulePlayer::GetLifeBars() const {
	if (lifeBars >= 0)
		return lifeBars;
	else return 0;
}

int ModulePlayer::GetLives()  const{
	return lives;
}

int ModulePlayer::GetMagicFlasks() const {
	return magicFlasks;
}

void ModulePlayer::AddMagicFlask() {
	if (magicFlasks < 4)
		magicFlasks++;
}

bool ModulePlayer::inRange(int y) const {
	return ((y - 5) <= pivot.y) && ((y + 5) >= pivot.y);
}

state ModulePlayer::GetState() const {
	return current_state;
}