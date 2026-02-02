/**
 * @file test_state.cpp
 * @brief Tests for State class
 */

#include <gtest/gtest.h>
#include "fsmconfig/state.hpp"
#include "fsmconfig/types.hpp"

using namespace fsmconfig;

/// Tests state construction with StateInfo
TEST(StateTest, ConstructionWithStateInfo) {
    StateInfo info;
    info.name = "idle";
    info.on_enter_callback = "on_idle_enter";
    info.on_exit_callback = "on_idle_exit";
    info.actions.push_back("log_idle");
    info.variables["count"] = VariableValue(0);

    State state(info);

    EXPECT_EQ(state.getName(), "idle");
    EXPECT_EQ(state.getOnEnterCallback(), "on_idle_enter");
    EXPECT_EQ(state.getOnExitCallback(), "on_idle_exit");
    EXPECT_EQ(state.getActions().size(), 1);
    EXPECT_EQ(state.getActions()[0], "log_idle");
    EXPECT_EQ(state.getVariables().size(), 1);
}

/// Tests state construction with name only
TEST(StateTest, ConstructionWithName) {
    StateInfo info("active");
    State state(info);

    EXPECT_EQ(state.getName(), "active");
    EXPECT_TRUE(state.getOnEnterCallback().empty());
    EXPECT_TRUE(state.getOnExitCallback().empty());
    EXPECT_TRUE(state.getActions().empty());
    EXPECT_TRUE(state.getVariables().empty());
}

/// Tests move construction
TEST(StateTest, MoveConstruction) {
    StateInfo info;
    info.name = "test_state";
    info.variables["key1"] = VariableValue(100);
    info.actions.push_back("action1");

    State original(info);
    EXPECT_EQ(original.getName(), "test_state");
    EXPECT_EQ(original.getVariables().size(), 1);

    State moved(std::move(original));

    EXPECT_EQ(moved.getName(), "test_state");
    EXPECT_EQ(moved.getVariables().size(), 1);
    EXPECT_EQ(moved.getActions().size(), 1);
}

/// Tests move assignment
TEST(StateTest, MoveAssignment) {
    StateInfo info1;
    info1.name = "state1";
    info1.variables["key1"] = VariableValue(100);

    StateInfo info2;
    info2.name = "state2";
    info2.variables["key2"] = VariableValue(200);

    State state1(info1);
    State state2(info2);

    state2 = std::move(state1);

    EXPECT_EQ(state2.getName(), "state1");
    EXPECT_EQ(state2.getVariables().size(), 1);
    EXPECT_EQ(state2.getVariables().at("key1").asInt(), 100);
}

/// Tests getName returns correct name
TEST(StateTest, GetName) {
    StateInfo info("my_state");
    State state(info);

    EXPECT_EQ(state.getName(), "my_state");
}

/// Tests getOnEnterCallback
TEST(StateTest, GetOnEnterCallback) {
    StateInfo info;
    info.name = "test";
    info.on_enter_callback = "enter_handler";

    State state(info);

    EXPECT_EQ(state.getOnEnterCallback(), "enter_handler");
}

/// Tests getOnExitCallback
TEST(StateTest, GetOnExitCallback) {
    StateInfo info;
    info.name = "test";
    info.on_exit_callback = "exit_handler";

    State state(info);

    EXPECT_EQ(state.getOnExitCallback(), "exit_handler");
}

/// Tests getActions returns correct actions
TEST(StateTest, GetActions) {
    StateInfo info;
    info.name = "test";
    info.actions.push_back("action1");
    info.actions.push_back("action2");
    info.actions.push_back("action3");

    State state(info);

    const auto& actions = state.getActions();
    EXPECT_EQ(actions.size(), 3);
    EXPECT_EQ(actions[0], "action1");
    EXPECT_EQ(actions[1], "action2");
    EXPECT_EQ(actions[2], "action3");
}

/// Tests getVariables returns correct variables
TEST(StateTest, GetVariables) {
    StateInfo info;
    info.name = "test";
    info.variables["int_var"] = VariableValue(42);
    info.variables["float_var"] = VariableValue(3.14f);
    info.variables["string_var"] = VariableValue(std::string("hello"));
    info.variables["bool_var"] = VariableValue(true);

    State state(info);

    const auto& vars = state.getVariables();
    EXPECT_EQ(vars.size(), 4);
    EXPECT_EQ(vars.at("int_var").asInt(), 42);
    EXPECT_FLOAT_EQ(vars.at("float_var").asFloat(), 3.14f);
    EXPECT_EQ(vars.at("string_var").asString(), "hello");
    EXPECT_TRUE(vars.at("bool_var").asBool());
}

