#include <iostream>
#include <string>
#include <utility>
#include <algorithm>

#include "decimal.h"

#define DECIMAL_DELIMITER ":"

struct DemoDecimal {
  Decimal decimal;
  std::string orig_value;
  int orig_scale;
};

DemoDecimal parse_decimal(std::stringstream &input) {
  std::string val;
  input >> val;

  auto position = val.find(DECIMAL_DELIMITER);
  if (position == std::string::npos) {
    throw std::invalid_argument("Invalid decimal format '" + val + "'");
  }
  auto decimal = val.substr(0, position + 1);
  auto scale = std::stoi(val.substr(position + 1));

  DemoDecimal result{ Decimal(decimal, scale), decimal, scale };
  return result;
}

Decimal compute_result(const Decimal &decimal1, const Decimal &decimal2, const std::string &op) {
  Decimal result(decimal1);

  // Addition
  if (op == "+") {
    result += decimal2;
  }
  // Subtraction
  else if (op == "-") {

  }
  // Multiplication
  else if (op == "*") {

  }
  // Division
  else if (op == "/") {

  }
  // Unexpected!
  else {
    throw std::invalid_argument("Invalid operation '" + op + "'");
  }

  return result;
}


int main(int argc, char *argv[]) {
  for (std::string line; std::getline(std::cin, line);) {
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    if (line == "quit") {
      break;
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
    std::string equals;
    input >> equals;
    if (equals != "=") {
      throw std::invalid_argument("Invalid operation format " + line);
    }

    // Expected Result Decimal
    auto expected = parse_decimal(input);

    // Execute!
    auto result = compute_result(val1.decimal, val2.decimal, op);
    std::cout << "Result: " << result.ToString(expected.orig_scale) << std::endl;
    break;
  }
  return 0;


}