#include <stdio.h>
#include <string>
#include <vector>

#include "decimal.h"
#include "gtest/gtest.h"

namespace libfixeypointy {

// NOLINTNEXTLINE
TEST(BasicTests, DISABLED_MixedScaleTest) {
  int scale0 = Decimal::MAX_SCALE - 1;
  std::string val0 = "9." + std::string(scale0, '9');
  for (int scale1 = 1; scale1 < (int)Decimal::MAX_SCALE - 1; scale1++) {
    std::string val1 = "5." + std::string(scale1, '5');
    std::string expected = "4." + std::string(scale1, '4') + std::string((int)Decimal::MAX_SCALE - scale1, '5');

    Decimal d0(val0, scale0);
    Decimal d1(val1, scale1);
    Decimal::MatchScales(&d0, &d1, scale0, scale1);
    d0 -= d1;
    auto result = d0.ToString(scale0);
    EXPECT_EQ(result, expected);
  }
}

}  // namespace libfixeypointy
