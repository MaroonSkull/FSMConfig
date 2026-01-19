#include <iostream>
#include <memory>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

/**
 * @file main.cpp
 * @brief Пример использования FSMConfig для управления состоянием игры
 *
 * Этот пример демонстрирует использование библиотеки FSMConfig
 * для управления состояниями игры (Menu, Playing, Paused, GameOver).
 * Показывает использование guard-условий и переменных состояния.
 */

/**
 * @class GameStateExample
 * @brief Пример использования конечного автомата для игровой логики
 *
 * Демонстрирует:
 * - Регистрацию коллбэков состояния (on_enter, on_exit)
 * - Регистрацию коллбэков переходов
 * - Регистрацию guard-условий для защиты переходов
 * - Регистрацию действий
 * - Работа с переменными (глобальными и состояния)
 * - Триггеринг событий для изменения состояний
 */
class GameStateExample {
public:
    /**
     * @brief Запуск примера
     */
    void run() {
        // Создаём конечный автомат из YAML конфигурации
        fsm_ptr = std::make_unique<StateMachine>("config.yaml");
        StateMachine& fsm = *fsm_ptr;
        
        // Регистрируем коллбэки состояния menu
        fsm.registerStateCallback("menu", "on_enter", &GameStateExample::onMenuEnter, this);
        fsm.registerStateCallback("menu", "on_exit", &GameStateExample::onMenuExit, this);
        
        // Регистрируем коллбэки состояния playing
        fsm.registerStateCallback("playing", "on_enter", &GameStateExample::onPlayingEnter, this);
        fsm.registerStateCallback("playing", "on_exit", &GameStateExample::onPlayingExit, this);
        
        // Регистрируем коллбэки состояния paused
        fsm.registerStateCallback("paused", "on_enter", &GameStateExample::onPausedEnter, this);
        fsm.registerStateCallback("paused", "on_exit", &GameStateExample::onPausedExit, this);
        
        // Регистрируем коллбэки состояния game_over
        fsm.registerStateCallback("game_over", "on_enter", &GameStateExample::onGameOverEnter, this);
        fsm.registerStateCallback("game_over", "on_exit", &GameStateExample::onGameOverExit, this);
        
        // Регистрируем коллбэки переходов
        fsm.registerTransitionCallback("menu", "playing", &GameStateExample::onStartGameTransition, this);
        fsm.registerTransitionCallback("playing", "paused", &GameStateExample::onPauseGameTransition, this);
        fsm.registerTransitionCallback("paused", "playing", &GameStateExample::onResumeGameTransition, this);
        fsm.registerTransitionCallback("paused", "menu", &GameStateExample::onQuitToMenuTransition, this);
        fsm.registerTransitionCallback("playing", "game_over", &GameStateExample::onPlayerDiedTransition, this);
        fsm.registerTransitionCallback("game_over", "menu", &GameStateExample::onRestartTransition, this);
        
        // Регистрируем guard-условие для перехода playing -> game_over
        fsm.registerGuard("playing", "game_over", "player_died", &GameStateExample::checkPlayerHealth, this);
        
        // Регистрируем действия
        fsm.registerAction("show_menu", &GameStateExample::showMenu, this);
        fsm.registerAction("load_save_data", &GameStateExample::loadSaveData, this);
        fsm.registerAction("start_game_loop", &GameStateExample::startGameLoop, this);
        fsm.registerAction("spawn_player", &GameStateExample::spawnPlayer, this);
        fsm.registerAction("show_pause_menu", &GameStateExample::showPauseMenu, this);
        fsm.registerAction("show_game_over_screen", &GameStateExample::showGameOverScreen, this);
        fsm.registerAction("save_high_score", &GameStateExample::saveHighScore, this);
        
        // Устанавливаем глобальные переменные
        fsm.setVariable("player_health", VariableValue(100));
        fsm.setVariable("player_level", VariableValue(1));
        
        // Запускаем конечный автомат
        std::cout << "=== Game State Machine Example ===\n";
        fsm.start();
        
        // Симулируем игровой процесс
        
        // Начинаем игру
        std::cout << "\n[Event] Starting game...\n";
        fsm.triggerEvent("start_game");
        
        // Ставим игру на паузу
        std::cout << "\n[Event] Pausing game...\n";
        fsm.triggerEvent("pause_game");
        
        // Возобновляем игру
        std::cout << "\n[Event] Resuming game...\n";
        fsm.triggerEvent("resume_game");
        
        // Игрок умирает (health <= 0)
        std::cout << "\n[Event] Player died...\n";
        fsm.setVariable("player_health", VariableValue(0));
        fsm.triggerEvent("player_died");
        
        // Перезапуск игры
        std::cout << "\n[Event] Restarting...\n";
        fsm.triggerEvent("restart");
        
        // Выход в меню
        std::cout << "\n[Event] Quitting to menu...\n";
        fsm.triggerEvent("quit_to_menu");
        
        // Останавливаем конечный автомат
        fsm.stop();
    }
    
private:
    // Коллбэки состояния menu
    void onMenuEnter() {
        std::cout << "  -> Entered menu state\n";
    }
    
    void onMenuExit() {
        std::cout << "  <- Exited menu state\n";
    }
    
    // Коллбэки состояния playing
    void onPlayingEnter() {
        std::cout << "  -> Entered playing state\n";
    }
    
    void onPlayingExit() {
        std::cout << "  <- Exited playing state\n";
    }
    
    // Коллбэки состояния paused
    void onPausedEnter() {
        std::cout << "  -> Entered paused state\n";
    }
    
    void onPausedExit() {
        std::cout << "  <- Exited paused state\n";
    }
    
    // Коллбэки состояния game_over
    void onGameOverEnter() {
        std::cout << "  -> Entered game_over state\n";
    }
    
    void onGameOverExit() {
        std::cout << "  <- Exited game_over state\n";
    }
    
    // Коллбэки переходов
    void onStartGameTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onPauseGameTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onResumeGameTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onQuitToMenuTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onPlayerDiedTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onRestartTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    // Guard-условие
    /**
     * @brief Проверка здоровья игрока перед переходом в game_over
     * @return true если здоровье игрока <= 0
     */
    bool checkPlayerHealth() {
        VariableValue health = fsm_ptr->getVariable("player_health");
        return health.asInt() <= 0;
    }
    
    // Действия
    void showMenu() {
        std::cout << "  [Action] Showing main menu\n";
    }
    
    void loadSaveData() {
        std::cout << "  [Action] Loading save data\n";
    }
    
    void startGameLoop() {
        std::cout << "  [Action] Starting game loop\n";
    }
    
    void spawnPlayer() {
        std::cout << "  [Action] Spawning player\n";
    }
    
    void showPauseMenu() {
        std::cout << "  [Action] Showing pause menu\n";
    }
    
    void showGameOverScreen() {
        std::cout << "  [Action] Showing game over screen\n";
    }
    
    void saveHighScore() {
        std::cout << "  [Action] Saving high score\n";
    }
    
    std::unique_ptr<StateMachine> fsm_ptr;
};

/**
 * @brief Точка входа в приложение
 */
int main() {
    GameStateExample example;
    example.run();
    return 0;
}
