#include "StateMachine.h"
#include "Defs.h"
#include "Log.h"
#include "SString.h"
#include <iostream>

StateMachine::StateMachine(){
    LOG("StateMachine::StateMachine()\n");
}

StateMachine::~StateMachine(){
    LOG("StateMachine::~StateMachine()\n");
}

void StateMachine::PreUpdate(){
    LOG("StateMachine::PreUpdate()\n");
    currentState->PreUpdate();
}

void StateMachine::Update(){
    LOG("StateMachine::Update()\n");
    currentState->Update();
}

void StateMachine::PostUpdate(){
    LOG("StateMachine::PostUpdate()\n");
    currentState->PostUpdate();
}

void StateMachine::AddState(State* newState){
    newState->StateMachineReference = this;
    
    if (currentState == nullptr){
        currentState = newState;
    }
    states.PushBack(newState);
}

void StateMachine::ChangeState(SString stateName){
    currentState->Exit();
    State* newState = FindState(stateName);
    if (newState != nullptr){
        currentState = newState;
    }
    else{
        LOG("State %s not found\n", stateName.GetString());
    }
    currentState->Enter();
}

State* StateMachine::FindState(SString stateName){
    for (int i = 0; i < states.Count(); i++){
        if (states[i]->name == stateName){
            return states[i];
        }
    }
    return nullptr;
}