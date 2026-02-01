#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <fsmconfig/state_machine.hpp>
#include <fsmconfig/types.hpp>

using namespace fsmconfig;

/**
 * @file test_integration.cpp
 * @brief Integration tests for FSMConfig
 */

class IntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<StateMachine> fsm;
    std::string test_config_path;
    
    IntegrationTest() = default;
    ~IntegrationTest() override = default;
    
    // Prohibit copying
    IntegrationTest(const IntegrationTest&) = delete;
    IntegrationTest& operator=(const IntegrationTest&) = delete;
    
    void SetUp() override {
        auto temp_dir = std::filesystem::temp_directory_path();
        test_config_path = (temp_dir / "fsmconfig_integration_test.yaml").string();
    }
    
    void TearDown() override {
        fsm.reset();
        std::remove(test_config_path.c_str());
    }
    
    void writeTestConfig(const std::string& content) {
        std::ofstream file;
        file.open(test_config_path);
        file << content;
        file.close();
    }
};

TEST_F(IntegrationTest, SimpleTwoStateMachine) {
    std::string yaml_content = R"(
states:
  idle:
    on_enter: on_idle_enter
    on_exit: on_idle_exit
    actions:
      - log_idle
  active:
    on_enter: on_active_enter
    on_exit: on_active_exit
    actions:
      - log_active

transitions:
  - from: idle
    to: active
    event: start
  - from: active
    to: idle
    event: stop
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestObserver : public StateObserver {
    public:
        int enter_count;
        int exit_count;
        int transition_count;
        TestObserver() : enter_count(0), exit_count(0), transition_count(0) {}
        
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
    
    EXPECT_EQ(fsm->getCurrentState(), "idle");
    EXPECT_EQ(observer.enter_count, 1);
    EXPECT_EQ(observer.exit_count, 0);
    EXPECT_EQ(observer.transition_count, 0);
    
    fsm->triggerEvent("start");
    
    EXPECT_EQ(fsm->getCurrentState(), "active");
    EXPECT_EQ(observer.enter_count, 2);
    EXPECT_EQ(observer.exit_count, 1);
    EXPECT_EQ(observer.transition_count, 1);
    
    fsm->triggerEvent("stop");
    
    EXPECT_EQ(fsm->getCurrentState(), "idle");
    EXPECT_EQ(observer.enter_count, 3);
    EXPECT_EQ(observer.exit_count, 2);
    EXPECT_EQ(observer.transition_count, 2);
}

TEST_F(IntegrationTest, GuardPreventsInvalidTransition) {
    std::string yaml_content = R"(
variables:
  player_health: 100

states:
  alive:
    on_enter: on_alive_enter
  dead:
    on_enter: on_dead_enter

transitions:
  - from: alive
    to: dead
    event: die
    guard: check_health
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    class TestGuard {
    public:
        StateMachine* fsm;
        TestGuard(StateMachine* machine) : fsm(machine) {}
        bool onGuard() {
            return fsm->getVariable("player_health").asInt() == 0;
        }
    };
    
    TestGuard guard(fsm.get());
    fsm->registerGuard("alive", "dead", "die", &TestGuard::onGuard, &guard);
    
    fsm->start();
    EXPECT_EQ(fsm->getCurrentState(), "alive");
    
    fsm->triggerEvent("die");
    
    EXPECT_EQ(fsm->getCurrentState(), "alive");
    
    fsm->setVariable("player_health", VariableValue(0));
    fsm->triggerEvent("die");
    
    EXPECT_EQ(fsm->getCurrentState(), "dead");
    
    fsm->setVariable("player_health", VariableValue(50));
    fsm->triggerEvent("die");
    
    EXPECT_EQ(fsm->getCurrentState(), "dead");
}

TEST_F(IntegrationTest, StateLocalVariablesHavePriority) {
    std::string yaml_content = R"(
variables:
  global_var: 100

states:
  state1:
    variables:
      local_var: 50
    on_enter: on_enter_state1
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    
    EXPECT_EQ(fsm->getVariable("local_var").asInt(), 50);
    
    fsm->setVariable("local_var", VariableValue(75));
    EXPECT_EQ(fsm->getVariable("local_var").asInt(), 75);
    
    fsm->setVariable("global_var", VariableValue(200));
    EXPECT_EQ(fsm->getVariable("global_var").asInt(), 200);
}

TEST_F(IntegrationTest, MultipleObserversReceiveCallbacks) {
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
        TestObserver() : callback_count(0) {}
        
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

TEST_F(IntegrationTest, ResetStateMachine) {
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
    
    class TestObserver : public StateObserver {
        public:
        bool initial_enter_called;
        bool other_enter_called;
        TestObserver() : initial_enter_called(false), other_enter_called(false) {}
        
        void onStateEnter(const std::string& state_name) override {
            (void)state_name;
            if (state_name == "initial_state") {
                initial_enter_called = true;
            } else if (state_name == "other_state") {
                other_enter_called = true;
            }
        }
        
        void onStateExit(const std::string& state_name) override {
            (void)state_name;
        }
        
        void onTransition(const TransitionEvent& event) override {
            (void)event;
        }
        
        void onError(const std::string& error_message) override {
            (void)error_message;
        }
    };
    
    TestObserver observer;
    fsm->registerStateObserver(&observer);
    
    fsm->start();
    EXPECT_TRUE(observer.initial_enter_called);
    EXPECT_FALSE(observer.other_enter_called);
    
    fsm->triggerEvent("move");
    EXPECT_TRUE(observer.other_enter_called);
    
    observer.initial_enter_called = false;
    observer.other_enter_called = false;
    
    fsm->reset();
    fsm->start();
    
    EXPECT_TRUE(observer.initial_enter_called);
    EXPECT_FALSE(observer.other_enter_called);
}

TEST_F(IntegrationTest, ErrorHandlerReceivesErrors) {
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
    
    last_error.clear();
    EXPECT_THROW(fsm->triggerEvent("another_nonexistent_event"), StateException);
    EXPECT_FALSE(last_error.empty());
}

TEST_F(IntegrationTest, ComplexTransitionChain) {
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

TEST_F(IntegrationTest, VariableValueCanBeModified) {
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

TEST_F(IntegrationTest, HasStateReturnsCorrectValue) {
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

TEST_F(IntegrationTest, GetAllStatesReturnsAllStates) {
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

TEST_F(IntegrationTest, TriggerEventWithData) {
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
    
    std::map<std::string, VariableValue> data;
    data["event_data"] = VariableValue(42);
    
    fsm->start();
    fsm->triggerEvent("move", data);
    EXPECT_EQ(fsm->getCurrentState(), "state2");
}

TEST_F(IntegrationTest, CannotTriggerEventWhenNotStarted) {
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

TEST_F(IntegrationTest, CannotStartWhenAlreadyStarted) {
    std::string yaml_content = R"(
states:
  initial_state:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    fsm->start();
    EXPECT_THROW(fsm->start(), StateException);
}

TEST_F(IntegrationTest, CannotStopWhenNotStarted) {
    std::string yaml_content = R"(
states:
  state1:
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_THROW(fsm->stop(), StateException);
}

TEST_F(IntegrationTest, GetVariableThrowsForNonexistentVariable) {
    std::string yaml_content = R"(
variables:
  existing_var: 100
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_THROW(fsm->getVariable("nonexistent_var"), StateException);
}

TEST_F(IntegrationTest, HasVariableReturnsCorrectValue) {
    std::string yaml_content = R"(
variables:
  global_var: 100
)";
    
    writeTestConfig(yaml_content);
    fsm = std::make_unique<StateMachine>(test_config_path);
    
    EXPECT_TRUE(fsm->hasVariable("global_var"));
    EXPECT_FALSE(fsm->hasVariable("nonexistent_var"));
}
