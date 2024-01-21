#include "OwlEnemyAttackState.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void OwlEnemyAttackState::Enter()
{
    LOG("OwlEnemyAttackState::Enter()\n");
    owlEnemy = StateMachineReference->owner;
}

void OwlEnemyAttackState::Update(float dt)
{
    // AUDIO DONE owl attack
    app->audio->PlayFx(owlEnemy->owlAttack);

    b2Vec2 attackDirection = { (float32)app->scene->player->position.x - owlEnemy->position.x, (float32)app->scene->player->position.y - owlEnemy->position.y };
    attackDirection.Normalize();

    b2Vec2 attackImpulse = { attackDirection.x, attackDirection.y };

    owlEnemy->pbody->body->ApplyLinearImpulse(attackImpulse, owlEnemy->pbody->body->GetWorldCenter(), true);

    owlEnemy->attackTimer.Start();
    StateMachineReference->ChangeState("move");
}

void OwlEnemyAttackState::Exit()
{
    LOG("OwlEnemyAttackState::Exit()\n");
}

