#include "OwlEnemy.h"
#include "App.h"
#include "Entity.h"
#include "Map.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "StateMachine.h"

#include "Window.h"
#include <cmath>
#include <iostream>

#include "States/OwlEnemy/OwlEnemyIdleState.h"
#include "States/OwlEnemy/OwlEnemyMoveState.h"
#include "States/OwlEnemy/OwlEnemyHurtState.h"
#include "States/OwlEnemy/OwlEnemyAttackState.h"
#include "States/OwlEnemy/OwlEnemyDeadState.h"

#ifdef __linux__
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#endif

OwlEnemy::OwlEnemy() : Entity(EntityType::OWLENEMY)
{
	name.Create("OwlEnemy");
}

OwlEnemy::~OwlEnemy() {

}

bool OwlEnemy::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	newPosition = spawnPosition = position;

	return true;
}

bool OwlEnemy::Start() {

	timer = Timer();
	timer.Start();

	movementDelay = Timer();
	timer.Start();

	player = app->scene->player;

	//load Animations
	idleAnim = *app->map->GetAnimByName("owl-1-idle");
	idleAnim.speed = 8.0f;
	flyAnim = *app->map->GetAnimByName("owl-1-flying");
	flyAnim.speed = 8.0f;
	hurtedAnim = *app->map->GetAnimByName("owl-1-hurted");
	hurtedAnim.speed = 8.0f;
	hurtedAnim.loop = false;
	sleepingAnim = *app->map->GetAnimByName("owl-1-sleeping");
	sleepingAnim.speed = 8.0f;

	// load audios
	owlAttack = app->audio->LoadFx("Assets/Audio/Fx/OwlAttack.wav");
	owlDeath = app->audio->LoadFx("Assets/Audio/Fx/OwlDeath.wav");
	owlHit = app->audio->LoadFx("Assets/Audio/Fx/OwlHit.wav");
	owlIdle = app->audio->LoadFx("Assets/Audio/Fx/OwlIdle.wav");
	owlDeath = app->audio->LoadFx("Assets/Audio/Fx/OwlDead.ogg");



	currentAnimation = &idleAnim;
	
	pbody = app->physics->CreateCircle(position.x, position.y, 15, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	//si quieres dar vueltos como la helice de un helicoptero Boeing AH-64 Apache pon en false la siguiente funcion
	pbody->body->SetFixedRotation(true);
	pbody->body->SetGravityScale(0);

	movementStateMachine = new StateMachine<OwlEnemy>(this);
	movementStateMachine->AddState(new OwlEnemyIdleState("idle"));
	movementStateMachine->AddState(new OwlEnemyMoveState("move"));
	movementStateMachine->AddState(new OwlEnemyHurtState("hurt"));
	movementStateMachine->AddState(new OwlEnemyAttackState("attack"));
	movementStateMachine->AddState(new OwlEnemyDeadState("dead"));

	return true;
}

bool OwlEnemy::Update(float dt)
{
	movementStateMachine->Update(dt);

	//Debug: Render the line between Owl and Player
	if (debug){
		app->render->DrawLine(position.x + 27, position.y + 17, player->position.x + 20, player->position.y + 10, 0, 0, 255);
	}

	// ------------------------------

	//Update OwlEnemie position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 24;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 17;

	// Render OwlEnemie texture
	app->render->DrawTexture(currentAnimation->texture, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.0f, pbody->body->GetAngle()*RADTODEG, flip);

	currentAnimation->Update(dt);
	return true;
}

void OwlEnemy::setIdleAnimation()
{
	currentAnimation = &idleAnim;
}

void OwlEnemy::setMoveAnimation()
{
	currentAnimation = &flyAnim;
	jumpAnim.Reset();
}

void OwlEnemy::setJumpAnimation()
{
	currentAnimation = &jumpAnim;
}

void OwlEnemy::Idle(float dt) {

}

void OwlEnemy::Move(float dt) {
	// TODO move logic
}

void OwlEnemy::Attack(float dt)
{
}

bool OwlEnemy::SaveState(pugi::xml_node& node) {

	pugi::xml_node owlEnemyAttributes = node.append_child("owlenemy");
	owlEnemyAttributes.append_attribute("x").set_value(this->position.x);
	owlEnemyAttributes.append_attribute("y").set_value(this->position.y);
	owlEnemyAttributes.append_attribute("angle").set_value(this->angle);
	owlEnemyAttributes.append_attribute("state").set_value((int)this->state);
	owlEnemyAttributes.append_attribute("lives").set_value(lives);

	return true;

}

bool OwlEnemy::LoadState(pugi::xml_node& node)
{
	pugi::xml_node OwlEnemyNode = node.child("owlenemy");

	pbody->body->SetTransform({ PIXEL_TO_METERS(OwlEnemyNode.attribute("x").as_int()), PIXEL_TO_METERS(OwlEnemyNode.attribute("y").as_int()) }, OwlEnemyNode.attribute("angle").as_int());
	lives = OwlEnemyNode.attribute("lives").as_int();
	movementStateMachine->ChangeState(OwlEnemyNode.attribute("state").as_string()); 
	// reset enemy physics
	//pbody->body->SetAwake(false);
	//pbody->body->SetAwake(true);

	return true;
}

void OwlEnemy::pathfindingMovement(float dt){
	iPoint origin = app->map->WorldToMap(newPosition.x, newPosition.y); //añadir el tile size / 2 hace que el owl se acerque mas

	if (timer.ReadMSec() > 250) {
		iPoint destination = app->map->WorldToMap(player->position.x, player->position.y);  //añadir el tile size / 2 hace que el owl se acerque mas
		app->map->pathfinding->CreatePath(origin, destination);
		timer.Start();
		currentPathPos = 0;
	}
	
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

	if (movementDelay.ReadMSec() > 100) {
		if (currentPathPos < path->Count())
		{
			newPosition = app->map->MapToWorld(path->At(currentPathPos)->x, path->At(currentPathPos)->y);
			currentPathPos++;
			//LOG("%d", currentPathPos);
			movementDelay.Start();
		}
	}

	pbody->body->SetTransform(
		{
			std::lerp(pbody->body->GetPosition().x, PIXEL_TO_METERS(newPosition.x), dt * moveSpeed / 1000),
			std::lerp(pbody->body->GetPosition().y, PIXEL_TO_METERS(newPosition.y), dt * moveSpeed / 1000)

		},

		angle * DEGTORAD
	);

	if (debug)
	{
		//Render Path
		if (path->Count() > 0)
		{
			for (uint i = 0; i < path->Count() - 1; ++i)
			{
				iPoint pos1 = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				iPoint pos2 = app->map->MapToWorld(path->At(i + 1)->x, path->At(i + 1)->y);
				app->render->DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, 0, 0, 255);
			}

		}
	}
}

void OwlEnemy::moveToSpawnPoint()
{
	position = spawnPosition;

	pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0);

	// reset OwlEnemie physics
	pbody->body->SetAwake(false);
	pbody->body->SetAwake(true);
}

bool OwlEnemy::CleanUp() {

	app->tex->UnLoad(texture);

	return true;
}

void OwlEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::DEATH:
		LOG("Collision DEATH");
		app->entityManager->DestroyEntity(this);
		break;
	case ColliderType::BULLET:
		LOG("Collision DEATH");
		if (state != EntityState::DEAD and !invencible){
			if (lives <= 1)
			{
				// AUDIO DONE owl death
				app->audio->PlayFx(owlDeath);
				movementStateMachine->ChangeState("dead");
				reviveTimer.Start();
			}
			else{
				// AUDIO DONE owl hit
				app->audio->PlayFx(owlHit);
				movementStateMachine->ChangeState("hurt");
				lives--;
			}
		}
		break;
	case ColliderType::LIMITS:
		LOG("Collision LIMITS");
		break;;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

void OwlEnemy::EndCollision(PhysBody* physA, PhysBody* physB) {
}