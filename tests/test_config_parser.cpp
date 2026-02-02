#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <fsmconfig/config_parser.hpp>
#include <fsmconfig/types.hpp>
#include <memory>
#include <string>
#include <tuple>

using namespace fsmconfig;

/**
 * @file test_config_parser.cpp
 * @brief Tests for ConfigParser
 */

class ConfigParserTest : public ::testing::Test {
 protected:
  std::unique_ptr<ConfigParser> parser;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes) - Test fixture requires protected access
  std::string test_config_path;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes) - Test fixture requires protected access

  void SetUp() override {
    parser = std::make_unique<ConfigParser>();
    auto temp_dir = std::filesystem::temp_directory_path();
    test_config_path = (temp_dir / "fsmconfig_parser_test.yaml").string();
  }

  void TearDown() override {
    parser.reset();
    std::remove(test_config_path.c_str());
  }

  void writeTestConfig(const std::string& content) {
    std::ofstream file(test_config_path);
    file << content;
    file.close();
  }
};

TEST_F(ConfigParserTest, LoadValidConfig) {
  const std::string yaml_content = R"(
variables:
  timeout: 5.0
  retry_count: 3

states:
  state1:
    variables:
      local_var: 42
    on_enter: on_enter_callback
    on_exit: on_exit_callback
    actions:
      - action1
      - action2

  state2:
    on_enter: on_enter_callback
    on_exit: on_exit_callback

transitions:
  - from: state1
    to: state2
    event: event1
    guard: guard_callback
    on_transition: transition_callback
    actions:
      - action3
  )";

  writeTestConfig(yaml_content);

  ASSERT_NO_THROW(parser->loadFromFile(test_config_path));

  EXPECT_EQ(parser->getGlobalVariables().size(), 2);
  EXPECT_EQ(parser->getStates().size(), 2);
  EXPECT_EQ(parser->getTransitions().size(), 1);
}

TEST_F(ConfigParserTest, LoadFromValidString) {
const   std::string yaml_content = R"(
variables:
  test_var: 100

states:
  test_state:
    on_enter: enter_cb
)";

  ASSERT_NO_THROW(parser->loadFromString(yaml_content));

  EXPECT_TRUE(parser->hasState("test_state"));
  EXPECT_FALSE(parser->hasState("nonexistent_state"));
}

TEST_F(ConfigParserTest, ParseVariableTypes) {
const   std::string yaml_content = R"(
variables:
  int_var: 42
  float_var: 3.14
  string_var: "hello"
  bool_var: true
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();

  EXPECT_EQ(vars["int_var"].type, VariableType::INT);
  EXPECT_EQ(vars["int_var"].asInt(), 42);

  EXPECT_EQ(vars["float_var"].type, VariableType::FLOAT);
  EXPECT_NEAR(vars["float_var"].asFloat(), 3.14, 0.001);

  EXPECT_EQ(vars["string_var"].type, VariableType::STRING);
  EXPECT_EQ(vars["string_var"].asString(), "hello");

  EXPECT_EQ(vars["bool_var"].type, VariableType::BOOL);
  EXPECT_TRUE(vars["bool_var"].asBool());
}

TEST_F(ConfigParserTest, ParseStateVariables) {
const   std::string yaml_content = R"(
states:
  state1:
    variables:
      local_var: 100
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto state = parser->getState("state1");
  EXPECT_EQ(state.variables.size(), 1);
  EXPECT_EQ(state.variables["local_var"].asInt(), 100);
}

TEST_F(ConfigParserTest, ParseStateCallbacks) {
const   std::string yaml_content = R"(
states:
  state1:
    on_enter: on_enter_cb
    on_exit: on_exit_cb
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto state = parser->getState("state1");
  EXPECT_EQ(state.on_enter_callback, "on_enter_cb");
  EXPECT_EQ(state.on_exit_callback, "on_exit_cb");
}

TEST_F(ConfigParserTest, ParseStateActions) {
const   std::string yaml_content = R"(
states:
  state1:
    actions:
      - action1
      - action2
      - action3
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto state = parser->getState("state1");
  EXPECT_EQ(state.actions.size(), 3);
  EXPECT_EQ(state.actions[0], "action1");
  EXPECT_EQ(state.actions[1], "action2");
  EXPECT_EQ(state.actions[2], "action3");
}

