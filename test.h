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
//     ...
//   }
//
//   // Fixture
//   class ClassName : public cyrus:Test {
//     void Setup() override { ... }
//     void TearDown() override { ... }
//   }
//
//   Test_F(ClassName, TestName) {
//      ...
//   }
//
//   Assertions:
//     EXPECT_TRUE(X)  : expect x to be true
//     EXPECT_FALSE(X) : expect x to be false
//
//     EXPECT_EQ(X, Y) : expect x == y
//     EXPECT_NE(X, Y) : expect x != y
//     EXPECT_GT(X, Y) : expect x >  y
//     EXPECT_GE(X, Y) : expect x >= y
//     EXPECT_LT(X, Y) : expect x <  y
//     EXPECT_LE(X, Y) : expect x <= y
//
//     EXPECT_NO_THROW({ BLOCK })         : expect no exception
//     EXPECT_THROW({ BLOCK }, EXCEPTION) : expect exception

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define RUN_ALL_TESTS() cyrus::TestManager::GetInstance()->RunAll()

#define _TEST(BaseClass, GroupName, TestName) \
class TestName : public BaseClass { \
public: \
  TestName() { \
    cyrus::TestManager::GetInstance()->Register( \
      #GroupName, cyrus::TestEntry(#TestName, this)); \
  } \
  void Do() override; \
}; \
TestName test_##GroupName_##TestName; \
void TestName::Do()

#define TEST(GroupName, TestName) _TEST(cyrus::Test, GroupName, TestName)
#define TEST_F(BaseClass, TestName) _TEST(BaseClass, BaseClass, TestName)

#define _PRINT_FILE_AND_LINE \
  std::cout << "\nError at " << __FILE__ << ":" << __LINE__ << "\n"; \

#define _EXPECT_SINGLE(X, EXPRESSION, TEXT) { \
  bool x = X; \
  if (!(EXPRESSION)) { \
    _PRINT_FILE_AND_LINE \
    std::cout << "Expected " << #X << " to be " << TEXT << ".\n"; \
    std::cout << "Got " #X " = " << x << "\n"; \
    throw cyrus::TestFailed(); \
  } \
}

#define EXPECT_TRUE(X) _EXPECT_SINGLE(X, x, "true")
#define EXPECT_FALSE(X) _EXPECT_SINGLE(X, !x, "false")

#define _EXPECT_PAIR(X, Y, EXPRESSION, TEXT) { \
  auto x = X; \
  auto y = Y; \
  if (!(EXPRESSION)) { \
    _PRINT_FILE_AND_LINE \
    std::cout << "Expected " #X " to be " TEXT " " #Y ".\n"; \
    std::cout << "Got " #X " = " << x << "\n"; \
    std::cout << "Got " #Y " = " << y << "\n"; \
    throw cyrus::TestFailed(); \
  } \
}

#define EXPECT_EQ(X, Y) _EXPECT_PAIR(X, Y, x == y, "equal to");
#define EXPECT_NE(X, Y) _EXPECT_PAIR(X, Y, x != y, "different from");
#define EXPECT_GT(X, Y) _EXPECT_PAIR(X, Y, x > y, "greather than");
#define EXPECT_GE(X, Y) _EXPECT_PAIR(X, Y, x >= y, "greather or equal to");
#define EXPECT_LT(X, Y) _EXPECT_PAIR(X, Y, x < y, "less than");
#define EXPECT_LE(X, Y) _EXPECT_PAIR(X, Y, x <= y, "less or equal to");

#define EXPECT_NO_THROW(EXPRESSION, EXCEPTION) { \
  try { \
    EXPRESSION; \
  } catch (EXCEPTION&) { \
    _PRINT_FILE_AND_LINE \
    std::cout << "Exception " #EXCEPTION " was thrown."; \
    throw cyrus::TestFailed(); \
  } \
}

#define EXPECT_THROW(EXPRESSION, EXCEPTION) { \
  try { \
    EXPRESSION; \
    _PRINT_FILE_AND_LINE \
    std::cout << "Exception " #EXCEPTION " was thrown."; \
    throw cyrus::TestFailed(); \
  } catch (EXCEPTION&) { \
  } \
}

namespace cyrus {

class TestFailed : public std::exception {};

class Test {
 public:
  Test() {}

  virtual void Setup() {}
  virtual void Do() = 0;
  virtual void TearDown() {}
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
    std::cout << "Found " << entry.name << ".\n";
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
