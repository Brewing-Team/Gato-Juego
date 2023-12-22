#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"
#include <Box2D/Common/b2Settings.h>

class PhysBody;
class b2Fixture;
class b2Vec2;

enum class EntityType
{
	PLAYER,
	ITEM,
	OWLENEMY,
	DOGENEMY,
	ROPE,
	FURBALL,
	UNKNOWN
};

enum class EntityState {
	NONE,
	IDLE,
	MOVE,
	JUMP,
	CLIMB,
	ATTACK,
	WIN,
	DEAD,
	NO_CLIP
};

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node& node)
	{
		return true;
	}

	virtual EntityState StateMachine(float dt) { return EntityState::NONE; };

	virtual void Move(float dt) {};
	virtual void Jump(float dt) {};
	virtual void Climb(float dt) {};
	virtual void Attack(float dt) {};

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

	virtual void EndCollision(PhysBody* physA, PhysBody* physB) {

	};

	virtual void OnRaycastHit(b2Fixture* fixture, const b2Vec2& point,
                       const b2Vec2& normal, float32 fraction) {
						
					   };

public:

	SString name;
	EntityType type;
	EntityState state;
	bool active = true;
	pugi::xml_node parameters; 

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;       
	iPoint newPosition;
	bool renderable = true;
};

#endif // __ENTITY_H__