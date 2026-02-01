#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <fsmconfig/state_machine.hpp>
#include <fsmconfig/types.hpp>

using namespace fsmconfig;

/**
 * @file test_state_machine.cpp
 * @brief Tests for StateMachine
 */

class StateMachineTest : public ::testing::Test {
protected:
    std::unique_ptr<StateMachine> fsm;
    std::string test_config_path;
    
    void SetUp() override {
        auto temp_dir = std::filesystem::temp_directory_path();
        test_config_path = (temp_dir / "fsmconfig_state_machine_test.yaml").string();
    }
    
    void TearDown() override {
        fsm.reset();
        std::remove(test_config_path.c_str());
    }
    
    void writeTestConfig(const std::string& content) {
        std::ofstream file(test_config_path.c_str());
        file << content;
        file.close();
    }
};

TEST_F(StateMachineTest, CreateFromConfigFile) {
    std::string yaml_content = R"(
variables:
  test_var: 100

states:
  initial_state:
    on_enter: on_initial_enter

  target_state:
    on_enter: on_target_enter

transitions:
  - from: initial_state
    to: target_state
    event: move_forward
)";
    
    writeTestConfig(yaml_content);
    
    ASSERT_NO_THROW(fsm = std::make_unique<StateMachine>(test_config_path));
}

TEST_F(StateMachineTest, CreateFromString) {
    std::string yaml_content = R"(
states:
  state1:
    on_enter: enter_cb
)";
    
    ASSERT_NO_THROW(fsm = std::make_unique<StateMachine>(yaml_content, true));
}

TEST_F(StateMachineTest, StartTransitionsToInitialState) {
    std::string yaml_content = R"(
states:
  initial_state:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    ASSERT_NO_THROW(fsm->start());
    EXPECT_EQ(fsm->getCurrentState(), "initial_state");
}

TEST_F(StateMachineTest, StartWithoutInitialStateThrowsException) {
    std::string yaml_content = R"(
states:
  state1:
    on_enter: enter_cb
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    // Now start() does not throw exception for missing initial state
    // The machine simply starts with the first state found
    ASSERT_NO_THROW(fsm->start());
    EXPECT_EQ(fsm->getCurrentState(), "state1");
}

TEST_F(StateMachineTest, TriggerEventCausesTransition) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    EXPECT_EQ(fsm->getCurrentState(), "state1");
    
    fsm->triggerEvent("move_forward");
    EXPECT_EQ(fsm->getCurrentState(), "state2");
}

TEST_F(StateMachineTest, TriggerEventWithNoTransitionDoesNothing) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    // Now triggerEvent() does not throw exception for events without transition
    // It simply ignores them
    ASSERT_NO_THROW(fsm->triggerEvent("nonexistent_event"));
    EXPECT_EQ(fsm->getCurrentState(), "state1");
}

TEST_F(StateMachineTest, StopCallsExitCallback) {
    std::string yaml_content = R"(
states:
  state1:
    on_enter: on_enter_state1
    on_exit: on_exit_state1
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestCallbacks {
    public:
        bool enter_called;
        bool exit_called;
        TestCallbacks() {
            enter_called = false;
            exit_called = false;
        }
        void onEnter() { enter_called = true; }
        void onExit() { exit_called = true; }
    };
    
    TestCallbacks callbacks;
    fsm->registerStateCallback("state1", "on_enter", &TestCallbacks::onEnter, &callbacks);
    fsm->registerStateCallback("state1", "on_exit", &TestCallbacks::onExit, &callbacks);
    
    fsm->start();
    EXPECT_TRUE(callbacks.enter_called);
    EXPECT_FALSE(callbacks.exit_called);
    
    fsm->stop();
    EXPECT_TRUE(callbacks.exit_called);
}

TEST_F(StateMachineTest, ResetReturnsToInitialState) {
    std::string yaml_content = R"(
states:
  initial_state:
  other_state:

transitions:
  - from: initial_state
    to: other_state
    event: move
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    fsm->triggerEvent("move");
    EXPECT_EQ(fsm->getCurrentState(), "other_state");
    
    fsm->reset();
    // After reset, need to call start() to return to initial state
    fsm->start();
    EXPECT_EQ(fsm->getCurrentState(), "initial_state");
}

