#include "fsmconfig/callback_registry.hpp"

#include <mutex>

namespace fsmconfig {

/**
 * @brief Реализация CallbackRegistry (Pimpl идиома)
 */
class CallbackRegistry::Impl {
   public:
    /// Коллбэки состояния: key = "state_name:callback_type"
    std::map<std::string, StateCallback> state_callbacks;

    /// Коллбэки перехода: key = "from_state:to_state"
    std::map<std::string, TransitionCallback> transition_callbacks;

    /// Guard-коллбэки: key = "from_state:to_state:event_name"
    std::map<std::string, GuardCallback> guards;

    /// Коллбэки действий: key = "action_name"
    std::map<std::string, ActionCallback> actions;

    /// Мьютекс для потокобезопасности
    mutable std::mutex mutex;

    /**
     * @brief Очистка всех коллбэков
     */
    void clear() {
        state_callbacks.clear();
        transition_callbacks.clear();
        guards.clear();
        actions.clear();
    }
};

// ============================================================================
// Конструкторы и деструктор
// ============================================================================

CallbackRegistry::CallbackRegistry() : impl_(std::make_unique<Impl>()) {}

CallbackRegistry::~CallbackRegistry() = default;

CallbackRegistry::CallbackRegistry(CallbackRegistry&& other) noexcept
    : impl_(std::move(other.impl_)) {}

CallbackRegistry& CallbackRegistry::operator=(CallbackRegistry&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

// ============================================================================
// Методы регистрации
// ============================================================================

void CallbackRegistry::registerStateCallback(const std::string& state_name,
                                             const std::string& callback_type,
                                             StateCallback callback) {
    if (!callback) {
        return;
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeStateCallbackKey(state_name, callback_type);
    impl_->state_callbacks[key] = std::move(callback);
}

void CallbackRegistry::registerTransitionCallback(const std::string& from_state,
                                                  const std::string& to_state,
                                                  TransitionCallback callback) {
    if (!callback) {
        return;
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeTransitionCallbackKey(from_state, to_state);
    impl_->transition_callbacks[key] = std::move(callback);
}

void CallbackRegistry::registerGuard(const std::string& from_state, const std::string& to_state,
                                     const std::string& event_name, GuardCallback callback) {
    if (!callback) {
        return;
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeGuardKey(from_state, to_state, event_name);
    impl_->guards[key] = std::move(callback);
}

void CallbackRegistry::registerAction(const std::string& action_name, ActionCallback callback) {
    if (!callback) {
        return;
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->actions[action_name] = std::move(callback);
}

// ============================================================================
// Методы вызова
// ============================================================================

void CallbackRegistry::callStateCallback(const std::string& state_name,
                                         const std::string& callback_type) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeStateCallbackKey(state_name, callback_type);

    auto it = impl_->state_callbacks.find(key);
    if (it != impl_->state_callbacks.end() && it->second) {
        it->second();
    }
}

void CallbackRegistry::callTransitionCallback(const std::string& from_state,
                                              const std::string& to_state,
                                              const TransitionEvent& event) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeTransitionCallbackKey(from_state, to_state);

    auto it = impl_->transition_callbacks.find(key);
    if (it != impl_->transition_callbacks.end() && it->second) {
        it->second(event);
    }
}

bool CallbackRegistry::callGuard(const std::string& from_state, const std::string& to_state,
                                 const std::string& event_name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeGuardKey(from_state, to_state, event_name);

    auto it = impl_->guards.find(key);
    if (it != impl_->guards.end() && it->second) {
        return it->second();
    }
    // Если guard не зарегистрирован, запрещаем переход
    return false;
}

void CallbackRegistry::callAction(const std::string& action_name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->actions.find(action_name);
    if (it != impl_->actions.end() && it->second) {
        it->second();
    }
}

// ============================================================================
// Методы проверки
// ============================================================================

bool CallbackRegistry::hasStateCallback(const std::string& state_name,
                                        const std::string& callback_type) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeStateCallbackKey(state_name, callback_type);

    auto it = impl_->state_callbacks.find(key);
    return it != impl_->state_callbacks.end() && static_cast<bool>(it->second);
}

bool CallbackRegistry::hasTransitionCallback(const std::string& from_state,
                                             const std::string& to_state) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeTransitionCallbackKey(from_state, to_state);

    auto it = impl_->transition_callbacks.find(key);
    return it != impl_->transition_callbacks.end() && static_cast<bool>(it->second);
}

bool CallbackRegistry::hasGuard(const std::string& from_state, const std::string& to_state,
                                const std::string& event_name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::string key = makeGuardKey(from_state, to_state, event_name);

    auto it = impl_->guards.find(key);
    return it != impl_->guards.end() && static_cast<bool>(it->second);
}

bool CallbackRegistry::hasAction(const std::string& action_name) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);

    auto it = impl_->actions.find(action_name);
    return it != impl_->actions.end() && static_cast<bool>(it->second);
}

// ============================================================================
// Методы управления
// ============================================================================

void CallbackRegistry::clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->clear();
}

size_t CallbackRegistry::getStateCallbackCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->state_callbacks.size();
}

size_t CallbackRegistry::getTransitionCallbackCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->transition_callbacks.size();
}

size_t CallbackRegistry::getGuardCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->guards.size();
}

size_t CallbackRegistry::getActionCount() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->actions.size();
}

// ============================================================================
// Вспомогательные методы
// ============================================================================

std::string CallbackRegistry::makeStateCallbackKey(const std::string& state_name,
                                                   const std::string& callback_type) const {
    return state_name + ":" + callback_type;
}

std::string CallbackRegistry::makeTransitionCallbackKey(const std::string& from_state,
                                                        const std::string& to_state) const {
    return from_state + ":" + to_state;
}

std::string CallbackRegistry::makeGuardKey(const std::string& from_state,
                                           const std::string& to_state,
                                           const std::string& event_name) const {
    return from_state + ":" + to_state + ":" + event_name;
}

}  // namespace fsmconfig
