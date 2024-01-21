#ifndef __DOGENEMYATTACKSTATE_H__
#define __DOGENEMYATTACKSTATE_H__

#include "DogEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class DogEnemyAttackState : public State<DogEnemy> {
    public:
    DogEnemyAttackState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    DogEnemy* dogEnemy;
};
#endif // __DOGENEMYATTACKSTATE_H__
