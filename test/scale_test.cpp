#include <stdio.h>
#include <string>
#include <vector>

#include "decimal.h"
#include "gtest/gtest.h"

namespace libfixeypointy {

// NOLINTNEXTLINE
TEST(BasicTests, MixedScaleTest) {
  int scale0 = Decimal::MAX_SCALE - 1;
  std::string val0 = "9." + std::string(scale0, '9');

  for (int scale1 = 1; scale1 < (int)Decimal::MAX_SCALE; scale1++) {
    std::string val1 = "5." + std::string(scale1, '5');
    std::string expected = "4." + std::string(scale1, '4') + std::string((int)Decimal::MAX_SCALE - 1 - scale1, '9');

    Decimal d0(val0, scale0);
    Decimal d1(val1, scale1);
    int scale = Decimal::MatchScales(&d0, &d1, scale0, scale1);

    // The new scale should always be equal to scale0
    EXPECT_EQ(scale, scale0);

    // Perform subtraction and make sure it equals what we expect
    d0 -= d1;
    auto result = d0.ToString(scale);
    // std::cout << "val0: " << val0 << ":" << scale0 << "\n";
    // std::cout << "val1: " << val1 << ":" << scale1 << "\n";
    // std::cout << "expt: " << expected << "\n";
    // std::cout << "rslt: " << result << ":" << scale << "\n\n";
    EXPECT_EQ(result, expected);
  }
}

}  // namespace libfixeypointy
