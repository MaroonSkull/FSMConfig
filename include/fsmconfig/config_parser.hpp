#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "types.hpp"

// Forward declaration for yaml-cpp
namespace YAML {
class Node;
}

namespace fsmconfig {

/**
 * @file config_parser.hpp
 * @brief YAML configuration parser for finite state machines
 */

/**
 * @class ConfigParser
 * @brief Class for parsing YAML configurations of finite state machines
 *
 * ConfigParser provides:
 * - Loading YAML files
 * - Parsing state and transition configurations
 * - Validation of configuration structure
 * - Access to loaded data through a convenient interface
 */
class ConfigParser {
 public:
  /**
   * @brief Default constructor
   */
  ConfigParser();

  /**
   * @brief Destructor
   */
  ~ConfigParser();

  // Copy prohibition
  ConfigParser(const ConfigParser&) = delete;
  ConfigParser& operator=(const ConfigParser&) = delete;

  // Move permission
  ConfigParser(ConfigParser&& other) noexcept;
  ConfigParser& operator=(ConfigParser&& other) noexcept;

  /**
   * @brief Load configuration from file
   * @param file_path Path to YAML configuration file
   * @throws ConfigException on load or parse errors
   */
  void loadFromFile(const std::string& file_path);

  /**
   * @brief Load configuration from string
   * @param yaml_content String with YAML content
   * @throws ConfigException on parse errors
   */
  void loadFromString(const std::string& yaml_content);

  /**
   * @brief Get global variables
   * @return Reference to global variables map
   */
  const std::map<std::string, VariableValue>& getGlobalVariables() const;

  /**
   * @brief Get state information
   * @return Reference to states map
   */
  const std::map<std::string, StateInfo>& getStates() const;

  /**
   * @brief Get transition information
   * @return Reference to transitions vector
   */
  const std::vector<TransitionInfo>& getTransitions() const;

  /**
   * @brief Check if state exists
   * @param state_name State name
   * @return true if state exists
   */
  bool hasState(const std::string& state_name) const;

  /**
   * @brief Get information about a specific state
   * @param state_name State name
   * @return Reference to state information
   * @throws ConfigException if state not found
   */
  const StateInfo& getState(const std::string& state_name) const;

  /**
   * @brief Get all transitions from a state
   * @param state_name Source state name
   * @return Vector of transitions from the specified state
   */
  std::vector<TransitionInfo> getTransitionsFrom(const std::string& state_name) const;

  /**
   * @brief Get transition by event
   * @param from_state Source state name
   * @param event_name Event name
   * @return Pointer to transition or nullptr if transition not found
   */
  const TransitionInfo* findTransition(const std::string& from_state, const std::string& event_name) const;

  /**
   * @brief Get initial state
   * @return Initial state name
   */
  std::string getInitialState() const;

  /**
   * @brief Clear loaded configuration
   */
  void clear();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  // Helper methods for parsing
  VariableValue parseVariable(const YAML::Node& node) const;
  StateInfo parseState(const std::string& name, const YAML::Node& node) const;
  TransitionInfo parseTransition(const YAML::Node& node) const;
  void validateConfig() const;

  // Private methods for parsing sections
  void parseGlobalVariables(const YAML::Node& node);
  void parseStates(const YAML::Node& node);
  void parseTransitions(const YAML::Node& node);
};

}  // namespace fsmconfig