/// Tests hasVariable with existing variable
TEST(StateTest, HasVariable_Existing) {
    StateInfo info;
    info.name = "test";
    info.variables["my_var"] = VariableValue(100);

    State state(info);

    EXPECT_TRUE(state.hasVariable("my_var"));
}

/// Tests hasVariable with non-existing variable
TEST(StateTest, HasVariable_NonExisting) {
    StateInfo info("test");
    State state(info);

    EXPECT_FALSE(state.hasVariable("nonexistent"));
}

/// Tests getVariable with existing variable
TEST(StateTest, GetVariable_Existing) {
    StateInfo info;
    info.name = "test";
    info.variables["int_val"] = VariableValue(42);
    info.variables["string_val"] = VariableValue(std::string("test"));

    State state(info);

    EXPECT_EQ(state.getVariable("int_val").asInt(), 42);
    EXPECT_EQ(state.getVariable("string_val").asString(), "test");
}

/// Tests getVariable with non-existing variable throws exception
TEST(StateTest, GetVariable_NonExisting) {
    StateInfo info("test");
    State state(info);

    EXPECT_THROW((void)state.getVariable("nonexistent"), StateException);
}

/// Tests setVariable updates existing variable
TEST(StateTest, SetVariable_UpdateExisting) {
    StateInfo info;
    info.name = "test";
    info.variables["counter"] = VariableValue(10);

    State state(info);

    EXPECT_EQ(state.getVariable("counter").asInt(), 10);

    state.setVariable("counter", VariableValue(20));

    EXPECT_EQ(state.getVariable("counter").asInt(), 20);
}

/// Tests setVariable adds new variable
TEST(StateTest, SetVariable_AddNew) {
    StateInfo info("test");
    State state(info);

    EXPECT_FALSE(state.hasVariable("new_var"));

    state.setVariable("new_var", VariableValue(100));

    EXPECT_TRUE(state.hasVariable("new_var"));
    EXPECT_EQ(state.getVariable("new_var").asInt(), 100);
}

/// Tests getAllVariables returns all variables
TEST(StateTest, GetAllVariables) {
    StateInfo info;
    info.name = "test";
    info.variables["var1"] = VariableValue(1);
    info.variables["var2"] = VariableValue(2);
    info.variables["var3"] = VariableValue(3);

    State state(info);

    const auto& vars = state.getAllVariables();
    EXPECT_EQ(vars.size(), 3);
    EXPECT_EQ(vars.at("var1").asInt(), 1);
    EXPECT_EQ(vars.at("var2").asInt(), 2);
    EXPECT_EQ(vars.at("var3").asInt(), 3);
}

/// Tests state with empty callbacks
TEST(StateTest, EmptyCallbacks) {
    StateInfo info;
    info.name = "test";

    State state(info);

    EXPECT_TRUE(state.getOnEnterCallback().empty());
    EXPECT_TRUE(state.getOnExitCallback().empty());
}

/// Tests state with empty actions
TEST(StateTest, EmptyActions) {
    StateInfo info("test");
    State state(info);

    EXPECT_TRUE(state.getActions().empty());
}

/// Tests state with empty variables
TEST(StateTest, EmptyVariables) {
    StateInfo info("test");
    State state(info);

    EXPECT_TRUE(state.getVariables().empty());
    EXPECT_FALSE(state.hasVariable("anything"));
}

/// Tests state with all types of variables
TEST(StateTest, AllVariableTypes) {
    StateInfo info;
    info.name = "test";
    info.variables["int_var"] = VariableValue(-123);
    info.variables["float_var"] = VariableValue(3.14159f);
    info.variables["string_var"] = VariableValue(std::string("hello world"));
    info.variables["bool_var"] = VariableValue(false);

    State state(info);

    EXPECT_EQ(state.getVariable("int_var").asInt(), -123);
    EXPECT_FLOAT_EQ(state.getVariable("float_var").asFloat(), 3.14159f);
    EXPECT_EQ(state.getVariable("string_var").asString(), "hello world");
    EXPECT_FALSE(state.getVariable("bool_var").asBool());
}

/// Tests multiple actions
TEST(StateTest, MultipleActions) {
    StateInfo info;
    info.name = "test";
    info.actions.push_back("action1");
    info.actions.push_back("action2");
    info.actions.push_back("action3");
    info.actions.push_back("action4");
    info.actions.push_back("action5");

    State state(info);

    const auto& actions = state.getActions();
    EXPECT_EQ(actions.size(), 5);
    for (size_t i = 0; i < actions.size(); ++i) {
        EXPECT_EQ(actions[i], "action" + std::to_string(i + 1));
    }
}

