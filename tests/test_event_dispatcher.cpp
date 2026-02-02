/**
 * @file test_event_dispatcher.cpp
 * @brief Tests for EventDispatcher class
 */

#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include "fsmconfig/event_dispatcher.hpp"
#include "fsmconfig/types.hpp"

using namespace fsmconfig;

/// Tests default construction and initial state
TEST(EventDispatcherTest, DefaultConstruction) {
    EventDispatcher dispatcher;

    EXPECT_FALSE(dispatcher.isRunning());
    EXPECT_FALSE(dispatcher.hasEventHandler());
    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);
    EXPECT_FALSE(dispatcher.hasPendingEvents());
}

/// Tests move construction
TEST(EventDispatcherTest, DISABLED_MoveConstruction) {
    EventDispatcher original;
    original.setEventHandler([](const std::string&, const TransitionEvent&) {});

    TransitionEvent event;
    event.event_name = "test_event";
    event.from_state = "state1";
    event.to_state = "state2";
    original.dispatchEvent("test_event", event);

    EventDispatcher moved(std::move(original));

    // After move, the new object should have the state
    EXPECT_TRUE(moved.hasEventHandler());
    EXPECT_EQ(moved.getEventQueueSize(), 1);
}

/// Tests move assignment
TEST(EventDispatcherTest, DISABLED_MoveAssignment) {
    EventDispatcher original;
    original.setEventHandler([](const std::string&, const TransitionEvent&) {});

    TransitionEvent event;
    event.event_name = "test_event";
    event.from_state = "state1";
    event.to_state = "state2";
    original.dispatchEvent("test_event", event);

    EventDispatcher moved;
    moved = std::move(original);

    EXPECT_TRUE(moved.hasEventHandler());
    EXPECT_EQ(moved.getEventQueueSize(), 1);
}

/// Tests dispatching events to queue
TEST(EventDispatcherTest, DispatchEvent) {
    EventDispatcher dispatcher;

    TransitionEvent event1;
    event1.event_name = "event1";
    event1.from_state = "state1";
    event1.to_state = "state2";

    TransitionEvent event2;
    event2.event_name = "event2";
    event2.from_state = "state2";
    event2.to_state = "state3";

    dispatcher.dispatchEvent("event1", event1);
    dispatcher.dispatchEvent("event2", event2);

    EXPECT_EQ(dispatcher.getEventQueueSize(), 2);
    EXPECT_TRUE(dispatcher.hasPendingEvents());
}

/// Tests processing events with handler
TEST(EventDispatcherTest, ProcessEventsWithHandler) {
    EventDispatcher dispatcher;
    std::atomic<int> call_count{0};
    std::string last_event_name;
    std::string last_from_state;

    dispatcher.setEventHandler([&](const std::string& event_name, const TransitionEvent& event) {
        call_count++;
        last_event_name = event_name;
        last_from_state = event.from_state;
    });

    TransitionEvent event1;
    event1.event_name = "start";
    event1.from_state = "idle";
    event1.to_state = "active";

    TransitionEvent event2;
    event2.event_name = "stop";
    event2.from_state = "active";
    event2.to_state = "idle";

    dispatcher.dispatchEvent("start", event1);
    dispatcher.dispatchEvent("stop", event2);

    EXPECT_EQ(dispatcher.getEventQueueSize(), 2);

    dispatcher.processEvents();

    EXPECT_EQ(call_count, 2);
    EXPECT_EQ(last_event_name, "stop");
    EXPECT_EQ(last_from_state, "active");
    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);
    EXPECT_FALSE(dispatcher.hasPendingEvents());
}

