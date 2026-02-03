#include <fsmconfig/state_machine.hpp>
#include <fsmconfig/types.hpp>
#include <iostream>
#include <memory>

using namespace fsmconfig;

/**
 * @class NetworkProtocolObserver
 * @brief Observer for network protocol state machine events
 *
 * Demonstrates the new observer API using std::shared_ptr.
 * Observers are notified when states change or transitions occur.
 */
class NetworkProtocolObserver : public StateObserver {
 public:
  /**
   * @brief Called when entering a state
   * @param state_name Name of the state being entered
   */
  void onStateEnter(const std::string& state_name) override {
    std::cout << "  [Observer] Entering state: " << state_name << "\n";
  }

  /**
   * @brief Called when exiting a state
   * @param state_name Name of the state being exited
   */
  void onStateExit(const std::string& state_name) override {
    std::cout << "  [Observer] Exiting state: " << state_name << "\n";
  }

  /**
   * @brief Called when a transition occurs
   * @param event Transition event containing from_state, to_state, and event_name
   */
  void onTransition(const TransitionEvent& event) override {
    std::cout << "  [Observer] Transition: " << event.from_state << " -> "
              << event.to_state << " (event: " << event.event_name << ")\n";
  }

  /**
   * @brief Called when an error occurs
   * @param error_message Error message
   */
  void onError(const std::string& error_message) override {
    std::cout << "  [Observer] Error: " << error_message << "\n";
  }
};

/**
 * @file main.cpp
 * @brief Example of using FSMConfig for a network protocol
 *
 * This example demonstrates the use of the FSMConfig library
 * for modeling network protocol states
 * (Disconnected, Connecting, Connected, Authenticating, Authenticated, Error).
 * Shows complex logic with guard conditions.
 */

/**
 * @class NetworkProtocolExample
 * @brief Example of using a finite state machine for a network protocol
 *
 * Demonstrates:
 * - Registration of state callbacks (on_enter, on_exit)
 * - Registration of transition callbacks
 * - Registration of guard conditions for transition protection
 * - Registration of actions
 * - Working with variables (global and state)
 * - Triggering events to change states
 * - Complex logic with multiple states and transitions
 */
class NetworkProtocolExample {
 public:
  /**
   * @brief Run the example
   */
  void run() {
    // Create a finite state machine from YAML configuration
    fsm_ptr = std::make_unique<StateMachine>("config.yaml");
    StateMachine& fsm = *fsm_ptr;

    // Create and register a state observer using std::make_shared
    auto observer = std::make_shared<NetworkProtocolObserver>();
    fsm.registerStateObserver(observer);

    // Register disconnected state callbacks
    fsm.registerStateCallback("disconnected", "on_enter", &NetworkProtocolExample::onDisconnectedEnter, this);
    fsm.registerStateCallback("disconnected", "on_exit", &NetworkProtocolExample::onDisconnectedExit, this);

    // Register connecting state callbacks
    fsm.registerStateCallback("connecting", "on_enter", &NetworkProtocolExample::onConnectingEnter, this);
    fsm.registerStateCallback("connecting", "on_exit", &NetworkProtocolExample::onConnectingExit, this);

    // Register connected state callbacks
    fsm.registerStateCallback("connected", "on_enter", &NetworkProtocolExample::onConnectedEnter, this);
    fsm.registerStateCallback("connected", "on_exit", &NetworkProtocolExample::onConnectedExit, this);

    // Register authenticating state callbacks
    fsm.registerStateCallback("authenticating", "on_enter", &NetworkProtocolExample::onAuthenticatingEnter, this);
    fsm.registerStateCallback("authenticating", "on_exit", &NetworkProtocolExample::onAuthenticatingExit, this);

    // Register authenticated state callbacks
    fsm.registerStateCallback("authenticated", "on_enter", &NetworkProtocolExample::onAuthenticatedEnter, this);
    fsm.registerStateCallback("authenticated", "on_exit", &NetworkProtocolExample::onAuthenticatedExit, this);

    // Register error state callbacks
    fsm.registerStateCallback("error", "on_enter", &NetworkProtocolExample::onErrorEnter, this);
    fsm.registerStateCallback("error", "on_exit", &NetworkProtocolExample::onErrorExit, this);

    // Register transition callbacks
    fsm.registerTransitionCallback("disconnected", "connecting", &NetworkProtocolExample::onConnectTransition, this);
    fsm.registerTransitionCallback("connecting", "connected",
                                   &NetworkProtocolExample::onConnectionEstablishedTransition, this);
    fsm.registerTransitionCallback("connecting", "error", &NetworkProtocolExample::onConnectionFailedTransition, this);
    fsm.registerTransitionCallback("connected", "authenticating", &NetworkProtocolExample::onAuthenticateTransition,
                                   this);
    fsm.registerTransitionCallback("authenticating", "authenticated",
                                   &NetworkProtocolExample::onAuthenticationSuccessTransition, this);
    fsm.registerTransitionCallback("authenticating", "error", &NetworkProtocolExample::onAuthenticationFailedTransition,
                                   this);
    fsm.registerTransitionCallback("authenticated", "disconnected", &NetworkProtocolExample::onDisconnectTransition,
                                   this);
    fsm.registerTransitionCallback("connected", "disconnected", &NetworkProtocolExample::onConnectionLostTransition,
                                   this);
    fsm.registerTransitionCallback("error", "disconnected", &NetworkProtocolExample::onRetryTransition, this);
    fsm.registerTransitionCallback("error", "disconnected", &NetworkProtocolExample::onGiveUpTransition, this);

    // Register guard condition for error -> disconnected transition (retry)
    fsm.registerGuard("error", "disconnected", "retry", &NetworkProtocolExample::checkRetryCount, this);

    // Register actions
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

    // Set global variables
    fsm.setVariable("max_retries", VariableValue(3));
    fsm.setVariable("timeout", VariableValue(5.0f));
    fsm.setVariable("retry_count", VariableValue(0));

    // Start the finite state machine
    std::cout << "=== Network Protocol State Machine Example ===\n";
    fsm.start();

    // Simulate network protocol

    // Try to connect
    std::cout << "\n[Event] Connecting...\n";
    fsm.triggerEvent("connect");

    // Connection established
    std::cout << "\n[Event] Connection established\n";
    fsm.triggerEvent("connection_established");

    // Start authentication
    std::cout << "\n[Event] Authenticating...\n";
    fsm.triggerEvent("authenticate");

    // Authentication successful
    std::cout << "\n[Event] Authentication successful\n";
    fsm.triggerEvent("authentication_success");

    // Disconnect
    std::cout << "\n[Event] Disconnecting...\n";
    fsm.triggerEvent("disconnect");

    // Simulate connection error
    std::cout << "\n[Event] Connecting again...\n";
    fsm.triggerEvent("connect");

    // Connection failed
    std::cout << "\n[Event] Connection failed\n";
    fsm.triggerEvent("connection_failed");

    // Try again (retry_count < max_retries)
    std::cout << "\n[Event] Retrying...\n";
    fsm.triggerEvent("retry");

    // Stop the finite state machine
    fsm.stop();
  }

