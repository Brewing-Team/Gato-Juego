#include "OwlEnemyMoveState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void OwlEnemyMoveState::Enter()
{
    LOG("OwlEnemyMoveState::Enter()\n");
    owlEnemy = StateMachineReference->owner;
    owlEnemy->currentAnimation = &owlEnemy->flyAnim;
}

void OwlEnemyMoveState::Update(float dt)
{
		owlEnemy->pathfindingMovement(dt);
		if (PIXEL_TO_METERS(app->scene->player->position.DistanceTo(owlEnemy->position)) < 1.0f) {
			if (owlEnemy->attackTimer.ReadSec() >= 2)
			{
				StateMachineReference->ChangeState("attack");
			}
		}
		else if ((PIXEL_TO_METERS(app->scene->player->position.DistanceTo(owlEnemy->position)) > 5.0f)) {
			owlEnemy->moveToSpawnPoint();
			StateMachineReference->ChangeState("idle");
		}
}

void OwlEnemyMoveState::Exit()
{
    LOG("OwlEnemyMoveState::Exit()\n");
}

