#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include<list>
#include "Globals.h"
#include "Module.h"

	class ModuleRender;
	class ModuleWindow;
	class ModuleTextures;
	class ModuleInput;
	class ModuleAudio;
	class ModuleFadeToBlack;

	class ModuleSceneIntro;
	class ModuleSceneLevel1;
	class ModulePlayer;
	/*
	class ModulePlayer;
	class ModuleScene;
	class ModuleParticles;
	*/
	class ModuleCollisions;
	class ModuleCameraController;

	class Application
	{
	public:

		Application();
		~Application();

		bool Init();
		update_status Update();
		bool CleanUp();

	public:
		ModuleRender* renderer;
		ModuleWindow* window;
		ModuleTextures* textures;
		ModuleInput* input;
		ModuleAudio* audio;
		ModuleFadeToBlack* fade;


		ModuleSceneIntro* intro;
		ModuleSceneLevel1* level1;
		ModulePlayer*		player;

		ModuleCollisions*	collisions;

		ModuleCameraController* camController;
		/*
		ModulePlayer* player;
		ModuleScene* scene;
		ModuleParticles* particles;
		ModuleCollisions* collisions; */

	private:

		std::list<Module*> modules;

	};

	extern Application* App;

#endif // __APPLICATION_CPP__