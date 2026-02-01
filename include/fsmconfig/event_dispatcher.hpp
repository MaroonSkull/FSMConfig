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

/// Event handler function type
using EventHandler =
    std::function<void(const std::string& event_name, const TransitionEvent& event)>;

/// Event dispatcher for finite state machine
class EventDispatcher {
   public:
    EventDispatcher();
    ~EventDispatcher();

    // Copy prohibition
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    // Move permission
    EventDispatcher(EventDispatcher&& other) noexcept;
    EventDispatcher& operator=(EventDispatcher&& other) noexcept;

    /// Dispatch event for processing
    void dispatchEvent(const std::string& event_name, const TransitionEvent& event);

    /// Process all events in queue (synchronously)
    void processEvents();

    /// Process one event from queue
    bool processOneEvent();

    /// Get number of events in queue
    size_t getEventQueueSize() const;

    /// Clear event queue
    void clearEventQueue();

    /// Check if there are events in queue
    bool hasPendingEvents() const;

    /// Set event handler
    void setEventHandler(EventHandler handler);

    /// Check if event handler is set
    bool hasEventHandler() const;

    /// Start dispatcher (for future asynchronous processing)
    void start();

    /// Stop dispatcher (for future asynchronous processing)
    void stop();

    /// Check if dispatcher is running
    bool isRunning() const;

    /// Wait for all events to be processed
    void waitForEmptyQueue() const;

   private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fsmconfig
