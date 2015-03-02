# minimal-cpp-test

Minimal C++ unit test single header library by Vahid Kazemi.

Usage:
```cpp
// Define in only one source file or alternatively call RUN_ALL_TESTS()
#define DEFINE_TEST_MAIN

#include "test.h"
```

Simple test:
```cpp
TEST(GroupName, TestName) {
  EXPECT_EQ(1 + 2, 3);
}
```

Fixtures:
```cpp
class ClassName : public cyrus:Test {
public:
  // Perform initialization
  void Setup() override { 
    x = 5;
    ... 
  }
  
  // Free resources
  void TearDown() override { ... }
  
  // declare member variables and functions
  int x;
  ...
}

Test_F(ClassName, Test1) {
  // Setup is called already here
  EXPECT_EQ(x, 5);
  ...
}

Test_F(ClassName, Test2) {
  // Setup is called already here
  ...
}

```

Assertions:
```cpp
EXPECT_TRUE(X)  // expect x to be true
EXPECT_FALSE(X) // expect x to be false

EXPECT_EQ(X, Y) // expect x == y
EXPECT_NE(X, Y) // expect x != y
EXPECT_GT(X, Y) // expect x >  y
EXPECT_GE(X, Y) // expect x >= y
EXPECT_LT(X, Y) // expect x <  y
EXPECT_LE(X, Y) // expect x <= y

EXPECT_NO_THROW({ BLOCK })         // expect no exception
EXPECT_THROW({ BLOCK }, EXCEPTION) // expect exception
```
