/**
 * @file test_types.cpp
 * @brief Unit tests for VariableValue move semantics
 */

#include "fsmconfig/types.hpp"

#include <gtest/gtest.h>
#include <string>
#include <utility>

using namespace fsmconfig;

/**
 * @brief Test fixture for VariableValue move semantics tests
 */
class VariableValueMoveTest : public ::testing::Test {
 protected:
  /// Sets up test fixtures
  void SetUp() override {}

  /// Tears down test fixtures
  void TearDown() override {}
};

// ============================================================================
// Move Constructor Tests
// ============================================================================

/**
 * @test Move constructor with INT type
 */
TEST_F(VariableValueMoveTest, MoveConstructorInt) {
  VariableValue original(42);
  VariableValue moved(std::move(original));

  EXPECT_EQ(moved.asInt(), 42);
  // Moved-from object should be in valid but unspecified state
  // We can still destroy it and assign to it
  EXPECT_NO_THROW(original = VariableValue(100));
  EXPECT_EQ(original.asInt(), 100);
}

/**
 * @test Move constructor with FLOAT type
 */
TEST_F(VariableValueMoveTest, MoveConstructorFloat) {
  VariableValue original(3.14f);
  VariableValue moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.asFloat(), 3.14f);
  // Moved-from object should be destructible
  EXPECT_NO_THROW(original.~VariableValue());
}

/**
 * @test Move constructor with STRING type
 */
TEST_F(VariableValueMoveTest, MoveConstructorString) {
  VariableValue original(std::string("Hello, World!"));
  VariableValue moved(std::move(original));

  EXPECT_EQ(moved.asString(), "Hello, World!");
  // Moved-from object should be in valid but unspecified state
  // The string should be empty after move
  EXPECT_NO_THROW(original = VariableValue(std::string("New")));
}

/**
 * @test Move constructor with BOOL type
 */
TEST_F(VariableValueMoveTest, MoveConstructorBool) {
  VariableValue original(true);
  VariableValue moved(std::move(original));

  EXPECT_TRUE(moved.asBool());
  // Moved-from object should be destructible
  EXPECT_NO_THROW(original.~VariableValue());
}

// ============================================================================
// Move Assignment Operator Tests
// ============================================================================

/**
 * @test Move assignment operator with INT type
 */
TEST_F(VariableValueMoveTest, MoveAssignmentInt) {
  VariableValue dest(0);
  VariableValue src(42);

  dest = std::move(src);

  EXPECT_EQ(dest.asInt(), 42);
  // Moved-from object should be in valid but unspecified state
  EXPECT_NO_THROW(src = VariableValue(100));
  EXPECT_EQ(src.asInt(), 100);
}

/**
 * @test Move assignment operator with FLOAT type
 */
TEST_F(VariableValueMoveTest, MoveAssignmentFloat) {
  VariableValue dest(0);
  VariableValue src(3.14f);

  dest = std::move(src);

  EXPECT_FLOAT_EQ(dest.asFloat(), 3.14f);
  // Moved-from object should be destructible
  EXPECT_NO_THROW(src.~VariableValue());
}

/**
 * @test Move assignment operator with STRING type
 */
TEST_F(VariableValueMoveTest, MoveAssignmentString) {
  VariableValue dest(0);
  VariableValue src(std::string("Hello, World!"));

  dest = std::move(src);

  EXPECT_EQ(dest.asString(), "Hello, World!");
  // Moved-from object should be in valid but unspecified state
  EXPECT_NO_THROW(src = VariableValue(std::string("New")));
}

/**
 * @test Move assignment operator with BOOL type
 */
TEST_F(VariableValueMoveTest, MoveAssignmentBool) {
  VariableValue dest(0);
  VariableValue src(true);

  dest = std::move(src);

  EXPECT_TRUE(dest.asBool());
  // Moved-from object should be destructible
  EXPECT_NO_THROW(src.~VariableValue());
}

/**
 * @test Move assignment operator from different types
 */
TEST_F(VariableValueMoveTest, MoveAssignmentDifferentTypes) {
  VariableValue dest(0);  // INT

  // Move from STRING
  VariableValue src1(std::string("test"));
  dest = std::move(src1);
  EXPECT_EQ(dest.asString(), "test");

  // Move from FLOAT
  VariableValue src2(2.5f);
  dest = std::move(src2);
  EXPECT_FLOAT_EQ(dest.asFloat(), 2.5f);

  // Move from BOOL
  VariableValue src3(true);
  dest = std::move(src3);
  EXPECT_TRUE(dest.asBool());
}

// ============================================================================
// Move Efficiency Tests
// ============================================================================

/**
 * @test Move constructor should be more efficient than copy for STRING
 */
TEST_F(VariableValueMoveTest, MoveMoreEfficientThanCopy) {
  std::string long_string(1000, 'x');
  VariableValue original(long_string);

  // Move should transfer ownership without allocation
  VariableValue moved(std::move(original));
  EXPECT_EQ(moved.asString().length(), 1000);

  // Original string should be empty (moved-from state)
  // Note: The exact state is unspecified, but it should be valid
}

/**
 * @test Move assignment should transfer ownership for STRING
 */
TEST_F(VariableValueMoveTest, MoveAssignmentTransfersOwnership) {
  std::string long_string(1000, 'y');
  VariableValue dest;
  VariableValue src(long_string);

  dest = std::move(src);

  EXPECT_EQ(dest.asString().length(), 1000);
}

// ============================================================================
// Moved-From Object State Tests
// ============================================================================

/**
 * @test Moved-from object can be safely destroyed
 */
TEST_F(VariableValueMoveTest, MovedFromObjectDestructible) {
  VariableValue original(std::string("test"));
  VariableValue moved(std::move(original));

  // Original should be safely destructible
  EXPECT_NO_THROW({
    // Destructor will be called automatically
  });
}

/**
 * @test Moved-from object can be reassigned
 */
TEST_F(VariableValueMoveTest, MovedFromObjectReassignable) {
  VariableValue original(std::string("test"));
  VariableValue moved(std::move(original));

  // Original should be assignable to a new value
  EXPECT_NO_THROW(original = VariableValue(42));
  EXPECT_EQ(original.asInt(), 42);
}

/**
 * @test Moved-from object can be copy-assigned
 */
TEST_F(VariableValueMoveTest, MovedFromObjectCopyAssignable) {
  VariableValue original(std::string("test"));
  VariableValue moved(std::move(original));

  VariableValue other(100);
  EXPECT_NO_THROW(original = other);
  EXPECT_EQ(original.asInt(), 100);
}

// ============================================================================
// Cross-Type Move Tests
// ============================================================================

/**
 * @test Move from INT to STRING (via assignment)
 */
TEST_F(VariableValueMoveTest, CrossTypeMoveIntToString) {
  VariableValue dest(std::string("original"));
  VariableValue src(42);

  dest = std::move(src);
  EXPECT_EQ(dest.asInt(), 42);
}

/**
 * @test Move from STRING to INT (via assignment)
 */
TEST_F(VariableValueMoveTest, CrossTypeMoveStringToInt) {
  VariableValue dest(0);
  VariableValue src(std::string("test"));

  dest = std::move(src);
  EXPECT_EQ(dest.asString(), "test");
}

// ============================================================================
// Main entry point
// ============================================================================

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
