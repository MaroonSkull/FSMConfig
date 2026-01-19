#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "types.hpp"

namespace fsmconfig {

// Предварительное объявление классов
class ConfigParser;
class CallbackRegistry;
class VariableManager;
class EventDispatcher;
class State;

/**
 * @file state_machine.hpp
 * @brief Основной класс конечного автомата StateMachine
 */

/**
 * @class StateMachine
 * @brief Класс конечного автомата для управления состояниями и переходами
 *
 * StateMachine является основным классом библиотеки, обеспечивающим:
 * - Загрузку конфигурации из YAML файлов или строк
 * - Регистрацию и выполнение коллбэков
 * - Диспетчеризацию событий
 * - Управление переменными состояния
 * - Переходы между состояниями с поддержкой guard-условий
 * - Наблюдение за изменениями состояния через StateObserver
 */
class StateMachine {
public:
    /**
     * @brief Конструктор из пути к файлу конфигурации
     * @param config_path Путь к YAML файлу конфигурации
     * @throws ConfigException при ошибках загрузки или парсинга
     */
    explicit StateMachine(const std::string& config_path);

    /**
     * @brief Конструктор из строки YAML содержимого
     * @param yaml_content Строка с YAML конфигурацией
     * @param is_content Флаг, указывающий, что первый параметр - это содержимое, а не путь
     * @throws ConfigException при ошибках парсинга
     */
    explicit StateMachine(const std::string& yaml_content, bool is_content);

    /**
     * @brief Деструктор
     */
    ~StateMachine();

    // Запрет копирования
    StateMachine(const StateMachine&) = delete;
    StateMachine& operator=(const StateMachine&) = delete;

    // Разрешение перемещения
    /**
     * @brief Конструктор перемещения
     * @param other Перемещаемый объект StateMachine
     */
    StateMachine(StateMachine&& other) noexcept;

    /**
     * @brief Оператор присваивания перемещением
     * @param other Перемещаемый объект StateMachine
     * @return Ссылка на текущий объект
     */
    StateMachine& operator=(StateMachine&& other) noexcept;

    // Lifecycle

    /**
     * @brief Запуск конечного автомата
     *
     * Переходит в начальное состояние, вызывает on_enter коллбэк,
     * выполняет действия состояния и уведомляет наблюдателей.
     *
     * @throws StateException если автомат уже запущен или начальное состояние не найдено
     */
    void start();

    /**
     * @brief Остановка конечного автомата
     *
     * Вызывает on_exit коллбэк текущего состояния и уведомляет наблюдателей.
     */
    void stop();

    /**
     * @brief Сброс конечного автомата в начальное состояние
     *
     * Останавливает автомат, если он запущен, и сбрасывает все состояния.
     */
    void reset();

    // State queries

    /**
     * @brief Получить имя текущего состояния
     * @return Имя текущего состояния
     */
    std::string getCurrentState() const;

    /**
     * @brief Проверить существование состояния
     * @param state_name Имя состояния
     * @return true если состояние существует
     */
    bool hasState(const std::string& state_name) const;

    /**
     * @brief Получить список всех состояний
     * @return Вектор имен всех состояний
     */
    std::vector<std::string> getAllStates() const;

    // Event handling

    /**
     * @brief Отправить событие без данных
     * @param event_name Имя события
     * @throws StateException если автомат не запущен или переход не найден
     */
    void triggerEvent(const std::string& event_name);

    /**
     * @brief Отправить событие с данными
     * @param event_name Имя события
     * @param data Данные события
     * @throws StateException если автомат не запущен или переход не найден
     */
    void triggerEvent(const std::string& event_name, const std::map<std::string, VariableValue>& data);

    // Callback registration (шаблонные методы)

    /**
     * @brief Зарегистрировать коллбэк состояния
     * @tparam T Тип класса-объекта
     * @param state_name Имя состояния
     * @param callback_type Тип коллбэка (например, "on_enter", "on_exit")
     * @param callback Метод-коллбэк
     * @param instance Указатель на экземпляр класса
     */
    template<typename T>
    void registerStateCallback(
        const std::string& state_name,
        const std::string& callback_type,
        void (T::*callback)(),
        T* instance
    );

    /**
     * @brief Зарегистрировать коллбэк перехода
     * @tparam T Тип класса-объекта
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param callback Метод-коллбэк
     * @param instance Указатель на экземпляр класса
     */
    template<typename T>
    void registerTransitionCallback(
        const std::string& from_state,
        const std::string& to_state,
        void (T::*callback)(const TransitionEvent&),
        T* instance
    );

