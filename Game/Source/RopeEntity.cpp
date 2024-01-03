#include "RopeEntity.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#ifdef __linux__
#include <Box2D/Dynamics/b2Body.h>
#endif

RopeEntity::RopeEntity() : Entity(EntityType::ROPE)
{
	name.Create("Rope");
}

RopeEntity::~RopeEntity() {}

bool RopeEntity::Awake() {

	spawnPosition.x = parameters.attribute("x").as_float();
	spawnPosition.y = parameters.attribute("y").as_float();
	startPos.x = parameters.attribute("startPosX").as_float();
	startPos.y = parameters.attribute("startPosY").as_float();
	endPos.x = parameters.attribute("endPosX").as_float();
	endPos.y = parameters.attribute("endPosY").as_float();
	ropeLength = parameters.attribute("length").as_int();
	
	segmentWidth = parameters.attribute("segmentWidth").as_int();
	segmentHeight = parameters.attribute("segmentHeight").as_int();

	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool RopeEntity::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	//Esto deberia de comprobarse diferente ya que de esta manera no se puede spawnear nada en 0,0. No es muy importante porque en 0,0 nunca spawneamos nada.
	if (spawnPosition.x == 0 or spawnPosition.y == 0) {
		if (startPos.x != 0 or startPos.y != 0) {
			if(endPos.x != 0 or endPos.y != 0){
				//Create Rope with two arnchors
				pbody = app->physics->CreateRope(startPos, endPos, ropeLength);
			}
			else{
				//Create Rope with one anchor
				pbody = app->physics->CreateRope(startPos, ropeLength);
			}
		}
	}
	else {
		//Create Rope with no anchor
		pbody = app->physics->CreateRope(spawnPosition.x ,spawnPosition.y, ropeLength);
	}

	for (int i = 0; i < ropeLength; i++)
	{
		pbody[i].ctype = ColliderType::PLATFORM;
	}

	return true;
}

bool RopeEntity::Update(float dt)
{
	for (int i = 0; i < ropeLength; i++)
	{
		app->render->DrawTexture(texture, METERS_TO_PIXELS(pbody[i].body->GetPosition().x), METERS_TO_PIXELS(pbody[i].body->GetPosition().y - pbody->height / 2), NULL, 1.0f, pbody[i].body->GetAngle() * RADTODEG);
	}

	//debug draw of ropes
/* 	for (int i = 0; i < ropeLength - 1; i++)
	{
		LOG("pbody[%d] x: %f y: %f", i, METERS_TO_PIXELS(pbody[i].body->GetPosition().x), pbody[i].body->GetPosition().y);
		app->render->DrawLine(METERS_TO_PIXELS(pbody[i].body->GetPosition().x - pbody->width / 2), METERS_TO_PIXELS(pbody[i].body->GetPosition().y), METERS_TO_PIXELS(pbody[i + 1].body->GetPosition().x - pbody->width / 2), METERS_TO_PIXELS(pbody[i + 1].body->GetPosition().y), (i * 14) % 256, (i * 28) % 256, (i * 56) % 256);
	} */

	return true;
}

bool RopeEntity::CleanUp()
{

	app->tex->UnLoad(texture);
	return true;
}