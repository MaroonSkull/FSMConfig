#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "types.hpp"

namespace fsmconfig {

/// Тип функции для обработки событий
using EventHandler =
    std::function<void(const std::string& event_name, const TransitionEvent& event)>;

/// Диспетчер событий для конечного автомата
class EventDispatcher {
   public:
    EventDispatcher();
    ~EventDispatcher();

    // Запрет копирования
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    // Разрешение перемещения
    EventDispatcher(EventDispatcher&& other) noexcept;
    EventDispatcher& operator=(EventDispatcher&& other) noexcept;

    /// Отправить событие для обработки
    void dispatchEvent(const std::string& event_name, const TransitionEvent& event);

    /// Обработать все события в очереди (синхронно)
    void processEvents();

    /// Обработать одно событие из очереди
    bool processOneEvent();

    /// Получить количество событий в очереди
    size_t getEventQueueSize() const;

    /// Очистить очередь событий
    void clearEventQueue();

    /// Проверить, есть ли события в очереди
    bool hasPendingEvents() const;

    /// Установить обработчик событий
    void setEventHandler(EventHandler handler);

    /// Проверить, установлен ли обработчик событий
    bool hasEventHandler() const;

    /// Запустить диспетчер (для будущей асинхронной обработки)
    void start();

    /// Остановить диспетчер (для будущей асинхронной обработки)
    void stop();

    /// Проверить, запущен ли диспетчер
    bool isRunning() const;

    /// Ожидать обработки всех событий
    void waitForEmptyQueue() const;

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fsmconfig
