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
  auto t_op_start = std::chrono::high_resolution_clock::now();
  auto t_op_end = std::chrono::high_resolution_clock::now();

  for (int trial = 0; trial < trials; ++trial) {
    std::ifstream in(bench_file);
    std::string op1_s;
    std::string op2_s;
    int64_t init_time = 0;
    int64_t op_time = 0;

    while (in >> op1_s >> op2_s) {
      std::vector<Decimal *> op1_arr;
      std::vector<Decimal *> op2_arr;
      op1_arr.resize(iterations);
      op2_arr.resize(iterations);
      Decimal::ScaleType op1_scale = 0;
      Decimal::ScaleType op2_scale = 0;

      auto t_init_start = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < iterations; ++i) {
        op1_arr[i] = new Decimal(op1_s, &op1_scale);
        op2_arr[i] = new Decimal(op2_s, &op2_scale);
      }
      auto t_init_end = std::chrono::high_resolution_clock::now();

      if (mode == "add") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i]->Add(*op2_arr[i]);
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "sub") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i]->Subtract(*op2_arr[i]);
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "mlt") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i]->Multiply(*op2_arr[i], op2_scale);
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else if (mode == "div") {
        t_op_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
          op1_arr[i]->Divide(*op2_arr[i], op2_scale);
        }
        t_op_end = std::chrono::high_resolution_clock::now();
      } else {
        std::cout << "Unsupported benchmark mode" << std::endl;
      }

      auto init_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_init_end - t_init_start);
      auto op_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_op_end - t_op_start);
      init_time += init_duration.count();
      op_time += op_duration.count();
    }

    if (trial >= skip) {
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
