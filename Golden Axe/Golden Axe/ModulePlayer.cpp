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
	idle_timer = new Timer(750);

	pivot.x = 30;
	pivot.y = 200;
	pivot.h = 10;
	pivot.w = 30;

	pivotCol = new Collider(pivot.x, pivot.y, pivot.w, pivot.h, this, PLAYER);

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
	current_state = IDLE;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		current_state = FORWARD;
		forward_walking = true;
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			current_state = UP_FORWARD;
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			current_state = DOWN_FORWARD;

		idle_timer->resetTimer();
	}		
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		current_state = BACKWARD;
		forward_walking = false;
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			current_state = DOWN_BACKWARD;
		else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			current_state = UP_BACKWARD;

		idle_timer->resetTimer();
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		current_state = DOWN;
		idle_timer->resetTimer();
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		current_state = UP;
		idle_timer->resetTimer();
	}


	if (idle_timer->hasPassed())
		repaint_frame = false;
	else repaint_frame = true;

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::Update() {
	bool ret = true;
	switch (current_state)
	{
	case(IDLE):
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
		else if (forward_walking) ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &idle);
			else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &idle);
		break;
	case(FORWARD) :
		pivot.x += 2;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(BACKWARD) :
		pivot.x -= 2;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x-30, pivot.y - player_height, &last_frame);
		break;
	case(DOWN) :
		pivot.y++;
		last_frame = forward.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x-30, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_FORWARD) :
		pivot.y++;
		pivot.x++;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (DOWN_BACKWARD) :
		pivot.y++;
		pivot.x--;
		last_frame = forward.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x - 30, pivot.y - player_height, &last_frame);
		break;
	case (UP) :
		pivot.y--;
		last_frame = up.GetCurrentFrame();
		if (forward_walking)
			ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		else ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case (UP_FORWARD) :
		pivot.y--;
		pivot.x++;
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->Blit(graphics, pivot.x, pivot.y - player_height, &last_frame);
		break;
	case(UP_BACKWARD) :
		pivot.y--;
		pivot.x--;
		last_frame = up.GetCurrentFrame();
		ret = App->renderer->BlitFlipH(graphics, pivot.x, pivot.y - player_height, &last_frame);
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
	return false;
}