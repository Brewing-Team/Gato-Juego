#ifndef __FOOD_H__
#define __FOOD_H__

#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class FoodItem : public Entity
{
public:

	FoodItem();
	virtual ~FoodItem();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;
	SDL_Texture* texture;
	const char* texturePath;

private:

	PhysBody* pbody;
};

#endif // __FOOD_H__