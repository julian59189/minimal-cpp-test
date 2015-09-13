// Minimal C++ unit test single header library.
// Author: Vahid Kazemi
//
// Usage:
//   // Define in only one source file or alternatively call RUN_ALL_TESTS()
//   #define DEFINE_TEST_MAIN
//
//   #include "test.h"
//
//   // Simple test:
//   TEST(GroupName, TestName) {
//     EXPECT_EQ(1 + 2, 3);
//   }
//
//   // Fixtures:
//   class ClassName : public cyrus:Test {
//     void Setup() override { ... }
//     void TearDown() override { ... }
//   }
//
//   Test_F(ClassName, TestName) {
//      ...
//   }
//
//   // Assertions:
//   EXPECT_TRUE(X)  // expect x to be true
//   EXPECT_FALSE(X) // expect x to be false
//
//   EXPECT_EQ(X, Y) // expect x == y
//   EXPECT_NE(X, Y) // expect x != y
//   EXPECT_GT(X, Y) // expect x >  y
//   EXPECT_GE(X, Y) // expect x >= y
//   EXPECT_LT(X, Y) // expect x <  y
//   EXPECT_LE(X, Y) // expect x <= y
//
//   EXPECT_NO_THROW({ BLOCK })         // expect no exception
//   EXPECT_THROW({ BLOCK }, EXCEPTION) // expect exception

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define RUN_ALL_TESTS() cyrus::TestManager::GetInstance()->RunAll()

#define _CYRUS_TEST(BaseClass, GroupName, TestName) \
class GroupName ## TestName : public BaseClass { \
 public: \
  GroupName ## TestName() { \
    cyrus::TestManager::GetInstance()->Register( \
      #GroupName, cyrus::TestEntry(#TestName, this)); \
  } \
  void Do() override; \
}; \
GroupName ## TestName _ ## GroupName ## TestName; \
void GroupName ## TestName::Do()

#define TEST(GroupName, TestName) _CYRUS_TEST(cyrus::Test, GroupName, TestName)
#define TEST_F(BaseClass, TestName) _CYRUS_TEST(BaseClass, BaseClass, TestName)

#define _CYRUS_TEST_PRINT_FILE_AND_LINE \
  std::cout << "\nError at " << __FILE__ << ":" << __LINE__ << "\n"; \

#define _CYRUS_TEST_FAILED(ABORT) \
  _CYRUS_TEST_FAILED_ = true; \
  if (ABORT) throw cyrus::TestFailed();

#define _CYRUS_EXPECT_SINGLE(X, EXPRESSION, TEXT, ABORT) { \
  bool x = X; \
  if (!(EXPRESSION)) { \
    _CYRUS_TEST_PRINT_FILE_AND_LINE \
    std::cout << "Expected " << #X << " to be " TEXT ".\n"; \
    std::cout << "Got " #X " = " << x << "\n"; \
    _CYRUS_TEST_FAILED(ABORT) \
  } \
}

#define EXPECT_TRUE(X) _CYRUS_EXPECT_SINGLE(X, x, "true", false)
#define EXPECT_FALSE(X) _CYRUS_EXPECT_SINGLE(X, !x, "false", false)

#define ASSERT_TRUE(X) _CYRUS_EXPECT_SINGLE(X, x, "true", true)
#define ASSERT_FALSE(X) _CYRUS_EXPECT_SINGLE(X, !x, "false", true)

#define _CYRUS_EXPECT_PAIR(X, Y, EXPRESSION, TEXT, ABORT) { \
  auto x = X; \
  auto y = Y; \
  if (!(EXPRESSION)) { \
    _CYRUS_TEST_PRINT_FILE_AND_LINE \
    std::cout << "Expected " #X " to be " TEXT " " #Y ".\n"; \
    std::cout << "Got " #X " = " << x << "\n"; \
    std::cout << "Got " #Y " = " << y << "\n"; \
    _CYRUS_TEST_FAILED(ABORT) \
  } \
}

