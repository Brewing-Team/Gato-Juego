#ifndef __PLAYERDEADSTATE_H__
#define __PLAYERDEADSTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerDeadState : public State<Player> {
    public:
    PlayerDeadState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERDEADSTATE_H__