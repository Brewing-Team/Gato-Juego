#include "DogEnemyMoveState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void DogEnemyMoveState::Enter()
{
    LOG("DogEnemyMoveState::Enter()\n");
    dogEnemy = StateMachineReference->owner;
	dogEnemy->setMoveAnimation();
}

void DogEnemyMoveState::Update(float dt)
{
	dogEnemy->pathfindingMovement(dt);
	if (PIXEL_TO_METERS(app->scene->player->position.DistanceTo(dogEnemy->position)) < 1.0f) {
		if (dogEnemy->attackTimer.ReadSec() >= 2)
		{
			StateMachineReference->ChangeState("attack");
		}
	}
	else if ((PIXEL_TO_METERS(app->scene->player->position.DistanceTo(dogEnemy->position)) > 5.0f)) {
		dogEnemy->moveToSpawnPoint();
		StateMachineReference->ChangeState("idle");
	}
}

void DogEnemyMoveState::Exit()
{
    LOG("DogEnemyMoveState::Exit()\n");
}

