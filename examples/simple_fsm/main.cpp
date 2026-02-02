#include <fsmconfig/state_machine.hpp>
#include <iostream>

using namespace fsmconfig;

/**
 * @file main.cpp
 * @brief Simple example of FSMConfig usage
 *
 * This example demonstrates basic usage of the FSMConfig library
 * for creating a simple finite state machine with two states: idle and active.
 */

/**
 * @class SimpleFSMExample
 * @brief Example of using a finite state machine for simple state switching logic
 *
 * Demonstrates:
 * - Registration of state callbacks (on_enter, on_exit)
 * - Registration of transition callbacks
 * - Registration of actions
 * - Triggering events to change states
 */
class SimpleFSMExample {
 public:
  /**
   * @brief Run the example
   */
  void run() {
    // Create a finite state machine from YAML configuration
    StateMachine fsm("config.yaml");

    // Register idle state callbacks
    fsm.registerStateCallback("idle", "on_enter", &SimpleFSMExample::onIdleEnter, this);
    fsm.registerStateCallback("idle", "on_exit", &SimpleFSMExample::onIdleExit, this);

    // Register active state callbacks
    fsm.registerStateCallback("active", "on_enter", &SimpleFSMExample::onActiveEnter, this);
    fsm.registerStateCallback("active", "on_exit", &SimpleFSMExample::onActiveExit, this);

    // Register transition callbacks
    fsm.registerTransitionCallback("idle", "active", &SimpleFSMExample::onStartTransition, this);
    fsm.registerTransitionCallback("active", "idle", &SimpleFSMExample::onStopTransition, this);

    // Register actions
    fsm.registerAction("log_idle_state", &SimpleFSMExample::logIdleState, this);
    fsm.registerAction("log_active_state", &SimpleFSMExample::logActiveState, this);

    // Start the finite state machine
    std::cout << "Starting FSM...\n";
    fsm.start();

    // Trigger "start" event for idle -> active transition
    std::cout << "\nTriggering 'start' event...\n";
    fsm.triggerEvent("start");

    // Trigger "stop" event for active -> idle transition
    std::cout << "\nTriggering 'stop' event...\n";
    fsm.triggerEvent("stop");

    // Stop the finite state machine
    std::cout << "\nStopping FSM...\n";
    fsm.stop();
  }

 private:
  /**
   * @brief Callback when entering idle state
   */
  void onIdleEnter() { std::cout << "  -> Entering idle state\n"; }

  /**
   * @brief Callback when exiting idle state
   */
  void onIdleExit() { std::cout << "  <- Exiting idle state\n"; }

  /**
   * @brief Callback when entering active state
   */
  void onActiveEnter() { std::cout << "  -> Entering active state\n"; }

  /**
   * @brief Callback when exiting active state
   */
  void onActiveExit() { std::cout << "  <- Exiting active state\n"; }

  /**
   * @brief Callback for idle -> active transition
   * @param event Transition event
   */
  void onStartTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  /**
   * @brief Callback for active -> idle transition
   * @param event Transition event
   */
  void onStopTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  /**
   * @brief Action to log idle state
   */
  void logIdleState() { std::cout << "  [Action] Now in idle state\n"; }

  /**
   * @brief Action to log active state
   */
  void logActiveState() { std::cout << "  [Action] Now in active state\n"; }
};

/**
 * @brief Application entry point
 */
int main() {
  SimpleFSMExample example;
  example.run();
  return 0;
}
