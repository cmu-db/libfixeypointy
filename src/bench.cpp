#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <stdarg.h>
#include <stdio.h>
#include "decimal.h"

using namespace libfixeypointy;

#define UNUSED_ATTRIBUTE __attribute__((unused))

void Bench(const std::string &mode, const int trials, const int skip, const int iterations,
           const std::string &bench_file) {
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();

  for (int trial = 0; trial < trials; ++trial) {
    std::ifstream in(bench_file);
    std::string op1_s;
    std::string op2_s;
    int64_t time = 0;

    while (in >> op1_s >> op2_s) {
      std::vector<Decimal> op1_arr;
      std::vector<Decimal> op2_arr;
      Decimal::ScaleType op1_scale = 0;
      Decimal::ScaleType op2_scale = 0;
      for (int i = 0; i < iterations; ++i) {
        op1_arr.push_back(Decimal(op1_s, &op1_scale));
        op2_arr.push_back(Decimal(op2_s, &op2_scale));
      }

      if (mode == "add") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i].Add(op2_arr[i]);
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "sub") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i].Subtract(op2_arr[i]);
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "mlt") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i].Multiply(op2_arr[i], op2_scale);
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "div") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i].Divide(op2_arr[i], op2_scale);
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else {
        std::cout << "Unsupported benchmark mode" << std::endl;
      }

      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
      time += duration.count();
    }

    if (trial >= skip) {
      std::cout << time << std::endl;
    }
  }
}

int main(UNUSED_ATTRIBUTE int argc, UNUSED_ATTRIBUTE char *argv[]) {
  std::string mode = argv[1];
  int trials = atoi(argv[2]);
  int skip = atoi(argv[3]);
  int iterations = atoi(argv[4]);
  std::string bench_file = argv[5];

  Bench(mode, trials, skip, iterations, bench_file);

  return 0;
}