#define EXPECT_EQ(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x == y, "equal to", false)
#define EXPECT_NE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x != y, "different from", false)
#define EXPECT_GT(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x > y, "greather than", false)
#define EXPECT_GE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x >= y, "greather or equal to", false)
#define EXPECT_LT(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x < y, "less than", false)
#define EXPECT_LE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x <= y, "less or equal to", false)

#define ASSERT_EQ(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x == y, "equal to", true)
#define ASSERT_NE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x != y, "different from", true)
#define ASSERT_GT(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x > y, "greather than", true)
#define ASSERT_GE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x >= y, "greather or equal to", true)
#define ASSERT_LT(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x < y, "less than", true)
#define ASSERT_LE(X, Y) _CYRUS_EXPECT_PAIR(X, Y, x <= y, "less or equal to", true)

#define _CYRUS_EXPECT_NO_THROW(EXPRESSION, ABORT) { \
  try { \
    EXPRESSION; \
  } catch (...) { \
    _CYRUS_TEST_PRINT_FILE_AND_LINE \
    std::cout << "An exception was thrown."; \
    _CYRUS_TEST_FAILED(ABORT) \
  } \
}

#define _CYRUS_EXPECT_THROW(EXPRESSION, EXCEPTION, ABORT) { \
  try { \
    EXPRESSION; \
    _CYRUS_TEST_PRINT_FILE_AND_LINE \
    std::cout << "Exception " #EXCEPTION " was thrown."; \
    _CYRUS_TEST_FAILED(ABORT) \
  } catch (EXCEPTION&) { \
  } \
}

#define EXPECT_NO_THROW(EXPRESSION) \
  _CYRUS_EXPECT_NO_THROW(EXPRESSION, false)
#define EXPECT_THROW(EXPRESSION, EXCEPTION) \
  _CYRUS_EXPECT_THROW(EXPRESSION, EXCEPTION, false)

#define ASSERT_NO_THROW(EXPRESSION) \
  _CYRUS_EXPECT_NO_THROW(EXPRESSION, true)
#define ASSERT_THROW(EXPRESSION, EXCEPTION) \
  _CYRUS_EXPECT_THROW(EXPRESSION, EXCEPTION, true)

namespace cyrus {

class TestFailed : public std::exception {};

class Test {
 public:
  Test() : _CYRUS_TEST_FAILED_(false) {}

  virtual void Setup() {}
  virtual void Do() = 0;
  virtual void TearDown() {}

  bool _CYRUS_TEST_FAILED_;
};

struct TestEntry {
  std::string name;
  Test* test;

  TestEntry(const std::string& name, Test* test)
    : name(name), test(test) {
  }
};

class TestManager {
 public:
  void Register(const std::string& name, const TestEntry& entry) {
    std::cout << "Found " << name << ":" << entry.name << ".\n";
    tests_.emplace(name, entry);
  }

  int RunAll() {
    std::cout << "------------------------\n";
    std::cout << "Running " << tests_.size() << " tests:\n";
    std::cout << "------------------------\n";
    int failure = 0;
    std::string cur_name;
    for (const auto& it : tests_) {
      if (it.first != cur_name) {
        cur_name = it.first;
        std::cout << "-- " << cur_name << "\n";
      }

      auto& test = it.second;
      std::cout << test.name << " ";
      test.test->Setup();
      try {
        test.test->Do();
        if (test.test->_CYRUS_TEST_FAILED_)
          throw TestFailed();
        std::cout << "[Passed]\n";
      } catch (TestFailed& ) {
        std::cout << "[Failed]\n";
        ++failure;
      }
      test.test->TearDown();
    }
    std::cout << "------------------------\n";
    int success = (int)tests_.size() - failure;
    std::cout << "A total of " << tests_.size() << " tests performed.\n";
    std::cout << success << " tests passed.\n";
    std::cout << failure << " tests failed.\n";
    return failure > 0 ? 1 : 0;
  }

  static TestManager* GetInstance() {
    static std::unique_ptr<TestManager> instance(new TestManager());
    return instance.get();
  }

 private:
  std::multimap<std::string, TestEntry> tests_;
};

}

#ifdef DEFINE_TEST_MAIN

int main(int argc, char** argv) {
  return RUN_ALL_TESTS();
}

#endif
