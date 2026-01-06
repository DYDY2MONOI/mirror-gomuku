#pragma once

#include <chrono>

class TimeManager {
public:
  using Clock = std::chrono::steady_clock;

  void start(std::chrono::milliseconds budget) {
    if (budget.count() < 0) {
      budget = std::chrono::milliseconds(0);
    }
    deadline_ = Clock::now() + budget;
  }

  bool expired() const { return Clock::now() >= deadline_; }

private:
  Clock::time_point deadline_ = Clock::time_point::min();
};

