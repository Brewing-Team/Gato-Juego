#ifndef __STATE_H__
#define __STATE_H__
#include "SString.h"
#include "StateMachine.h"

template <typename T>
class State {
    public:
    State() {}
    State(SString name) : name(name) {}
    virtual void Enter() = 0;
    virtual void PreUpdate() = 0;
    virtual void Update(float dt) = 0;
    virtual void PostUpdate() = 0;
    virtual void Exit() = 0;

    SString name;
    StateMachine<T>* StateMachineReference;
};
#endif // __STATE_H__