#ifndef __PLAYERWINSTATE_H__
#define __PLAYERWINSTATE_H__

#include "Player.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class PlayerWinState : public State<Player> {
    public:
    PlayerWinState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    Player* player;
};
#endif // __PLAYERWINSTATE_H__