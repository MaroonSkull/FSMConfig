#include <iostream>
#include <memory>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

/**
 * @file main.cpp
 * @brief Пример использования FSMConfig для сетевого протокола
 *
 * Этот пример демонстрирует использование библиотеки FSMConfig
 * для моделирования состояний сетевого протокола
 * (Disconnected, Connecting, Connected, Authenticating, Authenticated, Error).
 * Показывает сложную логику с guard-условиями.
 */

/**
 * @class NetworkProtocolExample
 * @brief Пример использования конечного автомата для сетевого протокола
 *
 * Демонстрирует:
 * - Регистрацию коллбэков состояния (on_enter, on_exit)
 * - Регистрацию коллбэков переходов
 * - Регистрацию guard-условий для защиты переходов
 * - Регистрацию действий
 * - Работа с переменными (глобальными и состояния)
 * - Триггеринг событий для изменения состояний
 * - Сложную логику с множеством состояний и переходов
 */
class NetworkProtocolExample {
public:
    /**
     * @brief Запуск примера
     */
    void run() {
        // Создаём конечный автомат из YAML конфигурации
        fsm_ptr = std::make_unique<StateMachine>("config.yaml");
        StateMachine& fsm = *fsm_ptr;
        
        // Регистрируем коллбэки состояния disconnected
        fsm.registerStateCallback("disconnected", "on_enter", &NetworkProtocolExample::onDisconnectedEnter, this);
        fsm.registerStateCallback("disconnected", "on_exit", &NetworkProtocolExample::onDisconnectedExit, this);
        
        // Регистрируем коллбэки состояния connecting
        fsm.registerStateCallback("connecting", "on_enter", &NetworkProtocolExample::onConnectingEnter, this);
        fsm.registerStateCallback("connecting", "on_exit", &NetworkProtocolExample::onConnectingExit, this);
        
        // Регистрируем коллбэки состояния connected
        fsm.registerStateCallback("connected", "on_enter", &NetworkProtocolExample::onConnectedEnter, this);
        fsm.registerStateCallback("connected", "on_exit", &NetworkProtocolExample::onConnectedExit, this);
        
        // Регистрируем коллбэки состояния authenticating
        fsm.registerStateCallback("authenticating", "on_enter", &NetworkProtocolExample::onAuthenticatingEnter, this);
        fsm.registerStateCallback("authenticating", "on_exit", &NetworkProtocolExample::onAuthenticatingExit, this);
        
        // Регистрируем коллбэки состояния authenticated
        fsm.registerStateCallback("authenticated", "on_enter", &NetworkProtocolExample::onAuthenticatedEnter, this);
        fsm.registerStateCallback("authenticated", "on_exit", &NetworkProtocolExample::onAuthenticatedExit, this);
        
        // Регистрируем коллбэки состояния error
        fsm.registerStateCallback("error", "on_enter", &NetworkProtocolExample::onErrorEnter, this);
        fsm.registerStateCallback("error", "on_exit", &NetworkProtocolExample::onErrorExit, this);
        
        // Регистрируем коллбэки переходов
        fsm.registerTransitionCallback("disconnected", "connecting", &NetworkProtocolExample::onConnectTransition, this);
        fsm.registerTransitionCallback("connecting", "connected", &NetworkProtocolExample::onConnectionEstablishedTransition, this);
        fsm.registerTransitionCallback("connecting", "error", &NetworkProtocolExample::onConnectionFailedTransition, this);
        fsm.registerTransitionCallback("connected", "authenticating", &NetworkProtocolExample::onAuthenticateTransition, this);
        fsm.registerTransitionCallback("authenticating", "authenticated", &NetworkProtocolExample::onAuthenticationSuccessTransition, this);
        fsm.registerTransitionCallback("authenticating", "error", &NetworkProtocolExample::onAuthenticationFailedTransition, this);
        fsm.registerTransitionCallback("authenticated", "disconnected", &NetworkProtocolExample::onDisconnectTransition, this);
        fsm.registerTransitionCallback("connected", "disconnected", &NetworkProtocolExample::onConnectionLostTransition, this);
        fsm.registerTransitionCallback("error", "disconnected", &NetworkProtocolExample::onRetryTransition, this);
        fsm.registerTransitionCallback("error", "disconnected", &NetworkProtocolExample::onGiveUpTransition, this);
        
        // Регистрируем guard-условие для перехода error -> disconnected (retry)
        fsm.registerGuard("error", "disconnected", "retry", &NetworkProtocolExample::checkRetryCount, this);
        
        // Регистрируем действия
        fsm.registerAction("close_connection", &NetworkProtocolExample::closeConnection, this);
        fsm.registerAction("log_disconnected", &NetworkProtocolExample::logDisconnected, this);
        fsm.registerAction("initiate_connection", &NetworkProtocolExample::initiateConnection, this);
        fsm.registerAction("start_timeout_timer", &NetworkProtocolExample::startTimeoutTimer, this);
        fsm.registerAction("start_heartbeat", &NetworkProtocolExample::startHeartbeat, this);
        fsm.registerAction("log_connected", &NetworkProtocolExample::logConnected, this);
        fsm.registerAction("send_credentials", &NetworkProtocolExample::sendCredentials, this);
        fsm.registerAction("wait_for_response", &NetworkProtocolExample::waitForResponse, this);
        fsm.registerAction("enable_data_transfer", &NetworkProtocolExample::enableDataTransfer, this);
        fsm.registerAction("log_authenticated", &NetworkProtocolExample::logAuthenticated, this);
        fsm.registerAction("log_error", &NetworkProtocolExample::logError, this);
        fsm.registerAction("cleanup_resources", &NetworkProtocolExample::cleanupResources, this);
        
        // Устанавливаем глобальные переменные
        fsm.setVariable("max_retries", VariableValue(3));
        fsm.setVariable("timeout", VariableValue(5.0f));
        fsm.setVariable("retry_count", VariableValue(0));
        
        // Запускаем конечный автомат
        std::cout << "=== Network Protocol State Machine Example ===\n";
        fsm.start();
        
        // Симулируем сетевой протокол
        
        // Пытаемся подключиться
        std::cout << "\n[Event] Connecting...\n";
        fsm.triggerEvent("connect");
        
        // Соединение установлено
        std::cout << "\n[Event] Connection established\n";
        fsm.triggerEvent("connection_established");
        
        // Начинаем аутентификацию
        std::cout << "\n[Event] Authenticating...\n";
        fsm.triggerEvent("authenticate");
        
        // Аутентификация успешна
        std::cout << "\n[Event] Authentication successful\n";
        fsm.triggerEvent("authentication_success");
        
        // Отключаемся
        std::cout << "\n[Event] Disconnecting...\n";
        fsm.triggerEvent("disconnect");
        
        // Симулируем ошибку соединения
        std::cout << "\n[Event] Connecting again...\n";
        fsm.triggerEvent("connect");
        
        // Соединение не удалось
        std::cout << "\n[Event] Connection failed\n";
        fsm.triggerEvent("connection_failed");
        
        // Пробуем снова (retry_count < max_retries)
        std::cout << "\n[Event] Retrying...\n";
        fsm.triggerEvent("retry");
        
        // Останавливаем конечный автомат
        fsm.stop();
    }
    
private:
    // Коллбэки состояния disconnected
    void onDisconnectedEnter() {
        std::cout << "  -> Entered disconnected state\n";
    }
    
