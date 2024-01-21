#ifndef __DOGENEMYIDLESTATE_H__
#define __DOGENEMYIDLESTATE_H__

#include "DogEnemy.h"
#include "State.h"
#include "SString.h"
#include "Defs.h"

class DogEnemyIdleState : public State<DogEnemy> {
    public:
    DogEnemyIdleState(SString name) : State(name) {}
    void Enter() override;
    void Update(float dt) override;
    void Exit() override;

    private:
    DogEnemy* dogEnemy;
};
#endif // __DOGENEMYIDLESTATE_H__
