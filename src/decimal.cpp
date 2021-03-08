#include "decimal.h"
#include "magic_numbers.h"

//===----------------------------------------------------------------------===//
//
// Decimal
//
//===----------------------------------------------------------------------===//


void CalculateMultiWordProduct128(const uint128_t *const half_words_a, const uint128_t *const half_words_b,
                                  uint128_t *half_words_result, uint32_t m, uint32_t n) {
  // Hacker's Delight [2E Figure 8-1]
  uint128_t k, t;
  uint32_t i, j;
  constexpr const uint128_t bottom_mask = (uint128_t{1} << 64) - 1;
  for (i = 0; i < m; i++) half_words_result[i] = 0;
  for (j = 0; j < n; j++) {
    k = 0;
    for (i = 0; i < m; i++) {
      t = half_words_a[i] * half_words_b[j] + half_words_result[i + j] + k;
      half_words_result[i + j] = t & bottom_mask;
      k = t >> 64;
    }
    half_words_result[j + m] = k;
  }
}

/**
  * Compute the number of leading zeroes in num, ASSUMING THAT num IS NOT ZERO.
  * @param num 128-bit unsigned integer, must not be 0.
  * @return The number of leading zeroes in 128-bit unsigned integer provided.
  */
  static uint32_t GetNumLeadingZeroesAssumingNonZero(uint128_t num) {
    uint64_t hi = num >> 64;
    uint64_t lo = num;
    int32_t retval[2] = {__builtin_clzll(hi), __builtin_clzll(lo) + 64};
    auto idx = static_cast<uint32_t>(hi == 0);
    return retval[idx];
  }

uint128_t CalculateUnsignedLongDivision128(uint128_t u1, uint128_t u0, uint128_t v) {
  // Hacker's Delight [2E Figure 9-3]
  if (u1 >= v) {
    // Result will overflow from 128 bits
    throw ("Decimal Overflow from 128 bits");
  }

  // Base 2^64
  uint128_t b = 1;
  b = b << 64;

  uint128_t un1, un0, vn1, vn0, q1, q0, un32, un21, un10, rhat;
  int128_t s = GetNumLeadingZeroesAssumingNonZero(v);

  // Normalize everything
  v = v << s;
  vn1 = v >> 64;
  vn0 = v & 0xFFFFFFFFFFFFFFFF;

  un32 = (u1 << s) | ((u0 >> (128 - s)) & ((-s) >> 127));
  un10 = u0 << s;
  un1 = un10 >> 64;
  un0 = un10 & 0xFFFFFFFFFFFFFFFF;

  q1 = un32 / vn1;
  rhat = un32 - q1 * vn1;

  do {
    if ((q1 >= b) || (q1 * vn0 > b * rhat + un1)) {
      q1 = q1 - 1;
      rhat = rhat + vn1;
    } else {
      break;
    }
  } while (rhat < b);

  un21 = un32 * b + un1 - q1 * v;

  q0 = un21 / vn1;
  rhat = un21 - q0 * vn1;

  do {
    if ((q0 >= b) || (q0 * vn0 > b * rhat + un0)) {
      q0 = q0 - 1;
      rhat = rhat + vn1;
    } else {
      break;
    }
  } while (rhat < b);

  return q1 * b + q0;
}

/** Some code that was refactored out of Rohan's stuff. Here be dragons. */
Decimal::NativeType DecimalComputeMagicNumbers128(const uint128_t (&half_words_result)[4], uint32_t algo,
                                                  uint32_t magic_p, uint128_t current_value) {
  // Hacker's Delight [2E Chapter 10 Integer Division by Constants]
  switch (algo) {
    case 0: {
      // Overflow Algorithm 1 - Magic number is < 2^128
      uint128_t result_upper = half_words_result[2] | (half_words_result[3] << 64);
      return result_upper >> magic_p;
    }
    case 1: {
      // Overflow Algorithm 2 - Magic number is >= 2^128
      uint128_t result_upper = half_words_result[2] | (half_words_result[3] << 64);
      uint128_t add_upper = current_value;

      result_upper += add_upper;

      auto carry = static_cast<uint128_t>(result_upper < add_upper);
      carry = carry << 127;

      result_upper = result_upper >> 1;
      result_upper |= carry;

      return result_upper >> (magic_p - 1);
    }
    default:
      throw "Unknown algorithm.";
  }
}

