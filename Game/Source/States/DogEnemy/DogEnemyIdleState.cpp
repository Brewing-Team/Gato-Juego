#include "DogEnemyIdleState.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Player.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"


void DogEnemyIdleState::Enter()
{
    LOG("DogEnemyIdleState::Enter()\n");
    dogEnemy = StateMachineReference->owner;
    dogEnemy->setIdleAnimation();
}

void DogEnemyIdleState::Update(float dt)
{
    if (PIXEL_TO_METERS(app->scene->player->position.DistanceTo(dogEnemy->position)) < 3.0f)
    {
        StateMachineReference->ChangeState("move");
        // AUDIO DONE dog idle
        app->audio->PlayFx(dogEnemy->dogBark);
    }
}

void DogEnemyIdleState::Exit()
{
    LOG("DogEnemyIdleState::Exit()\n");
}

