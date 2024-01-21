#include "Checkpoint.h"
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

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint() {}

bool Checkpoint::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Checkpoint::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x + size.x / 2, position.y + size.y / 2, size.x, size.y, bodyType::STATIC);
	pbody->ctype = ColliderType::CHECKPOINT;
	pbody->body->GetFixtureList()->SetSensor(true);
	pbody->listener = this;

	return true;
}

bool Checkpoint::Update(float dt)
{
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Checkpoint::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB){
	if (physB->ctype == ColliderType::PLAYER){
		app->scene->player->spawnPosition = position;
		app->physics->DestroyBody(pbody);
	}
}