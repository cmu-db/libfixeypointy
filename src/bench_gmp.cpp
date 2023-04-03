#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <stdarg.h>
#include <stdio.h>
#include "gmp.h"
#include "gmpxx.h"

#define UNUSED_ATTRIBUTE __attribute__((unused))

void AdditionBench() {
  int iterations = 1000000;
  int precision = 128;
  int base = 10;

  mpf_class gmp_ret("1.0", precision, base);
  mpf_class gmp_add("0.000000000000001", precision, base);

  auto t1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    gmp_ret += gmp_add;
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  auto gmp_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

  std::cout << "GMP: " << gmp_duration.count() << " us ";
  std::cout << std::setprecision(20);
  std::cout << gmp_ret.get_d() << std::endl;
}

void MultiplicationBench() {
  int iterations = 1000000;
  int precision = 128;
  int base = 10;

  mpf_class gmp_ret("1.0000", precision, base);
  mpf_class gmp_mult("1.00001", precision, base);
  // mpf_class gmp_div("12", precision, base);

  auto t1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; ++i) {
    gmp_ret *= gmp_mult;
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  auto gmp_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

  std::cout << "GMP: " << gmp_duration.count() << " us ";
  std::cout << std::setprecision(20);
  std::cout << gmp_ret.get_d() << std::endl;
}

int main(UNUSED_ATTRIBUTE int argc, UNUSED_ATTRIBUTE char *argv[]) {
  int trials = 5;
  for (int trial = 1; trial <= trials; ++trial) {
    AdditionBench();
    // MultiplicationBench();
  }

  return 0;
}
