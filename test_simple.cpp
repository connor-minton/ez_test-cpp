#include "ez_test.h"

using namespace test;

void passTest(TestContext& cx) {
  cx.expectEqual(1, 1);
}

void failTest(TestContext& cx) {
  cx.expectEqual(0, 1);
}

void slowTest(TestContext& cx) {
  int sum = 0;
  for (int i = 0; i < 10000; i++) {
    for (int j = 0; j < 10000; j++) {
      sum += i - j;
    }
  }
  cx.expectEqual(sum, 0);
}

int main() {
  TestContext cx;
  cx.test("This test should pass", passTest);
  cx.test("This test should fail", failTest);
  cx.test("This test should take a while", slowTest);
  cx.printResults();
}
