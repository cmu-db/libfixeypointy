#include <stdio.h>
#include <string>
#include <vector>

#include "decimal.h"
#include "gtest/gtest.h"

namespace libfixeypointy {

// NOLINTNEXTLINE
TEST(BasicTests, ToStringTest) {
  for (int i = 0; i < (int)Decimal::MAX_SCALE - 1; i++) {
    std::string expected = "9.";
    for (int j = 0; j <= i; j++) {
      expected += std::to_string(j % 10);
    }
    Decimal d(expected, i + 1);
    auto result = d.ToString(i + 1);
    EXPECT_EQ(result, expected);
  }
}

// NOLINTNEXTLINE
TEST(BasicTests, ToStringLeadingZeroTest) {
  // Make sure we always include a leading zero even if the number is negative
  std::vector<std::string> strings = {"0.001", "-0.001"};
  for (auto expected : strings) {
    Decimal d(expected, 3);
    auto result = d.ToString(3);
    EXPECT_EQ(result, expected);
  }
}

// NOLINTNEXTLINE
TEST(BasicTests, NegationTest) {
  std::vector<std::string> strings = {"0.001", "-0.001"};
  int scale = 3;
  for (auto val : strings) {
    Decimal orig(val, scale);
    auto result = orig.GetNegation();
    std::string expected = (val[0] == '-' ? val.substr(1) : "-" + val);
    EXPECT_EQ(result.ToString(scale), expected);
  }
}

// NOLINTNEXTLINE
TEST(BasicTests, AbsTest) {
  std::vector<std::string> strings = {"99990.09999", "-99990.09999"};
  int scale = 5;
  for (auto val : strings) {
    Decimal orig(val, scale);
    auto result = orig.GetAbs();
    std::string expected = strings[0];
    EXPECT_EQ(result.ToString(scale), expected);
  }
}

// NOLINTNEXTLINE
TEST(BasicTests, AdditionTest) {
  std::string expected = "3.3";
  for (int i = 1; i < (int)Decimal::MAX_SCALE - 1; i++) {
    Decimal d0("1.2", i);
    Decimal d1("2.1", i);
    d0 += d1;
    auto result = d0.ToString(i);
    EXPECT_EQ(result, expected);
    expected += "0";
  }
}

// NOLINTNEXTLINE
TEST(BasicTests, SubtractionTest) {
  std::string expected = "0.0";
  for (int i = 1; i < (int)Decimal::MAX_SCALE - 1; i++) {
    std::string value = "9.";
    for (int j = 0; j <= i; j++) {
      value += "9";
    }
    Decimal d0(value, i);
    Decimal d1(value, i);
    d0 -= d1;
    auto result = d0.ToString(i);
    EXPECT_EQ(result, expected);
    expected += "0";
  }
}

}  // namespace libfixeypointy
