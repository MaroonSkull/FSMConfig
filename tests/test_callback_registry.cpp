#include <gtest/gtest.h>
#include <fsmconfig/callback_registry.hpp>
#include <fsmconfig/types.hpp>

using namespace fsmconfig;

/**
 * @file test_callback_registry.cpp
 * @brief Тесты для CallbackRegistry
 */

class CallbackRegistryTest : public ::testing::Test {
protected:
    std::unique_ptr<CallbackRegistry> registry;
    
    void SetUp() override {
        registry = std::make_unique<CallbackRegistry>();
    }
    
    void TearDown() override {
        registry.reset();
    }
};

TEST_F(CallbackRegistryTest, RegisterStateCallback) {
    bool callback_called = false;
    
    registry->registerStateCallback("state1", "on_enter", [&callback_called]() {
        callback_called = true;
    });
    
    registry->callStateCallback("state1", "on_enter");
    EXPECT_TRUE(callback_called);
}

TEST_F(CallbackRegistryTest, RegisterTransitionCallback) {
    bool callback_called = false;
    TransitionEvent event;
    event.from_state = "state1";
    event.to_state = "state2";
    
    registry->registerTransitionCallback("state1", "state2", [&callback_called](const TransitionEvent& e) {
        (void)e;
        callback_called = true;
    });
    
    registry->callTransitionCallback("state1", "state2", event);
    EXPECT_TRUE(callback_called);
}

TEST_F(CallbackRegistryTest, RegisterGuard) {
    bool guard_called = false;
    
    registry->registerGuard("state1", "state2", "event1", [&guard_called]() -> bool {
        guard_called = true;
        return true;
    });
    
    bool result = registry->callGuard("state1", "state2", "event1");
    EXPECT_TRUE(guard_called);
    EXPECT_TRUE(result);
}

TEST_F(CallbackRegistryTest, RegisterAction) {
    bool action_called = false;
    
    registry->registerAction("action1", [&action_called]() {
        action_called = true;
    });
    
    registry->callAction("action1");
    EXPECT_TRUE(action_called);
}

TEST_F(CallbackRegistryTest, CallMissingCallbackDoesNothing) {
    registry->callStateCallback("nonexistent_state", "on_enter");
    registry->callTransitionCallback("nonexistent_state", "state2", TransitionEvent());
    registry->callGuard("nonexistent_state", "nonexistent_state", "event1");
    registry->callAction("nonexistent_action");
    
    // Should not throw
    SUCCEED();
}

TEST_F(CallbackRegistryTest, GuardReturnsFalseWhenNotFound) {
    bool result = registry->callGuard("nonexistent_state", "nonexistent_state", "event1");
    EXPECT_FALSE(result);
}

TEST_F(CallbackRegistryTest, HasStateCallbackReturnsCorrectValue) {
    registry->registerStateCallback("state1", "on_enter", []() {});
    
    EXPECT_TRUE(registry->hasStateCallback("state1", "on_enter"));
    EXPECT_FALSE(registry->hasStateCallback("state1", "on_exit"));
    EXPECT_FALSE(registry->hasStateCallback("nonexistent_state", "on_enter"));
}

TEST_F(CallbackRegistryTest, HasTransitionCallbackReturnsCorrectValue) {
    registry->registerTransitionCallback("state1", "state2", [](const TransitionEvent& e) {
        (void)e;
    });
    
    EXPECT_TRUE(registry->hasTransitionCallback("state1", "state2"));
    EXPECT_FALSE(registry->hasTransitionCallback("nonexistent_state", "state2"));
}

TEST_F(CallbackRegistryTest, HasGuardReturnsCorrectValue) {
    registry->registerGuard("state1", "state2", "event1", []() -> bool { return true; });
    
    EXPECT_TRUE(registry->hasGuard("state1", "state2", "event1"));
    EXPECT_FALSE(registry->hasGuard("nonexistent_state", "nonexistent_state", "event1"));
}

