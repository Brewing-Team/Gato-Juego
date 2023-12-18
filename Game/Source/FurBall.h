#ifndef __FURBALL_H__
#define __FURBALL_H__

#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class FurBall : public Entity
{
public:

	FurBall();
	virtual ~FurBall();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;
	PhysBody* pbody;

private:

	SDL_Texture* texture;
	const char* texturePath;
	
};

#endif // __FURBALL_H__