/** Some code that was refactored out of Rohan's stuff. Here be dragons. */
Decimal::NativeType DecimalComputeMagicNumbers256(const uint128_t (&a)[4], const uint128_t (&b)[4], uint32_t algo,
                                                  uint32_t magic_p) {
  // Hacker's Delight [2E Chapter 10 Integer Division by Constants]
  uint128_t half_words_magic_result[8];

  if (algo == 0) {
    // Overflow Algorithm 1 - Magic number is < 2^256

    // Magic Result
    // TODO(Rohan): Make optimization to calculate only upper half of the word
    CalculateMultiWordProduct128(a, b, half_words_magic_result, 4, 4);
    // Get the higher order result
    uint128_t result_lower = half_words_magic_result[4] | (half_words_magic_result[5] << 64);
    uint128_t result_upper = half_words_magic_result[6] | (half_words_magic_result[7] << 64);

    uint128_t overflow_checker = result_upper >> magic_p;
    if (overflow_checker > 0) {
      // Result will overflow from 128 bits
      throw ("Result overflow > 128 bits");
    }

    result_lower = result_lower >> magic_p;
    result_upper = result_upper << (128 - magic_p);
    return result_lower | result_upper;
  }
  // Overflow Algorithm 2 - Magic number is >= 2^256

  // TODO(Rohan): Make optimization to calculate only upper half of the word
  CalculateMultiWordProduct128(a, b, half_words_magic_result, 4, 4);
  // Get the higher order result
  uint128_t result_lower = a[0] | (a[1] << 64);
  uint128_t result_upper = a[2] | (a[3] << 64);

  uint128_t add_lower = half_words_magic_result[4] | (half_words_magic_result[5] << 64);
  uint128_t add_upper = half_words_magic_result[6] | (half_words_magic_result[7] << 64);

  // Perform addition
  result_lower += add_lower;
  result_upper += add_upper;
  // carry bit using conditional instructions
  result_upper += static_cast<uint128_t>(result_lower < add_lower);

  uint128_t overflow_checker = result_upper >> magic_p;
  if ((overflow_checker > 0) || (result_upper < add_upper)) {
    // Result will overflow from 128 bits
    throw ("Result overflow > 128 bits");
  }

  // We know that we only retain the lower 128 bits so there is no need of shri
  // We can safely drop the additional carry bit
  result_lower = result_lower >> magic_p;
  result_upper = result_upper << (128 - magic_p);
  return result_lower | result_upper;
}


Decimal Decimal::GetNegation() { return Decimal(-value_); }

Decimal Decimal::GetAbs() { return value_ < 0 ? Decimal(-value_) : Decimal(value_); }

