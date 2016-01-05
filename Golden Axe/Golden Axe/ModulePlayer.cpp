#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleCollisions.h"

ModulePlayer::ModulePlayer(bool enabled) : Module(enabled) {
	forward_walking = true;
	repaint_frame = false;

	jumping_up = false;
	max_jumpheight = 80;
	alpha_jump = 4;

	northLocked = false;
	eastLocked = false;
	westLocked = false;
	southLocked = false;
	//Time to wait to change to idle frame when idle
	idle_timer = new Timer(750);

	run_timer = new Timer(300);
	isRunning = false;

	pivot.x = 60;
	pivot.y = 200;
	pivot.h = 10;
	pivot.w = 30;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, PLAYER);

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

	player_height = 50;
}

ModulePlayer::~ModulePlayer() { 
	RELEASE(idle_timer);
}

bool ModulePlayer::Start() {
	bool ret = true;
	current_state = IDLE;

	graphics = App->textures->Load("Game/Sprites/dwarf.png");	// Sprite sheet
	attack_fx = App->audio->LoadFx("Game/fx/attack.wav");		// Attack audio effect

	App->collisions->AddCollider(pivotCol);
	return ret;
}


update_status ModulePlayer::PreUpdate() {
	
	southLocked = false;
	northLocked = false;
	eastLocked = false;
	westLocked = false;
	if (current_state != JUMPING)
	{
		current_state = IDLE;
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
			isRunning = false;

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		{
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			{
				if (!run_timer->hasPassed())	//Inside the run timer window
				{
					current_state = RUN_FORWARD;
					last_frame = forward.frames[0];
					forward_walking = true;
					isRunning = true;
				}
				else{
					run_timer->resetTimer();
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
				else current_state = RUN_FORWARD;
			}

			idle_timer->resetTimer();
		}
		else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		{
			if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
			{
				if (!run_timer->hasPassed())
				{
					current_state = RUN_BACKWARD;
					last_frame = forward.frames[0];
					forward_walking = false;
					isRunning = true;
				}
				else{
					run_timer->resetTimer();
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
				else current_state = RUN_BACKWARD;
			}

			idle_timer->resetTimer();
		}
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			isRunning = false;
			current_state = DOWN;
			idle_timer->resetTimer();
		}
		else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			isRunning = false;
			current_state = UP;
			idle_timer->resetTimer();
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			if (isRunning)
				max_jumpheight = 120;
			isRunning = false;
			current_state = JUMPING;
			jumping_up = true;
			jump_it = 1;
		}
	}
	else{			//JUMPING
		if (getJumpHeight(jump_it) <= 8 && !jumping_up)
		{
			current_state = IDLE;
			max_jumpheight = 80;
		}

		if (alpha_jump*jump_it >= 90)	//Max jump degree
			jumping_up = false;
	}
	


	if (idle_timer->hasPassed())
		repaint_frame = false;
	else repaint_frame = true;

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::Update() {
	bool ret = true;
	
	//Set pivot position
	
	switch (current_state) {
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
		break;
	case(RUN_BACKWARD) :
		if (!westLocked)
			pivot.x -= 4;
		break;
	}

	//Draw Animations
	switch (current_state)
	{
	case(IDLE) :
		if (repaint_frame)
		{
			if (forward_walking)
			{
				ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
			}
			else {
				if (last_frame.w == 60)
					ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
				else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
			}
		}
		else if (forward_walking) ret = App->renderer->Blit(graphics, pivot.x - 15, pivot.y - player_height, &idle);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x - 15, pivot.y - player_height, &idle);
		break;
	case(FORWARD) :
		//pivot.x += 2;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(BACKWARD) :
		//pivot.x -= 2;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case(DOWN) :
		//pivot.y++;
		last_frame = forward.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_FORWARD) :
		//pivot.y++;
		//pivot.x++;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_BACKWARD) :
		//pivot.y++;
		//pivot.x--;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case (UP) :
		//pivot.y--;
		last_frame = up.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (UP_FORWARD) :
		//pivot.y--;
		//pivot.x++;
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(UP_BACKWARD) :
		//pivot.y--;
		//pivot.x--;
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (JUMPING) :
		if (jumping_up)
		{
			if (forward_walking)
				ret = App->renderer->Blit(graphics, pivot.x - 25, pivot.y - player_height - abs(getJumpHeight(jump_it)), &jump_up);
			else ret = App->renderer->BlitFlipH(graphics, pivot.x - 5, pivot.y - player_height - abs(getJumpHeight(jump_it)), &jump_up);
		}
		else {
			if (forward_walking)
				ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height - abs(getJumpHeight(jump_it)), &jump_down);
			else ret = App->renderer->BlitFlipH(graphics, pivot.x - 5, pivot.y - player_height - abs(getJumpHeight(jump_it)), &jump_down);
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
		//pivot.x += 4;
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height + 5, &run.GetCurrentFrame());
		break;
	case (RUN_BACKWARD) :
		//pivot.x -= 4;
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 15, pivot.y - player_height + 5, &run.GetCurrentFrame());
		break;
	}

	pivotCol->SetPosition(pivot.x, pivot.y);

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
		if (b->GetRect()->x <= pivot.x && b->GetRect()->y == 0)
			westLocked = true;
		else if (b->GetRect()->x >= pivot.x)
			eastLocked = true;
	}
	
	return false;
}

int ModulePlayer::getJumpHeight(int i) {
	double value = sin((alpha_jump*i*M_PI)/ 180);
	return (int)(max_jumpheight*value);
}