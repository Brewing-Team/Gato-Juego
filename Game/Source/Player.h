#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#ifdef __linux__
#include <SDL.h>
#elif _MSC_VER
#include "SDL/include/SDL.h"
#endif

class b2FixtureDef;

struct SDL_Texture;

class Player : public Entity
{
public:

	EntityState StateMachine() override;
	void Move() override;
	void Jump() override;
	void Climb() override;

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	PhysBody* groundSensor;
	int pickCoinFxId;

	b2FixtureDef* currentFixture = nullptr;

	b2FixtureDef* straightFixture = nullptr;
	b2FixtureDef* crouchFixture = nullptr;

	//Movement
	bool inAir = false;
	int direction = 0;
	float moveForce = 1.0f;
	bool isGrounded = false;
};

#endif // __PLAYER_H__