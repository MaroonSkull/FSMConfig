#include <iostream>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

/**
 * @file main.cpp
 * @brief Простой пример использования FSMConfig
 *
 * Этот пример демонстрирует базовое использование библиотеки FSMConfig
 * для создания простого конечного автомата с двумя состояниями: idle и active.
 */

/**
 * @class SimpleFSMExample
 * @brief Пример использования конечного автомата для простой логики переключения состояний
 *
 * Демонстрирует:
 * - Регистрацию коллбэков состояния (on_enter, on_exit)
 * - Регистрацию коллбэков переходов
 * - Регистрацию действий
 * - Триггеринг событий для изменения состояний
 */
class SimpleFSMExample {
public:
    /**
     * @brief Запуск примера
     */
    void run() {
        // Создаём конечный автомат из YAML конфигурации
        StateMachine fsm("config.yaml");
        
        // Регистрируем коллбэки состояния idle
        fsm.registerStateCallback("idle", "on_enter", &SimpleFSMExample::onIdleEnter, this);
        fsm.registerStateCallback("idle", "on_exit", &SimpleFSMExample::onIdleExit, this);
        
        // Регистрируем коллбэки состояния active
        fsm.registerStateCallback("active", "on_enter", &SimpleFSMExample::onActiveEnter, this);
        fsm.registerStateCallback("active", "on_exit", &SimpleFSMExample::onActiveExit, this);
        
        // Регистрируем коллбэки переходов
        fsm.registerTransitionCallback("idle", "active", &SimpleFSMExample::onStartTransition, this);
        fsm.registerTransitionCallback("active", "idle", &SimpleFSMExample::onStopTransition, this);
        
        // Регистрируем действия
        fsm.registerAction("log_idle_state", &SimpleFSMExample::logIdleState, this);
        fsm.registerAction("log_active_state", &SimpleFSMExample::logActiveState, this);
        
        // Запускаем конечный автомат
        std::cout << "Starting FSM...\n";
        fsm.start();
        
        // Триггерим событие "start" для перехода idle -> active
        std::cout << "\nTriggering 'start' event...\n";
        fsm.triggerEvent("start");
        
        // Триггерим событие "stop" для перехода active -> idle
        std::cout << "\nTriggering 'stop' event...\n";
        fsm.triggerEvent("stop");
        
        // Останавливаем конечный автомат
        std::cout << "\nStopping FSM...\n";
        fsm.stop();
    }
    
private:
    /**
     * @brief Коллбэк при входе в состояние idle
     */
    void onIdleEnter() {
        std::cout << "  -> Entering idle state\n";
    }
    
    /**
     * @brief Коллбэк при выходе из состояния idle
     */
    void onIdleExit() {
        std::cout << "  <- Exiting idle state\n";
    }
    
    /**
     * @brief Коллбэк при входе в состояние active
     */
    void onActiveEnter() {
        std::cout << "  -> Entering active state\n";
    }
    
    /**
     * @brief Коллбэк при выходе из состояния active
     */
    void onActiveExit() {
        std::cout << "  <- Exiting active state\n";
    }
    
    /**
     * @brief Коллбэк перехода idle -> active
     * @param event Событие перехода
     */
    void onStartTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    /**
     * @brief Коллбэк перехода active -> idle
     * @param event Событие перехода
     */
    void onStopTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    /**
     * @brief Действие для логирования состояния idle
     */
    void logIdleState() {
        std::cout << "  [Action] Now in idle state\n";
    }
    
    /**
     * @brief Действие для логирования состояния active
     */
    void logActiveState() {
        std::cout << "  [Action] Now in active state\n";
    }
};

/**
 * @brief Точка входа в приложение
 */
int main() {
    SimpleFSMExample example;
    example.run();
    return 0;
}