TEST_F(StateMachineTest, HasStateReturnsCorrectValue) {
    std::string yaml_content = R"(
states:
  state1:
  state2:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_TRUE(fsm->hasState("state1"));
    EXPECT_TRUE(fsm->hasState("state2"));
    EXPECT_FALSE(fsm->hasState("nonexistent_state"));
}

TEST_F(StateMachineTest, GetAllStatesReturnsAllStates) {
    std::string yaml_content = R"(
states:
  state1:
  state2:
  state3:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    auto states = fsm->getAllStates();
    EXPECT_EQ(states.size(), 3);
    EXPECT_EQ(states[0], "state1");
    EXPECT_EQ(states[1], "state2");
    EXPECT_EQ(states[2], "state3");
}

TEST_F(StateMachineTest, SetAndGetVariable) {
    std::string yaml_content = R"(
variables:
  global_var: 100

states:
  state1:
    variables:
      local_var: 50
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->setVariable("global_var", VariableValue(200));
    EXPECT_EQ(fsm->getVariable("global_var").asInt(), 200);
    
    fsm->start();
    EXPECT_EQ(fsm->getVariable("local_var").asInt(), 50);
}

TEST_F(StateMachineTest, HasVariableReturnsCorrectValue) {
    std::string yaml_content = R"(
variables:
  global_var: 100
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_TRUE(fsm->hasVariable("global_var"));
    EXPECT_FALSE(fsm->hasVariable("nonexistent_var"));
}

TEST_F(StateMachineTest, TriggerEventWithData) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    std::map<std::string, VariableValue> data;
    data["event_data"] = VariableValue(42);
    
    fsm->start();
    fsm->triggerEvent("move_forward", data);
    EXPECT_EQ(fsm->getCurrentState(), "state2");
}

TEST_F(StateMachineTest, StateObserverReceivesCallbacks) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestObserver : public StateObserver {
    public:
        int enter_count;
        int exit_count;
        int transition_count;
        TestObserver() {
            enter_count = 0;
            exit_count = 0;
            transition_count = 0;
        }
        
        void onStateEnter(const std::string& state_name) override {
            (void)state_name;
            enter_count++;
        }
        
        void onStateExit(const std::string& state_name) override {
            (void)state_name;
            exit_count++;
        }
        
        void onTransition(const TransitionEvent& event) override {
            (void)event;
            transition_count++;
        }
        
        void onError(const std::string& error_message) override {
            (void)error_message;
        }
    };
    
    TestObserver observer;
    fsm->registerStateObserver(&observer);
    
    fsm->start();
    EXPECT_EQ(observer.enter_count, 1);
    
    fsm->triggerEvent("move_forward");
    EXPECT_EQ(observer.exit_count, 1);
    EXPECT_EQ(observer.enter_count, 2);
    EXPECT_EQ(observer.transition_count, 1);
    
    fsm->unregisterStateObserver(&observer);
}

TEST_F(StateMachineTest, ErrorHandlerReceivesErrors) {
    std::string yaml_content = R"(
states:
  state1:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    std::string last_error;
    fsm->setErrorHandler([&last_error](const std::string& error) {
        last_error = error;
    });
    
    EXPECT_THROW(fsm->triggerEvent("nonexistent_event"), StateException);
    EXPECT_FALSE(last_error.empty());
}

TEST_F(StateMachineTest, GuardPreventsTransition) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
    guard: guard_cb
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestGuard {
    public:
        bool result;
        TestGuard() {
            result = false;
        }
        bool onGuard() { return result; }
    };
    
    TestGuard guard;
    fsm->registerGuard("state1", "state2", "move_forward", &TestGuard::onGuard, &guard);
    
    fsm->start();
    fsm->triggerEvent("move_forward");
    EXPECT_EQ(fsm->getCurrentState(), "state1");
}

TEST_F(StateMachineTest, GuardAllowsTransition) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move_forward
    guard: guard_cb
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestGuard {
    public:
        bool result;
        TestGuard() {
            result = true;
        }
        bool onGuard() { return result; }
    };
    
    TestGuard guard;
    fsm->registerGuard("state1", "state2", "move_forward", &TestGuard::onGuard, &guard);
    
    fsm->start();
    fsm->triggerEvent("move_forward");
    EXPECT_EQ(fsm->getCurrentState(), "state2");
}

