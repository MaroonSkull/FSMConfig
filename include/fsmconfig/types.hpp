#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

namespace fsmconfig {

/**
 * @file types.hpp
 * @brief Основные типы данных для FSMConfig
 */

/**
 * @brief Перечисление типов переменных
 */
enum class VariableType {
    INT,     ///< Целочисленное значение
    FLOAT,   ///< Вещественное значение
    STRING,  ///< Строковое значение
    BOOL     ///< Логическое значение
};

/**
 * @brief Структура для хранения значения переменной с поддержкой разных типов
 *
 * Использует union для эффективного хранения значений разных типов.
 * Для std::string используется placement new и явный вызов деструктора.
 */
struct VariableValue {
    VariableType type;
    union {
        int int_value;
        float float_value;
        std::string string_value;
        bool bool_value;
    };

    /**
     * @brief Конструктор по умолчанию (инициализирует как INT со значением 0)
     */
    VariableValue();

    /**
     * @brief Конструктор для целочисленного значения
     * @param v Целочисленное значение
     */
    VariableValue(int v);

    /**
     * @brief Конструктор для вещественного значения
     * @param v Вещественное значение
     */
    VariableValue(float v);

    /**
     * @brief Конструктор для строкового значения
     * @param v Строковое значение
     */
    VariableValue(const std::string& v);

    /**
     * @brief Конструктор для логического значения
     * @param v Логическое значение
     */
    VariableValue(bool v);

    /**
     * @brief Деструктор
     *
     * Явно вызывает деструктор для std::string если тип STRING
     */
    ~VariableValue();

    /**
     * @brief Конструктор копирования
     * @param other Копируемое значение
     */
    VariableValue(const VariableValue& other);

    /**
     * @brief Оператор присваивания копированием
     * @param other Присваиваемое значение
     * @return Ссылка на текущий объект
     */
    VariableValue& operator=(const VariableValue& other);

    /**
     * @brief Получить значение как целое число
     * @return Целочисленное значение
     * @throw std::bad_cast Если тип не INT
     */
    int asInt() const;

    /**
     * @brief Получить значение как вещественное число
     * @return Вещественное значение
     * @throw std::bad_cast Если тип не FLOAT
     */
    float asFloat() const;

    /**
     * @brief Получить значение как строку
     * @return Строковое значение
     * @throw std::bad_cast Если тип не STRING
     */
    std::string asString() const;

    /**
     * @brief Получить значение как логическое
     * @return Логическое значение
     * @throw std::bad_cast Если тип не BOOL
     */
    bool asBool() const;

    /**
     * @brief Преобразовать значение в строковое представление
     * @return Строковое представление значения
     */
    std::string toString() const;
};

/**
 * @brief Событие перехода между состояниями
 *
 * Хранит информацию о переходе, включая имя события,
 * исходное и целевое состояние, данные события и временную метку.
 */
struct TransitionEvent {
    std::string event_name;                           ///< Имя события
    std::string from_state;                           ///< Исходное состояние
    std::string to_state;                             ///< Целевое состояние
    std::map<std::string, VariableValue> data;        ///< Данные события
    std::chrono::system_clock::time_point timestamp;  ///< Временная метка

    /**
     * @brief Конструктор по умолчанию
     *
     * Инициализирует временную метку текущим временем
     */
    TransitionEvent();
};

/**
 * @brief Информация о состоянии
 *
 * Содержит имя состояния, переменные состояния,
 * коллбэки и действия, связанные с состоянием.
 */
struct StateInfo {
    std::string name;                                ///< Имя состояния
    std::map<std::string, VariableValue> variables;  ///< Переменные состояния
    std::string on_enter_callback;                   ///< Коллбэк при входе
    std::string on_exit_callback;                    ///< Коллбэк при выходе
    std::vector<std::string> actions;                ///< Список действий

    /**
     * @brief Конструктор по умолчанию
     */
    StateInfo();

    /**
     * @brief Конструктор с именем состояния
     * @param name Имя состояния
     */
    explicit StateInfo(const std::string& name);
};

/**
 * @brief Информация о переходе
 *
 * Содержит информацию о переходе между состояниями,
 * включая условия, коллбэки и действия.
 */
struct TransitionInfo {
    std::string from_state;            ///< Исходное состояние
    std::string to_state;              ///< Целевое состояние
    std::string event_name;            ///< Имя события
    std::string guard_callback;        ///< Коллбэк-защита
    std::string transition_callback;   ///< Коллбэк перехода
    std::vector<std::string> actions;  ///< Список действий

    /**
     * @brief Конструктор по умолчанию
     */
    TransitionInfo();
};

/**
 * @brief Интерфейс наблюдателя за изменениями состояния
 *
 * Позволяет подписываться на события изменения состояния
 * и получать уведомления о переходах.
 */
class StateObserver {
   public:
    /**
     * @brief Виртуальный деструктор
     */
    virtual ~StateObserver() = default;

    /**
     * @brief Вызывается при входе в состояние
     * @param state_name Имя состояния
     */
    virtual void onStateEnter(const std::string& state_name) = 0;

    /**
     * @brief Вызывается при выходе из состояния
     * @param state_name Имя состояния
     */
    virtual void onStateExit(const std::string& state_name) = 0;

    /**
     * @brief Вызывается при переходе между состояниями
     * @param event Событие перехода
     */
    virtual void onTransition(const TransitionEvent& event) = 0;

    /**
     * @brief Вызывается при возникновении ошибки
     * @param error_message Сообщение об ошибке
     */
    virtual void onError(const std::string& error_message) = 0;
};

/**
 * @brief Тип функции для обработки ошибок
 */
using ErrorHandler = std::function<void(const std::string&)>;

/**
 * @brief Исключение для ошибок конфигурации
 *
 * Выбрасывается при ошибках парсинга конфигурации
 * или при некорректной структуре конфигурации.
 */
class ConfigException : public std::runtime_error {
   public:
    /**
     * @brief Конструктор с сообщением об ошибке
     * @param message Сообщение об ошибке
     */
    explicit ConfigException(const std::string& message);
};

/**
 * @brief Исключение для ошибок состояния
 *
 * Выбрасывается при ошибках, связанных с состояниями
 * или переходами между ними.
 */
class StateException : public std::runtime_error {
   public:
    /**
     * @brief Конструктор с сообщением об ошибке
     * @param message Сообщение об ошибке
     */
    explicit StateException(const std::string& message);
};

}  // namespace fsmconfig
