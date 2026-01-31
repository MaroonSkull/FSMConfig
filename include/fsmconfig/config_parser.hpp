#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "types.hpp"

// Предварительное объявление для yaml-cpp
namespace YAML {
class Node;
}

namespace fsmconfig {

/**
 * @file config_parser.hpp
 * @brief Парсер YAML конфигураций для конечных автоматов
 */

/**
 * @class ConfigParser
 * @brief Класс для парсинга YAML конфигураций конечных автоматов
 *
 * ConfigParser обеспечивает:
 * - Загрузку YAML файлов
 * - Парсинг конфигураций состояний и переходов
 * - Валидацию структуры конфигурации
 * - Доступ к загруженным данным через удобный интерфейс
 */
class ConfigParser {
   public:
    /**
     * @brief Конструктор по умолчанию
     */
    ConfigParser();

    /**
     * @brief Деструктор
     */
    ~ConfigParser();

    // Запрет копирования
    ConfigParser(const ConfigParser&) = delete;
    ConfigParser& operator=(const ConfigParser&) = delete;

    // Разрешение перемещения
    ConfigParser(ConfigParser&& other) noexcept;
    ConfigParser& operator=(ConfigParser&& other) noexcept;

    /**
     * @brief Загрузить конфигурацию из файла
     * @param file_path Путь к YAML файлу конфигурации
     * @throws ConfigException при ошибках загрузки или парсинга
     */
    void loadFromFile(const std::string& file_path);

    /**
     * @brief Загрузить конфигурацию из строки
     * @param yaml_content Строка с YAML содержимым
     * @throws ConfigException при ошибках парсинга
     */
    void loadFromString(const std::string& yaml_content);

    /**
     * @brief Получить глобальные переменные
     * @return Ссылка на карту глобальных переменных
     */
    const std::map<std::string, VariableValue>& getGlobalVariables() const;

    /**
     * @brief Получить информацию о состояниях
     * @return Ссылка на карту состояний
     */
    const std::map<std::string, StateInfo>& getStates() const;

    /**
     * @brief Получить информацию о переходах
     * @return Ссылка на вектор переходов
     */
    const std::vector<TransitionInfo>& getTransitions() const;

    /**
     * @brief Проверить, существует ли состояние
     * @param state_name Имя состояния
     * @return true если состояние существует
     */
    bool hasState(const std::string& state_name) const;

    /**
     * @brief Получить информацию о конкретном состоянии
     * @param state_name Имя состояния
     * @return Ссылка на информацию о состоянии
     * @throws ConfigException если состояние не найдено
     */
    const StateInfo& getState(const std::string& state_name) const;

    /**
     * @brief Получить все переходы из состояния
     * @param state_name Имя исходного состояния
     * @return Вектор переходов из указанного состояния
     */
    std::vector<TransitionInfo> getTransitionsFrom(const std::string& state_name) const;

    /**
     * @brief Получить переход по событию
     * @param from_state Имя исходного состояния
     * @param event_name Имя события
     * @return Указатель на переход или nullptr если переход не найден
     */
    const TransitionInfo* findTransition(const std::string& from_state,
                                         const std::string& event_name) const;

    /**
     * @brief Получить начальное состояние
     * @return Имя начального состояния
     */
    std::string getInitialState() const;

    /**
     * @brief Очистить загруженную конфигурацию
     */
    void clear();

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    // Вспомогательные методы для парсинга
    VariableValue parseVariable(const YAML::Node& node) const;
    StateInfo parseState(const std::string& name, const YAML::Node& node) const;
    TransitionInfo parseTransition(const YAML::Node& node) const;
    void validateConfig() const;

    // Приватные методы парсинга секций
    void parseGlobalVariables(const YAML::Node& node);
    void parseStates(const YAML::Node& node);
    void parseTransitions(const YAML::Node& node);
};

}  // namespace fsmconfig
