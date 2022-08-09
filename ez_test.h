/**
 * ez_test
 *
 * A woefully primitive "unit" testing "framework". C++03 approved.
 *
 * ---
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef EZ_TEST_H
#define EZ_TEST_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdexcept>

namespace test {

class TestContext;

typedef void(*TestFunction)(TestContext&);

/**
 * Stopwatch
 *
 * A crude stopwatch. The stopwatch is in a reset and paused state upon
 * construction. elapsed() can be called at any time to check the current
 * duration.
 */
class Stopwatch {
public:
  /**
   * Starts (unpauses) the stopwatch.
   */
  void start() {
    if (intervals.size() > 0 && intervals.back().running) {
      throw std::runtime_error("Stopwatch: start() called while stopwatch is running");
    }
    Interval iv = {
      .tStart = clock(),
      .tStop = 0,
      .running = true,
    };
    intervals.push_back(iv);
  }

  /**
   * Stops (pauses) the stopwatch. After the call, stopwatch can be resumed by
   * calling start() or reset by calling reset().
   */
  void stop() {
    if (intervals.size() == 0 || !intervals.back().running) {
      throw std::runtime_error("Stopwatch: stop() called while stopwatch is not running");
    }
    intervals.back().running = false;
    intervals.back().tStop = clock();
  }

  /**
   * Resets the stopwatch. Can be called at any time.
   */
  void reset() { intervals.clear(); }

  /**
   * Returns the approximate duration the stopwatch has been in the running state
   * since the last reset.
   */
  clock_t elapsed() const {
    clock_t sum = 0;
    std::vector<Interval>::const_iterator it;
    for (it = intervals.begin(); it != intervals.end(); ++it) {
      sum += it->elapsed();
    }
    return sum;
  }

private:
  struct Interval {
    clock_t tStart;
    clock_t tStop;
    bool running;
    clock_t elapsed() const {
      if (running)
        return (clock() - tStart) * 1000 / CLOCKS_PER_SEC;
      return (tStop - tStart) * 1000 / CLOCKS_PER_SEC;
    }
  };

  std::vector<Interval> intervals;
};

/**
 * TestContext
 *
 * Create a TestContext to measure the number of tests that succeed and fail.
 *
 * Example:
 *     TestContext cx;
 *     cx.test("This test should pass", [](_cx) { _cx.expectEqual(1, 1); });
 *     cx.test("This test should fail", [](_cx) { _cx.expectEqual(0, 1); });
 *     cx.printResults();
 */
class TestContext {
public:
  TestContext(std::ostream& out=std::cout)
    : out(out),
      assertionNum(1),
      successCt(0),
      failedCt(0),
      currentAssertionsFailed(0)
  { }

  /**
   * If actual == theoretical, does nothing. Otherwise, increments the tests failed count.
   * Returns the result of `actual == theoretical`.
   */
  template <typename T, typename U>
  bool expectEqual(const T& actual, const U& theoretical);

  /**
   * Runs the func and prints the results, including the time elapsed.
   */
  void test(std::string name, TestFunction func);

  /**
   * Prints the number of tests run and number of tests that failed.
   */
  void printResults() const;

private:
  // incremented for every expectXXX() call
  int assertionNum;

  // totals for passed and failed assertions
  int successCt;
  int failedCt;

  // Keeps track of the number of failed expectXXX() calls in the current test() call.
  int currentAssertionsFailed;

  std::ostream& out;
  Stopwatch watch;
};

#ifdef EZ_DEFINE_VECTOR_OUTPUT

  /**
  * Output a vector.
  */
  template <typename T>
  std::ostream& operator<<(std::ostream& out, const std::vector<T> v);

#endif // EZ_DEFINE_VECTOR_OUTPUT

// --------------------------

template <typename T, typename U>
bool TestContext::expectEqual(const T& actual, const U& theoretical) {
  watch.stop();
  bool result = true;
  if (theoretical != actual) {
    if (currentAssertionsFailed < 6) {
      std::cout << "\n  FAILED [" << assertionNum << "]: expected "
                << theoretical << ", got " << actual << std::endl;
    }
    result = false;
    failedCt++;
    currentAssertionsFailed++;
  }
  else {
    successCt++;
  }
  assertionNum++;
  watch.start();
  return result;
}

void TestContext::test(std::string name, TestFunction func) {
  currentAssertionsFailed = 0;
  std::cout << name << "...";
  std::cout.flush();
  watch.start();
  func(*this);
  watch.stop();
  if (currentAssertionsFailed > 5) {
    out << "[" << (currentAssertionsFailed-5) << " other failures omitted]\n";
  }

  if (currentAssertionsFailed == 0) {
    out << " PASS (" << watch.elapsed() << " ms)\n";
  }
  else {
    out << name << "... FAIL (" << watch.elapsed() << " ms)\n";
  }
  watch.reset();
}

void TestContext::printResults() const {
  out << "===================================\n"
      << "ASSERTIONS FAILED:    " << std::setw(7) << failedCt << '\n'
      << "ASSERTIONS MADE:      " << std::setw(7) << (failedCt + successCt) << '\n'
      << "===================================\n";
}

} // namespace test

#ifdef EZ_DEFINE_VECTOR_OUTPUT

  template <typename T>
  std::ostream& operator<<(std::ostream& out, const std::vector<T> v) {
    size_t len = v.size();
    out << '{';
    for (size_t i = 0; i < len; i++) {
      out << v[i];
      if (i < len - 1) {
        out << ',';
      }
    }
    out << '}';

    return out;
  }

#endif // EZ_DEFINE_VECTOR_OUTPUT

#endif
