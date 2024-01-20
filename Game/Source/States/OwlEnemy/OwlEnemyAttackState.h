#ifndef __OWLENEMYATTACKSTATE_H__
#define __OWLENEMYATTACKSTATE_H__

#include "OwlEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class OwlEnemyAttackState : public State<OwlEnemy> {
    public:
    OwlEnemyAttackState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    OwlEnemy* owlEnemy;
};
#endif // __OWLENEMYATTACKSTATE_H__