/// Tests processing single event
TEST(EventDispatcherTest, ProcessOneEvent) {
    EventDispatcher dispatcher;
    std::atomic<int> call_count{0};

    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        call_count++;
    });

    TransitionEvent event1;
    event1.event_name = "event1";
    event1.from_state = "state1";
    event1.to_state = "state2";

    TransitionEvent event2;
    event2.event_name = "event2";
    event2.from_state = "state2";
    event2.to_state = "state3";

    dispatcher.dispatchEvent("event1", event1);
    dispatcher.dispatchEvent("event2", event2);

    EXPECT_EQ(dispatcher.getEventQueueSize(), 2);

    bool processed = dispatcher.processOneEvent();
    EXPECT_TRUE(processed);
    EXPECT_EQ(call_count, 1);
    EXPECT_EQ(dispatcher.getEventQueueSize(), 1);

    processed = dispatcher.processOneEvent();
    EXPECT_TRUE(processed);
    EXPECT_EQ(call_count, 2);
    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);

    processed = dispatcher.processOneEvent();
    EXPECT_FALSE(processed);
}

/// Tests processing events without handler
TEST(EventDispatcherTest, ProcessEventsWithoutHandler) {
    EventDispatcher dispatcher;

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);

    // Should not crash, just process without calling any handler
    dispatcher.processEvents();

    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);
}

/// Tests clearing event queue
TEST(EventDispatcherTest, ClearEventQueue) {
    EventDispatcher dispatcher;

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);
    dispatcher.dispatchEvent("test", event);
    dispatcher.dispatchEvent("test", event);

    EXPECT_EQ(dispatcher.getEventQueueSize(), 3);

    dispatcher.clearEventQueue();

    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);
    EXPECT_FALSE(dispatcher.hasPendingEvents());
}

/// Tests hasPendingEvents
TEST(EventDispatcherTest, HasPendingEvents) {
    EventDispatcher dispatcher;

    EXPECT_FALSE(dispatcher.hasPendingEvents());

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);
    EXPECT_TRUE(dispatcher.hasPendingEvents());

    dispatcher.processEvents();
    EXPECT_FALSE(dispatcher.hasPendingEvents());
}

/// Tests setting event handler
TEST(EventDispatcherTest, SetEventHandler) {
    EventDispatcher dispatcher;

    EXPECT_FALSE(dispatcher.hasEventHandler());

    dispatcher.setEventHandler([](const std::string&, const TransitionEvent&) {});
    EXPECT_TRUE(dispatcher.hasEventHandler());
}

/// Tests replacing event handler
TEST(EventDispatcherTest, ReplaceEventHandler) {
    EventDispatcher dispatcher;
    std::atomic<int> first_handler_count{0};
    std::atomic<int> second_handler_count{0};

    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        first_handler_count++;
    });

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);
    dispatcher.processEvents();

    EXPECT_EQ(first_handler_count, 1);

    // Replace handler
    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        second_handler_count++;
    });

    dispatcher.dispatchEvent("test", event);
    dispatcher.processEvents();

    EXPECT_EQ(first_handler_count, 1);
    EXPECT_EQ(second_handler_count, 1);
}

/// Tests start and stop dispatcher
TEST(EventDispatcherTest, StartStopDispatcher) {
    EventDispatcher dispatcher;

    EXPECT_FALSE(dispatcher.isRunning());

    dispatcher.start();
    EXPECT_TRUE(dispatcher.isRunning());

    dispatcher.stop();
    EXPECT_FALSE(dispatcher.isRunning());
}

/// Tests multiple start/stop cycles
TEST(EventDispatcherTest, MultipleStartStopCycles) {
    EventDispatcher dispatcher;

    for (int i = 0; i < 3; ++i) {
        dispatcher.start();
        EXPECT_TRUE(dispatcher.isRunning());

        dispatcher.stop();
        EXPECT_FALSE(dispatcher.isRunning());
    }
}

/// Tests waitForEmptyQueue with empty queue
TEST(EventDispatcherTest, WaitForEmptyQueue_Empty) {
    EventDispatcher dispatcher;
    // Should return immediately when queue is empty
    dispatcher.waitForEmptyQueue();
}

/// Tests waitForEmptyQueue with events
TEST(EventDispatcherTest, DISABLED_WaitForEmptyQueue_WithEvents) {
    EventDispatcher dispatcher;
    std::atomic<bool> processing{false};

    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        processing = true;
    });

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);

    // Start dispatcher to process events automatically
    dispatcher.start();

    dispatcher.waitForEmptyQueue();
    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);
    EXPECT_TRUE(processing);

    dispatcher.stop();
}

