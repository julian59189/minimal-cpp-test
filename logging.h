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
  cyrus::LoggingStream(LEVEL).Stream(__FILE__, __LINE__)

#define _CYRUS_LOG_STATEMENT(LEVEL, STATEMENT) \
  cyrus::LoggingStream(LEVEL).Stream(__FILE__, __LINE__, STATEMENT)

#define CHECK(STATEMENT) \
  (STATEMENT) ? (void)0 : cyrus::VoidifyLoggingStream() & \
  _CYRUS_LOG_STATEMENT(FATAL, #STATEMENT)

namespace cyrus {

class LoggingStream {
 public:
  LoggingStream(LoggingLevel level) : level_(level) {}
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

  LoggingStream& Stream(const std::string& filename, int line) {
    std::cout << "Error at " << filename << ":" << line << "\n";
    return *this;
  }

  LoggingStream& Stream(const std::string& filename, int line,
    const std::string& statement) {
    std::cout << GetLevelString()
              << ": at " << filename << ":" << line
              << " while evaluating " << statement << ".\n";
    return *this;
  }

  const char* GetLevelString() {
    static const char* level_string_[] = {
      "Info", "Warning", "Error", "Fatal error"
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