    /**
     * @brief Зарегистрировать guard-коллбэк
     * @tparam T Тип класса-объекта
     * @param from_state Исходное состояние
     * @param to_state Целевое состояние
     * @param event_name Имя события
     * @param callback Метод-коллбэк
     * @param instance Указатель на экземпляр класса
     */
    template<typename T>
    void registerGuard(
        const std::string& from_state,
        const std::string& to_state,
        const std::string& event_name,
        bool (T::*callback)(),
        T* instance
    );

    /**
     * @brief Зарегистрировать коллбэк действия
     * @tparam T Тип класса-объекта
     * @param action_name Имя действия
     * @param callback Метод-коллбэк
     * @param instance Указатель на экземпляр класса
     */
    template<typename T>
    void registerAction(
        const std::string& action_name,
        void (T::*callback)(),
        T* instance
    );

    // Variable management

    /**
     * @brief Установить значение переменной
     * @param name Имя переменной
     * @param value Значение переменной
     */
    void setVariable(const std::string& name, const VariableValue& value);

    /**
     * @brief Получить значение переменной
     * @param name Имя переменной
     * @return Значение переменной
     * @throws StateException если переменная не существует
     */
    VariableValue getVariable(const std::string& name) const;

    /**
     * @brief Проверить существование переменной
     * @param name Имя переменной
     * @return true если переменная существует
     */
    bool hasVariable(const std::string& name) const;

    // Observers

    /**
     * @brief Зарегистрировать наблюдателя за изменениями состояния
     * @param observer Указатель на наблюдателя
     */
    void registerStateObserver(StateObserver* observer);

    /**
     * @brief Отменить регистрацию наблюдателя
     * @param observer Указатель на наблюдателя
     */
    void unregisterStateObserver(StateObserver* observer);

    // Error handling

    /**
     * @brief Установить обработчик ошибок
     * @param handler Функция-обработчик ошибок
     */
    void setErrorHandler(ErrorHandler handler);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    // Вспомогательные методы
    void performTransition(const TransitionEvent& event);
    bool evaluateGuard(const std::string& from_state, const std::string& to_state, const std::string& event_name);
    void executeStateActions(const std::string& state_name);
    void executeTransitionActions(const std::vector<std::string>& actions);

    // Вспомогательные методы для регистрации коллбэков (для шаблонных методов)
    void registerStateCallbackImpl(
        const std::string& state_name,
        const std::string& callback_type,
        std::function<void()> callback
    );

    void registerTransitionCallbackImpl(
        const std::string& from_state,
        const std::string& to_state,
        std::function<void(const TransitionEvent&)> callback
    );

    void registerGuardImpl(
        const std::string& from_state,
        const std::string& to_state,
        const std::string& event_name,
        std::function<bool()> callback
    );

    void registerActionImpl(
        const std::string& action_name,
        std::function<void()> callback
    );
};

// Реализация шаблонных методов в заголовке

template<typename T>
void StateMachine::registerStateCallback(
    const std::string& state_name,
    const std::string& callback_type,
    void (T::*callback)(),
    T* instance
) {
    auto cb = [instance, callback]() {
        (instance->*callback)();
    };
    registerStateCallbackImpl(state_name, callback_type, cb);
}

template<typename T>
void StateMachine::registerTransitionCallback(
    const std::string& from_state,
    const std::string& to_state,
    void (T::*callback)(const TransitionEvent&),
    T* instance
) {
    auto cb = [instance, callback](const TransitionEvent& event) {
        (instance->*callback)(event);
    };
    registerTransitionCallbackImpl(from_state, to_state, cb);
}

template<typename T>
void StateMachine::registerGuard(
    const std::string& from_state,
    const std::string& to_state,
    const std::string& event_name,
    bool (T::*callback)(),
    T* instance
) {
    auto cb = [instance, callback]() -> bool {
        return (instance->*callback)();
    };
    registerGuardImpl(from_state, to_state, event_name, cb);
}

template<typename T>
void StateMachine::registerAction(
    const std::string& action_name,
    void (T::*callback)(),
    T* instance
) {
    auto cb = [instance, callback]() {
        (instance->*callback)();
    };
    registerActionImpl(action_name, cb);
}

} // namespace fsmconfig