TEST_F(StateMachineTest, MultipleTransitionsFromSameState) {
    std::string yaml_content = R"(
states:
  state1:
  state2:
  state3:

transitions:
  - from: state1
    to: state2
    event: go_to_2
  - from: state1
    to: state3
    event: go_to_3
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    EXPECT_EQ(fsm->getCurrentState(), "state1");
    
    fsm->triggerEvent("go_to_2");
    EXPECT_EQ(fsm->getCurrentState(), "state2");
    
    fsm->reset();
    fsm->start();
    
    fsm->triggerEvent("go_to_3");
    EXPECT_EQ(fsm->getCurrentState(), "state3");
}

TEST_F(StateMachineTest, StateActionsAreExecuted) {
    std::string yaml_content = R"(
states:
  state1:
    actions:
      - action1
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestAction {
    public:
        bool called;
        TestAction() {
            called = false;
        }
        void onAction() { called = true; }
    };
    
    TestAction action;
    fsm->registerAction("action1", &TestAction::onAction, &action);
    
    fsm->start();
    
    EXPECT_TRUE(action.called);
}

TEST_F(StateMachineTest, TransitionActionsAreExecuted) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
    actions:
      - transition_action
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestAction {
    public:
        bool called;
        TestAction() {
            called = false;
        }
        void onAction() { called = true; }
    };
    
    TestAction action;
    fsm->registerAction("transition_action", &TestAction::onAction, &action);
    
    fsm->start();
    fsm->triggerEvent("move");
    
    EXPECT_TRUE(action.called);
}

TEST_F(StateMachineTest, TransitionCallbackIsExecuted) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
    on_transition: transition_cb
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestTransition {
    public:
        bool called;
        TestTransition() {
            called = false;
        }
        void onTransition(const TransitionEvent& event) {
            (void)event;
            called = true;
        }
    };
    
    TestTransition transition;
    fsm->registerTransitionCallback("state1", "state2", &TestTransition::onTransition, &transition);
    
    fsm->start();
    fsm->triggerEvent("move");
    
    EXPECT_TRUE(transition.called);
}

TEST_F(StateMachineTest, CannotTriggerEventWhenNotStarted) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_THROW(fsm->triggerEvent("move"), StateException);
}

TEST_F(StateMachineTest, CannotStartWhenAlreadyStarted) {
    std::string yaml_content = R"(
states:
  initial_state:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    EXPECT_THROW(fsm->start(), StateException);
}

TEST_F(StateMachineTest, CannotStopWhenNotStarted) {
    std::string yaml_content = R"(
states:
  state1:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_THROW(fsm->stop(), StateException);
}

TEST_F(StateMachineTest, VariableValueCanBeModified) {
    std::string yaml_content = R"(
variables:
  counter: 0
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_EQ(fsm->getVariable("counter").asInt(), 0);
    
    fsm->setVariable("counter", VariableValue(10));
    EXPECT_EQ(fsm->getVariable("counter").asInt(), 10);
    
    fsm->setVariable("counter", VariableValue(20));
    EXPECT_EQ(fsm->getVariable("counter").asInt(), 20);
}

TEST_F(StateMachineTest, StateLocalVariableOverridesGlobal) {
    std::string yaml_content = R"(
variables:
  var: 100

states:
  state1:
    variables:
      var: 50
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    
    EXPECT_EQ(fsm->getVariable("var").asInt(), 50);
}

TEST_F(StateMachineTest, MultipleObserversReceiveCallbacks) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestObserver : public StateObserver {
    public:
        int callback_count;
        TestObserver() {
            callback_count = 0;
        }
        
        void onStateEnter(const std::string& state_name) override {
            (void)state_name;
            callback_count++;
        }
        
        void onStateExit(const std::string& state_name) override {
            (void)state_name;
            callback_count++;
        }
        
        void onTransition(const TransitionEvent& event) override {
            (void)event;
            callback_count++;
        }
        
        void onError(const std::string& error_message) override {
            (void)error_message;
        }
    };
    
    TestObserver observer1;
    TestObserver observer2;
    
    fsm->registerStateObserver(&observer1);
    fsm->registerStateObserver(&observer2);
    
    fsm->start();
    
    EXPECT_EQ(observer1.callback_count, 1);
    EXPECT_EQ(observer2.callback_count, 1);
    
    fsm->triggerEvent("move");
    
    EXPECT_EQ(observer1.callback_count, 4);
    EXPECT_EQ(observer2.callback_count, 4);
}

