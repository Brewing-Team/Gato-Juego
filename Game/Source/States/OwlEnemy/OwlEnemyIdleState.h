#ifndef __OWLENEMYIDLESTATE_H__
#define __OWLENEMYIDLESTATE_H__

#include "OwlEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class OwlEnemyIdleState : public State<OwlEnemy> {
    public:
    OwlEnemyIdleState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    OwlEnemy* owlEnemy;
};
#endif // __OWLENEMYIDLESTATE_H__
