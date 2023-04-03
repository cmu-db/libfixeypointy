#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <stdarg.h>
#include <stdio.h>
#include "decimal.h"

using namespace libfixeypointy;

#define UNUSED_ATTRIBUTE __attribute__((unused))

void AdditionBench() {
  int iterations = 1000000;
  int scale = 15;

  auto lib_ret = Decimal("1.000000000000000", scale);
  auto lib_add = Decimal("0.000000000000001", scale);

  auto t3 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    lib_ret.Add(lib_add);
  }
  auto t4 = std::chrono::high_resolution_clock::now();
  auto lib_duration = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);

  std::cout << "libfixeypointy: " << lib_duration.count() << " us ";
  std::cout << "result: " << lib_ret.ToString(scale) << std::endl;
}

void MultiplicationBench() {
  int iterations = 1000000;
  int scale = 15;

  auto lib_ret = Decimal("1.000000000000000", scale);
  // auto lib_mult = Decimal("1.00001", scale);

  auto t3 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    // lib_ret.Multiply(lib_mult, scale);
    lib_ret.MultiplyByConstant(100001);
    lib_ret.DivideByConstant(100000);
  }
  auto t4 = std::chrono::high_resolution_clock::now();
  auto lib_duration = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);

  std::cout << "libfixeypointy: " << lib_duration.count() << " us ";
  std::cout << "result: " << lib_ret.ToString(scale) << std::endl;
}

int main(UNUSED_ATTRIBUTE int argc, UNUSED_ATTRIBUTE char *argv[]) {
  int trials = 5;

  for (int trial = 1; trial <= trials; ++trial) {
    AdditionBench();
    // MultiplicationBench();
  }

  return 0;
}