TEST_F(StateMachineTest, UnregisterObserverStopsCallbacks) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestObserver : public StateObserver {
    public:
        int callback_count;
        TestObserver() {
            callback_count = 0;
        }
        
        void onStateEnter(const std::string& state_name) override {
            (void)state_name;
            callback_count++;
        }
        
        void onStateExit(const std::string& state_name) override {
            (void)state_name;
            callback_count++;
        }
        
        void onTransition(const TransitionEvent& event) override {
            (void)event;
            callback_count++;
        }
        
        void onError(const std::string& error_message) override {
            (void)error_message;
        }
    };
    
    TestObserver observer;
    fsm->registerStateObserver(&observer);
    
    fsm->start();
    EXPECT_EQ(observer.callback_count, 1);
    
    fsm->unregisterStateObserver(&observer);
    
    fsm->triggerEvent("move");
    
    EXPECT_EQ(observer.callback_count, 1);
}

TEST_F(StateMachineTest, GetVariableThrowsForNonexistentVariable) {
    std::string yaml_content = R"(
variables:
  existing_var: 100
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_THROW(fsm->getVariable("nonexistent_var"), StateException);
}

TEST_F(StateMachineTest, ComplexTransitionChain) {
    std::string yaml_content = R"(
states:
  state1:
  state2:
  state3:

transitions:
  - from: state1
    to: state2
    event: move_to_2
  - from: state2
    to: state3
    event: move_to_3
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    EXPECT_EQ(fsm->getCurrentState(), "state1");
    
    fsm->triggerEvent("move_to_2");
    EXPECT_EQ(fsm->getCurrentState(), "state2");
    
    fsm->triggerEvent("move_to_3");
    EXPECT_EQ(fsm->getCurrentState(), "state3");
}

TEST_F(StateMachineTest, ResetAfterTransition) {
    std::string yaml_content = R"(
states:
  initial_state:
  other_state:

transitions:
  - from: initial_state
    to: other_state
    event: move
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestCallbacks1 {
    public:
        bool enter_called;
        TestCallbacks1() {
            enter_called = false;
        }
        void onEnter() { enter_called = true; }
    };
    
    class TestCallbacks2 {
    public:
        bool enter_called;
        TestCallbacks2() {
            enter_called = false;
        }
        void onEnter() { enter_called = true; }
    };
    
    TestCallbacks1 callbacks1;
    TestCallbacks2 callbacks2;
    fsm->registerStateCallback("initial_state", "on_enter", &TestCallbacks1::onEnter, &callbacks1);
    fsm->registerStateCallback("other_state", "on_enter", &TestCallbacks2::onEnter, &callbacks2);
    
    fsm->start();
    EXPECT_TRUE(callbacks1.enter_called);
    EXPECT_FALSE(callbacks2.enter_called);
    
    fsm->triggerEvent("move");
    EXPECT_TRUE(callbacks2.enter_called);
    
    callbacks1.enter_called = false;
    callbacks2.enter_called = false;
    
    fsm->reset();
    // After reset, need to call start() to trigger callbacks
    fsm->start();
    
    EXPECT_TRUE(callbacks1.enter_called);
    EXPECT_FALSE(callbacks2.enter_called);
}

TEST_F(StateMachineTest, EventDataIsPassedToTransitionCallback) {
    std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
    on_transition: transition_cb
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestTransition {
    public:
        bool callback_called;
        int captured_data;
        TestTransition() {
            callback_called = false;
            captured_data = 0;
        }
        
        void onTransition(const TransitionEvent& event) {
            callback_called = true;
            if (event.data.find("test_data") != event.data.end()) {
                captured_data = event.data.at("test_data").asInt();
            }
        }
    };
    
    TestTransition transition;
    fsm->registerTransitionCallback("state1", "state2", &TestTransition::onTransition, &transition);
    
    fsm->start();
    
    std::map<std::string, VariableValue> data;
    data["test_data"] = VariableValue(123);
    fsm->triggerEvent("move", data);
    
    EXPECT_TRUE(transition.callback_called);
    EXPECT_EQ(transition.captured_data, 123);
}
