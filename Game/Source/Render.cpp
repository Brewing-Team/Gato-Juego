#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Entity.h"

#include "Defs.h"
#include "Log.h"
#include <iostream>

#ifdef __linux__
#include <SDL_render.h>
#endif

#define VSYNC true

Render::Render() : Module()
{
	name.Create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;

	uint32_t flags = SDL_RENDERER_ACCELERATED;

	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(app->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		// camera.w = app->win->screenSurface->w;
		// camera.h = app->win->screenSurface->h;
		SDL_RendererInfo info;
		SDL_GetRendererOutputSize(renderer, &camera.w, &camera.h);
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);

	return true;
}

bool Render::Update(float dt)
{
	cameraInterpolation(camera.target, camera.lerpSpeed, dt, camera.offset);
	return true;
}

bool Render::PostUpdate()
{
	SetViewPort({
		0,
		0,
		camera.w,
		camera.h
		});
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_RenderPresent(renderer);
	
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{	
	//SDL_RenderSetClipRect
	SDL_RenderSetViewport(renderer, &rect);
	
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

/// @brief Camera interpolation
/// @return TODO remove hardcoded 16 and calculate the right offset
/// @param target Entity to follow
/// @param lerpSpeed the camera’s movement speed. Lower values result in a “lazier” camera.
/// @param offset position of the camera relative to the target. (0,0) is the center.
/// @param dt delta time
void Render::cameraInterpolation(Entity* target, float lerpSpeed, float dt, iPoint offset)
{	
	//DEBUG
	if(freeCam)
	{
		target = nullptr;
		camera.useInterpolation = false;
	}
	else
	{
		camera.useInterpolation = true;
	}

	if(target != nullptr)
	{
		if(camera.useInterpolation)
		{
			camera.x = std::ceil(std::lerp(camera.x, (int)(camera.w / 2 / app->win->GetScale()) - 16 - target->position.x - offset.x, dt * lerpSpeed / 1000));
			camera.y = std::ceil(std::lerp(camera.y, (int)(camera.h / 2 / app->win->GetScale()) - 16 - target->position.y - offset.y, dt * lerpSpeed / 1000));
		}
		else
		{
			camera.x = -target->position.x + camera.w / app->win->GetScale() / 2 - 16 - offset.x;
			camera.y = -target->position.y + camera.h / app->win->GetScale() / 2 - 16 - offset.y;
		}
	}
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, SDL_RendererFlip flip, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = ((int)(camera.x * speed) + x) * scale;
	rect.y = ((int)(camera.y * speed) + y) * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = ((int)(camera.x + rect.x) * scale);
		rec.y = ((int)(camera.y + rect.y) * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, (camera.x + x1) * scale, (camera.y + y1) * scale, (camera.x + x2) * scale, (camera.y + y2) * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = ((int)(x + camera.x) * scale + radius * cos(i * factor) * scale);
		points[i].y = ((int)(y + camera.y) * scale + radius * sin(i * factor) * scale);
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
