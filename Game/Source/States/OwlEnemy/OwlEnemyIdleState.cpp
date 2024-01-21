#include "OwlEnemyIdleState.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Player.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"


void OwlEnemyIdleState::Enter()
{
    LOG("OwlEnemyIdleState::Enter()\n");
    owlEnemy = StateMachineReference->owner;
    owlEnemy->currentAnimation = &owlEnemy->idleAnim;
}

void OwlEnemyIdleState::Update(float dt)
{
		if (PIXEL_TO_METERS(app->scene->player->position.DistanceTo(owlEnemy->position)) < 3.0f)
		{
			// AUDIO DONE owl idle
 			app->audio->PlayFx(owlEnemy->owlIdle);
            StateMachineReference->ChangeState("move");
		}
}

void OwlEnemyIdleState::Exit()
{
    LOG("OwlEnemyIdleState::Exit()\n");
}