TEST_F(ConfigParserTest, ParseTransitions) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: event1
    guard: guard_cb
    on_transition: transition_cb
    actions:
      - action1
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitions();
  ASSERT_EQ(transitions.size(), 1);

  EXPECT_EQ(transitions[0].from_state, "state1");
  EXPECT_EQ(transitions[0].to_state, "state2");
  EXPECT_EQ(transitions[0].event_name, "event1");
  EXPECT_EQ(transitions[0].guard_callback, "guard_cb");
  EXPECT_EQ(transitions[0].transition_callback, "transition_cb");
  EXPECT_EQ(transitions[0].actions.size(), 1);
  EXPECT_EQ(transitions[0].actions[0], "action1");
}

TEST_F(ConfigParserTest, GetTransitionsFromState) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:
  state3:

transitions:
  - from: state1
    to: state2
    event: event1
  - from: state1
    to: state3
    event: event2
  - from: state2
    to: state3
    event: event3
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitionsFrom("state1");
  EXPECT_EQ(transitions.size(), 2);
  EXPECT_EQ(transitions[0].to_state, "state2");
  EXPECT_EQ(transitions[1].to_state, "state3");
}

TEST_F(ConfigParserTest, FindTransition) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: event1
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  const auto* transition = parser->findTransition("state1", "event1");
  ASSERT_NE(transition, nullptr);
  EXPECT_EQ(transition->from_state, "state1");
  EXPECT_EQ(transition->to_state, "state2");
  EXPECT_EQ(transition->event_name, "event1");

  const auto* nonexistent = parser->findTransition("state1", "nonexistent_event");
  EXPECT_EQ(nonexistent, nullptr);
}

TEST_F(ConfigParserTest, ClearConfiguration) {
const   std::string yaml_content = R"(
variables:
  test_var: 100

states:
  state1:
    on_enter: enter_cb
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_FALSE(parser->getGlobalVariables().empty());
  EXPECT_FALSE(parser->getStates().empty());

  parser->clear();

  EXPECT_TRUE(parser->getGlobalVariables().empty());
  EXPECT_TRUE(parser->getStates().empty());
  EXPECT_TRUE(parser->getTransitions().empty());
}

TEST_F(ConfigParserTest, InvalidConfigThrowsException) {
const   std::string invalid_yaml = R"(
invalid yaml content
  not valid
)";

  writeTestConfig(invalid_yaml);

  EXPECT_THROW(parser->loadFromFile(test_config_path), ConfigException);
}

TEST_F(ConfigParserTest, MissingRequiredFieldThrowsException) {
const   std::string invalid_yaml = R"(
transitions:
  - from: nonexistent_state
    to: state2
    event: event1
)";

  writeTestConfig(invalid_yaml);

  EXPECT_THROW(parser->loadFromFile(test_config_path), ConfigException);
}

TEST_F(ConfigParserTest, DuplicateTransitionThrowsException) {
const   std::string invalid_yaml = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: event1
  - from: state1
    to: state2
    event: event1
)";

  writeTestConfig(invalid_yaml);

  EXPECT_THROW(parser->loadFromFile(test_config_path), ConfigException);
}

TEST_F(ConfigParserTest, EmptyConfig) {
const   std::string yaml_content = R"(
)";

  writeTestConfig(yaml_content);

  ASSERT_NO_THROW(parser->loadFromFile(test_config_path));
  EXPECT_TRUE(parser->getGlobalVariables().empty());
  EXPECT_TRUE(parser->getStates().empty());
  EXPECT_TRUE(parser->getTransitions().empty());
}

TEST_F(ConfigParserTest, ConfigWithOnlyVariables) {
const   std::string yaml_content = R"(
variables:
  var1: 10
  var2: 20
)";

  writeTestConfig(yaml_content);

  ASSERT_NO_THROW(parser->loadFromFile(test_config_path));
  EXPECT_EQ(parser->getGlobalVariables().size(), 2);
  EXPECT_TRUE(parser->getStates().empty());
}

