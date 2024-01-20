#ifndef __OWLENEMYMOVESTATE_H__
#define __OWLENEMYMOVESTATE_H__

#include "OwlEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class OwlEnemyMoveState : public State<OwlEnemy> {
    public:
    OwlEnemyMoveState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    OwlEnemy* owlEnemy;
};
#endif // __OWLENEMYMOVESTATE_H__
