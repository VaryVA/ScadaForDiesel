#include "state_machine.h"
#include <iostream>

StateMachine::StateMachine(ModelConfig& config){
    m_config = config;
    m_stageTimer = new QTimer(this);
}

StateMachine::~StateMachine(){
    try{
        delete m_stageTimer;
    }
    catch (const std::runtime_error& e) {
        // Перехватываем исключение
        std::cerr << "Ошибка: " << e.what() << std::endl;
    } 
}