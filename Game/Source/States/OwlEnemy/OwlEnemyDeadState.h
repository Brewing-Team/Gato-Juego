#ifndef __OWLENEMYDEADSTATE_H__
#define __OWLENEMYDEADSTATE_H__

#include "OwlEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class OwlEnemyDeadState : public State<OwlEnemy> {
    public:
    OwlEnemyDeadState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    OwlEnemy* owlEnemy;
};
#endif // __OWLENEMYDEADSTATE_H__
