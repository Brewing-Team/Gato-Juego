#include "PlayerIdleState.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerIdleState::Enter()
{
    LOG("PlayerIdleState::Enter()\n");
    player = StateMachineReference->owner;
    player->setIdleAnimation();
}

void PlayerIdleState::Update(float dt)
{
    LOG("PlayerIdleState::Update()\n");
    LOG("player is grounded: %d\n", player->isGrounded);

    if (!player->isAlive and !player->godMode) {
        //hacer la transicion al state muerto con el nuevo sistema
        //player->state = EntityState::DEAD;
    }

    if (app->scene->winCondition) {
        //hacer la transicion al state win con el nuevo sistema
        //player->state = EntityState::WIN;
    }

    if (player->isGrounded) {
        player->setIdleAnimation();
        if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
            //hacer la transicion al state jump/move con el nuevo sistema
            player->Jump(dt);
            player->jumpAnim.Reset();
        }
    }
    else
    {
        player->setJumpAnimation();
    }

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT or app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        //hacer la transicion al state move con el nuevo sistema
        //player->state = EntityState::MOVE;
        StateMachineReference->ChangeState("move");
    }
}

void PlayerIdleState::Exit()
{
    LOG("PlayerIdleState::Exit()\n");
}

