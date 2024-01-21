#ifndef __SCOREITEM_H__
#define __SCOREITEM_H__

#include "Entity.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;

class ScoreItem : public Entity
{
public:

	ScoreItem();
	virtual ~ScoreItem();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
};

#endif // __SCOREITEM_H__