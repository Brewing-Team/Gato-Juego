#include "DogEnemyAttackState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void DogEnemyAttackState::Enter()
{
    LOG("DogEnemyAttackState::Enter()\n");
    dogEnemy = StateMachineReference->owner;
}

void DogEnemyAttackState::Update(float dt)
{
// AUDIO DONE dog attack
    app->audio->PlayFx(dogEnemy->dogAttack);

    b2Vec2 attackDirection = { (float32)app->scene->player->position.x - dogEnemy->position.x, (float32)app->scene->player->position.y - dogEnemy->position.y };
    attackDirection.Normalize();

    b2Vec2 attackImpulse = { attackDirection.x / 4, attackDirection.y / 4 };

    dogEnemy->pbody->body->ApplyLinearImpulse(attackImpulse, dogEnemy->pbody->body->GetWorldCenter(), true);

    dogEnemy->attackTimer.Start();

    StateMachineReference->ChangeState("move");
}

void DogEnemyAttackState::Exit()
{
    LOG("DogEnemyAttackState::Exit()\n");
}

