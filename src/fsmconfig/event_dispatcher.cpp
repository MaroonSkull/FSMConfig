#include "fsmconfig/event_dispatcher.hpp"

namespace fsmconfig {

/**
 * @brief Реализация EventDispatcher (Pimpl идиома)
 */
class EventDispatcher::Impl {
public:
    /// Очередь событий: pair<event_name, event>
    std::queue<std::pair<std::string, TransitionEvent>> event_queue;
    
    /// Обработчик событий
    EventHandler event_handler;
    
    /// Мьютекс для защиты очереди
    mutable std::mutex queue_mutex;
    
    /// Условная переменная для ожидания
    mutable std::condition_variable queue_cv;
    
    /// Флаг работы диспетчера
    std::atomic<bool> running;
    
    /**
     * @brief Очистить очередь событий
     */
    void clear() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        while (!event_queue.empty()) {
            event_queue.pop();
        }
    }
};

EventDispatcher::EventDispatcher()
    : impl_(std::make_unique<Impl>()) {
    impl_->running = false;
}

EventDispatcher::~EventDispatcher() {
    stop();
}

EventDispatcher::EventDispatcher(EventDispatcher&& other) noexcept
    : impl_(std::move(other.impl_)) {
}

EventDispatcher& EventDispatcher::operator=(EventDispatcher&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

void EventDispatcher::dispatchEvent(
    const std::string& event_name,
    const TransitionEvent& event
) {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    impl_->event_queue.emplace(event_name, event);
    impl_->queue_cv.notify_one();
}

void EventDispatcher::processEvents() {
    while (processOneEvent()) {
        // Обрабатываем события пока очередь не станет пустой
    }
}

bool EventDispatcher::processOneEvent() {
    std::unique_lock<std::mutex> lock(impl_->queue_mutex);
    
    if (impl_->event_queue.empty()) {
        return false;
    }
    
    auto event_pair = std::move(impl_->event_queue.front());
    impl_->event_queue.pop();
    lock.unlock();
    
    // Вызываем обработчик событий, если он установлен
    if (impl_->event_handler) {
        impl_->event_handler(event_pair.first, event_pair.second);
    }
    
    return true;
}

size_t EventDispatcher::getEventQueueSize() const {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    return impl_->event_queue.size();
}

void EventDispatcher::clearEventQueue() {
    impl_->clear();
}

bool EventDispatcher::hasPendingEvents() const {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    return !impl_->event_queue.empty();
}

void EventDispatcher::setEventHandler(EventHandler handler) {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    impl_->event_handler = std::move(handler);
}

bool EventDispatcher::hasEventHandler() const {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    return static_cast<bool>(impl_->event_handler);
}

void EventDispatcher::start() {
    impl_->running = true;
}

void EventDispatcher::stop() {
    impl_->running = false;
    impl_->queue_cv.notify_all();
}

bool EventDispatcher::isRunning() const {
    return impl_->running.load();
}

void EventDispatcher::waitForEmptyQueue() const {
    std::unique_lock<std::mutex> lock(impl_->queue_mutex);
    impl_->queue_cv.wait(lock, [this]() {
        return impl_->event_queue.empty() || !impl_->running;
    });
}

} // namespace fsmconfig
