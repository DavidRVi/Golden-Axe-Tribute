#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "SDL/include/SDL_rect.h"
//#include "Point.h"
//#include "Animation.h" 

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);


void log(const char file[], int line, const char* format, ...);
struct SDL_Texture;


/*
struct _ParticleData {
	fPoint position;
	fPoint speed;
	Animation anim;
	SDL_Texture* tex;
};*/

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes a buffer
#define RELEASE( x ) \
{\
if (x != nullptr)\
{\
	delete x; \
	x = nullptr; \
	}\
}

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
{\
if (x != nullptr)\
{\
	delete[] x; \
	x = nullptr; \
	}\
	}

// Configuration -----------
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_SIZE 3
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Golden Axe Tribute"


#endif //__GLOBALS_H__