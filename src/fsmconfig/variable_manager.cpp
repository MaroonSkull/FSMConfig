#include "fsmconfig/variable_manager.hpp"
#include <algorithm>

namespace fsmconfig {

/**
 * @brief Реализация VariableManager (Pimpl идиома)
 */
class VariableManager::Impl {
public:
    /// Глобальные переменные
    std::map<std::string, VariableValue> global_variables;
    
    /// Локальные переменные состояний: key = state_name
    std::map<std::string, std::map<std::string, VariableValue>> state_variables;
    
    /// Мьютекс для потокобезопасности
    mutable std::mutex mutex;
    
    /**
     * @brief Очистка всех переменных
     */
    void clear() {
        global_variables.clear();
        state_variables.clear();
    }
};

// ============================================================================
// Конструкторы и деструктор
// ============================================================================

VariableManager::VariableManager() 
    : impl_(std::make_unique<Impl>()) {
}

VariableManager::~VariableManager() = default;

VariableManager::VariableManager(VariableManager&& other) noexcept = default;

VariableManager& VariableManager::operator=(VariableManager&& other) noexcept = default;

// ============================================================================
// Методы установки переменных
// ============================================================================

void VariableManager::setGlobalVariable(const std::string& name, const VariableValue& value) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->global_variables[name] = value;
}

void VariableManager::setStateVariable(
    const std::string& state_name,
    const std::string& name,
    const VariableValue& value
) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->state_variables[state_name][name] = value;
}

// ============================================================================
// Методы получения переменных
// ============================================================================

std::optional<VariableValue> VariableManager::getVariable(
    const std::string& state_name,
    const std::string& name
) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    // Сначала ищем локальную переменную
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        auto var_it = state_it->second.find(name);
        if (var_it != state_it->second.end()) {
            return var_it->second;
        }
    }
    
    // Если локальная не найдена, ищем глобальную
    auto global_it = impl_->global_variables.find(name);
    if (global_it != impl_->global_variables.end()) {
        return global_it->second;
    }
    
    return std::nullopt;
}

std::optional<VariableValue> VariableManager::getGlobalVariable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->global_variables.find(name);
    if (it != impl_->global_variables.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::optional<VariableValue> VariableManager::getStateVariable(
    const std::string& state_name,
    const std::string& name
) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        auto var_it = state_it->second.find(name);
        if (var_it != state_it->second.end()) {
            return var_it->second;
        }
    }
    
    return std::nullopt;
}

const std::map<std::string, VariableValue>& VariableManager::getGlobalVariables() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->global_variables;
}

const std::map<std::string, VariableValue>& VariableManager::getStateVariables(
    const std::string& state_name
) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    // Возвращаем пустую карту для несуществующего состояния
    static const std::map<std::string, VariableValue> empty_map;
    
    auto it = impl_->state_variables.find(state_name);
    if (it != impl_->state_variables.end()) {
        return it->second;
    }
    
    return empty_map;
}

// ============================================================================
// Методы проверки существования переменных
// ============================================================================

bool VariableManager::hasVariable(
    const std::string& state_name,
    const std::string& name
) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    // Сначала проверяем локальную переменную
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        if (state_it->second.find(name) != state_it->second.end()) {
            return true;
        }
    }
    
    // Если локальная не найдена, проверяем глобальную
    return impl_->global_variables.find(name) != impl_->global_variables.end();
}

bool VariableManager::hasGlobalVariable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->global_variables.find(name) != impl_->global_variables.end();
}

bool VariableManager::hasStateVariable(
    const std::string& state_name,
    const std::string& name
) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        return state_it->second.find(name) != state_it->second.end();
    }
    
    return false;
}

// ============================================================================
// Методы удаления переменных
// ============================================================================

bool VariableManager::removeVariable(
    const std::string& state_name,
    const std::string& name
) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    // Сначала пытаемся удалить локальную переменную
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        if (state_it->second.erase(name) > 0) {
            return true;
        }
    }
    
    // Если локальная не найдена, пытаемся удалить глобальную
    return impl_->global_variables.erase(name) > 0;
}

bool VariableManager::removeGlobalVariable(const std::string& name) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->global_variables.erase(name) > 0;
}

bool VariableManager::removeStateVariable(
    const std::string& state_name,
    const std::string& name
) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto state_it = impl_->state_variables.find(state_name);
    if (state_it != impl_->state_variables.end()) {
        return state_it->second.erase(name) > 0;
    }
    
    return false;
}

// ============================================================================
// Методы управления переменными
// ============================================================================

void VariableManager::clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->clear();
}

void VariableManager::clearStateVariables(const std::string& state_name) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->state_variables.erase(state_name);
}

void VariableManager::clearGlobalVariables() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->global_variables.clear();
}

void VariableManager::copyStateVariables(
    const std::string& from_state,
    const std::string& to_state
) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto from_it = impl_->state_variables.find(from_state);
    if (from_it != impl_->state_variables.end()) {
        impl_->state_variables[to_state] = from_it->second;
    }
}

// ============================================================================
// Методы подсчёта переменных
// ============================================================================

size_t VariableManager::getGlobalVariableCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->global_variables.size();
}

size_t VariableManager::getStateVariableCount(const std::string& state_name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    auto it = impl_->state_variables.find(state_name);
    if (it != impl_->state_variables.end()) {
        return it->second.size();
    }
    
    return 0;
}

} // namespace fsmconfig
