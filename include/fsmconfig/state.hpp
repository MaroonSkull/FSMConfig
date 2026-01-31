#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "fsmconfig/types.hpp"

namespace fsmconfig {

/**
 * @file state.hpp
 * @brief Класс состояния конечного автомата
 */

/**
 * @class State
 * @brief Класс представляющий состояние в конечном автомате
 *
 * Класс State обеспечивает:
 * - Хранение информации о состоянии
 * - Управление переменными состояния
 * - Предоставление интерфейса для доступа к данным состояния
 * - Использование идиомы Pimpl для скрытия реализации
 */
class State {
   public:
    /**
     * @brief Конструктор
     * @param info Информация о состоянии
     */
    explicit State(const StateInfo& info);

    /**
     * @brief Деструктор
     */
    ~State();

    // Запрет копирования
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    // Разрешение перемещения
    /**
     * @brief Конструктор перемещения
     * @param other Перемещаемое состояние
     */
    State(State&& other) noexcept;

    /**
     * @brief Оператор присваивания перемещением
     * @param other Перемещаемое состояние
     * @return Ссылка на текущий объект
     */
    State& operator=(State&& other) noexcept;

    /**
     * @brief Получить имя состояния
     * @return Имя состояния
     */
    const std::string& getName() const;

    /**
     * @brief Получить все переменные состояния
     * @return Ссылка на карту переменных
     */
    const std::map<std::string, VariableValue>& getVariables() const;

    /**
     * @brief Получить коллбэк при входе в состояние
     * @return Имя коллбэка
     */
    const std::string& getOnEnterCallback() const;

    /**
     * @brief Получить коллбэк при выходе из состояния
     * @return Имя коллбэка
     */
    const std::string& getOnExitCallback() const;

    /**
     * @brief Получить список действий состояния
     * @return Ссылка на вектор действий
     */
    const std::vector<std::string>& getActions() const;

    /**
     * @brief Проверить наличие переменной
     * @param name Имя переменной
     * @return true если переменная существует
     */
    bool hasVariable(const std::string& name) const;

    /**
     * @brief Получить значение переменной
     * @param name Имя переменной
     * @return Значение переменной
     * @throw StateException Если переменная не существует
     */
    VariableValue getVariable(const std::string& name) const;

    /**
     * @brief Установить значение переменной
     * @param name Имя переменной
     * @param value Значение переменной
     */
    void setVariable(const std::string& name, const VariableValue& value);

    /**
     * @brief Получить все переменные состояния
     * @return Ссылка на карту переменных
     */
    const std::map<std::string, VariableValue>& getAllVariables() const;

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fsmconfig