    void onDisconnectedExit() {
        std::cout << "  <- Exited disconnected state\n";
    }
    
    // Коллбэки состояния connecting
    void onConnectingEnter() {
        std::cout << "  -> Entered connecting state\n";
    }
    
    void onConnectingExit() {
        std::cout << "  <- Exited connecting state\n";
    }
    
    // Коллбэки состояния connected
    void onConnectedEnter() {
        std::cout << "  -> Entered connected state\n";
    }
    
    void onConnectedExit() {
        std::cout << "  <- Exited connected state\n";
    }
    
    // Коллбэки состояния authenticating
    void onAuthenticatingEnter() {
        std::cout << "  -> Entered authenticating state\n";
    }
    
    void onAuthenticatingExit() {
        std::cout << "  <- Exited authenticating state\n";
    }
    
    // Коллбэки состояния authenticated
    void onAuthenticatedEnter() {
        std::cout << "  -> Entered authenticated state\n";
    }
    
    void onAuthenticatedExit() {
        std::cout << "  <- Exited authenticated state\n";
    }
    
    // Коллбэки состояния error
    void onErrorEnter() {
        std::cout << "  -> Entered error state\n";
    }
    
    void onErrorExit() {
        std::cout << "  <- Exited error state\n";
    }
    
    // Коллбэки переходов
    void onConnectTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onConnectionEstablishedTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onConnectionFailedTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onAuthenticateTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onAuthenticationSuccessTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onAuthenticationFailedTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onDisconnectTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onConnectionLostTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onRetryTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onGiveUpTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    // Guard-условие
    /**
     * @brief Проверка количества попыток перед повторным подключением
     * @return true если retry_count < max_retries
     */
    bool checkRetryCount() {
        VariableValue retryCount = fsm_ptr->getVariable("retry_count");
        VariableValue maxRetries = fsm_ptr->getVariable("max_retries");
        return retryCount.asInt() < maxRetries.asInt();
    }
    
    // Действия
    void closeConnection() {
        std::cout << "  [Action] Closing connection\n";
    }
    
    void logDisconnected() {
        std::cout << "  [Action] Logged as disconnected\n";
    }
    
    void initiateConnection() {
        std::cout << "  [Action] Initiating connection\n";
    }
    
    void startTimeoutTimer() {
        std::cout << "  [Action] Starting timeout timer\n";
    }
    
    void startHeartbeat() {
        std::cout << "  [Action] Starting heartbeat\n";
    }
    
    void logConnected() {
        std::cout << "  [Action] Logged as connected\n";
    }
    
    void sendCredentials() {
        std::cout << "  [Action] Sending credentials\n";
    }
    
    void waitForResponse() {
        std::cout << "  [Action] Waiting for response\n";
    }
    
    void enableDataTransfer() {
        std::cout << "  [Action] Enabling data transfer\n";
    }
    
    void logAuthenticated() {
        std::cout << "  [Action] Logged as authenticated\n";
    }
    
    void logError() {
        std::cout << "  [Action] Logging error\n";
    }
    
    void cleanupResources() {
        std::cout << "  [Action] Cleaning up resources\n";
    }
    
    std::unique_ptr<StateMachine> fsm_ptr;
};

/**
 * @brief Точка входа в приложение
 */
int main() {
    NetworkProtocolExample example;
    example.run();
    return 0;
}
