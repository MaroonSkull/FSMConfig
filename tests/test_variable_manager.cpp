/**
 * @file test_variable_manager.cpp
 * @brief Tests for VariableManager class
 */

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "fsmconfig/variable_manager.hpp"
#include "fsmconfig/types.hpp"

using namespace fsmconfig;

/// Tests basic variable operations
TEST(VariableManagerTest, BasicOperations) {
    VariableManager vm;

    // Set and get global variable
    vm.setGlobalVariable("key1", VariableValue(100));
    auto vars = vm.getGlobalVariables();
    EXPECT_EQ(vars.size(), 1);
    EXPECT_EQ(vars["key1"].asInt(), 100);

    // Set and get state variable
    vm.setStateVariable("state1", "key2", VariableValue(200));
    auto stateVars = vm.getStateVariables("state1");
    EXPECT_EQ(stateVars.size(), 1);
    EXPECT_EQ(stateVars["key2"].asInt(), 200);
}

/// Tests thread safety of getGlobalVariables with concurrent reads
TEST(VariableManagerTest, ThreadSafety_GetGlobalVariables) {
    VariableManager vm;
    vm.setGlobalVariable("key1", VariableValue(100));
    vm.setGlobalVariable("key2", VariableValue(200));

    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};

    // Test concurrent reads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&vm, &success_count]() {
            auto vars = vm.getGlobalVariables();
            if (vars.size() == 2 && 
                vars["key1"].asInt() == 100 && 
                vars["key2"].asInt() == 200) {
                ++success_count;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(success_count, 10);
}

/// Tests thread safety with concurrent reads and writes
TEST(VariableManagerTest, ThreadSafety_ConcurrentReadWrite) {
    VariableManager vm;
    vm.setGlobalVariable("counter", VariableValue(0));

    std::atomic<int> read_count{0};
    std::vector<std::thread> threads;

    // Create multiple threads that both read and write
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&vm, &read_count]() {
            for (int j = 0; j < 100; ++j) {
                vm.setGlobalVariable("counter", VariableValue(j));
                auto vars = vm.getGlobalVariables();
                // Verify we got a valid snapshot
                if (vars.contains("counter")) {
                    ++read_count;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // All reads should succeed
    EXPECT_EQ(read_count, 500);
}

/// Tests thread safety of getStateVariables with concurrent reads
TEST(VariableManagerTest, ThreadSafety_GetStateVariables) {
    VariableManager vm;
    vm.setStateVariable("state1", "key1", VariableValue(100));
    vm.setStateVariable("state1", "key2", VariableValue(200));

    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};

    // Test concurrent reads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&vm, &success_count]() {
            auto vars = vm.getStateVariables("state1");
            if (vars.size() == 2 && 
                vars["key1"].asInt() == 100 && 
                vars["key2"].asInt() == 200) {
                ++success_count;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(success_count, 10);
}

/// Tests that returned copies are independent snapshots
TEST(VariableManagerTest, ReturnedCopiesAreSnapshots) {
    VariableManager vm;
    vm.setGlobalVariable("key1", VariableValue(100));

    // Get a snapshot
    auto snapshot1 = vm.getGlobalVariables();
    EXPECT_EQ(snapshot1["key1"].asInt(), 100);

    // Modify the manager
    vm.setGlobalVariable("key1", VariableValue(200));
    vm.setGlobalVariable("key2", VariableValue(300));

    // Original snapshot should be unchanged
    EXPECT_EQ(snapshot1.size(), 1);
    EXPECT_EQ(snapshot1["key1"].asInt(), 100);

    // New snapshot should reflect the changes
    auto snapshot2 = vm.getGlobalVariables();
    EXPECT_EQ(snapshot2.size(), 2);
    EXPECT_EQ(snapshot2["key1"].asInt(), 200);
    EXPECT_EQ(snapshot2["key2"].asInt(), 300);
}

/// Tests thread safety with high contention
TEST(VariableManagerTest, ThreadSafety_HighContention) {
    VariableManager vm;
    const int num_threads = 20;
    const int operations_per_thread = 50;

    std::vector<std::thread> threads;
    std::atomic<int> total_operations{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([i, &vm, &total_operations]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                std::string key = "key_" + std::to_string(i);
                vm.setGlobalVariable(key, VariableValue(j));
                auto vars = vm.getGlobalVariables();
                if (vars.contains(key)) {
                    ++total_operations;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(total_operations, num_threads * operations_per_thread);
}

/// Tests clear operations
TEST(VariableManagerTest, ClearOperations) {
    VariableManager vm;

    vm.setGlobalVariable("key1", VariableValue(100));
    vm.setStateVariable("state1", "key2", VariableValue(200));

    EXPECT_EQ(vm.getGlobalVariables().size(), 1);
    EXPECT_EQ(vm.getStateVariables("state1").size(), 1);

    vm.clearGlobalVariables();
    vm.clearStateVariables("state1");

    EXPECT_EQ(vm.getGlobalVariables().size(), 0);
    EXPECT_EQ(vm.getStateVariables("state1").size(), 0);
}