TEST_F(CallbackRegistryTest, HasActionReturnsCorrectValue) {
    registry->registerAction("action1", []() {});
    
    EXPECT_TRUE(registry->hasAction("action1"));
    EXPECT_FALSE(registry->hasAction("nonexistent_action"));
}

TEST_F(CallbackRegistryTest, GetCallbackCounts) {
    registry->registerStateCallback("state1", "on_enter", []() {});
    registry->registerStateCallback("state2", "on_exit", []() {});
    registry->registerTransitionCallback("state1", "state2", [](const TransitionEvent& e) {
        (void)e;
    });
    registry->registerGuard("state1", "state2", "event1", []() -> bool { return true; });
    registry->registerAction("action1", []() {});
    
    EXPECT_EQ(registry->getStateCallbackCount(), 2);
    EXPECT_EQ(registry->getTransitionCallbackCount(), 1);
    EXPECT_EQ(registry->getGuardCount(), 1);
    EXPECT_EQ(registry->getActionCount(), 1);
}

TEST_F(CallbackRegistryTest, ClearRemovesAllCallbacks) {
    registry->registerStateCallback("state1", "on_enter", []() {});
    registry->registerAction("action1", []() {});
    
    EXPECT_EQ(registry->getStateCallbackCount(), 1);
    EXPECT_EQ(registry->getActionCount(), 1);
    
    registry->clear();
    
    EXPECT_EQ(registry->getStateCallbackCount(), 0);
    EXPECT_EQ(registry->getTransitionCallbackCount(), 0);
    EXPECT_EQ(registry->getGuardCount(), 0);
    EXPECT_EQ(registry->getActionCount(), 0);
}

TEST_F(CallbackRegistryTest, GuardReturnsFalseWhenGuardReturnsFalse) {
    registry->registerGuard("state1", "state2", "event1", []() -> bool {
        return false;
    });
    
    bool result = registry->callGuard("state1", "state2", "event1");
    EXPECT_FALSE(result);
}

TEST_F(CallbackRegistryTest, GuardReturnsTrueWhenGuardReturnsTrue) {
    registry->registerGuard("state1", "state2", "event1", []() -> bool {
        return true;
    });
    
    bool result = registry->callGuard("state1", "state2", "event1");
    EXPECT_TRUE(result);
}

TEST_F(CallbackRegistryTest, MultipleStateCallbacks) {
    bool callback1_called = false;
    bool callback2_called = false;
    
    registry->registerStateCallback("state1", "on_enter", [&callback1_called]() {
        callback1_called = true;
    });
    registry->registerStateCallback("state2", "on_enter", [&callback2_called]() {
        callback2_called = true;
    });
    
    registry->callStateCallback("state1", "on_enter");
    registry->callStateCallback("state2", "on_enter");
    
    EXPECT_TRUE(callback1_called);
    EXPECT_TRUE(callback2_called);
}

TEST_F(CallbackRegistryTest, MultipleTransitionCallbacks) {
    bool callback1_called = false;
    bool callback2_called = false;
    TransitionEvent event1;
    TransitionEvent event2;
    event1.from_state = "state1";
    event1.to_state = "state2";
    event2.from_state = "state2";
    event2.to_state = "state3";
    
    registry->registerTransitionCallback("state1", "state2", [&callback1_called](const TransitionEvent& e) {
        (void)e;
        callback1_called = true;
    });
    registry->registerTransitionCallback("state2", "state3", [&callback2_called](const TransitionEvent& e) {
        (void)e;
        callback2_called = true;
    });
    
    registry->callTransitionCallback("state1", "state2", event1);
    registry->callTransitionCallback("state2", "state3", event2);
    
    EXPECT_TRUE(callback1_called);
    EXPECT_TRUE(callback2_called);
}

