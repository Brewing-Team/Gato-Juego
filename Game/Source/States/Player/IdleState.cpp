#include "IdleState.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void IdleState::Enter()
{
    LOG("IdleState::Enter()\n");
    player = StateMachineReference->owner;
    player->setIdleAnimation();
}

void IdleState::Update(float dt)
{
    LOG("IdleState::Update()\n");

    if (!player->isAlive and !player->godMode) {
        //hacer la transicion al state muerto con el nuevo sistema
        //player->state = EntityState::DEAD;
    }

    if (app->scene->winCondition) {
        //hacer la transicion al state win con el nuevo sistema
        //player->state = EntityState::WIN;
    }

    if (player->isGrounded) {
        if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
            //hacer la transicion al state jump/move con el nuevo sistema
            //Jump(dt);
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
    }
}

void IdleState::Exit()
{
    LOG("IdleState::Exit()\n");
}

