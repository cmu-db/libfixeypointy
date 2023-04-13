#include <stdio.h>
#include <string>
#include <vector>

#include "decimal.h"
#include "gtest/gtest.h"

namespace libfixeypointy {

// NOLINTNEXTLINE
TEST(ParsingTest, EmptyInputTest) {
  bool is_catched = false;
  Decimal::ScaleType scale;

  try {
    Decimal d("", &scale);
  } catch (const std::runtime_error& error) {
    is_catched = true;
  }
  
  EXPECT_TRUE(is_catched);
}

TEST(ParsingTest, IntegralDigitOverflowTest) {
  bool is_catched = false;
  std::string in = "";
  Decimal::ScaleType scale;

  try {
    for (int i = 0; i < 38; i++) {
      in += "1";
      Decimal d(in, &scale);
    }
  } catch (const std::runtime_error& error) {
    is_catched = true;
  }

  EXPECT_FALSE(is_catched);
  
  // This is the 39th digit
  // It should be overflowed
  in += "1";

  try {
    Decimal d(in, &scale);
  } catch (const std::runtime_error& error) {
    is_catched = true;
  }
  
  EXPECT_TRUE(is_catched);
}

// TEST(ParsingTest, IgnoreLeadingZeroTest) {

// }

// TEST(ParsingTest, IgnoreTrailingZeroTest) {

// }

}  // namespace libfixeypointy
