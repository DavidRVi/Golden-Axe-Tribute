#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneIntro.h"
#include "ModuleSceneLevel1.h"
#include "ModulePlayer.h"
/*
#include "ModuleScene.h"
#include "ModulePlayer.h"
#include "ModuleParticles.h"
*/

#include "ModuleCollisions.h"


using namespace std;

Application::Application()
{
	// Order matters: they will init/start/pre/update/post in this order
	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(audio = new ModuleAudio());

	modules.push_back(intro = new ModuleSceneIntro(false));
	modules.push_back(level1 = new ModuleSceneLevel1(false));
	modules.push_back(player = new ModulePlayer(false));
	modules.push_back(fade = new ModuleFadeToBlack());

	modules.push_back(collisions = new ModuleCollisions());
	//Game Modules
	/*
	modules.push_back(scene = new ModuleScene());

	modules.push_back(player = new ModulePlayer(false));
	

	// Particle module
	modules.push_back(particles = new ModuleParticles());

	modules.push_back(collisions = new ModuleCollisions());*/


}

Application::~Application()
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		RELEASE(*it);

	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init(); // we init everything, even if not anabled

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsEnabled() == true)
			ret = (*it)->Start();
	}

	// Start the first scene --
	fade->FadeToBlack(level1, nullptr, 3.0f);

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->PreUpdate();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->Update();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->CleanUp();

	return ret;
}