void Decimal::MultiplyAndSet(const Decimal &unsigned_input, uint32_t scale) {
  // 1. Multiply with the overflow check.
  // 2. If overflow, divide by 10^scale using 256-bit magic number division.
  // 3. If no overflow, divide by 10^scale using 128-bit magic number division.

  // Calculate the 256-bit multiplication result.
  uint128_t half_words_result[4];
  {
    // Split the two inputs into half-words.
    uint128_t a = value_;
    uint128_t b = unsigned_input.ToNative();
    uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {b & BOTTOM_MASK, (b & TOP_MASK) >> 64};
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  if (half_words_result[2] == 0 && half_words_result[3] == 0) {
    // TODO(Rohan): Optimize by sending in an array of half words
    value_ = half_words_result[0] | (half_words_result[1] << 64);
    UnsignedDivideConstant128BitPowerOfTen(scale);
    return;
  }

  // Magic number half words
  uint128_t magic[4] = {MAGIC_ARRAY[scale][3], MAGIC_ARRAY[scale][2],
                        MAGIC_ARRAY[scale][1], MAGIC_ARRAY[scale][0]};
  uint32_t magic_p = MAGIC_P_AND_ALGO_ARRAY[scale][0] - 256;
  uint32_t algo = MAGIC_P_AND_ALGO_ARRAY[scale][1];

  value_ = DecimalComputeMagicNumbers256(half_words_result, magic, algo, magic_p);
}

void Decimal::UnsignedDivideConstant128BitPowerOfTen(uint32_t exponent) {
  // Magic number division from Hacker's Delight [2E 10-9 Unsigned Division].

  // Calculate 256-bit multiplication result.
  uint128_t half_words_result[4];
  {
    uint128_t a = value_;
    uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {MAGIC_MAP128_BIT_POWER_TEN[exponent].lower_,
                                 MAGIC_MAP128_BIT_POWER_TEN[exponent].upper_};
    // TODO(Rohan): Calculate only upper half
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  uint32_t magic_p = MAGIC_MAP128_BIT_POWER_TEN[exponent].p_ - 128;
  uint32_t algo = MAGIC_MAP128_BIT_POWER_TEN[exponent].algo_;

  value_ = DecimalComputeMagicNumbers128(half_words_result, algo, magic_p, value_);
}

void Decimal::UnsignedDivideConstant128Bit(uint128_t constant) {
  // 1. If the constant is a power of 2, we right shift.
  // 2. If the magic numbers were precomputed for the constant, we use those.
  // 3. Otherwise, do a normal division.

  if (constant == 1) return;

  // 1. If possible, power of 2 division.
  {
    if ((constant & (constant - 1)) == 0) {
      uint32_t power_of_two = power_two[constant];
      value_ = static_cast<uint128_t>(value_) >> power_of_two;
      return;
    }
  }

  // 2. If not possible, regular division.
  {
    if (magic_map128_bit_constant_division.count(constant) == 0) {
      value_ = static_cast<uint128_t>(value_) / constant;
      return;
    }
  }

  // 3. Magic Number division.
  {
    // Calculate 256-bit multiplication result.
    uint128_t half_words_result[4];
    {
      uint128_t a = value_;
      uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
      uint128_t half_words_b[2] = {magic_map128_bit_constant_division[constant].lower_,
                                   magic_map128_bit_constant_division[constant].upper_};
      // TODO(Rohan): Calculate only upper half
      CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
    }

    uint32_t magic_p = magic_map128_bit_constant_division[constant].p_ - 128;
    uint32_t algo = magic_map128_bit_constant_division[constant].algo_;

    value_ = DecimalComputeMagicNumbers128(half_words_result, algo, magic_p, value_);
  }
}

void Decimal::SignedMultiplyWithDecimal(Decimal multiplier, uint32_t lower_scale) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  bool negative_result = (value_ < 0) != (multiplier.ToNative() < 0);
  value_ = value_ < 0 ? 0 - value_ : value_;
  MultiplyAndSet(multiplier.GetAbs(), lower_scale);
  // Because we convert to positive above, if the sign changed, we overflowed.
  if (value_ < 0) {
    throw ("Result overflow > 128 bits");
  }
  value_ = negative_result ? 0 - value_ : value_;
}

void Decimal::SignedMultiplyWithConstant(int64_t input) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  bool negative_result = (value_ < 0) != (input < 0);
  value_ = value_ < 0 ? 0 - value_ : value_;

  // Calculate 256-bit multiplication result.
  uint128_t half_words_result[4];
  {
    uint128_t a = value_;
    uint128_t b = input < 0 ? -input : input;
    uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {b & BOTTOM_MASK, (b & TOP_MASK) >> 64};
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  if (half_words_result[2] == 0 && half_words_result[3] == 0) {
    value_ = half_words_result[0] | (half_words_result[1] << 64);
  } else {
    throw ("Result overflow > 128 bits");
  }

  value_ = negative_result ? 0 - value_ : value_;
}

void Decimal::SignedDivideWithConstant(int64_t divisor) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  bool negative_result = (value_ < 0) != (divisor < 0);
  value_ = value_ < 0 ? 0 - value_ : value_;
  uint128_t constant = divisor < 0 ? -divisor : divisor;
  UnsignedDivideConstant128Bit(constant);
  value_ = negative_result ? 0 - value_ : value_;
}

