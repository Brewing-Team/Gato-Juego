#include "PlayerMoveState.h"
#include "Log.h"
#include "Scene.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

void PlayerMoveState::Enter()
{
    LOG("PlayerMoveState::Enter()\n");
    player = StateMachineReference->owner;
}

void PlayerMoveState::Update(float dt)
{
    LOG("PlayerMoveState::Update()\n");
    //angle = std::lerp(angle, 0, dt * 32 / 1000); // añadir al enter del state move

    if (player->isGrounded)
    {
        player->setMoveAnimation();
    }
    else
    {
        player->setJumpAnimation();
    }

    if (!player->isAlive and !player->godMode) {
        //player->state = EntityState::DEAD;
    }

    if (app->scene->winCondition) {
        //player->state = EntityState::WIN;
    }

    if (player->isGrounded && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

        player->Jump(dt);
        player->Move(dt);

    }
    else {
        player->Move(dt);

        if(player->pbody->body->GetLinearVelocity().Length() < 1.0f)
        {
            StateMachineReference->ChangeState("idle");
        }
    }
    if (player->isCollidingLeft or player->isCollidingRight and !player->isGrounded) {
			StateMachineReference->ChangeState("climb");
		}
}

void PlayerMoveState::Exit()
{
    LOG("PlayerMoveState::Exit()\n");
}

