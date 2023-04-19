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
#include "gmp.h"
#include "gmpxx.h"

#define UNUSED_ATTRIBUTE __attribute__((unused))

void Bench(const std::string &mode, const int trials, const int skip, const int precision, const int iterations,
           const std::string &bench_file) {
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();
  int base = 10;

  for (int trial = 0; trial < trials; ++trial) {
    std::ifstream in(bench_file);
    std::string op1_s;
    std::string op2_s;
    int64_t time = 0;

    while (in >> op1_s >> op2_s) {
      std::vector<mpf_class> op1_arr;
      std::vector<mpf_class> op2_arr;
      for (int i = 0; i < iterations; ++i) {
        op1_arr.push_back(mpf_class(op1_s, precision, base));
        op2_arr.push_back(mpf_class(op2_s, precision, base));
      }

      if (mode == "add") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] += op2_arr[i];
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "sub") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] -= op2_arr[i];
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "mlt") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] *= op2_arr[i];
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else if (mode == "div") {
        t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] /= op2_arr[i];
        }
        t2 = std::chrono::high_resolution_clock::now();
      } else {
        std::cout << "Unsupported benchmark mode" << std::endl;
      }

      // use variable so it doesn't get optimized away
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
  int precision = atoi(argv[4]);
  int iterations = atoi(argv[5]);
  std::string bench_file = argv[6];

  Bench(mode, trials, skip, precision, iterations, bench_file);

  return 0;
}
