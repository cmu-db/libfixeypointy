#include "decimal.h"
#include "common.h"
#include "magic_numbers.h"

namespace libfixeypointy {

// FIXME: now overflow check in Decimal creation, dangerous
Decimal::Decimal(std::string input, ScaleType *scale) : value_(0) {
  if (input.empty()) {
    throw std::runtime_error("Invalid empty input string '" + input + "'");
  }

  // We are going to parse the input string to extract the decimal
  // We use this to keep track of the position in the string.
  uint32_t pos = 0;

  // If the first character is a minus sign, then we obviously have
  // a negative number.
  bool is_negative = false;
  if (input[pos] == '-') {
    pos++;
    is_negative = true;
  }

  // Number before decimal point
  while (pos < input.size() && input[pos] != '.') {
    value_ += input[pos] - '0';
    if (pos < input.size() - 1) {
      value_ *= 10;
    }
    pos++;
  }

  // If the position before decimal point is the last number,
  // then there is no fractional part
  if (pos == input.size()) {
    // So, if it is negative, we can multiply by -1 directly
    if (is_negative) {
      value_ = -value_;
    }
    // Also, the scale is zero
    *scale = 0;
    // Then, we can return
    return;
  }

  // If not, this means it must be decimal point
  // So, skip the point to parse the fractional part
  pos++;

  // If there is no fractional part after decimal point,
  // then do the same thing as above
  if (pos == input.size()) {
    value_ /= 10;
    if (is_negative) {
      value_ = -value_;
    }
    *scale = 0;
    return;
  }

  // Now, we need scale
  *scale = 0;

  // Number after decimal point
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

Decimal::Decimal(const std::string &input, const ScaleType &scale) : value_(0) {
  if (input.empty()) {
    throw std::runtime_error("Invalid empty input string '" + input + "'");
  }

  // We are going to parse the input string to extract the decimal
  // We use this to keep track of the position in the string.
  uint32_t pos = 0;

  // If the first character is a minus sign, then we obviously have
  // a negative number.
  bool is_negative = false;
  if (input[pos] == '-') {
    pos++;
    is_negative = true;
  }

  // Digits before decimal point
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
    for (ScaleType i = 0; i < scale - 1; i++) {
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
    for (ScaleType i = 0; i < scale - 1; i++) {
      value_ *= 10;
    }
    if (is_negative) {
      value_ = -value_;
    }
    return;
  }

  for (ScaleType i = 1; i < scale; i++) {
    if (pos < input.size()) {
      value_ += input[pos] - '0';
      value_ *= 10;
      pos++;
    } else {
      for (ScaleType j = i; j < scale; j++) {
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

  // If negative, put - sign
  if (value < 0) {
    output.push_back('-');
    value = 0 - value;
  }

  // If there is scale, there must be a fractional part
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

    // Always include leading zero
    if (output.empty() || (output[0] == '-' && output.length() == 1)) output.push_back('0');
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

void Decimal::Add(const Decimal &other) {
  // TODO(Guide): Implementation with multiple branches maybe not performant, try reducing branches
  // If both values are positive, it is possible to get overflowed
  if (value_ > 0 && other.value_ > 0) {
    // Compute the maximum and safe value for other
    // E.g., 63 - 62 = 1
    // So, if other = 1, 62 + 1 = 63 (safe)
    // But if other = 2 (> 1), 62 + 2 = 64 (overflowed)
    int128_t other_bound = std::numeric_limits<__int128>::max() - value_;
    if (other.value_ > other_bound) {
      throw std::runtime_error("Result overflow > 128 bits");
    }
  }

  // If both values are negative, same as above
  else if (value_ < 0 && other.value_ < 0) {
    // Compute the minimum and safe value for other
    // E.g., -64 - -63 = -1
    // So, if other = -1, -63 + -1 = -64 (safe)
    // But if other = -2 (< -1), -63 + -2 = -65 (overflowed)
    int128_t other_bound = std::numeric_limits<__int128>::min() - value_;
    if (other.value_ < other_bound) {
      throw std::runtime_error("Result overflow > 128 bits");
    }
  }

  // Otherwise, it is impossible to overflow
  value_ += other.value_;
}

void Decimal::Subtract(const Decimal &other) {
  // TODO(Guide): Implementation with multiple branches maybe not performant, try reducing branches
  // If the first value is positive and the second value is negative,
  // it is possible to get overflowed
  if (value_ > 0 && other.value_ < 0) {
    // Compute the maximum value for other
    // E.g., 62 - 63 = -1
    // So, if other = -1, 62 - -1 = 63 (safe)
    // But if other = -2 (< -1), 62 - -2 = 64 (overflowed)
    int128_t other_bound = value_ - std::numeric_limits<__int128>::max();
    if (other.value_ < other_bound) {
      throw std::runtime_error("Result overflow > 128 bits");
    }
  }

  // If the first value is negative and the second value is positive,
  // same as above
  else if (value_ < 0 && other.value_ > 0) {
    // Compute the maximum value for other
    // E.g., -63 - -64 = 1
    // So, if other = 1, -63 - 1 = -64 (safe)
    // But if other = 2 (> 1), -63 - 2 = -65 (overflowed)
    int128_t other_bound = value_ - std::numeric_limits<__int128>::min();
    if (other.value_ > other_bound) {
      throw std::runtime_error("Result overflow > 128 bits");
    }
  }

  // Otherwise, it is impossible to overflow
  value_ -= other.value_;
}

void Decimal::Multiply(const Decimal &multiplier, const ScaleType &scale) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  // Save the sign of the result since we want to multiply regardless of the sign
  bool negative_result = (value_ < 0) != (multiplier.ToNative() < 0);
  // Extract the absolute value of the first decimal
  value_ = (value_ < 0 ? 0 - value_ : value_);
  // Compute multiplication
  MultiplyAndSet(multiplier.GetAbs(), scale);
  // Because we convert to positive above, if the sign changed, we overflowed.
  if (value_ < 0) {
    throw std::runtime_error("Result overflow > 128 bits");
  }
  // Apply the sign we saved at the beginning
  value_ = negative_result ? 0 - value_ : value_;
}

void Decimal::MultiplyByConstant(const int64_t &value) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  // Save the sign of the result since we want to multiply regardless of the sign
  bool negative_result = (value_ < 0) != (value < 0);
  // Extract the absolute value of the first decimal
  value_ = (value_ < 0 ? 0 - value_ : value_);

  // Calculate 256-bit multiplication result.
  uint128_t half_words_result[4];
  {
    uint128_t a = value_;
    // Extract the absolute value of the second decimal
    uint128_t b = value < 0 ? -value : value;
    // Split each decimal into 2 half words using BOTTOM_MASK and TOP_MASK
    // E.g., 0b11001010 -> {0b1010, 0b1100}
    uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {b & BOTTOM_MASK, (b & TOP_MASK) >> 64};
    // Compute multiplication
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  // The multiplication is overflowed if the higher word (128-bit) is not zero
  // TODO(Guide): Do not forget that the original decimals contain signs
  // So, it may be possible that we may need to check the lower word
  // Only lower 127 bits are safe to overflow
  if (half_words_result[2] == 0 && half_words_result[3] == 0) {
    value_ = half_words_result[0] | (half_words_result[1] << 64);
  } else {
    throw std::runtime_error("Result overflow > 128 bits");
  }

  // Apply the sign we saved at the beginning
  value_ = (negative_result ? 0 - value_ : value_);
}

void Decimal::DivideByConstant(const int64_t &value) {
  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  // Save the sign since we want to apply division regardless of the sign
  bool negative_result = (value_ < 0) != (value < 0);
  // Take absolute value of the value_
  value_ = (value_ < 0 ? 0 - value_ : value_);
  // Take absolute value of the constant (i.e., argument)
  uint128_t constant = value < 0 ? -value : value;
  // Compute division
  UnsignedDivideConstant128Bit(constant);
  // Apply the sign we saved at the beginning
  value_ = negative_result ? 0 - value_ : value_;
}

void Decimal::Divide(const Decimal &denominator, const ScaleType &scale) {
  // 1. Multiply the dividend with 10^(denominator scale), with overflow checking.
  // 2. If overflow, divide by the denominator with multi-word 256-bit division.
  // 3. If no overflow, divide by the denominator with magic numbers if available, otherwise use 128-bit division.
  // Moreover, the result is in the numerator's scale for technical reasons.
  // If the result were to be in the denominator's scale, the first step would need to be multiplication with
  // 10^(2*denominator scale - numerator scale) which requires 256-bit multiply and 512-bit overflow check.

  // The method in Hacker Delight 2-14 is not used because shift needs to be agnostic of underlying T
  // Will be needed to change in the future when storage optimizations happen
  // Save the sign since we want to apply division regardless of the sign
  bool negative_result = (value_ < 0) != (denominator.ToNative() < 0);
  // Take absolute value of the value_
  value_ = value_ < 0 ? 0 - value_ : value_;
  // Take absolute value of the denominator
  NativeType constant = denominator < 0 ? -denominator.ToNative() : denominator.ToNative();

  // 1. Multiply with 10^(denominator scale), keeping result in numerator scale.
  // E.g., 10.??? (10???) / 2.?? (2??), then numerator = 10??? * 10^2 = 10???00
  uint128_t half_words_result[4];
  {
    // Split each decimal into 2 half words using BOTTOM_MASK and TOP_MASK
    // E.g., 0b11001010 -> {0b1010, 0b1100}
    uint128_t half_words_a[2] = {value_ & BOTTOM_MASK, (value_ & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {POWER_OF_TEN[scale][1], POWER_OF_TEN[scale][0]};
    // Compute multiplication
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  // TODO(Guide): Does this predicate check correctly?
  if (half_words_result[2] == 0 && half_words_result[3] == 0) {
    // 2. If overflow, divide by the denominator with multi-word 256-bit division.
    // E.g., 1 / 2.?, then 1 * 10^1 = 10 (log2(10) << 64)
    value_ = half_words_result[0] | (half_words_result[1] << 64);
    // Apply divisions
    UnsignedDivideConstant128Bit(constant);
  } else if (MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION.count(constant) > 0) {
    // 3. If no overflow, and have magic numbers, use magic numbers.
    value_ = UnsignedMagicDivideConstantNumerator256Bit(half_words_result, constant);
  } else {
    // 4. If no overflow, and no magic numbers, divide by the denominator with 128-bit division.
    value_ = CalculateUnsignedLongDivision128(half_words_result[2] | (half_words_result[3] << 64),
                                              half_words_result[0] | (half_words_result[1] << 64), constant);
  }

  // Because we convert to positive above, if the sign changed, we overflowed.
  if (value_ < 0) {
    throw std::runtime_error("Result overflow > 128 bits");
  }

  value_ = (negative_result ? 0 - value_ : value_);
}

/** Some code that was refactored out of Rohan's stuff. Here be dragons. */
Decimal::NativeType DecimalComputeMagicNumbers128(const uint128_t (&half_words_result)[4], AlgorithmType algo,
                                                  uint32_t magic_p, uint128_t current_value) {
  // Hacker's Delight [2E Chapter 10 Integer Division by Constants]
  switch (algo) {
    case AlgorithmType::OVERFLOW_SMALL: {
      // Overflow Algorithm 1 - Magic number is < 2^128
      uint128_t result_upper = half_words_result[2] | (half_words_result[3] << 64);
      return result_upper >> magic_p;
    }
    case AlgorithmType::OVERFLOW_LARGE: {
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
      throw std::runtime_error("Unknown algorithm.");
  }
}

Decimal Decimal::GetNegation() const { return Decimal(-value_); }

Decimal Decimal::GetAbs() const { return value_ < 0 ? Decimal(-value_) : Decimal(value_); }

void Decimal::MultiplyAndSet(const Decimal &unsigned_input, ScaleType scale) {
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
    DivideByConstantPowerOfTen128(scale);
    return;
  }

  // Magic number half words
  uint128_t magic[4] = {MAGIC_ARRAY[scale][3], MAGIC_ARRAY[scale][2], MAGIC_ARRAY[scale][1], MAGIC_ARRAY[scale][0]};
  uint32_t magic_p = MAGIC_P_AND_ALGO_ARRAY[scale].first - 256;
  AlgorithmType algo = MAGIC_P_AND_ALGO_ARRAY[scale].second;

  value_ = DivideByMagicNumbers256(half_words_result, magic, algo, magic_p);
}

void Decimal::DivideByConstantPowerOfTen128(uint32_t exponent) {
  // Magic number division from Hacker's Delight [2E 10-9 Unsigned Division].

  // Calculate 256-bit multiplication result.
  uint128_t half_words_result[4];
  {
    uint128_t a = value_;
    uint128_t half_words_a[2] = {a & BOTTOM_MASK, (a & TOP_MASK) >> 64};
    uint128_t half_words_b[2] = {MAGIC_MAP128_BIT_POWER_TEN[exponent].chunk1_,
                                 MAGIC_MAP128_BIT_POWER_TEN[exponent].chunk0_};
    // TODO(Rohan): Calculate only upper half
    CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
  }

  uint32_t magic_p = MAGIC_MAP128_BIT_POWER_TEN[exponent].p_ - 128;
  AlgorithmType algo = MAGIC_MAP128_BIT_POWER_TEN[exponent].algo_;

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
      uint32_t power_of_two = POWER_OF_TWO[constant];
      value_ = static_cast<uint128_t>(value_) >> power_of_two;
      return;
    }
  }

  // 2. If not possible, regular division.
  {
    if (MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION.count(constant) == 0) {
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
      uint128_t half_words_b[2] = {MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION[constant].chunk1_,
                                   MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION[constant].chunk0_};
      // TODO(Rohan): Calculate only upper half
      CalculateMultiWordProduct128(half_words_a, half_words_b, half_words_result, 2, 2);
    }

    uint32_t magic_p = MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION[constant].p_ - 128;
    AlgorithmType algo = MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION[constant].algo_;

    value_ = DecimalComputeMagicNumbers128(half_words_result, algo, magic_p, value_);
  }
}

uint128_t Decimal::UnsignedMagicDivideConstantNumerator256Bit(const uint128_t (&unsigned_dividend)[4],
                                                              uint128_t unsigned_constant) {
  uint128_t magic[4] = {MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].chunk3_,
                        MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].chunk2_,
                        MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].chunk1_,
                        MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].chunk0_};
  uint32_t magic_p = MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].p_ - 256;
  AlgorithmType algo = MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION[unsigned_constant].algo_;

  return DivideByMagicNumbers256(unsigned_dividend, magic, algo, magic_p);
}

uint128_t Decimal::CalculateUnsignedLongDivision128(uint128_t u1, uint128_t u0, uint128_t v) {
  // Hacker's Delight [2E Figure 9-3]
  if (u1 >= v) {
    // Result will overflow from 128 bits
    throw std::runtime_error("Decimal Overflow from 128 bits");
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

void Decimal::CalculateMultiWordProduct128(const uint128_t *const half_words_a, const uint128_t *const half_words_b,
                                           uint128_t *half_words_result, uint32_t m, uint32_t n) const {
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

/** Some code that was refactored out of Rohan's stuff. Here be dragons. */
Decimal::NativeType Decimal::DivideByMagicNumbers256(const uint128_t (&a)[4], const uint128_t (&b)[4],
                                                     AlgorithmType algo, uint32_t magic_p) {
  // Hacker's Delight [2E Chapter 10 Integer Division by Constants]
  uint128_t half_words_magic_result[8];
  NativeType final_result;

  switch (algo) {
    // Overflow Algorithm 1 - Magic number is < 2^256
    case AlgorithmType::OVERFLOW_SMALL: {
      // Magic Result
      // TODO(Rohan): Make optimization to calculate only upper half of the word
      CalculateMultiWordProduct128(a, b, half_words_magic_result, 4, 4);
      // Get the higher order result
      uint128_t result_lower = half_words_magic_result[4] | (half_words_magic_result[5] << 64);
      uint128_t result_upper = half_words_magic_result[6] | (half_words_magic_result[7] << 64);

      uint128_t overflow_checker = result_upper >> magic_p;
      if (overflow_checker > 0) {
        // Result will overflow from 128 bits
        throw std::runtime_error("Result overflow > 128 bits");
      }

      result_lower = result_lower >> magic_p;
      result_upper = result_upper << (128 - magic_p);
      final_result = result_lower | result_upper;
      break;
    }
    // Overflow Algorithm 2 - Magic number is >= 2^256
    case AlgorithmType::OVERFLOW_LARGE: {
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
        throw std::runtime_error("Result overflow > 128 bits");
      }

      // We know that we only retain the lower 128 bits so there is no need of shri
      // We can safely drop the additional carry bit
      result_lower = result_lower >> magic_p;
      result_upper = result_upper << (128 - magic_p);
      final_result = result_lower | result_upper;
      break;
    }
    default: {
      throw std::runtime_error("Unsupported overflow algorithm type");
    }
  }
  return (final_result);
}

}  // namespace libfixeypointy