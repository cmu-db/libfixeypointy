#include <bits/stdc++.h>

#include "common.h"

namespace libfixeypointy {

/**
 * A generic fixed point decimal value. This only serves as a storage container for decimals of various sizes.
 * Operations on decimals require a scale.
 *
 * The decimals do not support specifying a precision.
 * The precision is always maximum (38) and the scale ranges from 0 to MAX_SCALE (38).
 */
class Decimal {
 public:
  /** Underlying native data and scale factor types. */
  using NativeType = int128_t;
  using ScaleType = int8_t;

  /** The default scale for a Decimal. */
  static constexpr uint32_t DEFAULT_SCALE = 18;
  /** The maximum scale supported by a Decimal. */
  static constexpr ScaleType MAX_SCALE = 38;

  /**
   * Create a decimal value using the given raw underlying encoded value.
   * @param value The value to set this decimal to.
   */
  explicit Decimal(const NativeType &value) : value_(value) {}

  /**
   * Copy Constructor
   * @param orig The original Decimal to copy into this new object
   */
  Decimal(const Decimal &orig) : value_(orig.value_) {}

  /**
   * Empty constructor.
   */
  Decimal() = default;

  /**
   * Convert an input string into a decimal representation.
   * @param input       The input string to convert.
   *                    If the input string has more digits than the specified scale, the value is rounded up.
   * @param scale       Number of significant digits.
   *                    The scale must be <= MAX_SCALE.
   */
  Decimal(const std::string &input, const ScaleType &scale);

  /**
   * Convert an input string into a decimal representation, taking as many digits as possible.
   * @param input           The input string to convert.
   * @param[out] scale      The scale that the decimal was read with.
   */
  Decimal(std::string input, ScaleType *scale);

  /**
   * @return The raw underlying encoded decimal value.
   */
  operator NativeType() const { return value_; }  // NOLINT

  /**
   * Compute the hash value of this decimal instance.
   * @param seed The value to seed the hash with.
   * @return The hash value for this decimal instance.
   */
  hash_t Hash(const hash_t seed) const {
    uint128_t x = value_ * seed;
    const uint64_t k_mul = 0x9ddfea08eb382d69ULL;
    uint128_t low_mask = 0xFFFFFFFFFFFFFFFF;
    uint64_t a = ((x & low_mask) ^ (x >> 64)) * k_mul;
    a ^= (a >> 47);
    uint64_t b = ((x >> 64) ^ a) * k_mul;
    b ^= (b >> 47);
    b *= k_mul;
    return b;
  }

  /**
   * @return The hash value of this decimal instance.
   */
  hash_t Hash() const { return Hash(1); }

  /**
   * Add the encoded decimal to this decimal and produce a new decimal.
   * @warning The other decimal value MUST be of the same scale.
   * @param that The value to add.
   * @return A new decimal value
   */
  const Decimal operator+(const NativeType &that) {
    return Decimal(this->value_ + that);
  }

  /**
   * Add the encoded decimal value @em that to this decimal value.
   * @warning The other decimal value MUST be of the same scale.
   * @param that The value to add.
   * @return This decimal value.
   */
  const Decimal &operator+=(const NativeType &that) {
    value_ += that;
    return *this;
  }

  /**
   * Subtract the encoded decimal value @em that from this decimal value.
   * @warning The other decimal value MUST be of the same scale.
   * @param that The value to subtract.
   * @return This decimal value.
   */
  const Decimal &operator-=(const NativeType &that) {
    value_ -= that;
    return *this;
  }

  /**
   * Subtract the encoded decimal to this decimal and produce a new decimal.
   * @warning The other decimal value MUST be of the same scale.
   * @param that The value to subtract.
   * @return A new decimal value
   */
  const Decimal operator-(const NativeType &that) {
    return Decimal(this->value_ - that);
  }

  /**
   * Get the string representation of the current decimal. Requires knowing the scale.
   *
   * @param scale The scale of the current decimal. This must be accurate!
   * @return The string representation of this decimal.
   */
  std::string ToString(uint32_t scale) const;

  /**
   * @return The native representation of the decimal.
   */
  NativeType ToNative() const { return value_; }

  /**
   * Compute the negative value of the current decimal
   * @return negative value
   */
  Decimal GetNegation() const;

  /**
   * Compute the absolute value (always positive) version of the current decimal.
   * @return absolute value
   */
  Decimal GetAbs() const;

  /**
   * Add the encoded decimal to this decimal and produce a new decimal.
   * @warning The other decimal value MUST be of the same scale.
   * @param other The value to add.
   * @return A new decimal value
   */
  void Add(const Decimal &other);

  /**
   * Subtract the encoded decimal to this decimal and produce a new decimal.
   * @warning The other decimal value MUST be of the same scale.
   * @param other The value to add.
   * @return A new decimal value
   */
  void Subtract(const Decimal &other);

  /**
   * Multiply the current decimal by the given decimal.
   * The result is in the higher scale of the current decimal and the
   * multiplier.
   * @param multiplier          The decimal to multiply by.
   * @param scale         The lower scale of the two decimals.
   */
  void Multiply(const Decimal &multiplier, const ScaleType &scale);

