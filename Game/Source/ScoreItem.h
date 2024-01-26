#ifndef __SCOREITEM_H__
#define __SCOREITEM_H__

#include "Entity.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

struct SDL_Texture;
class Animation;

class ScoreItem : public Entity
{
public:

	ScoreItem();
	virtual ~ScoreItem();

	bool Awake() override;

	bool Start() override;

	bool Update(float dt) override;

	bool CleanUp() override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

public:

	bool isPicked = false;
	SDL_Texture* texture;
	const char* texturePath;

private:

	PhysBody* pbody;

	Animation* starAnim;
};

#endif // __SCOREITEM_H__