TEST_F(CallbackRegistryTest, MultipleActions) {
    bool action1_called = false;
    bool action2_called = false;
    
    registry->registerAction("action1", [&action1_called]() {
        action1_called = true;
    });
    registry->registerAction("action2", [&action2_called]() {
        action2_called = true;
    });
    
    registry->callAction("action1");
    registry->callAction("action2");
    
    EXPECT_TRUE(action1_called);
    EXPECT_TRUE(action2_called);
}

TEST_F(CallbackRegistryTest, OverwriteStateCallback) {
    bool first_called = false;
    bool second_called = false;
    
    registry->registerStateCallback("state1", "on_enter", [&first_called]() {
        first_called = true;
    });
    
    registry->callStateCallback("state1", "on_enter");
    EXPECT_TRUE(first_called);
    EXPECT_FALSE(second_called);
    
    // Overwrite callback
    registry->registerStateCallback("state1", "on_enter", [&second_called]() {
        second_called = true;
    });
    
    first_called = false;
    registry->callStateCallback("state1", "on_enter");
    EXPECT_FALSE(first_called);
    EXPECT_TRUE(second_called);
}

TEST_F(CallbackRegistryTest, OverwriteTransitionCallback) {
    bool first_called = false;
    bool second_called = false;
    TransitionEvent event;
    event.from_state = "state1";
    event.to_state = "state2";
    
    registry->registerTransitionCallback("state1", "state2", [&first_called](const TransitionEvent& e) {
        (void)e;
        first_called = true;
    });
    
    registry->callTransitionCallback("state1", "state2", event);
    EXPECT_TRUE(first_called);
    EXPECT_FALSE(second_called);
    
    // Overwrite callback
    registry->registerTransitionCallback("state1", "state2", [&second_called](const TransitionEvent& e) {
        (void)e;
        second_called = true;
    });
    
    first_called = false;
    registry->callTransitionCallback("state1", "state2", event);
    EXPECT_FALSE(first_called);
    EXPECT_TRUE(second_called);
}

TEST_F(CallbackRegistryTest, OverwriteGuard) {
    bool first_called = false;
    bool second_called = false;
    
    registry->registerGuard("state1", "state2", "event1", [&first_called]() -> bool {
        first_called = true;
        return true;
    });
    
    bool result1 = registry->callGuard("state1", "state2", "event1");
    EXPECT_TRUE(first_called);
    EXPECT_TRUE(result1);
    
    // Overwrite guard
    registry->registerGuard("state1", "state2", "event1", [&second_called]() -> bool {
        second_called = true;
        return false;
    });
    
    first_called = false;
    bool result2 = registry->callGuard("state1", "state2", "event1");
    EXPECT_FALSE(first_called);
    EXPECT_TRUE(second_called);
    EXPECT_FALSE(result2);
}

TEST_F(CallbackRegistryTest, OverwriteAction) {
    bool first_called = false;
    bool second_called = false;
    
    registry->registerAction("action1", [&first_called]() {
        first_called = true;
    });
    
    registry->callAction("action1");
    EXPECT_TRUE(first_called);
    EXPECT_FALSE(second_called);
    
    // Overwrite action
    registry->registerAction("action1", [&second_called]() {
        second_called = true;
    });
    
    first_called = false;
    registry->callAction("action1");
    EXPECT_FALSE(first_called);
    EXPECT_TRUE(second_called);
}

TEST_F(CallbackRegistryTest, MultipleGuards) {
    bool guard1_called = false;
    bool guard2_called = false;
    
    registry->registerGuard("state1", "state2", "event1", [&guard1_called]() -> bool {
        guard1_called = true;
        return true;
    });
    registry->registerGuard("state1", "state3", "event2", [&guard2_called]() -> bool {
        guard2_called = true;
        return false;
    });
    
    bool result1 = registry->callGuard("state1", "state2", "event1");
    bool result2 = registry->callGuard("state1", "state3", "event2");
    
    EXPECT_TRUE(guard1_called);
    EXPECT_TRUE(guard2_called);
    EXPECT_TRUE(result1);
    EXPECT_FALSE(result2);
}
