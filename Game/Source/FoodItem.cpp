#include "FoodItem.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include <cstdlib>

#ifdef __linux__
#include <Box2D/Dynamics/b2Body.h>
#endif

FoodItem::FoodItem() : Entity(EntityType::FOODITEM)
{
	name.Create("food");
}

FoodItem::~FoodItem() {}

bool FoodItem::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool FoodItem::Start() {

	// Initialize textures
	foodTextures = app->map->GetAnimByName("Food");

	// Pick a random texture
	//int randomIndex = rand() % foodTextures->totalFrames;
	//foodTextures->currentFrame = randomIndex;
	foodTextures->loop = true;

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::FOOD;
	pbody->listener = this;

	return true;
}

bool FoodItem::Update(float dt)
{
	// L07 DONE 4: Add a physics to an food - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(foodTextures->texture, position.x, position.y, &foodTextures->GetCurrentFrame());
	foodTextures->Update(dt);

	return true;
}

bool FoodItem::CleanUp()
{
	app->tex->UnLoad(texture);
	app->physics->DestroyBody(pbody);
	return true;
}

void FoodItem::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER){
		if(app->scene->player->lives < 7) app->scene->player->lives++;
		app->entityManager->DestroyEntity(this);
	}
}