 private:
  // Disconnected state callbacks
  void onDisconnectedEnter() { std::cout << "  -> Entered disconnected state\n"; }

  void onDisconnectedExit() { std::cout << "  <- Exited disconnected state\n"; }

  // Connecting state callbacks
  void onConnectingEnter() { std::cout << "  -> Entered connecting state\n"; }

  void onConnectingExit() { std::cout << "  <- Exited connecting state\n"; }

  // Connected state callbacks
  void onConnectedEnter() { std::cout << "  -> Entered connected state\n"; }

  void onConnectedExit() { std::cout << "  <- Exited connected state\n"; }

  // Authenticating state callbacks
  void onAuthenticatingEnter() { std::cout << "  -> Entered authenticating state\n"; }

  void onAuthenticatingExit() { std::cout << "  <- Exited authenticating state\n"; }

  // Authenticated state callbacks
  void onAuthenticatedEnter() { std::cout << "  -> Entered authenticated state\n"; }

  void onAuthenticatedExit() { std::cout << "  <- Exited authenticated state\n"; }

  // Error state callbacks
  void onErrorEnter() { std::cout << "  -> Entered error state\n"; }

  void onErrorExit() { std::cout << "  <- Exited error state\n"; }

  // Transition callbacks
  void onConnectTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onConnectionEstablishedTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onConnectionFailedTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onAuthenticateTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onAuthenticationSuccessTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onAuthenticationFailedTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onDisconnectTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onConnectionLostTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onRetryTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  void onGiveUpTransition(const TransitionEvent& event) {
    std::cout << "  Transition: " << event.from_state << " -> " << event.to_state << "\n";
  }

  // Guard condition
  /**
   * @brief Check retry count before reconnecting
   * @return true if retry_count < max_retries
   */
  bool checkRetryCount() {
    VariableValue retryCount = fsm_ptr->getVariable("retry_count");
    VariableValue maxRetries = fsm_ptr->getVariable("max_retries");
    return retryCount.asInt() < maxRetries.asInt();
  }

  // Actions
  void closeConnection() { std::cout << "  [Action] Closing connection\n"; }

  void logDisconnected() { std::cout << "  [Action] Logged as disconnected\n"; }

  void initiateConnection() { std::cout << "  [Action] Initiating connection\n"; }

  void startTimeoutTimer() { std::cout << "  [Action] Starting timeout timer\n"; }

  void startHeartbeat() { std::cout << "  [Action] Starting heartbeat\n"; }

  void logConnected() { std::cout << "  [Action] Logged as connected\n"; }

  void sendCredentials() { std::cout << "  [Action] Sending credentials\n"; }

  void waitForResponse() { std::cout << "  [Action] Waiting for response\n"; }

  void enableDataTransfer() { std::cout << "  [Action] Enabling data transfer\n"; }

  void logAuthenticated() { std::cout << "  [Action] Logged as authenticated\n"; }

  void logError() { std::cout << "  [Action] Logging error\n"; }

  void cleanupResources() { std::cout << "  [Action] Cleaning up resources\n"; }

  std::unique_ptr<StateMachine> fsm_ptr;
};

/**
 * @brief Application entry point
 */
int main() {
  NetworkProtocolExample example;
  example.run();
  return 0;
}
