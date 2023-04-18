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

void Bench(const std::string &mode, const int trials, const int skip, const int iterations, const int precision,
           const std::string &op1, const std::string &op2) {
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();
  int base = 10;

  for (int trial = 0; trial < trials; ++trial) {
    mpf_class ret(op1, precision, base);
    mpf_class op(op2, precision, base);

    if (mode == "add") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret += op;
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "sub") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret -= op;
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "mlt") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret *= op;
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "div") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret /= op;
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else {
      std::cout << "Unsupported benchmark mode" << std::endl;
    }

    if (trial >= skip) {
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
      std::cout << duration.count() << std::endl;
    }
  }
}

int main(UNUSED_ATTRIBUTE int argc, UNUSED_ATTRIBUTE char *argv[]) {
  std::string mode = argv[1];
  int trials = atoi(argv[2]);
  int skip = atoi(argv[3]);
  int iterations = atoi(argv[4]);
  int precision = atoi(argv[5]);
  std::string op1 = argv[6];
  std::string op2 = argv[7];

  Bench(mode, trials, skip, iterations, precision, op1, op2);

  return 0;
}
