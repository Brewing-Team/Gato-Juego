#ifndef __ROPEENTITY_H__
#define __ROPEENTITY_H__

#include "Entity.h"
#include "Point.h"

#ifdef __linux__
#include <SDL.h>
#include <Box2D/Common/b2Math.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#include "Box2D/Box2D/Common/b2math.h"
#endif

struct SDL_Texture;

class RopeEntity : public Entity
{
public:

	RopeEntity();
	virtual ~RopeEntity();

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

	b2Vec2 spawnPosition;
	b2Vec2 startPos;
	b2Vec2 endPos;

	int ropeLength;
	int segmentWidth;
	int segmentHeight;

	int tmp;
};

#endif // __ROPEENTITY_H__