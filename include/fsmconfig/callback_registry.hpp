#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "types.hpp"

namespace fsmconfig {

/**
 * @file callback_registry.hpp
 * @brief Реестр коллбэков для конечного автомата
 */

/**
 * @brief Тип коллбэка для состояния (on_enter, on_exit)
 */
using StateCallback = std::function<void()>;

/**
 * @brief Тип коллбэка для перехода
 */
using TransitionCallback = std::function<void(const TransitionEvent&)>;

/**
 * @brief Тип guard-коллбэка (возвращает bool)
 */
using GuardCallback = std::function<bool()>;

/**
 * @brief Тип коллбэка для действия
 */
using ActionCallback = std::function<void()>;

/**
 * @class CallbackRegistry
 * @brief Реестр коллбэков для конечного автомата
 *
 * CallbackRegistry обеспечивает:
 * - Регистрацию коллбэков состояний (on_enter, on_exit)
 * - Регистрацию коллбэков переходов
 * - Регистрацию guard-коллбэков для проверки условий
 * - Регистрацию коллбэков действий
 * - Выполнение зарегистрированных коллбэков
 * - Потокобезопасность при работе с коллбэками
 */
class CallbackRegistry {
   public:
    /**
     * @brief Конструктор по умолчанию
     */
    CallbackRegistry();

    /**
     * @brief Деструктор
     */
    ~CallbackRegistry();

    // Запрет копирования
    CallbackRegistry(const CallbackRegistry&) = delete;
    CallbackRegistry& operator=(const CallbackRegistry&) = delete;

    // Разрешение перемещения
    /**
     * @brief Конструктор перемещения
     */
    CallbackRegistry(CallbackRegistry&& other) noexcept;

    /**
     * @brief Оператор присваивания перемещением
     */
    CallbackRegistry& operator=(CallbackRegistry&& other) noexcept;

    /**
     * @brief Регистрация коллбэка состояния
     * @param state_name Имя состояния
     * @param callback_type Тип коллбэка (например, "on_enter", "on_exit")
     * @param callback Функция коллбэка
     */
    void registerStateCallback(const std::string& state_name, const std::string& callback_type,
                               StateCallback callback);

    /**
     * @brief Регистрация коллбэка перехода
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param callback Функция коллбэка
     */
    void registerTransitionCallback(const std::string& from_state, const std::string& to_state,
                                    TransitionCallback callback);

    /**
     * @brief Регистрация guard-коллбэка
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event_name Имя события
     * @param callback Функция guard-коллбэка
     */
    void registerGuard(const std::string& from_state, const std::string& to_state,
                       const std::string& event_name, GuardCallback callback);

    /**
     * @brief Регистрация коллбэка действия
     * @param action_name Имя действия
     * @param callback Функция коллбэка
     */
    void registerAction(const std::string& action_name, ActionCallback callback);

    /**
     * @brief Вызов коллбэка состояния
     * @param state_name Имя состояния
     * @param callback_type Тип коллбэка
     */
    void callStateCallback(const std::string& state_name, const std::string& callback_type) const;

    /**
     * @brief Вызов коллбэка перехода
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event Событие перехода
     */
    void callTransitionCallback(const std::string& from_state, const std::string& to_state,
                                const TransitionEvent& event) const;

    /**
     * @brief Вызов guard-коллбэка
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event_name Имя события
     * @return true если guard существует и вернул true, иначе false
     */
    bool callGuard(const std::string& from_state, const std::string& to_state,
                   const std::string& event_name) const;

    /**
     * @brief Вызов коллбэка действия
     * @param action_name Имя действия
     */
    void callAction(const std::string& action_name) const;

    /**
     * @brief Проверка наличия коллбэка состояния
     * @param state_name Имя состояния
     * @param callback_type Тип коллбэка
     * @return true если коллбэк зарегистрирован
     */
    bool hasStateCallback(const std::string& state_name, const std::string& callback_type) const;

    /**
     * @brief Проверка наличия коллбэка перехода
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @return true если коллбэк зарегистрирован
     */
    bool hasTransitionCallback(const std::string& from_state, const std::string& to_state) const;

    /**
     * @brief Проверка наличия guard-коллбэка
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event_name Имя события
     * @return true если guard зарегистрирован
     */
    bool hasGuard(const std::string& from_state, const std::string& to_state,
                  const std::string& event_name) const;

    /**
     * @brief Проверка наличия коллбэка действия
     * @param action_name Имя действия
     * @return true если коллбэк зарегистрирован
     */
    bool hasAction(const std::string& action_name) const;

    /**
     * @brief Очистка всех коллбэков
     */
    void clear();

    /**
     * @brief Получение количества зарегистрированных коллбэков состояния
     * @return Количество коллбэков состояния
     */
    size_t getStateCallbackCount() const;

    /**
     * @brief Получение количества зарегистрированных коллбэков перехода
     * @return Количество коллбэков перехода
     */
    size_t getTransitionCallbackCount() const;

    /**
     * @brief Получение количества зарегистрированных guard-коллбэков
     * @return Количество guard-коллбэков
     */
    size_t getGuardCount() const;

    /**
     * @brief Получение количества зарегистрированных коллбэков действий
     * @return Количество коллбэков действий
     */
    size_t getActionCount() const;

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    /**
     * @brief Вспомогательный метод для создания ключа коллбэка состояния
     * @param state_name Имя состояния
     * @param callback_type Тип коллбэка
     * @return Ключ в формате "state_name:callback_type"
     */
    std::string makeStateCallbackKey(const std::string& state_name,
                                     const std::string& callback_type) const;

    /**
     * @brief Вспомогательный метод для создания ключа коллбэка перехода
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @return Ключ в формате "from_state:to_state"
     */
    std::string makeTransitionCallbackKey(const std::string& from_state,
                                          const std::string& to_state) const;

    /**
     * @brief Вспомогательный метод для создания ключа guard-коллбэка
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event_name Имя события
     * @return Ключ в формате "from_state:to_state:event_name"
     */
    std::string makeGuardKey(const std::string& from_state, const std::string& to_state,
                             const std::string& event_name) const;
};

}  // namespace fsmconfig
