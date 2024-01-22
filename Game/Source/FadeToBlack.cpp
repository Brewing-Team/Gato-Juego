#include "FadeToBlack.h"

#include "App.h"
#include "Window.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"

#ifdef __linux__
#include <SDL2/SDL_render.h>
#elif _MSC_VER
#include "SDL/include/SDL_render.h"
#endif

FadeToBlack::FadeToBlack(bool startEnabled) : Module(startEnabled)
{
    name.Create("fade");
    
}

FadeToBlack::~FadeToBlack()
{

}

bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");

    uint width, height;
    app->win->GetWindowSize(width, height);
	screenRect = {0, 0, static_cast<int>(width * app->win->GetScale()), static_cast<int>(height * app->win->GetScale())};

	currentStep = Fade_Step::NONE;

	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update(float dt)
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			// TODO 1: Enable / Disable the modules received when FadeToBlacks(...) gets called
			moduleToDisable->Disable();
			moduleToEnable->Enable();
			
			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool FadeToBlack::Fade(Module* moduleToDisable, Module* moduleToEnable, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if(currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		// TODO 1: How do we keep track of the modules received in this function?
		this->moduleToDisable = moduleToDisable;
		this->moduleToEnable = moduleToEnable;
		
		ret = true;
	}

	return ret;
}