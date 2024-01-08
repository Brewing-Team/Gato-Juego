#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include "DynArray.h"
#include "Log.h"
#include "SString.h"
#include <vector>

template <typename T>
class State;

template <typename T>
class StateMachine {
public:
    StateMachine(T* owner) : owner(owner){
    LOG("StateMachine::StateMachine(owner)\n");
    }

    ~StateMachine(){
        LOG("StateMachine::~StateMachine()\n");
    }

    void PreUpdate(){
        LOG("StateMachine::PreUpdate()\n");
        currentState->PreUpdate();
    }

    void Update(float dt){
        LOG("StateMachine::Update()\n");
        currentState->Update(dt);
    }

    void PostUpdate(){
        LOG("StateMachine::PostUpdate()\n");
        currentState->PostUpdate();
    }

    void AddState(State<T>* newState){
        newState->StateMachineReference = this;
        
        if (currentState == nullptr){
            currentState = newState;
        }
        states.PushBack(newState);
    }

State<T>* FindState(SString stateName){
    for (int i = 0; i < states.Count(); i++){
        if (states[i]->name == stateName){
            return states[i];
        }
    }
    return nullptr;
}

void ChangeState(SString stateName){
    currentState->Exit();
    State<T>* newState = FindState(stateName);
    if (newState != nullptr){
        currentState = newState;
    }
    else{
        LOG("State %s not found\n", stateName.GetString());
    }
    currentState->Enter();
}
public:
    T* owner;
private:
    State<T>* currentState;
    DynArray<State<T>*> states; //ver si tienen que ser punteros o no
};
#endif // __STATEMACHINE_H__