void Decimal::SignedDivideWithDecimal(Decimal denominator, uint32_t denominator_scale) {
  // 1. Multiply the dividend with 10^(denominator scale), with overflow checking.
  // 2. If overflow, divide by the denominator with multi-word 256-bit division.
  // 3. If no overflow, divide by the denominator with magic numbers if available, otherwise use 128-bit division.
  // Moreover, the result is in the numerator's scale for technical reasons.
  // If the result were to be in the denominator's scale, the first step would need to be multiplication with
  // 10^(2*denominator scale - numerator scale) which requires 256-bit multiply and 512-bit overflow check.

  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  bool negative_result = (value_ < 0) != (denominator.ToNative() < 0);
  value_ = value_ < 0 ? 0 - value_ : value_;

  uint128_t constant = denominator < 0 ? -denominator.ToNative() : denominator.ToNative();

  // 1. Multiply with 10^(denominator scale), keeping result in numerator scale.
  uint128_t half_words_result[4];
  {
    uint128_t half_words_a[2] = {value_ & BOTTOM_MASK, (value_ & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {POWER_OF_TEN[denominator_scale][1],
                                 POWER_OF_TEN[denominator_scale][0]};
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  if (half_words_result[2] == 0 && half_words_result[3] == 0) {
    // 2. If overflow, divide by the denominator with multi-word 256-bit division.
    value_ = half_words_result[0] | (half_words_result[1] << 64);
    UnsignedDivideConstant128Bit(constant);
  } else {
    if (magic_map256_bit_constant_division.count(constant) > 0) {
      // 3. If no overflow, and have magic numbers, use magic numbers.
      value_ = Decimal::UnsignedMagicDivideConstantNumerator256Bit(half_words_result, constant);
    } else {
      // 3. If no overflow, and no magic numbers, divide by the denominator with 128-bit division.
      value_ = CalculateUnsignedLongDivision128(half_words_result[2] | (half_words_result[3] << 64),
                                                half_words_result[0] | (half_words_result[1] << 64), constant);
    }
  }

  // Because we convert to positive above, if the sign changed, we overflowed.
  if (value_ < 0) {
    throw ("Result overflow > 128 bits");
  }

  value_ = negative_result ? 0 - value_ : value_;
}

uint128_t Decimal::UnsignedMagicDivideConstantNumerator256Bit(const uint128_t (&unsigned_dividend)[4],
                                                              uint128_t unsigned_constant) {
  uint128_t magic[4] = {magic_map256_bit_constant_division[unsigned_constant].d_,
                        magic_map256_bit_constant_division[unsigned_constant].c_,
                        magic_map256_bit_constant_division[unsigned_constant].b_,
                        magic_map256_bit_constant_division[unsigned_constant].a_};
  uint32_t magic_p = magic_map256_bit_constant_division[unsigned_constant].p_ - 256;
  uint32_t algo = magic_map256_bit_constant_division[unsigned_constant].algo_;

  return DecimalComputeMagicNumbers256(unsigned_dividend, magic, algo, magic_p);
}

Decimal::Decimal(std::string input, uint32_t *scale) {
  value_ = 0;

  if (input.empty()) {
    // TODO(WAN): Find appropriate error code.
    throw "Invalid input.";
  }

  uint32_t pos = 0;

  bool is_negative = false;
  if (input[pos] == '-') {
    pos++;
    is_negative = true;
  }

  while (pos < input.size() && input[pos] != '.') {
    value_ += input[pos] - '0';
    if (pos < input.size() - 1) {
      value_ *= 10;
    }
    pos++;
  }

  if (pos == input.size()) {
    if (is_negative) {
      value_ = -value_;
    }
    *scale = 0;
    return;
  }
  pos++;

  if (pos == input.size()) {
    value_ /= 10;
    if (is_negative) {
      value_ = -value_;
    }
    *scale = 0;
    return;
  }

  *scale = 0;
  while (pos < input.size()) {
    value_ += input[pos] - '0';
    if (pos < input.size() - 1) {
      value_ *= 10;
    }
    pos++;
    (*scale) = (*scale) + 1;
  }

  if (is_negative) {
    value_ = -value_;
  }
}

Decimal::Decimal(std::string input, uint32_t scale) {
  value_ = 0;

  if (input.empty()) {
    // TODO(WAN): Find appropriate error code.
    throw "Invalid input.";
  }

  uint32_t pos = 0;

  bool is_negative = false;
  if (input[pos] == '-') {
    pos++;
    is_negative = true;
  }

  while (pos < input.size() && input[pos] != '.') {
    value_ += input[pos] - '0';
    value_ *= 10;
    pos++;
  }

  if (scale == 0) {
    value_ /= 10;
    if (pos != input.size()) {
      if (pos + 1 < input.size()) {
        pos++;
        if (input[pos] - '0' > 5) {
          value_ += 1;
        } else if (input[pos] - '0' == 5 && value_ % 2 == 1) {
          value_ += 1;
        }
      }
    }
    if (is_negative) {
      value_ = -value_;
    }
    return;
  }

  // No decimal point case
  if (pos == input.size()) {
    for (uint32_t i = 0; i < scale - 1; i++) {
      value_ *= 10;
    }
    if (is_negative) {
      value_ = -value_;
    }
    return;
  }
  // Skip decimal point
  pos++;
  // Nothing after decimal point case
  if (pos == input.size()) {
    for (uint32_t i = 0; i < scale - 1; i++) {
      value_ *= 10;
    }
    if (is_negative) {
      value_ = -value_;
    }
    return;
  }

  for (uint32_t i = 1; i < scale; i++) {
    if (pos < input.size()) {
      value_ += input[pos] - '0';
      value_ *= 10;
      pos++;
    } else {
      for (uint32_t j = i; j < scale; j++) {
        value_ *= 10;
      }
      if (is_negative) {
        value_ = -value_;
      }
      return;
    }
  }

  if (pos == input.size()) {
    if (is_negative) {
      value_ = -value_;
    }
    return;
  }

  if (pos == input.size() - 1) {
    // No Rounding required
    value_ += input[pos] - '0';
  } else {
    if (input[pos + 1] - '0' > 5) {
      // Round Up
      value_ += input[pos] - '0' + 1;
    } else if (input[pos + 1] - '0' < 5) {
      // No Rounding will happen
      value_ += input[pos] - '0';
    } else {
      if ((input[pos] - '0') % 2 == 0) {
        // Round up if ODD
        value_ += input[pos] - '0';
      } else {
        // Round up if ODD
        value_ += input[pos] - '0' + 1;
      }
    }
  }

  if (is_negative) {
    value_ = -value_;
  }
}

std::string Decimal::ToString(uint32_t scale) const {
  std::string output;
  int128_t value = value_;
  if (value < 0) {
    output.push_back('-');
    value = 0 - value;
  }

  if (scale != 0) {
    int128_t fractional = value;
    std::string fractional_string;
    for (uint32_t i = 0; i < scale; i++) {
      auto remainder = static_cast<uint32_t>(fractional % 10);
      fractional_string.push_back('0' + remainder);
      fractional /= 10;
    }

    int128_t integral = fractional;

    std::string integral_string;

    while (integral != 0) {
      auto remainder = static_cast<int32_t>(integral % 10);
      integral_string.push_back('0' + remainder);
      integral /= 10;
    }

    std::reverse(integral_string.begin(), integral_string.end());
    output.append(integral_string);

    output.push_back('.');

    std::reverse(fractional_string.begin(), fractional_string.end());
    output.append(fractional_string);

    return output;
  }

  int128_t integral = value;
  std::string integral_string;
  while (integral != 0) {
    auto remainder = static_cast<int32_t>(integral % 10);
    integral_string.push_back('0' + remainder);
    integral /= 10;
  }
  std::reverse(integral_string.begin(), integral_string.end());
  output.append(integral_string);
  return output;
}
