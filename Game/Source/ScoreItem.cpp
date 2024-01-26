#include "ScoreItem.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "ScoreItem.h"
#include "Map.h"
#include "Physics.h"

#ifdef __linux__
#include <Box2D/Dynamics/b2Body.h>
#endif

ScoreItem::ScoreItem() : Entity(EntityType::SCOREITEM)
{
	name.Create("scoreitem");
}

ScoreItem::~ScoreItem() {}

bool ScoreItem::Awake() {

	//position.x = parameters.attribute("x").as_int();
	//position.y = parameters.attribute("y").as_int();
	//texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool ScoreItem::Start() {

	//initilize textures

	starAnim = app->map->GetAnimByName("star");
	starAnim->speed = 8.0f;
	starAnim->loop = true;
	starAnim->pingpong = true;

	texture = starAnim->texture;
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::SCOREITEM;
	pbody->listener = this;

	return true;
}

bool ScoreItem::Update(float dt)
{
	// L07 DONE 4: Add a physics to an scoreitem - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y, &starAnim->GetCurrentFrame());

	starAnim->Update(dt);

	return true;
}

bool ScoreItem::CleanUp()
{
	app->tex->UnLoad(texture);
	app->physics->DestroyBody(pbody);
	return true;
}

void ScoreItem::OnCollision(PhysBody* physA, PhysBody* physB){
	if (physB->ctype == ColliderType::PLAYER){
		app->scene->player->score += 100;
		app->entityManager->DestroyEntity(this);
	}
}