#ifndef __PLAYERIDLESTATE_H__
#define __PLAYERIDLESTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerIdleState : public State<Player> {
    public:
    PlayerIdleState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERIDLESTATE_H__