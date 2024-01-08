#include "IdleState.h"
#include "Log.h"
#include "StateMachine.h"
#include "SString.h"
#include "State.h"

IdleState::IdleState(SString name) : State(name)
{
    LOG("IdleState::IdleState(name)\n");
}

void IdleState::Enter()
{
    LOG("IdleState::Enter()\n");
}

void IdleState::PreUpdate()
{
    LOG("IdleState::PreUpdate()\n");
}

void IdleState::Update(float dt)
{
    LOG("IdleState::Update()\n");
}

void IdleState::PostUpdate()
{
    LOG("IdleState::PostUpdate()\n");
}

void IdleState::Exit()
{
    LOG("IdleState::Exit()\n");
}