TEST_F(ConfigParserTest, ConfigWithOnlyStates) {
const   std::string yaml_content = R"(
states:
  state1:
    on_enter: enter1
  state2:
    on_exit: exit2
)";

  writeTestConfig(yaml_content);

  ASSERT_NO_THROW(parser->loadFromFile(test_config_path));
  EXPECT_EQ(parser->getStates().size(), 2);
  EXPECT_TRUE(parser->getTransitions().empty());
}

TEST_F(ConfigParserTest, ConfigWithOnlyTransitions) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
)";

  writeTestConfig(yaml_content);

  ASSERT_NO_THROW(parser->loadFromFile(test_config_path));
  EXPECT_EQ(parser->getTransitions().size(), 1);
}

TEST_F(ConfigParserTest, GetNonexistentStateThrowsException) {
const   std::string yaml_content = R"(
states:
  state1:
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_THROW([[maybe_unused]] auto _ = parser->getState("nonexistent_state"), ConfigException);
}

TEST_F(ConfigParserTest, GetTransitionsFromNonexistentState) {
const   std::string yaml_content = R"(
states:
  state1:
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitionsFrom("nonexistent_state");
  EXPECT_TRUE(transitions.empty());
}

TEST_F(ConfigParserTest, ParseNegativeIntVariable) {
const   std::string yaml_content = R"(
variables:
  negative_var: -42
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["negative_var"].asInt(), -42);
}

TEST_F(ConfigParserTest, ParseZeroIntVariable) {
const   std::string yaml_content = R"(
variables:
  zero_var: 0
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["zero_var"].asInt(), 0);
}

TEST_F(ConfigParserTest, ParseNegativeFloatVariable) {
const   std::string yaml_content = R"(
variables:
  negative_float: -3.14
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_NEAR(vars["negative_float"].asFloat(), -3.14, 0.001);
}

TEST_F(ConfigParserTest, ParseFalseBoolVariable) {
const   std::string yaml_content = R"(
variables:
  false_var: false
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_FALSE(vars["false_var"].asBool());
}

TEST_F(ConfigParserTest, ParseEmptyStringVariable) {
const   std::string yaml_content = R"(
variables:
  empty_string: ""
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["empty_string"].asString(), "");
}

TEST_F(ConfigParserTest, ParseComplexStateWithAllFields) {
const   std::string yaml_content = R"(
states:
  complex_state:
    variables:
      var1: 10
      var2: "test"
    on_enter: enter_cb
    on_exit: exit_cb
    actions:
      - action1
      - action2
      - action3
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto state = parser->getState("complex_state");
  EXPECT_EQ(state.variables.size(), 2);
  EXPECT_EQ(state.on_enter_callback, "enter_cb");
  EXPECT_EQ(state.on_exit_callback, "exit_cb");
  EXPECT_EQ(state.actions.size(), 3);
}

TEST_F(ConfigParserTest, ParseMultipleTransitionsFromSameState) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:
  state3:
  state4:

transitions:
  - from: state1
    to: state2
    event: event1
  - from: state1
    to: state3
    event: event2
  - from: state1
    to: state4
    event: event3
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitionsFrom("state1");
  EXPECT_EQ(transitions.size(), 3);
}

TEST_F(ConfigParserTest, ParseTransitionWithoutOptionalFields) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitions();
  ASSERT_EQ(transitions.size(), 1);

  EXPECT_TRUE(transitions[0].guard_callback.empty());
  EXPECT_TRUE(transitions[0].transition_callback.empty());
  EXPECT_TRUE(transitions[0].actions.empty());
}

TEST_F(ConfigParserTest, LoadMultipleConfigsSequentially) {
const   std::string yaml_content1 = R"(
variables:
  var1: 10
states:
  state1:
)";

const   std::string yaml_content2 = R"(
variables:
  var2: 20
states:
  state2:
)";

  writeTestConfig(yaml_content1);
  parser->loadFromFile(test_config_path);

  EXPECT_EQ(parser->getGlobalVariables().size(), 1);
  EXPECT_EQ(parser->getStates().size(), 1);

  parser->clear();

  writeTestConfig(yaml_content2);
  parser->loadFromFile(test_config_path);

  EXPECT_EQ(parser->getGlobalVariables().size(), 1);
  EXPECT_EQ(parser->getStates().size(), 1);
  EXPECT_TRUE(parser->hasState("state2"));
}

