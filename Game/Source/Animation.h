#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "App.h"
#include "SString.h"
#ifdef __linux__
#include <SDL_render.h>
#include "SDL2/SDL_rect.h"
#elif _MSC_VER
#include "SDL/include/SDL_rect.h"
#endif

#define MAX_FRAMES 30

class Animation
{
public:
	SString name;
	SDL_Texture* texture;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	bool loop = true;
	// Allows the animation to keep going back and forth
	bool pingpong = false;
	bool flip = false;

private:
	float currentFrame = 0.0f;
	int totalFrames = 0;
	int loopCount = 0;
	int pingpongCount = 0;
	int pingpongDirection = 1;

public:

	void PushBack(const SDL_Rect& rect)
	{
		frames[totalFrames++] = rect;
	}

	void Reset()
	{
		currentFrame = 0;
	}
	
	void ResetLoopCount()
	{
		loopCount = 0;
	}


	bool HasFinished()
	{
		return !loop && !pingpong && loopCount > 0;
	}

	void Update(float dt)
	{
		currentFrame += speed * (dt/1000); //revisar esto no se muy bien como multiplicar por dt aqui por no ser 0.016 sino 16.666
		if (currentFrame >= totalFrames)
		{
			currentFrame = (loop || pingpong) ? 0.0f : totalFrames - 1;
			++loopCount;
			

			if (pingpong) {
				pingpongDirection = -pingpongDirection;
				if (currentFrame == 0) {
					pingpong = false;
				}
			}
				
		}
	}

	int GetLoopCount() const 
	{
		return loopCount;
	}
	
	int GetPingPongCount() const 
	{
		return pingpongCount;
	}
	
	float GetCurrentFrameCount() const
	{
		return currentFrame;
	}

	const SDL_Rect& GetCurrentFrame() const
	{
		int actualFrame = currentFrame;
		if (pingpongDirection == -1)
			actualFrame = totalFrames - currentFrame;

		return frames[actualFrame];
	}
};

#endif