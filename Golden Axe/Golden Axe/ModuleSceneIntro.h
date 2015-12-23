#ifndef __MODULESCENE_INTRO_H__
#define __MODULESCENE_INTRO_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class ModuleSceneIntro : public Module {
public:
	ModuleSceneIntro(bool enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* graphics = nullptr;
	SDL_Texture* background_tex = nullptr;
	SDL_Rect symbol;
	SDL_Rect title;
	Animation press_start;
	SDL_Rect sega;
	SDL_Rect background;
	
};
#endif