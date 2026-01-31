#include "fsmconfig/types.hpp"

#include <sstream>
#include <stdexcept>

namespace fsmconfig {

// ============================================================================
// VariableValue implementation
// ============================================================================

VariableValue::VariableValue() : type(VariableType::INT), int_value(0) {}

VariableValue::VariableValue(int v) : type(VariableType::INT), int_value(v) {}

VariableValue::VariableValue(float v) : type(VariableType::FLOAT), float_value(v) {}

VariableValue::VariableValue(const std::string& v) : type(VariableType::STRING) {
    new (&string_value) std::string(v);
}

VariableValue::VariableValue(bool v) : type(VariableType::BOOL), bool_value(v) {}

VariableValue::~VariableValue() {
    if (type == VariableType::STRING) {
        string_value.~basic_string();
    }
}

VariableValue::VariableValue(const VariableValue& other) : type(other.type) {
    switch (type) {
        case VariableType::INT:
            int_value = other.int_value;
            break;
        case VariableType::FLOAT:
            float_value = other.float_value;
            break;
        case VariableType::STRING:
            new (&string_value) std::string(other.string_value);
            break;
        case VariableType::BOOL:
            bool_value = other.bool_value;
            break;
    }
}

VariableValue& VariableValue::operator=(const VariableValue& other) {
    if (this != &other) {
        // Сначала уничтожаем старое значение если это STRING
        if (type == VariableType::STRING) {
            string_value.~basic_string();
        }

        // Копируем тип
        type = other.type;

        // Копируем значение
        switch (type) {
            case VariableType::INT:
                int_value = other.int_value;
                break;
            case VariableType::FLOAT:
                float_value = other.float_value;
                break;
            case VariableType::STRING:
                new (&string_value) std::string(other.string_value);
                break;
            case VariableType::BOOL:
                bool_value = other.bool_value;
                break;
        }
    }
    return *this;
}

int VariableValue::asInt() const {
    if (type != VariableType::INT) {
        throw std::bad_cast();
    }
    return int_value;
}

float VariableValue::asFloat() const {
    if (type != VariableType::FLOAT) {
        throw std::bad_cast();
    }
    return float_value;
}

std::string VariableValue::asString() const {
    if (type != VariableType::STRING) {
        throw std::bad_cast();
    }
    return string_value;
}

bool VariableValue::asBool() const {
    if (type != VariableType::BOOL) {
        throw std::bad_cast();
    }
    return bool_value;
}

std::string VariableValue::toString() const {
    switch (type) {
        case VariableType::INT:
            return std::to_string(int_value);
        case VariableType::FLOAT:
            return std::to_string(float_value);
        case VariableType::STRING:
            return string_value;
        case VariableType::BOOL:
            return bool_value ? "true" : "false";
        default:
            return "unknown";
    }
}

// ============================================================================
// TransitionEvent implementation
// ============================================================================

TransitionEvent::TransitionEvent() : timestamp(std::chrono::system_clock::now()) {}

// ============================================================================
// StateInfo implementation
// ============================================================================

StateInfo::StateInfo() = default;

StateInfo::StateInfo(const std::string& name) : name(name) {}

// ============================================================================
// TransitionInfo implementation
// ============================================================================

TransitionInfo::TransitionInfo() = default;

// ============================================================================
// ConfigException implementation
// ============================================================================

ConfigException::ConfigException(const std::string& message) : std::runtime_error(message) {}

// ============================================================================
// StateException implementation
// ============================================================================

StateException::StateException(const std::string& message) : std::runtime_error(message) {}

}  // namespace fsmconfig