  /**
   * Divide the current decimal by the given decimal.
   * The result is in the numerator's (current decimal's) scale.
   * @param denominator             The decimal to divide by.
   * @param scale       The scale of the denominator.
   */
  void Divide(const Decimal &denominator, const ScaleType &scale);

  /**
   * Multiple the current decimal by a constant integer value.
   * This method is faster than generating a decimal with the
   * constant and then multiplying two decimals.
   * @param value the constant to be multiplied with.
   */
  void MultiplyByConstant(const int64_t &value);

  /**
   * Divide the current decimal by the value provided.
   * This method is faster than generating a decimal with the
   * constant and then multiplying two decimals.
   * @param value The value to divide by.
   */
  void DivideByConstant(const int64_t &value);

  /**
   * Match the scales of the two decimals by rescaling the less precise of
   * the inputs to the higher scale. This will change the underlying
   * native value of one of the given Decimals.
   * @param left                The left decimal value.
   * @param right               The right decimal value.
   * @param left_scale          The scale of the left decimal value.
   * @param right_scale         The scale of the right decimal value.
   * @return new_scale          The new scale that the decimals have been scaled to
   */
  static uint32_t MatchScales(Decimal *left, Decimal *right, ScaleType left_scale, ScaleType right_scale) {
    // TODO(Rohan): Optimize this by performing a binary search.
    NativeType intermediate_value;
    uint32_t new_scale;
    if (left_scale < right_scale) {
      new_scale = right_scale;
      intermediate_value = left->ToNative();
      auto adjust_factor = right_scale - left_scale;
      for (ScaleType i = 0; i < adjust_factor; i++) {
        intermediate_value *= 10;
      }
      left->value_ = intermediate_value;
    } else if (left_scale > right_scale) {
      new_scale = left_scale;
      intermediate_value = right->ToNative();
      auto adjust_factor = left_scale - right_scale;
      for (ScaleType i = 0; i < adjust_factor; i++) {
        intermediate_value *= 10;
      }
      right->value_ = intermediate_value;
    } else {
      // The left_scale is the same as right_scale
      new_scale = left_scale;
    }
    return new_scale;
  }

 private:
  // The encoded decimal value
  NativeType value_;

  /** A mask for the bottom half of a 128-bit decimal. */
  static constexpr const uint128_t BOTTOM_MASK = (uint128_t{1} << 64) - 1;
  /** A mask for the top half of a 128-bit decimal. */
  static constexpr const uint128_t TOP_MASK = ~BOTTOM_MASK;

  /**
   * Divide the current decimal by the unsigned 128-bit constant supplied.
   * @warning Assumes that the current decimal is non-negative.
   * @param constant The unsigned 128-bit constant to divide by.
   */
  void UnsignedDivideConstant128Bit(uint128_t constant);

  /**
   * Divide the 256-bit unsigned dividend with a 128-bit unsigned divisor.
   * @warning Assumes that the magic number required is already present in MAGIC_MAP256_BIT_CONSTANT_DIVISION.
   * @param unsigned_dividend   The 256-bit unsigned dividend, represented as an array of 128 bit numbers,
   *                            where each 128-bit number only has the lower 64 bits set.
   * @param unsigned_constant   The 128-bit unsigned divisor.
   * @return                    The result of the division.
   */
  uint128_t UnsignedMagicDivideConstantNumerator256Bit(const uint128_t (&unsigned_dividend)[4],
                                                              uint128_t unsigned_constant);

  /**
   * Multiply the current decimal with an unsigned decimal.
   * The scale of the result depends on the scale provided.
   * @param unsigned_input  The input decimal to multiply against. Must be unsigned!
   * @param scale           The number of significant digits after the decimal point.
   *                        To obtain higher scale result, pass in the lower scale of the operands.
   *                        To obtain lower scale result, pass in the higher scale of the operands.
   */
  void MultiplyAndSet(const Decimal &unsigned_input, ScaleType scale);

  /**
   * Divide the current positive unsigned 128-bit integer by a power of ten.
   *
   * @warning   The current decimal is assumed to be positive, unsigned, and 128-bit.
   *
   * @param exponent The exponent of the power of ten to divide by. (i in 10^i)
   */
  void UnsignedDivideConstant128BitPowerOfTen(uint32_t exponent);

  /**
   *
   * @param u1
   * @param u0
   * @param v
   * @return
   */
  uint128_t CalculateUnsignedLongDivision128(uint128_t u1, uint128_t u0, uint128_t v);

  /**
   *
   * @param half_words_a
   * @param half_words_b
   * @param half_words_result
   * @param m
   * @param n
   */
  void CalculateMultiWordProduct128(const uint128_t *const half_words_a,
                                    const uint128_t *const half_words_b,
                                    uint128_t *half_words_result,
                                    uint32_t m,
                                    uint32_t n) const;

  /**
   *
   * @param a
   * @param b
   * @param algo
   * @param magic_p
   * @return
   */
  NativeType DecimalComputeMagicNumbers256(const uint128_t (&a)[4], const uint128_t (&b)[4], AlgorithmType algo,
                                                    uint32_t magic_p);

};

}  // namespace libfixeypointy