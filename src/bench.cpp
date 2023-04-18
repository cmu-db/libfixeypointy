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

void Bench(const std::string &mode, const int trials, const int skip, const int iterations, const int scale,
           const std::string &op1, const std::string &op2) {
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();

  for (int trial = 0; trial < trials; ++trial) {
    auto ret = Decimal(op1, scale);
    auto op = Decimal(op2, scale);

    if (mode == "add") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret.Add(op);
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "sub") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret.Subtract(op);
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "mlt") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret.Multiply(op, scale);
      }
      t2 = std::chrono::high_resolution_clock::now();
    } else if (mode == "div") {
      t1 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        ret.Divide(op, scale);
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
  int scale = atoi(argv[5]);
  std::string op1 = argv[6];
  std::string op2 = argv[7];

  Bench(mode, trials, skip, iterations, scale, op1, op2);

  return 0;
}