# FSMConfig Examples

This document provides examples of how to use the FSMConfig library.

## Table of Contents

- [Simple FSM Example](#simple-fsm-example)
- [Game State Example](#game-state-example)
- [Network Protocol Example](#network-protocol-example)

## Simple FSM Example

### Overview

The simple FSM example demonstrates the basic functionality of the library with a 2-state machine.

### Configuration

```yaml
# Global variables
variables:
  max_attempts: 3

# State definitions
states:
  idle:
    on_enter: on_idle_enter
    on_exit: on_idle_exit
    actions:
      - log_idle_state
  
  active:
    on_enter: on_active_enter
    on_exit: on_active_exit
    actions:
      - log_active_state

# Transition definitions
transitions:
  - from: idle
    to: active
    event: start
    on_transition: on_start_transition
  
  - from: active
    to: idle
    event: stop
    on_transition: on_stop_transition
```

### Usage

```cpp
#include <iostream>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

class SimpleFSMExample {
public:
    void run() {
        // Create state machine from YAML configuration
        StateMachine fsm("config.yaml");
        
        // Register state callbacks
        fsm.registerStateCallback("idle", "on_enter", &SimpleFSMExample::onIdleEnter, this);
        fsm.registerStateCallback("idle", "on_exit", &SimpleFSMExample::onIdleExit, this);
        fsm.registerStateCallback("active", "on_enter", &SimpleFSMExample::onActiveEnter, this);
        fsm.registerStateCallback("active", "on_exit", &SimpleFSMExample::onActiveExit, this);
        
        fsm.registerTransitionCallback("idle", "active", &SimpleFSMExample::onStartTransition, this);
        fsm.registerTransitionCallback("active", "idle", &SimpleFSMExample::onStopTransition, this);
        
        fsm.registerAction("log_idle_state", &SimpleFSMExample::logIdleState, this);
        fsm.registerAction("log_active_state", &SimpleFSMExample::logActiveState, this);
        
        // Start the state machine
        std::cout << "Starting FSM...\n";
        fsm.start();
        
        // Trigger 'start' event
        std::cout << "\nTriggering 'start' event...\n";
        fsm.triggerEvent("start");
        
        // Trigger 'stop' event
        std::cout << "\nTriggering 'stop' event...\n";
        fsm.triggerEvent("stop");
        
        // Stop the state machine
        std::cout << "\nStopping FSM...\n";
        fsm.stop();
    }
    
private:
    void onIdleEnter() {
        std::cout << "  -> Entering idle state\n";
    }
    
    void onIdleExit() {
        std::cout << "  <- Exiting idle state\n";
    }
    
    void onActiveEnter() {
        std::cout << "  -> Entering active state\n";
    }
    
    void onActiveExit() {
        std::cout << "  <- Exiting active state\n";
    }
    
    void onStartTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void onStopTransition(const TransitionEvent& event) {
        std::cout << "  Transition: " << event.from_state 
                  << " -> " << event.to_state << "\n";
    }
    
    void logIdleState() {
        std::cout << "  [Action] Now in idle state\n";
    }
    
    void logActiveState() {
        std::cout << "  [Action] Now in active state\n";
    }
};

int main() {
    SimpleFSMExample example;
    example.run();
    return 0;
}
```

### Expected Output

```
Starting FSM...
  -> Entering idle state
  [Action] Now in idle state

Triggering 'start' event...
  Transition: idle -> active
  <- Exiting idle state
  -> Entering active state
  [Action] Now in active state

Triggering 'stop' event...
  Transition: active -> idle
  <- Exiting active state
  -> Entering idle state
  [Action] Now in idle state

Stopping FSM...
  <- Exiting idle state
```

## Game State Example

### Overview

The game state example demonstrates guard conditions and state-local variables.

### Configuration

```yaml
# Global variables
variables:
  player_health: 100
  player_level: 1

# State definitions
states:
  menu:
    on_enter: on_menu_enter
    on_exit: on_menu_exit
    actions:
      - show_menu
      - load_save_data
  
  playing:
    on_enter: on_playing_enter
    on_exit: on_playing_exit
    variables:
      score: 0
      time_elapsed: 0.0
    actions:
      - start_game_loop
      - spawn_player
  
  paused:
    on_enter: on_paused_enter
    on_exit: on_paused_exit
    actions:
      - show_pause_menu
  
  game_over:
    on_enter: on_game_over_enter
    on_exit: on_game_over_exit
    actions:
      - show_game_over_screen
      - save_high_score

# Transition definitions
transitions:
  - from: menu
    to: playing
    event: start_game
    on_transition: on_start_game_transition
  
  - from: playing
    to: paused
    event: pause_game
    on_transition: on_pause_game_transition
  
  - from: paused
    to: playing
    event: resume_game
    on_transition: on_resume_game_transition
  
  - from: paused
    to: menu
    event: quit_to_menu
    on_transition: on_quit_to_menu_transition
  
  - from: playing
    to: game_over
    event: player_died
    on_transition: on_player_died_transition
    guard: check_player_health
  
  - from: game_over
    to: menu
    event: restart
    on_transition: on_restart_transition
```

### Usage

```cpp
#include <iostream>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

class GameStateExample {
public:
    void run() {
        // Create state machine from YAML configuration
        StateMachine fsm("config.yaml");
        
        // Register state callbacks
        fsm.registerStateCallback("menu", "on_enter", &GameStateExample::onMenuEnter, this);
        fsm.registerStateCallback("menu", "on_exit", &GameStateExample::onMenuExit, this);
        fsm.registerStateCallback("playing", "on_enter", &GameStateExample::onPlayingEnter, this);
        fsm.registerStateCallback("playing", "on_exit", &GameStateExample::onPlayingExit, this);
        fsm.registerStateCallback("paused", "on_enter", &GameStateExample::onPausedEnter, this);
        fsm.registerStateCallback("paused", "on_exit", &GameStateExample::onPausedExit, this);
        fsm.registerStateCallback("game_over", "on_enter", &GameStateExample::onGameOverEnter, this);
        fsm.registerStateCallback("game_over", "on_exit", &GameStateExample::onGameOverExit, this);
        
        fsm.registerTransitionCallback("menu", "playing", &GameStateExample::onStartGameTransition, this);
        fsm.registerTransitionCallback("playing", "paused", &GameStateExample::onPauseGameTransition, this);
        fsm.registerTransitionCallback("paused", "playing", &GameStateExample::onResumeGameTransition, this);
        fsm.registerTransitionCallback("paused", "menu", &GameStateExample::onQuitToMenuTransition, this);
        fsm.registerTransitionCallback("playing", "game_over", &GameStateExample::onPlayerDiedTransition, this);
        fsm.registerTransitionCallback("game_over", "menu", &GameStateExample::onRestartTransition, this);
        
        fsm.registerGuard("playing", "game_over", "player_died", &GameStateExample::checkPlayerHealth, this);
        
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
        
        // Start the state machine
        std::cout << "=== Game State Machine Example ===\n";
        fsm.start();
        
        // Simulate game process
        std::cout << "\n[Event] Starting game...\n";
        fsm.triggerEvent("start_game");
        
        std::cout << "\n[Event] Pausing game...\n";
        fsm.triggerEvent("pause_game");
        
        std::cout << "\n[Event] Resuming game...\n";
        fsm.triggerEvent("resume_game");
        
        std::cout << "\n[Event] Player died...\n";
        fsm.setVariable("player_health", VariableValue(0));
        fsm.triggerEvent("player_died");
        
        std::cout << "\n[Event] Restarting...\n";
        fsm.triggerEvent("restart");
        
        // Stop the state machine
        std::cout << "\n[Event] Quitting to menu...\n";
        fsm.triggerEvent("quit_to_menu");
        fsm.stop();
    }
    
private:
    void onMenuEnter() {
        std::cout << "  -> Entered menu state\n";
    }
    
    void onMenuExit() {
        std::cout << "  <- Exited menu state\n";
    }
    
    void onPlayingEnter() {
        std::cout << "  -> Entered playing state\n";
    }
    
    void onPlayingExit() {
        std::cout << "  <- Exited playing state\n";
    }
    
    void onPausedEnter() {
        std::cout << "  -> Entered paused state\n";
    }
    
    void onPausedExit() {
        std::cout << "  <- Exited paused state\n";
    }
    
    void onGameOverEnter() {
        std::cout << "  -> Entered game_over state\n";
    }
    
    void onGameOverExit() {
        std::cout << "  <- Exited game_over state\n";
    }
    
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
    
    bool checkPlayerHealth() {
        auto health = fsm.getVariable("player_health");
        return health.has_value() && health.value().asInt() <= 0;
    }
    
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
    
    StateMachine fsm;
};

int main() {
    GameStateExample example;
    example.run();
    return 0;
}
```

### Expected Output

```
=== Game State Machine Example ===
  -> Entered menu state
  [Action] Showing main menu
  [Action] Loading save data

[Event] Starting game...
  Transition: menu -> playing
  <- Exited menu state
  -> Entered playing state
  [Action] Starting game loop
  [Action] Spawning player

[Event] Pausing game...
  Transition: playing -> paused
  <- Exited playing state
  -> Entered paused state
  [Action] Showing pause menu

[Event] Resuming game...
  Transition: paused -> playing
  <- Exited paused state
  -> Entered playing state
  [Action] Starting game loop
  [Action] Spawning player

[Event] Player died...
  Transition: playing -> game_over
  <- Exited playing state
  -> Entered game_over state
  [Action] Showing game over screen
  [Action] Saving high score

[Event] Restarting...
  Transition: game_over -> menu
  <- Exited game_over state
  -> Entered menu state
  [Action] Showing main menu
  [Action] Loading save data

[Event] Quitting to menu...
  <- Exited menu state
```

## Network Protocol Example

### Overview

The network protocol example demonstrates complex guard logic and retry mechanisms.

### Configuration

```yaml
# Global variables
variables:
  max_retries: 3
  timeout: 5.0

# State definitions
states:
  disconnected:
    on_enter: on_disconnected_enter
    on_exit: on_disconnected_exit
    actions:
      - close_connection
      - log_disconnected
  
  connecting:
    on_enter: on_connecting_enter
    on_exit: on_connecting_exit
    actions:
      - initiate_connection
      - start_timeout_timer
  
  connected:
    on_enter: on_connected_enter
    on_exit: on_connected_exit
    variables:
      connection_time: 0.0
      bytes_sent: 0
      bytes_received: 0
    actions:
      - start_heartbeat
      - log_connected
  
  authenticating:
    on_enter: on_authenticating_enter
    on_exit: on_authenticating_exit
    actions:
      - send_credentials
      - wait_for_response
  
  authenticated:
    on_enter: on_authenticated_enter
    on_exit: on_authenticated_exit
    actions:
      - enable_data_transfer
      - log_authenticated
  
  error:
    on_enter: on_error_enter
    on_exit: on_error_exit
    actions:
      - log_error
      - cleanup_resources

# Transition definitions
transitions:
  - from: disconnected
    to: connecting
    event: connect
    on_transition: on_connect_transition
  
  - from: connecting
    to: connected
    event: connection_established
    on_transition: on_connection_established_transition
  
  - from: connecting
    to: error
    event: connection_failed
    on_transition: on_connection_failed_transition
  
  - from: connected
    to: authenticating
    event: authenticate
    on_transition: on_authenticate_transition
  
  - from: authenticating
    to: authenticated
    event: authentication_success
    on_transition: on_authentication_success_transition
  
  - from: authenticating
    to: error
    event: authentication_failed
    on_transition: on_authentication_failed_transition
  
  - from: authenticated
    to: disconnected
    event: disconnect
    on_transition: on_disconnect_transition
  
  - from: connected
    to: disconnected
    event: connection_lost
    on_transition: on_connection_lost_transition
  
  - from: error
    to: disconnected
    event: retry
    guard: check_retry_count
    on_transition: on_retry_transition
  
  - from: error
    to: disconnected
    event: give_up
    on_transition: on_give_up_transition
```

### Usage

```cpp
#include <iostream>
#include <fsmconfig/state_machine.hpp>

using namespace fsmconfig;

class NetworkProtocolExample {
public:
    void run() {
        // Create state machine from YAML configuration
        StateMachine fsm("config.yaml");
        
        // Register state callbacks
        fsm.registerStateCallback("disconnected", "on_enter", &NetworkProtocolExample::onDisconnectedEnter, this);
        fsm.registerStateCallback("disconnected", "on_exit", &NetworkProtocolExample::onDisconnectedExit, this);
        fsm.registerStateCallback("connecting", "on_enter", &NetworkProtocolExample::onConnectingEnter, this);
        fsm.registerStateCallback("connecting", "on_exit", &NetworkProtocolExample::onConnectingExit, this);
        fsm.registerStateCallback("connected", "on_enter", &NetworkProtocolExample::onConnectedEnter, this);
        fsm.registerStateCallback("connected", "on_exit", &NetworkProtocolExample::onConnectedExit, this);
        fsm.registerStateCallback("authenticating", "on_enter", &NetworkProtocolExample::onAuthenticatingEnter, this);
        fsm.registerStateCallback("authenticating", "on_exit", &NetworkProtocolExample::onAuthenticatingExit, this);
        fsm.registerStateCallback("authenticated", "on_enter", &NetworkProtocolExample::onAuthenticatedEnter, this);
        fsm.registerStateCallback("authenticated", "on_exit", &NetworkProtocolExample::onAuthenticatedExit, this);
        fsm.registerStateCallback("error", "on_enter", &NetworkProtocolExample::onErrorEnter, this);
        fsm.registerStateCallback("error", "on_exit", &NetworkProtocolExample::onErrorExit, this);
        
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
        
        fsm.registerGuard("error", "disconnected", "retry", &NetworkProtocolExample::checkRetryCount, this);
        
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
        
        // Set variables
        fsm.setVariable("max_retries", VariableValue(3));
        fsm.setVariable("timeout", VariableValue(5.0f));
        fsm.setVariable("retry_count", VariableValue(0));
        
        // Start the state machine
        std::cout << "=== Network Protocol State Machine Example ===\n";
        fsm.start();
        
        // Simulate network protocol
        std::cout << "\n[Event] Connecting...\n";
        fsm.triggerEvent("connect");
        
        std::cout << "\n[Event] Connection established\n";
        fsm.triggerEvent("connection_established");
        
        std::cout << "\n[Event] Authenticating...\n";
        fsm.triggerEvent("authenticate");
        
        std::cout << "\n[Event] Authentication successful\n";
        fsm.triggerEvent("authentication_success");
        
        std::cout << "\n[Event] Disconnecting...\n";
        fsm.triggerEvent("disconnect");
        
        // Simulate connection failure
        std::cout << "\n[Event] Connecting again...\n";
        fsm.triggerEvent("connect");
        
        std::cout << "\n[Event] Connection failed\n";
        fsm.triggerEvent("connection_failed");
        
        std::cout << "\n[Event] Retrying...\n";
        fsm.triggerEvent("retry");
        
        // Stop the state machine
        fsm.stop();
    }
    
private:
    void onDisconnectedEnter() {
        std::cout << "  -> Entered disconnected state\n";
    }
    
    void onDisconnectedExit() {
        std::cout << "  <- Exited disconnected state\n";
    }
    
    void onConnectingEnter() {
        std::cout << "  -> Entered connecting state\n";
    }
    
    void onConnectingExit() {
        std::cout << "  <- Exited connecting state\n";
    }
    
    void onConnectedEnter() {
        std::cout << "  -> Entered connected state\n";
    }
    
    void onConnectedExit() {
        std::cout << "  <- Exited connected state\n";
    }
    
    void onAuthenticatingEnter() {
        std::cout << "  -> Entered authenticating state\n";
    }
    
    void onAuthenticatingExit() {
        std::cout << "  <- Exited authenticating state\n";
    }
    
    void onAuthenticatedEnter() {
        std::cout << "  -> Entered authenticated state\n";
    }
    
    void onAuthenticatedExit() {
        std::cout << "  <- Exited authenticated state\n";
    }
    
    void onErrorEnter() {
        std::cout << "  -> Entered error state\n";
    }
    
    void onErrorExit() {
        std::cout << "  <- Exited error state\n";
    }
    
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
    
    bool checkRetryCount() {
        auto retryCount = fsm.getVariable("retry_count");
        auto maxRetries = fsm.getVariable("max_retries");
        if (!retryCount.has_value() || !maxRetries.has_value()) {
            return false;
        }
        return retryCount.value().asInt() < maxRetries.value().asInt();
    }
    
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
    
    StateMachine fsm;
};

int main() {
    NetworkProtocolExample example;
    example.run();
    return 0;
}
```

### Expected Output

```
=== Network Protocol State Machine Example ===
  -> Entered disconnected state
  [Action] Closing connection
  [Action] Logged as disconnected

[Event] Connecting...
  Transition: disconnected -> connecting
  <- Exited disconnected state
  -> Entered connecting state
  [Action] Initiating connection
  [Action] Starting timeout timer

[Event] Connection established
  Transition: connecting -> connected
  <- Exited connecting state
  -> Entered connected state
  [Action] Starting heartbeat
  [Action] Logged as connected

[Event] Authenticating...
  Transition: connected -> authenticating
  <- Exited connected state
  -> Entered authenticating state
  [Action] Sending credentials
  [Action] Waiting for response

[Event] Authentication successful
  Transition: authenticating -> authenticated
  <- Exited authenticating state
  -> Entered authenticated state
  [Action] Enabling data transfer
  [Action] Logged as authenticated

[Event] Disconnecting...
  Transition: authenticated -> disconnected
  <- Exited authenticated state
  -> Entered disconnected state
  [Action] Closing connection
  [Action] Logged as disconnected

[Event] Connecting again...
  Transition: disconnected -> connecting
  <- Exited disconnected state
  -> Entered connecting state
  [Action] Initiating connection
  [Action] Starting timeout timer

[Event] Connection failed
  Transition: connecting -> error
  <- Exited connecting state
  -> Entered error state
  [Action] Logging error
  [Action] Cleaning up resources

[Event] Retrying...
  Transition: error -> disconnected
  <- Exited error state
  -> Entered disconnected state
  [Action] Closing connection
  [Action] Logged as disconnected

Stopping FSM...
  <- Exited disconnected state
```

## Building the Examples

To build the examples, use CMake:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

To build a specific example:

```bash
cmake --build simple_fsm
cmake --build game_state
cmake --build network_protocol
```

To run an example:

```bash
./build/examples/simple_fsm/simple_fsm
./build/examples/game_state/game_state
./build/examples/network_protocol/network_protocol
```

## Conclusion

These examples demonstrate the key features of the FSMConfig library:
- Declarative state machine definition in YAML
- Type-safe callback registration
- Guard conditions for conditional transitions
- Global and state-local variables
- Action execution
- Observer pattern for monitoring state changes

For more information, see the [Architecture](architecture.md) and [API Reference](api_reference.md) documentation.
