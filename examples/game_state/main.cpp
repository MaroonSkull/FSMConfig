#include <fsmconfig/state_machine.hpp>
#include <iostream>
#include <memory>

using namespace fsmconfig;

/**
 * @file main.cpp
 * @brief Example of using FSMConfig for game state management
 *
 * This example demonstrates the use of the FSMConfig library
 * for managing game states (Menu, Playing, Paused, GameOver).
 * Shows the use of guard conditions and state variables.
 */

/**
 * @class GameStateExample
 * @brief Example of using a finite state machine for game logic
 *
 * Demonstrates:
 * - Registration of state callbacks (on_enter, on_exit)
 * - Registration of transition callbacks
 * - Registration of guard conditions for transition protection
 * - Registration of actions
 * - Working with variables (global and state)
 * - Triggering events to change states
 */
class GameStateExample {
 public:
  /**
   * @brief Run the example
   */
  void run() {
    // Create a finite state machine from YAML configuration
    fsm_ptr = std::make_unique<StateMachine>("config.yaml");
    StateMachine& fsm = *fsm_ptr;

    // Register menu state callbacks
    fsm.registerStateCallback("menu", "on_enter", &GameStateExample::onMenuEnter, this);
    fsm.registerStateCallback("menu", "on_exit", &GameStateExample::onMenuExit, this);

    // Register playing state callbacks
    fsm.registerStateCallback("playing", "on_enter", &GameStateExample::onPlayingEnter, this);
    fsm.registerStateCallback("playing", "on_exit", &GameStateExample::onPlayingExit, this);

    // Register paused state callbacks
    fsm.registerStateCallback("paused", "on_enter", &GameStateExample::onPausedEnter, this);
    fsm.registerStateCallback("paused", "on_exit", &GameStateExample::onPausedExit, this);

    // Register game_over state callbacks
    fsm.registerStateCallback("game_over", "on_enter", &GameStateExample::onGameOverEnter, this);
    fsm.registerStateCallback("game_over", "on_exit", &GameStateExample::onGameOverExit, this);

    // Register transition callbacks
    fsm.registerTransitionCallback("menu", "playing", &GameStateExample::onStartGameTransition, this);
    fsm.registerTransitionCallback("playing", "paused", &GameStateExample::onPauseGameTransition, this);
    fsm.registerTransitionCallback("paused", "playing", &GameStateExample::onResumeGameTransition, this);
    fsm.registerTransitionCallback("paused", "menu", &GameStateExample::onQuitToMenuTransition, this);
    fsm.registerTransitionCallback("playing", "game_over", &GameStateExample::onPlayerDiedTransition, this);
    fsm.registerTransitionCallback("game_over", "menu", &GameStateExample::onRestartTransition, this);

    // Register guard condition for playing -> game_over transition
    fsm.registerGuard("playing", "game_over", "player_died", &GameStateExample::checkPlayerHealth, this);

    // Register actions
    fsm.registerAction("show_menu", &GameStateExample::showMenu, this);
    fsm.registerAction("load_save_data", &GameStateExample::loadSaveData, this);
    fsm.registerAction("start_game_loop", &GameStateExample::startGameLoop, this);
    fsm.registerAction("spawn_player", &GameStateExample::spawnPlayer, this);
    fsm.registerAction("show_pause_menu", &GameStateExample::showPauseMenu, this);
    fsm.registerAction("show_game_over_screen", &GameStateExample::showGameOverScreen, this);
    fsm.registerAction("save_high_score", &GameStateExample::saveHighScore, this);

    // Set global variables
    fsm.setVariable("player_health", VariableValue(100));
    fsm.setVariable("player_level", VariableValue(1));

    // Start the finite state machine
    std::cout << "=== Game State Machine Example ===\n";
    fsm.start();

    // Simulate game process

    // Start game
    std::cout << "\n[Event] Starting game...\n";
    fsm.triggerEvent("start_game");

    // Pause game
    std::cout << "\n[Event] Pausing game...\n";
    fsm.triggerEvent("pause_game");

    // Resume game
    std::cout << "\n[Event] Resuming game...\n";
    fsm.triggerEvent("resume_game");

    // Player dies (health <= 0)
    std::cout << "\n[Event] Player died...\n";
    fsm.setVariable("player_health", VariableValue(0));
    fsm.triggerEvent("player_died");

    // Restart game
    std::cout << "\n[Event] Restarting...\n";
    fsm.triggerEvent("restart");

    // Quit to menu
    std::cout << "\n[Event] Quitting to menu...\n";
    fsm.triggerEvent("quit_to_menu");

    // Stop the finite state machine
    fsm.stop();
  }

 private:
  // Menu state callbacks
  void onMenuEnter() { std::cout << "  -> Entered menu state\n"; }

  void onMenuExit() { std::cout << "  <- Exited menu state\n"; }

  // Playing state callbacks
  void onPlayingEnter() { std::cout << "  -> Entered playing state\n"; }

  void onPlayingExit() { std::cout << "  <- Exited playing state\n"; }

  // Paused state callbacks
  void onPausedEnter() { std::cout << "  -> Entered paused state\n"; }

  void onPausedExit() { std::cout << "  <- Exited paused state\n"; }

  // Game_over state callbacks
  void onGameOverEnter() { std::cout << "  -> Entered game_over state\n"; }

  void onGameOverExit() { std::cout << "  <- Exited game_over state\n"; }

  // Transition callbacks
  void onStartGameTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onPauseGameTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onResumeGameTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onQuitToMenuTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onPlayerDiedTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onRestartTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  // Guard condition
  /**
   * @brief Check player health before transitioning to game_over
   * @return true if player health <= 0
   */
  bool checkPlayerHealth() {
    VariableValue health = fsm_ptr->getVariable("player_health");
    return health.asInt() <= 0;
  }

  // Actions
  void showMenu() { std::cout << "  [Action] Showing main menu\n"; }

  void loadSaveData() { std::cout << "  [Action] Loading save data\n"; }

  void startGameLoop() { std::cout << "  [Action] Starting game loop\n"; }

  void spawnPlayer() { std::cout << "  [Action] Spawning player\n"; }

  void showPauseMenu() { std::cout << "  [Action] Showing pause menu\n"; }

  void showGameOverScreen() { std::cout << "  [Action] Showing game over screen\n"; }

  void saveHighScore() { std::cout << "  [Action] Saving high score\n"; }

  std::unique_ptr<StateMachine> fsm_ptr;
};

/**
 * @brief Application entry point
 */
int main() {
  GameStateExample example;
  example.run();
  return 0;
}