TEST_F(ConfigParserTest, ParseLargeNumberOfVariables) {
  std::string yaml_content = "variables:\n";
  for (int i = 0; i < 100; ++i) {
    yaml_content += "  var" + std::to_string(i) + ": " + std::to_string(i) + "\n";
  }

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_EQ(parser->getGlobalVariables().size(), 100);
}

TEST_F(ConfigParserTest, ParseLargeNumberOfStates) {
  std::string yaml_content = "states:\n";
  for (int i = 0; i < 50; ++i) {
    yaml_content += "  state" + std::to_string(i) + ":\n";
  }

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_EQ(parser->getStates().size(), 50);
}

TEST_F(ConfigParserTest, ParseLargeNumberOfTransitions) {
  std::string yaml_content = "states:\n";
  for (int i = 0; i < 20; ++i) {
    yaml_content += "  state" + std::to_string(i) + ":\n";
  }
  yaml_content += "transitions:\n";
  for (int i = 0; i < 19; ++i) {
    yaml_content += "  - from: state" + std::to_string(i) + "\n";
    yaml_content += "    to: state" + std::to_string(i + 1) + "\n";
    yaml_content += "    event: move" + std::to_string(i) + "\n";
  }

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_EQ(parser->getTransitions().size(), 19);
}

TEST_F(ConfigParserTest, HasStateReturnsTrueForExistingState) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_TRUE(parser->hasState("state1"));
  EXPECT_TRUE(parser->hasState("state2"));
}

TEST_F(ConfigParserTest, HasStateReturnsFalseForNonexistentState) {
const   std::string yaml_content = R"(
states:
  state1:
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  EXPECT_FALSE(parser->hasState("nonexistent_state"));
}

TEST_F(ConfigParserTest, ParseTransitionWithMultipleActions) {
const   std::string yaml_content = R"(
states:
  state1:
  state2:

transitions:
  - from: state1
    to: state2
    event: move
    actions:
      - action1
      - action2
      - action3
      - action4
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto transitions = parser->getTransitions();
  ASSERT_EQ(transitions.size(), 1);
  EXPECT_EQ(transitions[0].actions.size(), 4);
}

TEST_F(ConfigParserTest, ParseStateWithoutOptionalFields) {
const   std::string yaml_content = R"(
states:
  minimal_state:
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto state = parser->getState("minimal_state");
  EXPECT_TRUE(state.variables.empty());
  EXPECT_TRUE(state.on_enter_callback.empty());
  EXPECT_TRUE(state.on_exit_callback.empty());
  EXPECT_TRUE(state.actions.empty());
}

TEST_F(ConfigParserTest, ParseFloatWithHighPrecision) {
const   std::string yaml_content = R"(
variables:
  precise_float: 3.141592653589793
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_NEAR(vars["precise_float"].asFloat(), 3.141592653589793, 0.0001);
}

TEST_F(ConfigParserTest, ParseStringWithSpaces) {
const   std::string yaml_content = R"(
variables:
  spaced_string: "hello world with spaces"
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["spaced_string"].asString(), "hello world with spaces");
}

TEST_F(ConfigParserTest, ParseStringWithSpecialCharacters) {
const   std::string yaml_content = R"(
variables:
  special_string: "test123"
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["special_string"].asString(), "test123");
}

TEST_F(ConfigParserTest, ParseZeroFloatVariable) {
const   std::string yaml_content = R"(
variables:
  zero_float: 0.0
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_NEAR(vars["zero_float"].asFloat(), 0.0, 0.001);
}

TEST_F(ConfigParserTest, ParseVeryLargeIntVariable) {
const   std::string yaml_content = R"(
variables:
  large_int: 2147483647
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["large_int"].asInt(), 2147483647);
}

TEST_F(ConfigParserTest, ParseVerySmallIntVariable) {
const   std::string yaml_content = R"(
variables:
  small_int: -2147483648
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_EQ(vars["small_int"].asInt(), -2147483648);
}

TEST_F(ConfigParserTest, ParseScientificNotationFloat) {
const   std::string yaml_content = R"(
variables:
  scientific_float: 1.5e2
)";

  writeTestConfig(yaml_content);
  parser->loadFromFile(test_config_path);

  auto vars = parser->getGlobalVariables();
  EXPECT_NEAR(vars["scientific_float"].asFloat(), 150.0, 0.1);
}