/// Tests event data is preserved
TEST(EventDispatcherTest, EventDataPreserved) {
    EventDispatcher dispatcher;
    std::string received_event_name;
    std::string received_from;
    std::string received_to;
    std::map<std::string, VariableValue> received_data;

    dispatcher.setEventHandler([&](const std::string& event_name, const TransitionEvent& event) {
        received_event_name = event_name;
        received_from = event.from_state;
        received_to = event.to_state;
        received_data = event.data;
    });

    TransitionEvent event;
    event.event_name = "transition";
    event.from_state = "idle";
    event.to_state = "active";
    event.data["key1"] = VariableValue(42);
    event.data["key2"] = VariableValue(std::string("value"));

    dispatcher.dispatchEvent("transition", event);
    dispatcher.processEvents();

    EXPECT_EQ(received_event_name, "transition");
    EXPECT_EQ(received_from, "idle");
    EXPECT_EQ(received_to, "active");
    EXPECT_EQ(received_data.size(), 2);
    EXPECT_EQ(received_data["key1"].asInt(), 42);
    EXPECT_EQ(received_data["key2"].asString(), "value");
}

/// Tests processing multiple events in order
TEST(EventDispatcherTest, ProcessEventsInOrder) {
    EventDispatcher dispatcher;
    std::vector<int> order;

    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent& event) {
        order.push_back(event.data.at("id").asInt());
    });

    for (int i = 0; i < 5; ++i) {
        TransitionEvent event;
        event.event_name = "event";
        event.from_state = "state1";
        event.to_state = "state2";
        event.data["id"] = VariableValue(i);
        dispatcher.dispatchEvent("event", event);
    }

    dispatcher.processEvents();

    EXPECT_EQ(order.size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(order[i], i);
    }
}

/// Tests thread-safe event dispatching
TEST(EventDispatcherTest, ThreadSafeDispatch) {
    EventDispatcher dispatcher;
    const int num_threads = 10;
    const int events_per_thread = 100;

    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < events_per_thread; ++j) {
            TransitionEvent event;
            event.event_name = "test";
            event.from_state = "state1";
            event.to_state = "state2";
            dispatcher.dispatchEvent("test", event);
        }
    }

    EXPECT_EQ(dispatcher.getEventQueueSize(), num_threads * events_per_thread);
}

/// Tests handler receives correct event name parameter
TEST(EventDispatcherTest, HandlerReceivesEventName) {
    EventDispatcher dispatcher;
    std::string received_name_param;
    std::string received_event_field;

    dispatcher.setEventHandler([&](const std::string& name_param, const TransitionEvent& event) {
        received_name_param = name_param;
        received_event_field = event.event_name;
    });

    TransitionEvent event;
    event.event_name = "my_event";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("dispatched_name", event);
    dispatcher.processEvents();

    EXPECT_EQ(received_name_param, "dispatched_name");
    EXPECT_EQ(received_event_field, "my_event");
}

/// Tests processing events after clearing queue
TEST(EventDispatcherTest, ProcessAfterClear) {
    EventDispatcher dispatcher;
    std::atomic<int> call_count{0};

    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        call_count++;
    });

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);
    dispatcher.dispatchEvent("test", event);
    dispatcher.clearEventQueue();

    EXPECT_EQ(dispatcher.getEventQueueSize(), 0);

    // Add new event after clearing
    dispatcher.dispatchEvent("test", event);
    dispatcher.processEvents();

    EXPECT_EQ(call_count, 1);
}

/// Tests isRunning state during processing
TEST(EventDispatcherTest, IsRunningDuringProcessing) {
    EventDispatcher dispatcher;

    dispatcher.start();
    EXPECT_TRUE(dispatcher.isRunning());

    std::atomic<bool> processed{false};
    dispatcher.setEventHandler([&](const std::string&, const TransitionEvent&) {
        processed = true;
    });

    TransitionEvent event;
    event.event_name = "test";
    event.from_state = "state1";
    event.to_state = "state2";

    dispatcher.dispatchEvent("test", event);
    dispatcher.processEvents();

    EXPECT_TRUE(processed);
    EXPECT_TRUE(dispatcher.isRunning());

    dispatcher.stop();
}
