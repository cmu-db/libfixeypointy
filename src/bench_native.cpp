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

#define UNUSED_ATTRIBUTE __attribute__((unused))

void Bench(const std::string &mode, const int trials, const int skip, const int iterations,
           const std::string &bench_file) {
  auto t_op_start = std::chrono::high_resolution_clock::now();
  auto t_op_end = std::chrono::high_resolution_clock::now();

  for (int trial = 0; trial < trials; ++trial) {
    std::ifstream in(bench_file);
    std::string op1_s;
    std::string op2_s;
    int64_t init_time = 0;
    int64_t op_time = 0;

    while (in >> op1_s >> op2_s) {
      std::vector<double> op1_arr;
      std::vector<double> op2_arr;
      op1_arr.resize(iterations);
      op2_arr.resize(iterations);

      auto t_init_start = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        op1_arr[i] = std::stod(op1_s);
        op2_arr[i] = std::stod(op2_s);
      }
      auto t_init_end = std::chrono::high_resolution_clock::now();

      if (mode == "add") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] += op2_arr[i];
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "sub") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] -= op2_arr[i];
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "mlt") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] *= op2_arr[i];
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "div") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i] /= op2_arr[i];
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else {
        std::cout << "Unsupported benchmark mode" << std::endl;
      }

      auto init_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t_init_end - t_init_start);
      auto op_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t_op_end - t_op_start);
      init_time += init_duration.count();
      op_time += op_duration.count();
    }

    if (trial >= skip) {
      init_time /= 1000;
      op_time /= 1000;
      std::cout << init_time << "," << op_time << std::endl;
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
