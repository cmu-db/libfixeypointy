#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <chrono>

#include "decimal.h"

#define DECIMAL_DELIMITER ":"
#define UNUSED_ATTRIBUTE __attribute__((unused))

using namespace libfixeypointy;
using namespace std::chrono;

struct DemoDecimal {
  Decimal *decimal = nullptr;
  std::string orig_value;
  uint32_t orig_scale;

  ~DemoDecimal() {
    if (decimal != nullptr) delete decimal;
  }
};

/**
 *
 * @param input
 * @return
 */
DemoDecimal parse_decimal(std::stringstream &input) {
  std::string val;
  input >> val;

  auto position = val.find(DECIMAL_DELIMITER);

  // Decimal String
  auto decimal = val.substr(0, position);

  // If there is no delimiter, then we will just use '1' for the scale
  uint32_t scale = Decimal::DEFAULT_SCALE;
  if (position != std::string::npos) {
    scale = std::stoi(val.substr(position + 1));
  }

  DemoDecimal result{new Decimal(decimal, scale), decimal, scale};
  return result;
}

/**
 *
 * @param decimal1
 * @param decimal2
 * @param op
 * @param scale
 * @return
 */
void compute_result(DemoDecimal &decimal1, DemoDecimal &decimal2, const std::string &op, const uint32_t scale) {
  Decimal result(*decimal1.decimal);

  double duration = 0.0;

  // if (op == "+") {
  //   result += *decimal2.decimal;
  // }
  // // Subtraction
  // else if (op == "-") {
  //   result -= *decimal2.decimal;
  // }
  // // Multiplication
  // else if (op == "*") {
  //   result.Multiply(*decimal2.decimal, scale);
  // }
  // // Division
  // else if (op == "/") {
  //   result.Divide(*decimal2.decimal, scale);
  // }

  // if (op == "+") {
  //   result += *decimal2.decimal;
  // }
  // // Subtraction
  // else if (op == "-") {
  //   result -= *decimal2.decimal;
  // }
  // // Multiplication
  // else if (op == "*") {
  //   result.Multiply(*decimal2.decimal, scale);
  // }
  // // Division
  // else if (op == "/") {
  //   result.Divide(*decimal2.decimal, scale);
  // }

  // Addition
  if (op == "+") {
    auto start = high_resolution_clock::now();
    result += *decimal2.decimal;
    auto end = high_resolution_clock::now();

    auto gap = duration_cast<nanoseconds>(end - start);
    duration = gap.count();
  }
  // Subtraction
  else if (op == "-") {
    auto start = high_resolution_clock::now();
    result -= *decimal2.decimal;
    auto end = high_resolution_clock::now();

    auto gap = duration_cast<nanoseconds>(end - start);
    duration = gap.count();
  }
  // Multiplication
  else if (op == "*") {
    auto start = high_resolution_clock::now();
    result.Multiply(*decimal2.decimal, scale);
    auto end = high_resolution_clock::now();

    auto gap = duration_cast<nanoseconds>(end - start);
    duration = gap.count();
  }
  // Division
  else if (op == "/") {
    auto start = high_resolution_clock::now();
    result.Divide(*decimal2.decimal, scale);
    auto end = high_resolution_clock::now();

    auto gap = duration_cast<nanoseconds>(end - start);
    duration = gap.count();
  }
  // Unexpected!
  else {
    throw std::invalid_argument("Invalid operation '" + op + "'");
  }

  std::cout << ">>> " << decimal1.decimal->ToString(scale) << " " << op << " " << decimal2.decimal->ToString(scale)
            << " = " << result.ToString(scale) << " ";
  std::cout << duration;
  // if (has_equals) {
  //   //      [EXPECTED:" << expected.orig_value << "]" << std::endl;
  // }
  std::cout << std::endl;
}

int main(UNUSED_ATTRIBUTE int argc, UNUSED_ATTRIBUTE char *argv[]) {
  std::string equals;
  // bool has_equals = false;
  DemoDecimal expected;

  for (std::string line; std::getline(std::cin, line);) {
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    if (line == "quit") {
      break;
    } else if (line.empty()) {
      continue;
    }

    // INPUT FORMAT: <DECIMAL>:<SCALE> <OP> <DECIMAL>:<SCALE> = <DECIMAL>:<SCALE>
    std::stringstream input(line);

    // Decimal #1
    auto val1 = parse_decimal(input);

    // Operator
    std::string op;
    input >> op;

    // Decimal #2
    auto val2 = parse_decimal(input);

    // Equals

    // has_equals = false;
    input >> equals;
    if (!equals.empty()) {
      if (equals != "=") {
        throw std::invalid_argument("Invalid operation format '" + line + "'");
      }
      // Expected Result Decimal
      expected = parse_decimal(input);
      // has_equals = true;
    }

    // Execute!
    uint32_t new_scale = Decimal::MatchScales(val1.decimal, val2.decimal, val1.orig_scale, val2.orig_scale);

    compute_result(val1, val2, op, new_scale);

    //    break;
  }
  return 0;
}
