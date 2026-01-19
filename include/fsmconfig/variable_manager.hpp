#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include "types.hpp"

namespace fsmconfig {

/// Область видимости переменных
enum class VariableScope {
    GLOBAL,     ///< Глобальная переменная (доступна во всех состояниях)
    STATE_LOCAL  ///< Локальная переменная состояния
};

/**
 * @brief Менеджер переменных для конечного автомата
 *
 * VariableManager обеспечивает управление переменными двух типов:
 * - Глобальные переменные (доступны во всех состояниях)
 * - Локальные переменные состояния (доступны только в конкретном состоянии)
 *
 * Локальные переменные имеют приоритет над глобальными при поиске.
 * Все операции потокобезопасны благодаря использованию std::mutex.
 */
class VariableManager {
public:
    /**
     * @brief Конструктор по умолчанию
     */
    VariableManager();
    
    /**
     * @brief Деструктор
     */
    ~VariableManager();
    
    // Запрет копирования
    VariableManager(const VariableManager&) = delete;
    VariableManager& operator=(const VariableManager&) = delete;
    
    // Разрешение перемещения
    VariableManager(VariableManager&& other) noexcept;
    VariableManager& operator=(VariableManager&& other) noexcept;
    
    /**
     * @brief Установка глобальной переменной
     * @param name Имя переменной
     * @param value Значение переменной
     */
    void setGlobalVariable(const std::string& name, const VariableValue& value);
    
    /**
     * @brief Установка локальной переменной состояния
     * @param state_name Имя состояния
     * @param name Имя переменной
     * @param value Значение переменной
     */
    void setStateVariable(
        const std::string& state_name,
        const std::string& name,
        const VariableValue& value
    );
    
    /**
     * @brief Получение переменной (сначала ищет локальную, затем глобальную)
     * @param state_name Имя состояния для поиска локальной переменной
     * @param name Имя переменной
     * @return Значение переменной или std::nullopt если не найдена
     *
     * Локальные переменные имеют приоритет над глобальными.
     */
    std::optional<VariableValue> getVariable(
        const std::string& state_name,
        const std::string& name
    ) const;
    
    /**
     * @brief Получение глобальной переменной
     * @param name Имя переменной
     * @return Значение переменной или std::nullopt если не найдена
     */
    std::optional<VariableValue> getGlobalVariable(const std::string& name) const;
    
    /**
     * @brief Получение локальной переменной состояния
     * @param state_name Имя состояния
     * @param name Имя переменной
     * @return Значение переменной или std::nullopt если не найдена
     */
    std::optional<VariableValue> getStateVariable(
        const std::string& state_name,
        const std::string& name
    ) const;
    
    /**
     * @brief Проверка существования переменной (сначала локальной, затем глобальной)
     * @param state_name Имя состояния для проверки локальной переменной
     * @param name Имя переменной
     * @return true если переменная существует
     *
     * Локальные переменные имеют приоритет над глобальными.
     */
    bool hasVariable(
        const std::string& state_name,
        const std::string& name
    ) const;
    
    /**
     * @brief Проверка существования глобальной переменной
     * @param name Имя переменной
     * @return true если глобальная переменная существует
     */
    bool hasGlobalVariable(const std::string& name) const;
    
    /**
     * @brief Проверка существования локальной переменной состояния
     * @param state_name Имя состояния
     * @param name Имя переменной
     * @return true если локальная переменная существует
     */
    bool hasStateVariable(
        const std::string& state_name,
        const std::string& name
    ) const;
    
    /**
     * @brief Удаление переменной (сначала локальной, затем глобальной)
     * @param state_name Имя состояния для удаления локальной переменной
     * @param name Имя переменной
     * @return true если переменная была удалена, false если не существовала
     *
     * Сначала пытается удалить локальную переменную, затем глобальную.
     */
    bool removeVariable(
        const std::string& state_name,
        const std::string& name
    );
    
    /**
     * @brief Удаление глобальной переменной
     * @param name Имя переменной
     * @return true если переменная была удалена, false если не существовала
     */
    bool removeGlobalVariable(const std::string& name);
    
    /**
     * @brief Удаление локальной переменной состояния
     * @param state_name Имя состояния
     * @param name Имя переменной
     * @return true если переменная была удалена, false если не существовала
     */
    bool removeStateVariable(
        const std::string& state_name,
        const std::string& name
    );
    
    /**
     * @brief Получение всех глобальных переменных
     * @return Константная ссылка на карту глобальных переменных
     */
    const std::map<std::string, VariableValue>& getGlobalVariables() const;
    
    /**
     * @brief Получение всех локальных переменных состояния
     * @param state_name Имя состояния
     * @return Константная ссылка на карту локальных переменных состояния
     */
    const std::map<std::string, VariableValue>& getStateVariables(
        const std::string& state_name
    ) const;
    
    /**
     * @brief Очистка всех переменных (глобальных и локальных)
     */
    void clear();
    
    /**
     * @brief Очистка локальных переменных состояния
     * @param state_name Имя состояния
     */
    void clearStateVariables(const std::string& state_name);
    
    /**
     * @brief Очистка глобальных переменных
     */
    void clearGlobalVariables();
    
    /**
     * @brief Копирование переменных из состояния в другое состояние
     * @param from_state Имя исходного состояния
     * @param to_state Имя целевого состояния
     *
     * Копирует все локальные переменные из одного состояния в другое.
     * Если целевое состояние уже имеет переменные, они будут перезаписаны.
     */
    void copyStateVariables(
        const std::string& from_state,
        const std::string& to_state
    );
    
    /**
     * @brief Получение количества глобальных переменных
     * @return Количество глобальных переменных
     */
    size_t getGlobalVariableCount() const;
    
    /**
     * @brief Получение количества локальных переменных состояния
     * @param state_name Имя состояния
     * @return Количество локальных переменных состояния
     */
    size_t getStateVariableCount(const std::string& state_name) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace fsmconfig