/// Tests variable modification affects state
TEST(StateTest, VariableModification) {
    StateInfo info;
    info.name = "test";
    info.variables["counter"] = VariableValue(0);

    State state(info);

    EXPECT_EQ(state.getVariable("counter").asInt(), 0);

    state.setVariable("counter", VariableValue(10));
    EXPECT_EQ(state.getVariable("counter").asInt(), 10);

    state.setVariable("counter", VariableValue(100));
    EXPECT_EQ(state.getVariable("counter").asInt(), 100);
}

/// Tests state with complex variable names
TEST(StateTest, ComplexVariableNames) {
    StateInfo info;
    info.name = "test";
    info.variables["var_with_underscore"] = VariableValue(1);
    info.variables["var-with-dash"] = VariableValue(2);
    info.variables["var.with.dot"] = VariableValue(3);
    info.variables["var::with::colons"] = VariableValue(4);

    State state(info);

    EXPECT_TRUE(state.hasVariable("var_with_underscore"));
    EXPECT_TRUE(state.hasVariable("var-with-dash"));
    EXPECT_TRUE(state.hasVariable("var.with.dot"));
    EXPECT_TRUE(state.hasVariable("var::with::colons"));
}

/// Tests state with special characters in name
TEST(StateTest, SpecialCharactersInName) {
    StateInfo info;
    info.name = "state_with_special_chars_123";

    State state(info);

    EXPECT_EQ(state.getName(), "state_with_special_chars_123");
}

/// Tests empty state name
TEST(StateTest, EmptyStateName) {
    StateInfo info("");
    State state(info);

    EXPECT_TRUE(state.getName().empty());
}

/// Tests variable type changes
TEST(StateTest, VariableTypeChange) {
    StateInfo info;
    info.name = "test";
    info.variables["my_var"] = VariableValue(42);

    State state(info);

    EXPECT_EQ(state.getVariable("my_var").asInt(), 42);
    EXPECT_EQ(state.getVariable("my_var").toString(), "42");

    // Change to string
    state.setVariable("my_var", VariableValue(std::string("hello")));
    EXPECT_EQ(state.getVariable("my_var").asString(), "hello");
    EXPECT_EQ(state.getVariable("my_var").toString(), "hello");

    // Change to bool
    state.setVariable("my_var", VariableValue(true));
    EXPECT_TRUE(state.getVariable("my_var").asBool());
    EXPECT_EQ(state.getVariable("my_var").toString(), "true");
}

/// Tests state with many variables
TEST(StateTest, ManyVariables) {
    StateInfo info;
    info.name = "test";

    const int num_vars = 100;
    for (int i = 0; i < num_vars; ++i) {
        info.variables["var_" + std::to_string(i)] = VariableValue(i);
    }

    State state(info);

    EXPECT_EQ(state.getVariables().size(), num_vars);

    for (int i = 0; i < num_vars; ++i) {
        std::string var_name = "var_" + std::to_string(i);
        EXPECT_TRUE(state.hasVariable(var_name));
        EXPECT_EQ(state.getVariable(var_name).asInt(), i);
    }
}

/// Tests state with many actions
TEST(StateTest, ManyActions) {
    StateInfo info;
    info.name = "test";

    const int num_actions = 50;
    for (int i = 0; i < num_actions; ++i) {
        info.actions.push_back("action_" + std::to_string(i));
    }

    State state(info);

    EXPECT_EQ(state.getActions().size(), num_actions);

    const auto& actions = state.getActions();
    for (int i = 0; i < num_actions; ++i) {
        EXPECT_EQ(actions[i], "action_" + std::to_string(i));
    }
}

/// Tests getVariables returns const reference
TEST(StateTest, GetVariablesReturnsConstReference) {
    StateInfo info;
    info.name = "test";
    info.variables["key"] = VariableValue(42);

    State state(info);

    const auto& vars1 = state.getVariables();
    const auto& vars2 = state.getVariables();

    // Should return the same reference
    EXPECT_EQ(&vars1, &vars2);
}

/// Tests getAllVariables returns const reference
TEST(StateTest, GetAllVariablesReturnsConstReference) {
    StateInfo info;
    info.name = "test";
    info.variables["key"] = VariableValue(42);

    State state(info);

    const auto& vars1 = state.getAllVariables();
    const auto& vars2 = state.getAllVariables();

    // Should return the same reference
    EXPECT_EQ(&vars1, &vars2);
}

/// Tests getActions returns const reference
TEST(StateTest, GetActionsReturnsConstReference) {
    StateInfo info;
    info.name = "test";
    info.actions.push_back("action1");

    State state(info);

    const auto& actions1 = state.getActions();
    const auto& actions2 = state.getActions();

    // Should return the same reference
    EXPECT_EQ(&actions1, &actions2);
}
