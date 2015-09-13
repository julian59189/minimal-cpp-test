// Minimal C++ logging single header library.
// Author: Vahid Kazemi
//
// Usage:
//   #include "logging.h"
//
//   LOG(INFO) << "Message.";
//   CHECK(CONDITION) << "Message."

#pragma once

#include <exception>
#include <iostream>

enum LoggingLevel {
  INFO,
  WARNING,
  ERROR,
  FATAL
};

#define LOG(LEVEL) \
  cyrus::LoggingStream(LEVEL) \
  << __FILE__ << ":" << __LINE__ << "]: "

#define _CYRUS_CAT_HELPER(X, Y) X ## Y
#define _CYRUS_CAT(X, Y) _CYRUS_CAT_HELPER(X, Y)
#define _CYRUS_UNAME(X) _CYRUS_CAT(X, __LINE__)

#define _CYRUS_CHECK_SINGLE(X, EXPRESSION, TEXT) \
  bool _CYRUS_UNAME(x) = X; \
  EXPRESSION ? (void)0 : cyrus::VoidifyLoggingStream() & \
  LOG(FATAL) << "Expected " #X " to be " TEXT ".\n" \
             << "Got " #X " = " << _CYRUS_UNAME(x) << "\n"

#define CHECK_TRUE(X) _CYRUS_CHECK_SINGLE(X, _CYRUS_UNAME(x), "true")
#define CHECK_FALSE(X) _CYRUS_CHECK_SINGLE(X, !_CYRUS_UNAME(x), "false")
#define CHECK(X) CHECK_TRUE(X)

#define _CYRUS_CHECK_PAIR(X, Y, EXPRESSION, TEXT) \
  auto _CYRUS_UNAME(x) = X; \
  auto _CYRUS_UNAME(y) = Y; \
  EXPRESSION ? (void)0 : cyrus::VoidifyLoggingStream() & \
  LOG(FATAL) << "Expected " #X " to be " TEXT " " #Y ".\n" \
             << "Got " #X " = " << _CYRUS_UNAME(x) << "\n" \
             << "Got " #Y " = " << _CYRUS_UNAME(y) << "\n"

#define CHECK_EQ(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) == _CYRUS_UNAME(y), "equal to")
#define CHECK_NE(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) != _CYRUS_UNAME(y), "different from")
#define CHECK_GT(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) > _CYRUS_UNAME(y), "greather than")
#define CHECK_GE(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) >= _CYRUS_UNAME(y), "greather or equal to")
#define CHECK_LT(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) < _CYRUS_UNAME(y), "less than")
#define CHECK_LE(X, Y) \
  _CYRUS_CHECK_PAIR(X, Y, _CYRUS_UNAME(x) <= _CYRUS_UNAME(y), "less or equal to")

namespace cyrus {

class LoggingStream {
 public:
  LoggingStream(LoggingLevel level) : level_(level) {
    std::cout << "[" << GetLevelString() << ":";
  }
  ~LoggingStream() {
    std::cout << std::endl;
    if (level_ == FATAL) {
      exit(0);
    }
  }

  template<typename T>
  LoggingStream& operator<<(T val) {
    std::cout << val;
    return *this;
  }

  const char* GetLevelString() {
    static const char* level_string_[] = {
      "INFO", "WARNING", "ERROR", "FATAL_ERROR"
    };
    return level_string_[level_];
  }

 private:
  LoggingLevel level_;
};

class VoidifyLoggingStream {
 public:
  VoidifyLoggingStream() { }

  void operator&(const LoggingStream&) { }
};

}
