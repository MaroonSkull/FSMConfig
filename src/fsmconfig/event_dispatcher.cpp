#include "fsmconfig/event_dispatcher.hpp"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

#include "fsmconfig/types.hpp"

namespace fsmconfig {

/**
 * @brief EventDispatcher implementation (Pimpl idiom)
 */
class EventDispatcher::Impl {
 public:
  /// Event queue: pair<event_name, event>
  std::queue<std::pair<std::string, TransitionEvent>> event_queue;

  /// Event handler
  EventHandler event_handler;

  /// Mutex for queue protection
  mutable std::mutex queue_mutex;

  /// Condition variable for waiting
  mutable std::condition_variable queue_cv;

  /// Dispatcher running flag
  std::atomic<bool> running;

  /**
   * @brief Clear event queue
   */
  void clear() {
    std::scoped_lock const lock(queue_mutex);
    while (!event_queue.empty()) {
      event_queue.pop();
    }
  }
};

EventDispatcher::EventDispatcher() : impl_(std::make_unique<Impl>()) { impl_->running = false; }

EventDispatcher::~EventDispatcher() { stop(); }

EventDispatcher::EventDispatcher(EventDispatcher&& other) noexcept : impl_(std::move(other.impl_)) {}

EventDispatcher& EventDispatcher::operator=(EventDispatcher&& other) noexcept {
  if (this != &other) {
    impl_ = std::move(other.impl_);
  }
  return *this;
}

void EventDispatcher::dispatchEvent(const std::string& event_name, const TransitionEvent& event) {
  std::scoped_lock const lock(impl_->queue_mutex);
  impl_->event_queue.emplace(event_name, event);
  impl_->queue_cv.notify_one();
}

void EventDispatcher::processEvents() {
  while (processOneEvent()) {
    // Process events until queue becomes empty
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

  // Call event handler if set
  if (impl_->event_handler) {
    impl_->event_handler(event_pair.first, event_pair.second);
  }

  return true;
}

size_t EventDispatcher::getEventQueueSize() const {
  std::scoped_lock const lock(impl_->queue_mutex);
  return impl_->event_queue.size();
}

void EventDispatcher::clearEventQueue() { impl_->clear(); }

bool EventDispatcher::hasPendingEvents() const {
  std::scoped_lock const lock(impl_->queue_mutex);
  return !impl_->event_queue.empty();
}

void EventDispatcher::setEventHandler(EventHandler handler) {
  std::scoped_lock const lock(impl_->queue_mutex);
  impl_->event_handler = std::move(handler);
}

bool EventDispatcher::hasEventHandler() const {
  std::scoped_lock const lock(impl_->queue_mutex);
  return static_cast<bool>(impl_->event_handler);
}

void EventDispatcher::start() { impl_->running = true; }

void EventDispatcher::stop() {
  impl_->running = false;
  impl_->queue_cv.notify_all();
}

bool EventDispatcher::isRunning() const { return impl_->running.load(); }

void EventDispatcher::waitForEmptyQueue() const {
  std::unique_lock<std::mutex> lock(impl_->queue_mutex);
  impl_->queue_cv.wait(lock, [this]() { return impl_->event_queue.empty() || !impl_->running; });
}

}  // namespace fsmconfig
